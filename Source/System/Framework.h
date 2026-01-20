#pragma once

#include "Common/HighResolutionTimer.h"
#include "Graphics/Graphics.h"
#include "Input/input.h"
#include "Scene/SceneManage.h"
#include "Audio/AudioManager.h"
#include <windows.h>

class Framework
{
public:
	Framework(HWND hWnd);
	~Framework();

private:
	void Update();
	void Render();

	void CalculateFrameStats();

public:
	int Run();
	LRESULT CALLBACK HandleMessage(HWND _hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	const HWND hWnd;
	HighResolutionTimer timer;
	AudioManager audio;
	Graphics graphics;
	Input input;
	SceneManager scene;
};

