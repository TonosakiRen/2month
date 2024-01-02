#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
void Player::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	modelSize_ = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, name, modelHandle_);
	worldTransform_.translation_ = { 0.0f,modelSize_.y / 2.0f,-7.0f };
	modelWorldTransform_.Initialize();
	modelWorldTransform_.SetParent(&worldTransform_);
	//modelの中心からmodelの高さの半分下にmodelWorldTransformを配置
	modelWorldTransform_.translation_ = { 0.0f, -modelSize_.y / 2.0f,0.0f };

	worldTransform_.quaternion_ = MakeFromAngleAxis({ 0.0f,1.0f,0.0f }, -Radian(90.0f));

	velocisity_ = { 0.0f,0.0f,0.0f };
	acceleration_ = { 0.0f,-0.05f,0.0f };

	worldTransform_.Update();
	modelWorldTransform_.Update();
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
		velocisity_.y = 0.3f;
		size_t handle = audio_->SoundLoadWave("jump.wav");
		size_t jumpHandle = audio_->SoundPlayWave(handle);
		audio_->SetValume(jumpHandle, 0.1f);
	}

	worldTransform_.translation_ += move;
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, modelSize_.y / 2.0f, FLT_MAX);
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -8.0f + modelSize_.z / 2.0f, FLT_MAX);

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

void Player::DrawImGui() {
#ifdef _DEBUG
	static Vector3 rotate;
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	
	// 座標更新
	worldTransform_.Update();
	modelWorldTransform_.Update();
#endif // _DEBUG
}
