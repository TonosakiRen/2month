#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "./Game/Character/EnemyManager.h"
#include "Compute.h"
#include "GameScene.h"
#include "Easing.h"

int32_t Player::hitShadowEnemyIndex_ = -1;
Vector3 Player::hitShadowEnemyPos_ = {0.0f,0.0f,0.0f};
Vector3 Player::playerPos_ = { 0.0f,0.0f,0.0f };
Collider* Player::hitCollider_ = nullptr;
Player::HitReaction Player::hitReaction_ = Player::knockBack;


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
	MUTEKITime_ = -1;

	uint32_t hpHandle = TextureManager::Load("hp.png");
	hpSprite_.Initialize(hpHandle, { 230.0f,1000.0f });
	hpSprite_.size_ = {260.0f,32.0f};

	HpbarSprite_.Initialize(TextureManager::Load("Hpbar.png"),{196.0f,979.0f});
	HpbarSprite_.size_ = { 340.0f,100.0f };

	isBlink_ = false;
	isDash_ = false;

	shadowHitParticle_.Initialize({ -0.5f,0.5f,0.0f }, { 0.5f,1.0f,0.0f });
	hitParticle_.Initialize({ -1.0f,-1.0f,0.0f }, { 1.0f,1.0f,0.0f });

	coinNum_ = 0;

	
	deadFrame_ = 0;
	deadParticle_.Initialize({-0.5f,0.5f,-0.5f},{0.5f,1.0f,0.5f});
	deadParticleHandle_ = TextureManager::Load("blackEffect.png");

	deadParticle_.emitterWorldTransform_.SetParent(&worldTransform_);
	deadParticle_.emitterWorldTransform_.translation_ = { 0.0f,-2.0f,0.0f };
	deadParticle_.EmitNum_ = 1;

	isDead_ = false;

	isEndDeadAnimation_ = false;

	deadT = 0.0f;

	isClear_ = false;

	isEndClearAnimation = false;

	ty_.Initialize("goal");
	ty_.worldTransform_.translation_ = { 0.0f,30.0f,0.0f };
	ty_.worldTransform_.scale_ = { 3.0f,3.0f,3.0f };

	date_.attackIndex = -1;

	coinUI_.Initialize(TextureManager::Load("coin.png"),{91.0f,83.0f});
	coinUI_.size_ = { 131.0f,110.0f };

	one_.Initialize(TextureManager::Load("dekisokonai36.png"), {248.0f,92.0f});
	ten_.Initialize(TextureManager::Load("dekisokonai36.png"), {190.0f,92.0f});
	one_.size_ = { 50.0f,50.0f };
	ten_.size_ = { 50.0f,50.0f };
	one_.texSize_ = { 36.0f,36.0f };
	one_.texBase_ = { 0.0f,0.0f };
	ten_.texSize_ = { 36.0f,36.0f };
	ten_.texBase_ = { 0.0f,0.0f };
}

void Player::SetGlobalVariable()
{
	std::string name = "Player";
	globalVariables_->CreateGroup(name);
	globalVariables_->AddItem(name, "jumpPower_", jumpPower_);
	globalVariables_->AddItem(name, "speed_", speed_);
	globalVariables_->AddItem(name, "attackSpeed_", attackSpeed_);
	globalVariables_->AddItem(name, "kmaxattackCoolTime_", kmaxattackCoolTime_);
	globalVariables_->AddItem(name, "backHeadSpeed_", backHeadSpeed_);
	globalVariables_->AddItem(name, "attackReadySpeed_", attackReadySpeed_);
	globalVariables_->AddItem(name, "knockBackPowerX_", knockBackPowerX_);
	globalVariables_->AddItem(name, "knockBackPowerY_", knockBackPowerY_);
	globalVariables_->AddItem(name, "damage_", damage_);
	globalVariables_->AddItem(name, "maxMUTEKITime_", maxMUTEKITime_);
	globalVariables_->AddItem(name, "heal_", heal_);
	globalVariables_->AddItem(name, "blinkingTime_", blinkingTime_);
	globalVariables_->AddItem(name, "attackDamage", static_cast<int>(attackParam_.damage_));
	globalVariables_->AddItem(name, "dashSpeed_", dashSpeed_);
	globalVariables_->LoadFile(name);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable()
{
	std::string name = "Player";
	jumpPower_ = globalVariables_->GetFloatValue(name, "jumpPower_");
	speed_ = globalVariables_->GetFloatValue(name, "speed_");
	attackSpeed_ = globalVariables_->GetFloatValue(name, "attackSpeed_");
	kmaxattackCoolTime_ = globalVariables_->GetFloatValue(name, "kmaxattackCoolTime_");
	backHeadSpeed_ = globalVariables_->GetFloatValue(name, "backHeadSpeed_");
	attackReadySpeed_ = globalVariables_->GetFloatValue(name, "attackReadySpeed_");
	knockBackPowerX_ = globalVariables_->GetFloatValue(name, "knockBackPowerX_");
	knockBackPowerY_ = globalVariables_->GetFloatValue(name, "knockBackPowerY_");
	maxMUTEKITime_ = globalVariables_->GetIntValue(name, "maxMUTEKITime_");
	heal_ = globalVariables_->GetIntValue(name, "heal_");
	blinkingTime_ = globalVariables_->GetIntValue(name, "blinkingTime_");
	dashSpeed_ = globalVariables_->GetFloatValue(name, "dashSpeed_");
	attackParam_.damage_ = globalVariables_->GetIntValue(name, "attackDamage");
}

void Player::Update()
{
	ApplyGlobalVariable();
#ifdef _DEBUG
	ImGui::Begin("Player");
	DrawImGui();
	ImGui::DragFloat2("hpBar", &one_.position_.x);
	ImGui::DragFloat2("hpBarSize", &one_.size_.x);
	ImGui::DragFloat2("tenHpBar", &ten_.position_.x);
	ImGui::DragFloat2("tenHpBarSize", &ten_.size_.x);
	if (input_->TriggerKey(DIK_K)) {
		hp_ = 0;
	}
	ImGui::End();
#endif

	DeadUpdate();
	ClearUpdate();
	attackParam_.id_ = 0;
	if (isDead_ == false && isClear_ == false) {
		if (MUTEKITime_ > -1) {
			MUTEKITime_--;
		}

		if (!isKnockBack_) {
			if (!isDash_) {
				Move();
				Jump();
			}
		}
		Attack();
	}
	
	jumpParam_.velocity_.y = clamp(jumpParam_.velocity_.y, -0.5f, 200.0f);
	jumpParam_.velocity_ += jumpParam_.acceleration_;
	
	if (!isDash_) {
		worldTransform_.translation_ += jumpParam_.velocity_;
	}
	else {
		worldTransform_.translation_.x += jumpParam_.velocity_.x;
	}
	


	MoveLimit();
	
	UpdateTrans();
	InsertData();
	UIUpdate();
	shadowHitParticle_.Update();
	hitParticle_.Update();
	playerPos_ = MakeTranslation(worldTransform_.matWorld_);
	deadParticle_.Update();
}

void Player::UIUpdate()
{
	hp_;
	hp_ = clamp(hp_, 0, maxHp_);
	//285
	hpSprite_.size_.x = 260.0f * (hp_ / float(maxHp_));
	hpSprite_.size_.x = clamp(hpSprite_.size_.x, 0.0f, 265.0f);
	hpSprite_.position_.x = 230.0f - (maxHp_ - hp_) * 1.5f;


	one_.texBase_.x = 36 * (coinNum_ % 10);
	ten_.texBase_.x = 36 * (coinNum_ / 10);
}

void Player::Draw() {
	if (MUTEKITime_ > -1) {
		if (MUTEKITime_ % blinkingTime_ == 0) {
			if (isBlink_ == true) {
				isBlink_ = false;
			}
			else {
				isBlink_ = true;
			}
		}
	}
	else {
		isBlink_ = false;
	}

	if (isBlink_ == false) {
		bodyCollider_.Draw();
		GameObject::PlayerDraw(bodyWorldTransform_, { 1.0f,1.0f,1.0f,1.0f });

		headCollider_.Draw();
		GameObject::PlayerDraw(headWorldTransform_, headModelHandle_);
	}
	else {
		bodyCollider_.Draw();
		GameObject::PlayerDraw(bodyWorldTransform_, { 1.0f,0.0f,0.0f,1.0f });

		headCollider_.Draw();
		GameObject::PlayerDraw(headWorldTransform_, headModelHandle_, { 0.2f,0.0f,0.0f,1.0f });
	}
	if (isClear_ == true) {
		ty_.Draw({1.0f,1.0f,0.0f,1.0f});
	}

}

void Player::EnemyShadowCollision()
{

	const int hitStopFrame = 2;

	if (hitShadowEnemyIndex_ != -1 ) {
		//もしhitReactionがhealじゃなかったら
		if (hitReaction_ != heal && hitReaction_ != coin) {

			Vector3* aa = static_cast<Vector3*>(Compute::GetPosData());
			shadowHitParticle_.emitterWorldTransform_.translation_ = aa[0];
			shadowHitParticle_.emitterWorldTransform_.translation_.z -= 1.0f;

			if (hitReaction_ == damage) {
				if (isKnockBack_ == false && MUTEKITime_ <= -1) {
					
					isKnockBack_ = true;
					Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - hitShadowEnemyPos_;
					knockBackDirection_ = Normalize(Vector3{ vec.x,0.0f,0.0f });
					jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
					
					GameScene::SetHitStop(hitStopFrame);
					shadowHitParticle_.particle_->material_.color_ = { 1.0f, 0.2f, 1.0f, 1.0f };
					shadowHitParticle_.SetIsEmit(true);
					hp_ -= damage_;
					MUTEKITime_ = maxMUTEKITime_;

					size_t handle = audio_->SoundLoadWave("hit.wav");
					size_t hitHandle = audio_->SoundPlayWave(handle);
					audio_->SetValume(hitHandle, 1.0f);

					attackParam_.phase = 4;
				}
			}else
			if (attackParam_.id_ != 1) {
				if (isKnockBack_ == false && MUTEKITime_ <= -1) {
					if (hitReaction_ == knockBack) {
						isKnockBack_ = true;
						Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - hitShadowEnemyPos_;
						knockBackDirection_ = Normalize(Vector3{ vec.x,0.0f,0.0f });
						jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
					}
					GameScene::SetHitStop(hitStopFrame);
					shadowHitParticle_.particle_->material_.color_ = { 1.0f, 0.2f, 1.0f, 1.0f };
					shadowHitParticle_.SetIsEmit(true);
					hp_ -= damage_;
					MUTEKITime_ = maxMUTEKITime_;

					size_t handle = audio_->SoundLoadWave("hit.wav");
					size_t hitHandle = audio_->SoundPlayWave(handle);
					audio_->SetValume(hitHandle, 1.0f);
				}
			}
			else {
				GameScene::SetHitStop(3);
				shadowHitParticle_.SetIsEmit(true);
				shadowHitParticle_.particle_->material_.color_ = { 1.0f, 0.7f, 0.1f,1.0f };
			}
		}//healだったら
		else if (hitReaction_ == heal){
			hp_ += heal_;
			hp_ = clamp(hp_, -100, maxHp_);
			size_t handle = audio_->SoundLoadWave("heal.wav");
			size_t healHandle = audio_->SoundPlayWave(handle);
			audio_->SetValume(healHandle, 1.0f);
		}
		else {
			coinNum_++;
		}
		
	}
}

void Player::EnemyCollision()
{
	const int hitStopFrame = 2;

	if (hitCollider_ ) {
		if (hitReaction_ != heal && hitReaction_ != coin) {
			Vector3 hitPos = MakeTranslation(hitCollider_->worldTransform_.matWorld_);
			hitParticle_.emitterWorldTransform_.translation_ = hitPos + ((MakeTranslation(worldTransform_.matWorld_) - hitPos) / 2.0f);
			if (hitReaction_ == damage) {
				if (isKnockBack_ == false && MUTEKITime_ <= -1) {

					isKnockBack_ = true;
					Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - hitShadowEnemyPos_;
					knockBackDirection_ = Normalize(Vector3{ vec.x,0.0f,0.0f });
					jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };

					hitParticle_.particleBox_->material_.color_ = { 1.0f, 0.2f, 1.0f, 1.0f };
					GameScene::SetHitStop(hitStopFrame);
					hitParticle_.SetIsEmit(true);
					hp_ -= damage_;
					MUTEKITime_ = maxMUTEKITime_;

					size_t handle = audio_->SoundLoadWave("hit.wav");
					size_t hitHandle = audio_->SoundPlayWave(handle);
					audio_->SetValume(hitHandle, 1.0f);

					attackParam_.phase = 4;
				}
			}else
			if (attackParam_.id_ != 1) {
				if (isKnockBack_ == false && MUTEKITime_ <= -1) {
					isKnockBack_ = true;
					Vector3 vec = MakeTranslation(worldTransform_.matWorld_) - MakeTranslation(hitCollider_->worldTransform_.matWorld_);
					knockBackDirection_ = Normalize(vec);
					jumpParam_.velocity_ = { knockBackDirection_.x * knockBackPowerX_, 1.0f * knockBackPowerY_, knockBackDirection_.z * knockBackPowerX_ };
					
					hitParticle_.particleBox_->material_.color_ = { 1.0f, 0.2f, 1.0f, 1.0f };
					GameScene::SetHitStop(hitStopFrame);
					hitParticle_.SetIsEmit(true);
					hp_ -= damage_;
					MUTEKITime_ = maxMUTEKITime_;

					size_t handle = audio_->SoundLoadWave("hit.wav");
					size_t hitHandle = audio_->SoundPlayWave(handle);
					audio_->SetValume(hitHandle, 1.0f);
				}
			}
			else {

				hitParticle_.particleBox_->material_.color_ = { 1.0f, 0.7f, 0.1f, 1.0f };
				hitParticle_.SetIsEmit(true);
				GameScene::SetHitStop(hitStopFrame);
			}
		}
		else if (hitReaction_ == heal){
			hp_ += heal_;
			hp_ = clamp(hp_, -100, maxHp_);

			size_t handle = audio_->SoundLoadWave("heal.wav");
			size_t healHandle = audio_->SoundPlayWave(handle);
			audio_->SetValume(healHandle, 1.0f);
		}
		else {
			coinNum_++;
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

void Player::DrawUI() {
	hpSprite_.Draw();
	HpbarSprite_.Draw();
	coinUI_.Draw();
	one_.Draw();
	ten_.Draw();
}

void Player::CollisionProcess(const Vector3& pushBackVector) {
   

	pushBackVector_ = pushBackVector;
	worldTransform_.translation_ += pushBackVector;
	// 座標更新
	worldTransform_.Update();
	bodyWorldTransform_.Update();
	headWorldTransform_.Update();
	if (Normalize(pushBackVector).y >= 0.8f) {
		jumpParam_.isJumped_ = false;
		isKnockBack_ = false;
		jumpParam_.velocity_.x = 0.0f;
		jumpParam_.velocity_.y = 0.0f;
		jumpParam_.velocity_.z = 0.0f;
	}
}

void Player::Move() {

	if (!isDash_) {
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
			inputQuaternion_ = MakeLookRotation(direction);
			worldTransform_.quaternion_ = Slerp(0.2f, worldTransform_.quaternion_, inputQuaternion_);
			deadParticle_.SetIsEmit(true);
		}
		else {
			deadParticle_.SetIsEmit(false);
		}
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
	if (attackCoolTime_ <= 0) {
		if ((input_->TriggerKey(DIK_V) || input_->TriggerButton(XINPUT_GAMEPAD_X)) && !attackParam_.isAttacked && !isKnockBack_) {
			attackParam_.phase = 0;
			attackParam_.isAttacked = true;
			easing_tBack = 0;
			attackCoolTime_ = kmaxattackCoolTime_;

			size_t handle = audio_->SoundLoadWave("attack.wav");
			size_t attackHandle = audio_->SoundPlayWave(handle);
			audio_->SetValume(attackHandle, 0.33f);
			date_.attackIndex++;
		}
	}
	else {
		attackCoolTime_-=0.01f;
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
				worldTransform_.scale_ = Vector3(0.7f, 0.7f, 0.7f);
				attackParam_.phase = 1;
			}
			isKnockBack_ = false;
			isDash_ = true;
			
			worldTransform_.quaternion_ = inputQuaternion_;
			worldTransform_.translation_ += Normalize(Vector3{ 0.0f,0.0f,1.0f } * inputQuaternion_) * dashSpeed_;
			

			break;
		case 1:
			//頭でattackする
			headRotate.x += attackSpeed_;
			attackParam_.id_ = 1;
			if (headRotate.x >= 90.0f) {
				attackParam_.phase = 2;
				//easing_t = 0.0f;
			}
			isDash_ = true;
			{
				//worldTransform_.quaternion_ = worldTransform_.quaternion_*  MakeForXAxis(0.5f) ;
				worldTransform_.scale_ = Easing::easing(easing_t, Vector3(0.7f, 0.7f, 0.7f), Vector3(1.5f, 1.5f, 1.5f), 0.1f, Easing::easeInQuad, true);
			}
			
				worldTransform_.quaternion_ = worldTransform_.quaternion_ * MakeForXAxis(0.5f);
				worldTransform_.translation_ += Normalize(Vector3{ 0.0f,0.0f,1.0f } * inputQuaternion_) * dashSpeed_;
			
			break;
		case 2: {
			//頭で元の位置に戻る
			headRotate.x -= backHeadSpeed_;
			attackParam_.id_ = 1;
			worldTransform_.scale_ = Easing::easing(easing_tBack, Vector3(1.5f, 1.5f, 1.5f), Vector3(1.0f, 1.0f, 1.0f), 0.1f, Easing::easeInQuad, true);
			if (headRotate.x <= 0.0f) {
				headRotate.x = 0.0f;
				easing_t = 0.0f;
				worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
				attackParam_.isAttacked = false;
			}
			Vector3 foward = Vector3(0.0f, 0.0f, 1.0f) * worldTransform_.quaternion_;
			foward.y = 0.0f;
			worldTransform_.quaternion_ = MakeLookRotation(foward);
			
			isDash_ = false;
			break;
		}
		case 3:
			//攻撃準備中にノックバックしてしまって頭で元の位置に戻る
			headRotate.x += backHeadSpeed_;
			attackParam_.id_ = 0u;
			if (headRotate.x >= 0.0f) {
				headRotate.x = 0.0f;
				attackParam_.isAttacked = false;
				worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
				easing_t = 0;
				easing_tBack = 0;
			}
			worldTransform_.quaternion_.x = 0.0f;
			worldTransform_.quaternion_.z = 0.0f;
			worldTransform_.quaternion_ = Normalize(worldTransform_.quaternion_);
			worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
			isDash_ = false;

			break;
		case 4:
			//とげに当たってノックバック
			easing_tBack = 0;
				headRotate.x = 0.0f;
				easing_t = 0.0f;
				worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
				attackParam_.isAttacked = false;
				isDash_ = false;
				Vector3 foward = Vector3(0.0f, 0.0f, 1.0f) * worldTransform_.quaternion_;
				foward.y = 0.0f;
				worldTransform_.quaternion_ = MakeLookRotation(foward);
			break;
		}
	}

	Vector3 handle = Vector3(Radian(headRotate.x), Radian(headRotate.y), Radian(headRotate.z));
	headWorldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	headWorldTransform_.Update();
}

void Player::MoveLimit() {
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -10.5f + bodyModelSize_.z / 2.0f, FLT_MAX);
	if (worldTransform_.GetWorldTranslate().y <= -20.0f) {
		hp_ = 0;
	}
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

void Player::DrawParticle()
{
	shadowHitParticle_.Draw(TextureManager::GetInstance()->Load("syadlonTex.png"));
	deadParticle_.Draw({ 1.0f,1.0f,1.0f,1.0f }, deadParticleHandle_);
}

void Player::DrawParticleBox()
{
	hitParticle_.Draw();
}

void Player::DeadUpdate()
{
	const int particleFrame = 130;
	if (hp_ == 0) {
		if (deadFrame_ == 0) {
			//初期化
			isDead_ = true;
			deadParticle_.SetIsEmit(true);
			deadT = 0;
			deadParticle_.emitterWorldTransform_.translation_ = { 0.0f,0.1f,0.3f };
			deadParticle_.EmitNum_ = 3;
		}
		MUTEKITime_ = -1;
		deadFrame_++;

		jumpParam_.velocity_.y = clamp(jumpParam_.velocity_.y, -0.5f, 200.0f);
		jumpParam_.velocity_ += jumpParam_.acceleration_;

		worldTransform_.translation_ += jumpParam_.velocity_;
		worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);

		headWorldTransform_.translation_.y += jumpParam_.velocity_.y;
		headWorldTransform_.translation_.y = clamp(headWorldTransform_.translation_.y, -0.9f, FLT_MAX);

		bodyWorldTransform_.scale_ = Easing::easing(deadT, { 1.0f,1.0f,1.0f }, { 0.1f,0.1f,0.1f }, 1.0f / particleFrame, Easing::easeNormal, true);

		if (deadFrame_ + 30 >= particleFrame) {
			deadParticle_.SetIsEmit(false);
			isEndDeadAnimation_ = true;
		}

	}
}

void Player::ClearUpdate()
{

	rotate_ = { 1.6f,3.2f,0.0f };

	ty_.worldTransform_.quaternion_ = MakeFromEulerAngle(rotate_);

	if (isClear_ == true) {
		worldTransform_.translation_.x += speed_;
		inputQuaternion_ = MakeLookRotation({1.0f,0.0f,0.0f});
		worldTransform_.quaternion_ = Slerp(0.2f, worldTransform_.quaternion_, inputQuaternion_);
		worldTransform_.translation_.x = clamp(worldTransform_.translation_.x, 0.0f, ty_.worldTransform_.translation_.x + 20.0f);

		ty_.worldTransform_.translation_.y = Easing::easing(0.02f, ty_.worldTransform_.translation_.y, tySaveY_, Easing::easeNormal);

		if (ty_.worldTransform_.translation_.y <= tySaveY_ + 0.1f) {
			isEndClearAnimation = true;
		}

		ty_.UpdateMatrix();

	}
}


