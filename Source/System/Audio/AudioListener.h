#pragma once
#include "Component/Component.h"
#include "Component/Movement.h"
#include <x3daudio.h>

class AudioListener : public Component
{
private:
	// インスタンスカウンター
	// 基本的にオーディオリスナーはシーン上に1つしか必要ないが、
	// コンポーネント指向の都合上2つ以上存在する可能性があるため警告ログを表示するように使用
	static int instanceCount;

	// 3Dサウンド用リスナーのポインター
	// 主に使用するパラメータは以下の通り
	// Listener.OrientFront ： リスナーの前方向(要正規化)
	// Listener.OrientTop ： リスナーの上方向(要正規化)
	// Listener.Position ： リスナーの位置
	// Listener.Velocity ： リスナーの移動方向&速度
	// Listener.pCone ： リスナーの音の指向性を設定
	X3DAUDIO_LISTENER* listener = nullptr;

	// Listenerが音を聞く指向性を与えるための要素
	// InnerAngleは内側コーン角度であり、0.0からX3DAUDIO_2PIの範囲である必要があります。
	// OuterAngleは外側コーン角度であり、InnerAngleからX3DAUDIO_2PIの範囲内でなければなりません。
	//	InnerVolumeはインナーコーン上または内のボリュームレベルを示し、行列計算にのみ使用されます。
	// OuterVolumeは外側コーン上または外側のボリュームレベルを示し、使用時には0.0から2.0の範囲内である必要があります。
	//	InnerLPFはインナーコーン上または内のLPF(ダイレクトパスとリバーブパスの両方)の係数を示し、
	// 使用時は0.0から1.0の範囲内である必要があります。
	// OuterLPFはアウターコーン上または外側のLPF係数を示し、LPFの計算にのみ使用されます。
	//	InnerReverbはインナーコーンのリバーブセンドレベルスケーラーを示し、
	// OuterReverbは外側コーンのリバーブセンドレベルスケーラーを示します。
	X3DAUDIO_CONE cone = {};

	// 移動コンポーネント
	Movement* movement = nullptr;

public:
	AudioListener();
	~AudioListener();
	void Start();
	void Update();
	void RenderDebugGUI();
};

