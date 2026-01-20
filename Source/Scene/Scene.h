#pragma once
#include <unordered_map>
#include <string>
#include "Component/Object.h"

class Scene
{
public:
	// シーン上に存在するオブジェクトのリスト
	ObjectList objectList;

	Scene(){}
	virtual ~Scene();
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

