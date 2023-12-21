#include "Floor.h"
#include "ImGuiManager.h"
void Floor::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_,name,{20.0f, 1.0f, 1.0f});
}

void Floor::Update()
{
	collider_.AdjustmentScale();
	UpdateMatrix();
}

void Floor::Draw() {
	collider_.Draw();
	GameObject::Draw();
}