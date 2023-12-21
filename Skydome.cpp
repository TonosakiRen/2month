#include "Skydome.h"

void Skydome::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = false;
	material_.Update();
	worldTransform_.scale_ = worldTransform_.scale_ * 50.0f;
}

void Skydome::Update()
{
	worldTransform_.Update();
}