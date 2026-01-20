#include "Stage.h"
#include "System/Common/Misc.h"

Stage::Stage()
{
	model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

Stage::~Stage()
{
	SafeDelete(model);
}

void Stage::Update()
{
}

void Stage::Render(ID3D11DeviceContext* dc, Shader* shader) const
{
	shader->Draw(dc, model);
}
