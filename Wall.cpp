#include "Wall.h"
#include "ModelManager.h"
void Wall::Initialize(Vector3 position)
{
	GameObject::Initialize("wall");
	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, "wall", { modelSize.x, modelSize.y, 2.0f }, { 0.0f, 5.0f, 0.0f });
	worldTransform_.translation_ = position;
}

void Wall::Update()
{
	collider_.AdjustmentScale();
	UpdateMatrix();
}

void Wall::Draw() {
	collider_.Draw();
	GameObject::Draw();
}