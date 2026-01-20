#pragma once

#include "Component/Component.h"
#include "System/Graphics/Model.h"
#include "System/Graphics/Shader.h"

class Stage:public Component
{
public:
	Stage();
	~Stage();

	void Update();

	void Render(ID3D11DeviceContext* dc, Shader* shader) const;

private:
	Model* model = nullptr;
};

