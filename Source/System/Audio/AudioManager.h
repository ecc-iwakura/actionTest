#pragma once

#include "AudioData.h"
#include "AudioPlayer.h"
#include "Macro/Macro.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <x3daudio.h>
#include <xaudio2.h>

// サウンドシステム(XAudio2)及び音源データを管理するクラス
// 再生などの操作は以下のコンポーネントクラスを経由して行う想定
// リスナーの位置：AudioListenerクラス
// 音源の再生：AudioPlayerクラス
class AudioManager
{
public:
	// 最大音量
	static const float MaxVolume;

	// インスタンス取得
	static inline AudioManager* Instance() { return instance; }

	void PlayBGM(audio::BGM index, bool loop = true, bool crossFade = false, float fadeTime = 0.0f);
	void StopBGM();

	void PlaySE(audio::SE index);
	void StopSE(audio::SE index);
	void StopAllSE();

	// BGM読み込み
	// 読み込まれたBGMはBGMデータリストの末尾に追加される
	// 読み込みに成功するとTrue、失敗するとFalseを返す
	bool LoadBGM(std::string filename, audio::BGM index);
	bool LoadBGM(std::ifstream& file, audio::BGM index);

	// SE読み込み
	// 読み込みに成功するとTrue、失敗するとFalseを返す
	bool LoadSE(std::string filename, audio::SE index);
	bool LoadSE(std::ifstream& loadFile, audio::SE index);

	// 音量調整関連
	// マスター(全体)音量の調整
	void SetMasterVolume(const float vol);
	// BGMの音量調整
	void SetBGMVolume(const float vol);
	// SEの音量調整
	void SetSEVolume(const float vol);

	// 音声データ取得
	std::weak_ptr<AudioData> GetBGMData(audio::BGM index);
	std::weak_ptr<AudioData> GetSEData(audio::SE index);

private:
	// 暗号化されたファイルを復号化する関数
	// 暗号化されたデータを引数fileに渡すと復号化したデータに変更される
	void DecodeBuffer(std::ifstream& file);

private:
	static AudioManager* instance;

	// XAudioシステム
	IXAudio2* xAudio = nullptr;
	// マスタリングボイス
	IXAudio2MasteringVoice* masterVoice = nullptr;


	// 3Dサウンド用のリスナー
	// 主に使用するパラメータは以下の通り
	// Listener.OrientFront ： リスナーの前方向(要正規化)
	// Listener.OrientTop ： リスナーの上方向(要正規化)
	// Listener.Position ： リスナーの位置
	// Listener.Velocity ： リスナーの移動方向&速度
	X3DAUDIO_LISTENER listener;

	// 3Dサウンド用出力形式のチャンネルマスク
	DWORD channelMask;
	// 3Dサウンド用ハンドル
	X3DAUDIO_HANDLE x3DAudioHandle;


	// BGM再生用ボイス
	std::vector<std::shared_ptr<AudioPlayer>> bgmPlayer;
	// BGM用サブミックスボイス
	IXAudio2SubmixVoice* bgmSubmixVoice;
	// 同時再生可能数(BGM)
	static const int MaxBgmVoice;
	// BGMデータのリスト
	std::unordered_map<audio::BGM, std::shared_ptr<AudioData>> bgmDataArr;

	// SE再生用ボイス
	std::vector<std::shared_ptr<AudioPlayer>> sePlayer;
	// SE用サブミックスボイス
	IXAudio2SubmixVoice* seSubmixVoice;
	// 同時再生可能数(SE)
	static const int MaxSeVoice;
	// SEデータのリスト
	std::unordered_map<audio::SE, std::shared_ptr<AudioData>> seDataArr;

public:
	// XAudioシステム関連取得関数
	inline IXAudio2* GetXAudio2() { return xAudio; }
	inline IXAudio2MasteringVoice* GetMasteringVoice() { return masterVoice; }
	inline X3DAUDIO_LISTENER* Get3DAudioListener() { return &listener; }
	inline IXAudio2SubmixVoice* GetSESubmixVoice() { return seSubmixVoice; }
	inline IXAudio2SourceVoice* GetSourceVoice() { return sePlayer.begin()->get()->GetSourceVoice(); }
	inline X3DAUDIO_HANDLE* Get3DAudioHandle() { return &x3DAudioHandle; }

public:
	AudioManager();
	~AudioManager();

};

