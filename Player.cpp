#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
void Player::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	input_ = Input::GetInstance();
	modelSize_ = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	Vector3 modelCenter = ModelManager::GetInstance()->GetModelCenter(modelHandle_);
	collider_.Initialize(&worldTransform_, name,modelSize_, modelCenter);
	worldTransform_.translation_ = { 0.0f,modelSize_.y / 2.0f,-7.0f };
	modelWorldTransform_.Initialize();
	modelWorldTransform_.SetParent(&worldTransform_);
	//modelの中心からmodelの高さの半分したにmodelWorldTransformを配置
	modelWorldTransform_.translation_ = { 0.0f, -modelSize_.y / 2.0f,0.0f };

	velocisity_ = { 0.0f,0.0f,0.0f };
	acceleration_ = { 0.0f,-0.05f,0.0f };
}

void Player::Update()
{

	Vector3 move = {0.0f,0.0f,0.0f};

	if (input_->PushKey(DIK_A)) {
		move.x -= 0.3f;
	}
	if (input_->PushKey(DIK_D)) {
		move.x += 0.3f;
	}
	if (input_->PushKey(DIK_S)) {
		move.z -= 0.3f;
	}
	if (input_->PushKey(DIK_W)) {
		move.z += 0.3f;
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		velocisity_.y = 1.0f;
	}

	worldTransform_.translation_ += move;
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, modelSize_.y / 2.0f, FLT_MAX);
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -11.0f + modelSize_.z / 2.0f, FLT_MAX);

#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	//ImGui::DragFloat3("rotation", &worldTransform_.quaternion_.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::End();
#endif

	collider_.AdjustmentScale();
	velocisity_.y = clamp(velocisity_.y, -0.5f, 200.0f);
	velocisity_ += acceleration_;
	worldTransform_.translation_ += velocisity_;
	worldTransform_.Update();
	modelWorldTransform_.Update();
}

void Player::Collision(Collider& otherCollider)
{
	Vector3 pushBackVector;
	if (collider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
		modelWorldTransform_.Update();
	}

}

void Player::Draw() {
	collider_.Draw();
	GameObject::Draw(modelWorldTransform_,{1.0f,1.0f,1.0f});
}