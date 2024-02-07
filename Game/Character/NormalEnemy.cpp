#include "NormalEnemy.h"
#include "ModelManager.h"
#include "Player.h"
#include "Compute.h"
#include "Audio.h"

void NormalEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"enemy01", // 親
	};

	BaseInitialize(1, names);

	collider_.Initialize(&worldTransform_, "Enemy", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;


	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(models_.at(0).modelHandle_);
	// とりあえず一個だけ
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();
}

void NormalEnemy::Update(const Vector3& playerPosition) {
	playerPosition_ = playerPosition;
	float distance = Distance(playerPosition, worldTransform_.translation_);

	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
		return;
	}
	isActive_ = true;
	
	if (!isHit_) {
		//Move(playerPosition);
		Vector3 vec = playerPosition - MakeTranslation(worldTransform_.matWorld_);
		vec = Normalize(vec);
		if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
			vec = Vector3(0.0f, 0.0f, 0.0f);
		}
		/*vec.y = 0.0f;
		vec.z = 0.0f;*/

		const float kSpeed = 0.0f;

		worldTransform_.quaternion_ = MakeLookRotation(-vec);
		worldTransform_.translation_ += vec * kSpeed;
		UpdateTransform();
	}
	else {
		CollisionProcess();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
}

void NormalEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	// 攻撃Idがリセットされているのであれば都度リセット
	if (date.id == 0u) {
		id_ = date.id;
	}
	if (!isActive_) { return; }
	else if (hp_ <= 0) { return; }

	bool isColl = false;

	bool isShadowHit = false;
	bool isHit = false;
	
	uint32_t* shadowDate = static_cast<uint32_t*>(Compute::GetData());
	if (shadowDate[kNumber_] == 1) {
		isColl = true;
		isShadowHit = true;
	}

	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector)&& !shadowOnly_) {
		isColl = true;
		isHit = true;
	}

	if (isColl) {
		if (date.isAttack_ == false) {
			if (isHit) {
				Player::hitCollider_ = &collider_;
				Player::hitReaction_ = Player::knockBack;
			}
			else if (isShadowHit) {
				Player::hitShadowEnemyIndex_ = kNumber_;
				Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
				Player::hitReaction_ = Player::knockBack;
			}
		}else
		if (attackIndex_ != date.attackIndex) {
			if (isHit) {
				Player::hitCollider_ = &collider_;
				Player::hitReaction_ = Player::knockBack;
			}
			else if (isShadowHit) {
				Player::hitShadowEnemyIndex_ = kNumber_;
				Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
				Player::hitReaction_ = Player::knockBack;
			}
		}

		if (!isHit_ && date.isAttack_ && (id_ != date.id)) {
			// 当たった瞬間
			isHit_ = true;
			id_ = date.id;
			knockBackVector_ = playerPosition_ - worldTransform_.translation_;
			hp_ -= date.damage_;
			Audio* audio = Audio::GetInstance();
			size_t handle = audio->SoundLoadWave("hit.wav");
			size_t hitHandle = audio->SoundPlayWave(handle);
			audio->SetValume(hitHandle, 1.0f);

			attackIndex_ = date.attackIndex;

		}
		UpdateTransform();
	}
}

void NormalEnemy::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	BaseDraw();
}

void NormalEnemy::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	BaseEnemyDraw();
}

Vector3 NormalEnemy::Move(const Vector3& playerPosition) {

	Vector3 vec = playerPosition - MakeTranslation(worldTransform_.matWorld_);
	vec = Normalize(vec);
	if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
		vec = Vector3(0.0f, 0.0f, 0.0f);
	}
	vec.y = 0.0f;
	vec.z = 0.0f;

	const float kSpeed = 0.05f;

	worldTransform_.quaternion_ = MakeLookRotation(-vec);
	worldTransform_.translation_ += vec * kSpeed;
	UpdateTransform();

	return vec;
}

void NormalEnemy::KnockBack() {
	Vector3 vec = knockBackVector_;
	vec = Normalize(vec);
	vec.y = 0.0f;
	vec.z = 0.0f;
	const float speed = 1.2f;

	worldTransform_.translation_ += -vec * speed;
	if (++count >= 5) {
		isHit_ = false;
		count = 0;
	}

}

void NormalEnemy::DownAnimation() {
	worldTransform_.translation_.y += 0.5f;
	Vector3 vec = knockBackVector_;
	vec = Normalize(vec);
	const float speed = 1.2f;
	worldTransform_.translation_ += -vec * speed;
	//KnockBack();
	rotate.y += 2.0f;
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	if (60.0f < count++) {
		isAlive_ = false;
	}
}

void NormalEnemy::CollisionProcess() {

	// 死亡した時
	if (hp_ <= 0) {
		DownAnimation();
		return;
	}

	// 衝突時にノックバックとアニメーション
	KnockBack();

}
