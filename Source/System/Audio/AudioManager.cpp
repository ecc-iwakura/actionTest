#include "AudioManager.h"
#include "System/Common/Misc.h"
#include "System/Common/Logger.h"
#include <fstream>
#include <sstream>

using namespace std;

AudioManager* AudioManager::instance = nullptr;

// 同時再生可能数(BGM)
const int AudioManager::MaxBgmVoice = 2;
// 同時再生可能数(SE)
const int AudioManager::MaxSeVoice = 20;
// 最大音量
const float AudioManager::MaxVolume = 1.0f;

AudioManager::AudioManager()
{
	// すでに実体が生成されていればアサート表示を行う
	_ASSERT_EXPR(instance == nullptr, "already instantiated");
	instance = this;

	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// オーディオシステム生成
	hr = XAudio2Create(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// マスタリングボイス生成
	hr = xAudio->CreateMasteringVoice(&masterVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// サブミックスボイス生成
	xAudio->CreateSubmixVoice(&bgmSubmixVoice, 2, 44100);
	xAudio->CreateSubmixVoice(&seSubmixVoice, 1, 44100);

	// マスタリングボイスからチャンネルマスク取得
	masterVoice->GetChannelMask(&channelMask);

	// スピーカーのセットアップ、1 秒あたりのユーザー定義ワールド 単位でのサウンドの速度、
	// X3DAudio エンジンのインスタンスを返すハンドルを示すフラグを取得
	//X3DAudioInitialize(SPEAKER_STEREO, X3DAUDIO_SPEED_OF_SOUND, x3DAudioHandle);
	X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3DAudioHandle);

	// 3Dサウンドリスナーの初期化
	listener = {};

	// BGM,SEそれぞれの再生用変数初期化
	//for (int i = 0; i < MaxBgmVoice; i++)
	//{
	//	bgmPlayer.emplace_back(make_unique<AudioPlayer>(xAudio, bgmSubmixVoice, masterVoice));
	//}
	//for (int i = 0; i < MaxSeVoice; i++)
	//{
	//	sePlayer.emplace_back(make_unique<AudioPlayer>(xAudio, seSubmixVoice, masterVoice));
	//}
}

AudioManager::~AudioManager()
{
	// 再生用のボイス破棄
	// マスタリングボイスの破棄より先に行わないとエラーが発生する
	for (auto& bv : bgmPlayer) bv->VoiceRelease();
	bgmPlayer.clear();
	for (auto& sv : sePlayer) sv->VoiceRelease();
	sePlayer.clear();

	// サブミックスボイス破棄
	if (bgmSubmixVoice != nullptr)
	{
		bgmSubmixVoice->DestroyVoice();
		bgmSubmixVoice = nullptr;
	}
	if (seSubmixVoice != nullptr)
	{
		seSubmixVoice->DestroyVoice();
		seSubmixVoice = nullptr;
	}

	// マスタリングボイス破棄
	if (masterVoice != nullptr)
	{
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}

	// XAudio終了
	if (xAudio != nullptr)
	{
		xAudio->Release();
		xAudio = nullptr;
	}

	// COM終了化
	CoUninitialize();
}

void AudioManager::PlayBGM(audio::BGM index, bool loop, bool crossFade, float fadeTime)
{
	// indexが範囲外ならなにもしない
	if (index >= audio::BGM::Max)return;

	// クロスフェードをする場合、再生中のAudioPlayerを探してフェードアウトのフラグを立てる
	if (crossFade)
	{
		for (auto& bgm : bgmPlayer)
		{
			if (bgm->IsPlaying())
			{
				bgm->SetFade(audio::Fade::Out, fadeTime);
				break;
			}
		}
	}

	// 再生中でないAudioPlayerを探す
	for (auto& bgm : bgmPlayer)
	{
		if (!bgm->IsPlaying())
		{
			// 再生中でなければデータをセットして再生し、ループを抜ける
			if (crossFade)
			{
				bgm->Play(bgmDataArr[index], loop, static_cast<int>( index ), audio::Fade::In, fadeTime);
				break;
			}
			else
			{
				bgm->Play(bgmDataArr[index], loop, static_cast<int>( index ));
				break;
			}
		}
	}
}

void AudioManager::StopBGM()
{
	for (auto& bgm : bgmPlayer)if (bgm->IsPlaying()) bgm->Stop();
}

void AudioManager::PlaySE(audio::SE index)
{
	// indexが範囲外ならなにもしない
	if (index >= audio::SE::Max)return;
	// 再生中でないAudioPlayerでSEを再生する
	for (auto& se : sePlayer)
	{
		if (!se->IsPlaying())
		{
			// 再生中でなければデータをセットしてループを抜ける
			se->Play(seDataArr[index], false, static_cast<int>( index ));
			break;
		}
	}
}

void AudioManager::StopSE(audio::SE index)
{
	// indexが範囲外ならなにもしない
	if (index >= audio::SE::Max)return;

	for (auto& se : sePlayer)
	{
		// indexで指定した再生中のSEを全て止める
		if (se->GetPlayingIndex() == static_cast<int>( index ))
		{
			se->Stop();
		}
	}
}

void AudioManager::StopAllSE()
{
	for (auto& se : sePlayer) se.get()->Stop();
}

bool AudioManager::LoadBGM(string filename, audio::BGM index)
{
	// ファイル読み込み
	ifstream loadFile(filename, ios_base::in | ios_base::binary);
	// ファイルが開けなかったらfalseを返す
	if (!loadFile) return false;
	return LoadBGM(loadFile, index);
}

bool AudioManager::LoadBGM(ifstream& file, audio::BGM index)
{
	// 読み込んだファイルをBGMデータ配列に追加
	pair<unordered_map<audio::BGM, shared_ptr<AudioData>>::iterator, bool> ret;
	ret = bgmDataArr.try_emplace(index, make_shared<AudioData>(file));
	// 追加に失敗した場合はfalseを返す
	if (ret.second == false)
	{
		Logger::Print("追加しようとしたインデックス番号のデータはすでに存在しています");
		return true;
	}
	// 追加した音源データが何もなかった場合は配列からデータを消してfalseを返す
	if (ret.first->second->GetAudioBytes() == 0)
	{
		Logger::Print("追加された波形データのサイズが0バイトでした");
		bgmDataArr.erase(ret.first);
		return false;
	}
	return true;
}

bool AudioManager::LoadSE(string filename, audio::SE index)
{
	// ファイル読み込み
	ifstream loadFile(filename, ios_base::in | ios_base::binary);
	if (!loadFile) return false;
	return LoadSE(loadFile, index);
}

bool AudioManager::LoadSE(ifstream& file, audio::SE index)
{
	// 読み込んだファイルをSEデータ配列に追加
	pair < unordered_map<audio::SE, shared_ptr<AudioData>>::iterator, bool> ret;
	ret = seDataArr.try_emplace(index, make_shared<AudioData>(file));
	// 追加に失敗した場合はfalseを返す
	if (ret.second == false)
	{
		Logger::Print("追加しようとしたインデックス番号のデータはすでに存在しています");
		return false;
	}
	// 追加した音源データが何もなかった場合は配列からデータを消してfalseを返す
	if (ret.first->second->GetAudioBytes() == 0)
	{
		Logger::Print("追加された波形データのサイズが0バイトでした");
		seDataArr.erase(ret.first);
		return false;
	}
	return true;
}

void AudioManager::SetMasterVolume(const float vol)
{
	// 0.0f～MaxVolume の間で設定
	if (vol > MaxVolume)masterVoice->SetVolume(MaxVolume);
	else if (vol < 0.0f)masterVoice->SetVolume(0.0f);
	else masterVoice->SetVolume(vol);
}

void AudioManager::SetBGMVolume(const float vol)
{
	// 0.0f～MaxVolume の間で設定
	if (vol > MaxVolume)bgmSubmixVoice->SetVolume(MaxVolume);
	else if (vol < 0.0f)bgmSubmixVoice->SetVolume(0.0f);
	else bgmSubmixVoice->SetVolume(vol);
}

void AudioManager::SetSEVolume(const float vol)
{
	// 0.0f～MaxVolume の間で設定
	if (vol > MaxVolume)seSubmixVoice->SetVolume(MaxVolume);
	else if (vol < 0.0f)seSubmixVoice->SetVolume(0.0f);
	else seSubmixVoice->SetVolume(vol);
}

std::weak_ptr<AudioData> AudioManager::GetBGMData(audio::BGM index)
{
	// 指定インデックス番号の要素を検索
	auto it = bgmDataArr.find(index);
	// 要素がなければロードする
	if (it == bgmDataArr.end())
	{

	}
	return it->second;
}

std::weak_ptr<AudioData> AudioManager::GetSEData(audio::SE index)
{
	// 指定インデックス番号の要素を検索
	auto it = seDataArr.find(index);
	// 要素がなければロードする
	if (it == seDataArr.end())
	{

	}
	return it->second;
}

void AudioManager::DecodeBuffer(ifstream& encodeFile)
{
	// 復号化用の文字列
	const string KEY = "ecc_comp";

	// ファイルサイズ分のバッファを作成し、暗号化されたファイルを読み込む
	encodeFile.seekg(0, ios_base::end);
	size_t fileSize = encodeFile.tellg();
	encodeFile.seekg(0, ios_base::beg);
	char* buffer = new char[fileSize];
	encodeFile.read(buffer, fileSize);
	// 暗号化されたファイルは必要ないので閉じる
	encodeFile.close();

	// XOR演算を利用して復号化する
	for (size_t i = 0; i < fileSize; i++)
	{
		buffer[i] = buffer[i] ^ KEY[i & 0x07];
	}

	// 復号化したbufferを一時データのbufファイルに出力する
	ofstream out("buf", ios_base::binary);
	out.write(buffer, fileSize);
	out.close();
	delete[] buffer;

	// 復号化されたデータがあるbufファイルを読み込む
	// 一時データの削除はAudioDataのコンストラクタで行う想定
	encodeFile.open("buf", ios_base::in | ios_base::binary);
}
