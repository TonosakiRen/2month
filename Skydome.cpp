#include "Skydome.h"

void Skydome::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = false;
	material_.Update();
	worldTransform_.scale_ = worldTransform_.scale_ * 50.0f;
	worldTransform_.Update();
}

void Skydome::Update(const Vector3& translate)
{
	// playerに追従して天球を動かす
	worldTransform_.translation_ = translate;
	worldTransform_.translation_.y = 0.0f;
	
	worldTransform_.Update();
}