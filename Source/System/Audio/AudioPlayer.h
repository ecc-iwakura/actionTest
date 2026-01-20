#pragma once
#include "AudioCallback.h"
#include "AudioData.h"
#include "Component/Component.h"
#include "Macro/Macro.h"
#include <x3daudio.h>

// オーディオソース
class AudioPlayer :public Component
{
public:
	enum class Fade;
	AudioPlayer() {}
	~AudioPlayer();

	void VoiceRelease();
	void CreateSourceVoice(IXAudio2SubmixVoice* submixVoice);

	// 再生
	void Play(audio::BGM index);
	void Play(audio::SE index);
	void Play(const std::weak_ptr<AudioData> data, bool loop, int index, audio::Fade type = audio::Fade::None, float fadeTime = 0.0f);

	// 停止
	void Stop();

	// 再生中チェック
	bool IsPlaying();

	// 再生中の曲のインデックス番号を取得
	int GetPlayingIndex() const { return playingAudioIndex; }

	// 更新処理
	// フェードと3Dサウンド関連処理の更新を行う
	void Update();

	void RenderDebugGUI();

	// フェード設定関数(秒数をfloatで設定)
	// フェード処理中に呼び出しても何もしない想定
	void SetFade(audio::Fade type, float fadeTime);

	// フェード処理チェック
	bool IsFading() const { return isFadeIn || isFadeOut ? true : false; }

	IXAudio2SourceVoice* GetSourceVoice() { return sourceVoice; }
	X3DAUDIO_EMITTER* GetAudioEmitter() { return &emitter; }
	X3DAUDIO_DSP_SETTINGS* GetDSPSettings() { return &DSPSettings; }
	XAUDIO2_VOICE_DETAILS* GetVoiceDetails() { return &voiceDetails; }

private:
	IXAudio2SourceVoice* sourceVoice = nullptr;
	IXAudio2SubmixVoice* submixVoice = nullptr;
	AudioCallback voiceCallback;
	bool isFadeIn = false;
	bool isFadeOut = false;
	float fadeTime = 0.0f;
	float volume = 1.0f;
	int playingAudioIndex = -1;

	// 3Dサウンド用のエミッター
	// 主に使用するパラメータは以下の通り
	// Emitter.OrientFront ： 音源の前方向(要正規化)
	// Emitter.OrientTop ： 音源の上方向(要正規化)
	// Emitter.Position ： 音源の場所
	// Emitter.Velocity ： 音源の移動方向&速度
	X3DAUDIO_EMITTER emitter = {};
	X3DAUDIO_CONE cone = {};
	X3DAUDIO_DISTANCE_CURVE volumeCurve = {};
	X3DAUDIO_DISTANCE_CURVE LFECurve = {};
	X3DAUDIO_DISTANCE_CURVE LPFDirectCurve = {};
	X3DAUDIO_DISTANCE_CURVE LPFReverbCurve = {};
	X3DAUDIO_DISTANCE_CURVE ReverbCurve = {};
	float matrix[8] = {};
	X3DAUDIO_DSP_SETTINGS DSPSettings = {};
	XAUDIO2_VOICE_DETAILS voiceDetails = {};
};
