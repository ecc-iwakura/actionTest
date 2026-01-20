#pragma once

namespace audio
{
	// BGM管理用
	enum class BGM
	{
		Title,
		Game,
		Battle,
		Max,	// 終端
	};

	// SE管理用
	enum class SE
	{
		SE1,
		SE2,
		SE3,
		SE4,
		SE5,
		SE6,
		SE7,
		SE8,
		Max,	// 終端
	};

	// フェードの種類
	enum class Fade
	{
		None = -1,
		In = 0,
		Out = 1
	};

}