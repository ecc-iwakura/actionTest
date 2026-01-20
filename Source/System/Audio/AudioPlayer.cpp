#include "AudioManager.h"
#include "AudioPlayer.h"
#include "Component/Transform.h"
#include "System/Common/Logger.h"
#include "System/Common/Misc.h"
#include <imgui.h>

using namespace std;

constexpr unsigned int SOURCE_CHANNELS = 1;

// デストラクタ
AudioPlayer::~AudioPlayer()
{
	SafeDelete(emitter.pChannelAzimuths);
	SafeDeleteArr(DSPSettings.pMatrixCoefficients);

	VoiceRelease();
}

void AudioPlayer::VoiceRelease()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

void AudioPlayer::CreateSourceVoice(IXAudio2SubmixVoice* submixVoice)
{
	// Waveファイルのフォーマットを設定
	// 複数のソースボイスで再生することを考えて、
	// Waveファイルのフォーマット形式を統一しておく想定のため、
	// 違うフォーマット形式のWaveファイルの読み込みは想定しない
	// フォーマット形式の確認はフリーソフトの「真空波動研」などを使用すると良い
	// フォーマット形式：PCM
	// チャンネル数：2（ステレオ）
	// サンプリングレート：44.1kHz
	// サンプルビット数：16
	WAVEFORMATEX wfx{ 0 };
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = SOURCE_CHANNELS;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = ( wfx.wBitsPerSample >> 3 ) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = sizeof(WAVEFORMATEX);

	// サブミックスボイスの有無を確認
	// サブミックスボイスの設定がある場合
	this->submixVoice = submixVoice;
	XAUDIO2_SEND_DESCRIPTOR SFXSend = { 0, submixVoice };
	XAUDIO2_VOICE_SENDS SFXSendList = { 1, &SFXSend };
	// ソースボイスを生成
	IXAudio2* xaudio = AudioManager::Instance()->GetXAudio2();
	HRESULT	hr = xaudio->CreateSourceVoice(&sourceVoice, (WAVEFORMATEX*)&wfx,
		0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback, &SFXSendList, NULL);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// エミッターの初期化
	// CurveDistanceScaler、DopplerScaler メンバーは、 FLT_MIN ～ FLT_MAX 範囲内にする必要がある
	emitter = {};
	emitter.Position = { 0,0,0 };
	emitter.OrientFront = { 0,0,1 };
	emitter.OrientTop = { 0,1,0 };
	emitter.CurveDistanceScaler = emitter.DopplerScaler = 1.0f;
	// チャンネル数設定
	// 1チャンネルを超える場合は、エミッタの ChannelRadius と pChannelAzimuths 値も設定する必要がある
	emitter.ChannelCount = SOURCE_CHANNELS;
	// ChannelRadius は文字通りチャンネルの半径
	// 1チャンネル以上のエミッターでのマトリックス計算に使用される
	//	設定する数値は 0.0f 以上でなければならない
	emitter.ChannelRadius = 20.0f;
	// pChannelAzimuths はチャンネルの方位配列
	//	1チャンネル以上のエミッターでのみ設定する必要がある
	//	上面方位ベクトルと直交する面の前面方位ベクトルに対して
	// チャンネル半径に沿ったラジアン単位で表される各チャンネル位置を含む
	//	またはLFEチャンネルを指定するX3DAUDIO_2PIは
	// 少なくともChannelCount要素を持っている必要があります
	//	設定する数値は 0.0f～X3DAUDIO_2PI の範囲内でなければならない
	emitter.pChannelAzimuths = new FLOAT32(X3DAUDIO_PI);
	//volumeCurve = X3DAudioDefault_LinearCurve;
	//emitter.pVolumeCurve = &volumeCurve;
	//LFECurve = X3DAudioDefault_LinearCurve;
	//emitter.pLFECurve = &LFECurve;
	//LPFDirectCurve = X3DAudioDefault_LinearCurve;
	//emitter.pLPFDirectCurve = &LPFDirectCurve;
	//LPFReverbCurve = X3DAudioDefault_LinearCurve;
	//emitter.pLPFReverbCurve = &LPFReverbCurve;
	//ReverbCurve = X3DAudioDefault_LinearCurve;
	//emitter.pReverbCurve = &ReverbCurve;
	cone = X3DAudioDefault_DirectionalCone;
	emitter.pCone = &cone;

	IXAudio2MasteringVoice* masteringVoice = AudioManager::Instance()->GetMasteringVoice();
	masteringVoice->GetVoiceDetails(&voiceDetails);
	//sourceVoice->GetVoiceDetails(&voiceDetails);
	FLOAT32* matrix = new FLOAT32[voiceDetails.InputChannels];
	DSPSettings.SrcChannelCount = SOURCE_CHANNELS;
	DSPSettings.DstChannelCount = voiceDetails.InputChannels;
	DSPSettings.pMatrixCoefficients = matrix;
}

void AudioPlayer::Update()
{
	// SouuceVoiceが初期化されていなければ何もしない
	if (sourceVoice == nullptr)
	{
		return;
	}
	// 固定FPSの値(仮)外部から直接設定するのもあり
	constexpr float FPS = 60.0f;

	// 再生中でなければ何もしない
	if (IsPlaying() == false)
	{
		return;
	}

	// フェードのフラグに応じてボリュームを変更する
	if (isFadeIn)
	{
		// フェードイン処理
		volume += AudioManager::MaxVolume / ( FPS * fadeTime );

		// ボリュームが範囲内になるように補正する
		if (volume > AudioManager::MaxVolume)
		{
			volume = AudioManager::MaxVolume;
			isFadeIn = false;
		}
		sourceVoice->SetVolume(volume);
	}
	else if (isFadeOut)
	{
		// フェードアウト処理
		volume -= AudioManager::MaxVolume / ( FPS * fadeTime );

		// ボリュームが範囲内になるように補正する
		if (volume < 0.0f)
		{
			volume = 0.0f;
			sourceVoice->Stop();
			isFadeOut = false;
		}
		sourceVoice->SetVolume(volume);
	}
	emitter.Position = transform->position;

	AudioManager* audio = AudioManager::Instance();
	// EmitterとListenerの位置関係などから音の聞こえ方などを計算する
	// 正しい計算結果を求める場合、EmitterもしくはListenerのパラメーターが変更されるたびに
	// X3DAudioCalculate関数を1度呼び出す必要がある
	X3DAudioCalculate(*audio->Get3DAudioHandle(), audio->Get3DAudioListener(), &emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER |
		X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
		&DSPSettings);
	sourceVoice->SetFrequencyRatio(DSPSettings.DopplerFactor);
	sourceVoice->SetOutputMatrix(submixVoice, SOURCE_CHANNELS, voiceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
	//sourceVoice->SetOutputMatrix(audio->GetMasteringVoice(), SOURCE_CHANNELS, voiceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
	//sourceVoice->SetOutputMatrix(submixVoice, SOURCE_CHANNELS, voiceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
	//sourceVoice->SetOutputMatrix(submixVoice, SOURCE_CHANNELS, voiceDetails.InputChannels, &DSPSettings.ReverbLevel);
	//XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
	//sourceVoice->SetFilterParameters(&FilterParameters);
}

void AudioPlayer::Play(audio::BGM index)
{
	Play(AudioManager::Instance()->GetBGMData(index), false, static_cast<int>( index ));
}

void AudioPlayer::Play(audio::SE index)
{
	Play(AudioManager::Instance()->GetSEData(index), false, static_cast<int>( index ));
}

// 再生
void AudioPlayer::Play(const weak_ptr<AudioData> data, bool loop, int index, audio::Fade type, float fadeTime)
{
	// SouuceVoiceが初期化されていなければ何もしない
	if (sourceVoice == nullptr)
	{
		Logger::Print("SourceVoiceが初期化されていません");
		return;
	}
	// 音声データが開放されていたら何もしない
	if (!data.lock()) return;
	// フェード設定がされていない場合は現在再生中の音声を止める
	if (type == audio::Fade::None) Stop();

	// ソースボイスにデータを送信
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = data.lock()->GetAudioBytes();
	buffer.pAudioData = data.lock()->GetAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	sourceVoice->SubmitSourceBuffer(&buffer);

	// 再生開始
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// フェード処理に合わせて音量を設定する
	SetFade(type, fadeTime);

	playingAudioIndex = index;
}

// 停止
void AudioPlayer::Stop()
{
	// SouuceVoiceが初期化されていなければ何もしない
	if (sourceVoice == nullptr)
	{
		Logger::Print("SourceVoiceが初期化されていません");
		return;
	}
	// 再生を停止した後にバッファをクリアする
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
	playingAudioIndex = -1;
}

// 再生中チェック
bool AudioPlayer::IsPlaying()
{
	// SouuceVoiceが初期化されていなければ何もしない
	if (sourceVoice == nullptr)
	{
		Logger::Print("SourceVoiceが初期化されていません");
		return false;
	}
	// ステートからバッファのキューの数を確認し、0以上であればtrueを返す
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
	return state.BuffersQueued != 0 ? true : false;
}

void AudioPlayer::SetFade(audio::Fade type, float fadeTime)
{
	// SouuceVoiceが初期化されていなければ何もしない
	if (sourceVoice == nullptr)
	{
		Logger::Print("SourceVoiceが初期化されていません");
		return;
	}
	// フェードする時間が0秒以下なら何もしない
	if (fadeTime < 0.0f)return;

	// 現在フェード処理中なら何もしない
	if (IsFading())return;

	// フェードの種類に応じてフラグを立てたり、
	// ボリュームを設定もしく取得する
	if (type == audio::Fade::None)
	{
		sourceVoice->SetVolume(AudioManager::MaxVolume);
		return;
	}
	else if (type == audio::Fade::In)
	{
		sourceVoice->SetVolume(0.0f);
		volume = 0.0f;
		isFadeIn = true;
	}
	else if (type == audio::Fade::Out)
	{
		sourceVoice->GetVolume(&volume);
		isFadeOut = true;
	}
	this->fadeTime = fadeTime;
}

void AudioPlayer::RenderDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("AudioPlayer", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Emitter", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Front", &emitter.OrientFront.x);
			ImGui::InputFloat3("Position", &emitter.Position.x);
			ImGui::InputFloat3("Velocity", &emitter.Velocity.x);
		}
	}
	ImGui::End();
}
