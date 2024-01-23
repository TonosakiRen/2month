#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
void Player::Initialize(const std::string name)
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 体のモデルの初期化
	GameObject::Initialize(name);
	bodyModelSize_ = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	// 頭のモデルの初期化
	headModelHandle_ = ModelManager::GetInstance()->Load("playerHead");
	headModelSize_ = ModelManager::GetInstance()->GetModelSize(headModelHandle_);

	modelHeight_ = bodyModelSize_.y + headModelSize_.y;
	worldTransform_.translation_ = { 0.0f,modelHeight_ / 2.0f,-7.0f };

	//体のWorldTransform
	bodyWorldTransform_.Initialize();
	bodyWorldTransform_.SetParent(&worldTransform_);
	bodyWorldTransform_.translation_ = { 0.0f, -bodyModelSize_.y / 2.0f - (modelHeight_ / 2.0f - bodyModelSize_.y),0.0f };

	//体のコライダー
	bodyCollider_.Initialize(&bodyWorldTransform_, name, modelHandle_);

	worldTransform_.quaternion_ = MakeFromAngleAxis({ 0.0f,1.0f,0.0f }, -Radian(90.0f));

	jumpParam_.velocisity_ = { 0.0f,0.0f,0.0f };
	jumpParam_.acceleration_ = { 0.0f,-0.05f,0.0f };
	jumpParam_.isJumped_ = false;

	worldTransform_.Update();
	bodyWorldTransform_.Update();

	//頭のWorldTransform
	headWorldTransform_.Initialize();
	headWorldTransform_.SetParent(&worldTransform_);
	headWorldTransform_.translation_ = { 0.0f,-headModelSize_.y / 2.0f + bodyModelSize_.y - modelHeight_ / 2.0f + headModelSize_.y,0.0f };
	headCollider_.Initialize(&headWorldTransform_, "playerHead", headModelHandle_);

	headWorldTransform_.quaternion_ = worldTransform_.quaternion_;
	headWorldTransform_.Update();
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	DrawImGui();
	ImGui::End();
#endif

	Move();
	Jump();
	Attack();

	MoveLimit();


	
	UpdateTrans();
	InsertData();
}

void Player::Collision(Collider& otherCollider)
{
	Vector3 pushBackVector;
 	if (bodyCollider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
		bodyWorldTransform_.Update();
		headWorldTransform_.Update();
	}
	if (headCollider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
		bodyWorldTransform_.Update();
		headWorldTransform_.Update();
	}

}

void Player::Draw() {
	bodyCollider_.Draw();
	GameObject::PlayerDraw(bodyWorldTransform_,{1.0f,1.0f,1.0f,1.0f});
	
	headCollider_.Draw();
	GameObject::PlayerDraw(headWorldTransform_, headModelHandle_);
}

void Player::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &headWorldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	headWorldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &headWorldTransform_.translation_.x, 0.1f);
	
	// 座標更新
	worldTransform_.Update();
	bodyWorldTransform_.Update();
	headWorldTransform_.Update();
#endif // _DEBUG
}

void Player::CollisionProcess(const Vector3& pushBackVector) {
	worldTransform_.translation_ += pushBackVector;
	// 座標更新
	worldTransform_.Update();
	bodyWorldTransform_.Update();
	headWorldTransform_.Update();
	jumpParam_.isJumped_ = false;
}

void Player::Move() {
	Vector3 move = { 0.0f,0.0f,0.0f };

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
	if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
		worldTransform_.translation_ += move;
		worldTransform_.quaternion_ = MakeLookRotation(move);
	}
	
}

void Player::Jump() {
	if (input_->TriggerKey(DIK_SPACE) && !jumpParam_.isJumped_) {
		jumpParam_.velocisity_.y = 0.5f;
		size_t handle = audio_->SoundLoadWave("jump.wav");
		size_t jumpHandle = audio_->SoundPlayWave(handle);
		audio_->SetValume(jumpHandle, 0.1f);
		jumpParam_.isJumped_ = true;
	}
	jumpParam_.velocisity_.y = clamp(jumpParam_.velocisity_.y, -0.5f, 200.0f);
	jumpParam_.velocisity_ += jumpParam_.acceleration_;
	worldTransform_.translation_ += jumpParam_.velocisity_;
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_V) && !attackParam_.isAttacked) {
		attackParam_.phase = 0;
		attackParam_.isAttacked = true;
		attackParam_.id_++;
	}
	if (attackParam_.isAttacked) {
		switch (attackParam_.phase) {
		case 0: // 初期化
			headRotate.x = -30.0f;
			attackParam_.phase = 1;
			break;
		case 1:
			headRotate.x += 2.0f;
			if (headRotate.x >= 90.0f) {
				attackParam_.phase = 2;
			}
			break;
		case 2:
			headRotate.x = 0.0f;
			attackParam_.isAttacked = false;
			attackParam_.id_ = 0u;
			break;
		}
	}

	Vector3 handle = Vector3(Radian(headRotate.x), Radian(headRotate.y), Radian(headRotate.z));
	headWorldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	headWorldTransform_.Update();
}

void Player::MoveLimit() {
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, modelHeight_ / 2.0f - 1.0f, FLT_MAX);
	if (worldTransform_.translation_.y <= modelHeight_ / 2.0f - 1.0f) {
		jumpParam_.isJumped_ = false;
	}
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -10.5f + bodyModelSize_.z / 2.0f, FLT_MAX);

}

void Player::InsertData() {
	Matrix4x4 worldMat = worldTransform_.matWorld_;
	date_.position_ = Vector3(worldMat.m[3][0],worldMat.m[3][1],worldMat.m[3][2]); // playerのworldPosition
	date_.isAttack_ = attackParam_.isAttacked;
	date_.damage_ = attackParam_.damage_;
	date_.id = attackParam_.id_;
}

void Player::UpdateTrans() {
	bodyCollider_.AdjustmentScale();
	worldTransform_.Update();
	bodyWorldTransform_.Update();

	headCollider_.AdjustmentScale();
	headWorldTransform_.Update();
}
