#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "./Game/Character/EnemyManager.h"

int32_t Player::hitShadowEnemyIndex_ = -1;
Vector3 Player::hitShadowEnemyPos_ = {0.0f,0.0f,0.0f};
Vector3 Player::playerPos_ = { 0.0f,0.0f,0.0f };
Collider* Player::hitCollider_ = nullptr;

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

	jumpParam_.velocity_ = { 0.0f,0.0f,0.0f };
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

	globalVariables_ = GlobalVariables::GetInstance();
	SetGlobalVariable();

	uint32_t hpHandle = TextureManager::Load("hp.png");
	hpSprite_.Initialize(hpHandle, { 180.0f,680.0f });
	hpSprite_.size_ = {285.0f,32.0f};
}

void Player::SetGlobalVariable()
{
	std::string name = "Player";
	globalVariables_->CreateGroup(name);
	globalVariables_->AddItem(name, "jumpPower_", jumpPower_);
	globalVariables_->AddItem(name, "speed_", speed_);
	globalVariables_->AddItem(name, "attackSpeed_", attackSpeed_);
	globalVariables_->AddItem(name, "backHeadSpeed_", backHeadSpeed_);
	globalVariables_->AddItem(name, "attackReadySpeed_", attackReadySpeed_);
	globalVariables_->AddItem(name, "knockBackPowerX_", knockBackPowerX_);
	globalVariables_->AddItem(name, "knockBackPowerY_", knockBackPowerY_);
	globalVariables_->AddItem(name, "damage_", damage_);
	globalVariables_->LoadFile(name);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable()
{
	std::string name = "Player";
	jumpPower_ = globalVariables_->GetFloatValue(name, "jumpPower_");
	speed_ = globalVariables_->GetFloatValue(name, "speed_");
	attackSpeed_ = globalVariables_->GetFloatValue(name, "attackSpeed_");
	backHeadSpeed_ = globalVariables_->GetFloatValue(name, "backHeadSpeed_");
	attackReadySpeed_ = globalVariables_->GetFloatValue(name, "attackReadySpeed_");
	knockBackPowerX_ = globalVariables_->GetFloatValue(name, "knockBackPowerX_");
	knockBackPowerY_ = globalVariables_->GetFloatValue(name, "knockBackPowerY_");
	damage_ = globalVariables_->GetIntValue(name, "damage_");
}

void Player::Update()
{
	ApplyGlobalVariable();
#ifdef _DEBUG
	ImGui::Begin("Player");
	DrawImGui();
	ImGui::DragFloat2("hpBar", &hpSprite_.position_.x);
	ImGui::DragFloat2("hpBarSize", &hpSprite_.size_.x);
	ImGui::End();
#endif

	if (!isKnockBack_) {
		Move();
		Jump();
	}
	Attack();

	jumpParam_.velocity_.y = clamp(jumpParam_.velocity_.y, -0.5f, 200.0f);
	jumpParam_.velocity_ += jumpParam_.acceleration_;
	worldTransform_.translation_ += jumpParam_.velocity_;

	MoveLimit();
	
	UpdateTrans();
	InsertData();
	UIUpdate();
	playerPos_ = MakeTranslation(worldTransform_.matWorld_);
}

void Player::UIUpdate()
{
	hp_;
	hp_ = clamp(hp_, 0, maxHp_);
	//285
	hpSprite_.size_.x = 285.0f * (hp_ / float(maxHp_));
}

void Player::Draw() {
	bodyCollider_.Draw();
	GameObject::PlayerDraw(bodyWorldTransform_,{1.0f,1.0f,1.0f,1.0f});
	
	headCollider_.Draw();
	GameObject::PlayerDraw(headWorldTransform_, headModelHandle_);
}

void Player::EnemyShadowCollision()
{
	if (hitShadowEnemyIndex_ != -1) {
		if (attackParam_.id_ != 1) {
			if (isKnockBack_ == false) {
				isKnockBack_ = true;
				Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - hitShadowEnemyPos_;
				knockBackDirection_ = Normalize(Vector3{ vec.x,0.0f,0.0f });
				jumpParam_.velocity_ = {knockBackDirection_.x* knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z* knockBackPowerX_ };
				hp_ -= damage_;
			}
		}
		else if (hitCollider_->GetName() == "CannonBullet") {
			if (isKnockBack_ == false) {
				isKnockBack_ = true;
				Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - MakeTranslation(hitCollider_->worldTransform_.matWorld_);
				knockBackDirection_ = Normalize(vec);
				jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
				hp_ -= damage_;
				if (headRotate.x < 0.0f) {
					attackParam_.phase = 3;
				}
				else {
					attackParam_.phase = 2;
				}
			}
		}
	}
}

void Player::EnemyCollision()
{
	if (hitCollider_) {
		if (attackParam_.id_ != 1) {
			if (isKnockBack_ == false) {
				isKnockBack_ = true;
				Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - MakeTranslation(hitCollider_->worldTransform_.matWorld_);
				knockBackDirection_ = Normalize(vec);
				jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
				hp_ -= damage_;
			}
		}
		else if (hitCollider_->GetName() == "CannonBullet") {
			if (isKnockBack_ == false) {
				isKnockBack_ = true;
				Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - MakeTranslation(hitCollider_->worldTransform_.matWorld_);
				knockBackDirection_ = Normalize(vec);
				jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
				hp_ -= damage_;
				if (headRotate.x < 0.0f) {
					attackParam_.phase = 3;
				}
				else {
					attackParam_.phase = 2;
				}
			}
		}
	}
}

void Player::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	//worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	
	// 座標更新
	worldTransform_.Update();
	bodyWorldTransform_.Update();
	headWorldTransform_.Update();
#endif // _DEBUG
}

void Player::DrawUI()
{
	hpSprite_.Draw();
}

void Player::CollisionProcess(const Vector3& pushBackVector) {
	pushBackVector_ = pushBackVector;
	worldTransform_.translation_ += pushBackVector;
	// 座標更新
	worldTransform_.Update();
	bodyWorldTransform_.Update();
	headWorldTransform_.Update();
	if (Normalize(pushBackVector).y == 1.0f) {
		jumpParam_.isJumped_ = false;
		isKnockBack_ = false;
		jumpParam_.velocity_.x = 0.0f;
		jumpParam_.velocity_.z = 0.0f;
	}
}

void Player::Move() {
	Vector3 direction = { 0.0f,0.0f,0.0f };

	if (input_->GetIsGamePadConnect()) {
		// 移動量
		direction = {
			input_->GetLStick().x / SHRT_MAX, 0.0f,
			input_->GetLStick().y / SHRT_MAX };
	}
	else {
		if (input_->PushKey(DIK_A)) {
			direction.x -= 1.0f;
		}
		if (input_->PushKey(DIK_D)) {
			direction.x += 1.0f;
		}
		if (input_->PushKey(DIK_S)) {
			direction.z -= 1.0f;
		}
		if (input_->PushKey(DIK_W)) {
			direction.z += 1.0f;
		}
	}

	Vector3 move = Normalize(direction) * speed_;

	if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f) {
		worldTransform_.translation_ += move;
		worldTransform_.quaternion_ = Slerp(0.2f, worldTransform_.quaternion_,MakeLookRotation(direction));
	}
	
}

void Player::Jump() {
	if ((input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !jumpParam_.isJumped_) {
		jumpParam_.velocity_.y = jumpPower_;
		size_t handle = audio_->SoundLoadWave("jump.wav");
		size_t jumpHandle = audio_->SoundPlayWave(handle);
		audio_->SetValume(jumpHandle, 0.1f);
		jumpParam_.isJumped_ = true;
	}

}

void Player::Attack() {
	if ((input_->TriggerKey(DIK_V) || input_->TriggerButton(XINPUT_GAMEPAD_X)) && !attackParam_.isAttacked && !isKnockBack_) {
		attackParam_.phase = 0;
		attackParam_.isAttacked = true;
	}
	if (attackParam_.isAttacked) {
		switch (attackParam_.phase) {
		case 0: 
			//頭が後ろに下がる
			headRotate.x -= attackReadySpeed_;
			if (isKnockBack_) {
				attackParam_.phase = 3;
			}
			if (headRotate.x <= -30.0f) {
				headRotate.x = -30.0f;
				attackParam_.phase = 1;
			}
			break;
		case 1:
			//頭でattackする
			headRotate.x += attackSpeed_;
			attackParam_.id_ = 1;
			if (headRotate.x >= 90.0f) {
				attackParam_.phase = 2;
			}
			break;
		case 2:
			//頭で元の位置に戻る
			headRotate.x -= backHeadSpeed_;
			attackParam_.id_ = 0u;
			if (headRotate.x <= 0.0f) {
				headRotate.x = 0.0f;
				attackParam_.isAttacked = false;
			}
			break;
		case 3:
			//攻撃準備中にノックバックしてしまって頭で元の位置に戻る
			headRotate.x += backHeadSpeed_;
			attackParam_.id_ = 0u;
			if (headRotate.x >= 0.0f) {
				headRotate.x = 0.0f;
				attackParam_.isAttacked = false;
			}
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
