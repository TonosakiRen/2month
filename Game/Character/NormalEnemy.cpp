#include "NormalEnemy.h"
#include "ModelManager.h"
#include "Player.h"
#include "Compute.h"

void NormalEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"enemy01", // 親
	};

	BaseInitialize(1, names);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;


	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(models_.at(0).modelHandle_);
	// とりあえず一個だけ
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();

	collider_.Initialize(&worldTransform_, "Enemy", models_.at(0).modelHandle_);
}

void NormalEnemy::Update(const Vector3& playerPosition) {
	playerPosition_ = playerPosition;
	float distance = Distance(playerPosition, worldTransform_.translation_);

	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		return;
	}
	
	if (!isHit_) {
		Move(playerPosition);
	}
	else {
		KnockBack();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
}

void NormalEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	// 攻撃Idがリセットされているのであれば都度リセット
	if (date.id == 0u) {
		id_ = date.id;
	}
	bool isColl = false;
	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector)) {
		isColl = true;
	}

	uint32_t* shadowDate = static_cast<uint32_t*>(Compute::GetData());
	if (shadowDate[kNumber_] == 1) {
		isColl = true;
	}

	if (isColl) {
		if (!isHit_ && date.isAttack_ && (id_ != date.id)) {
			isHit_ = true;
			id_ = date.id;
			knockBackVector_ = playerPosition_ - worldTransform_.translation_;
		}
		UpdateTransform();
	}
}

void NormalEnemy::Draw() {
	collider_.Draw();
	BaseDraw();
}

void NormalEnemy::EnemyDraw() {
	BaseEnemyDraw();
}

Vector3 NormalEnemy::Move(const Vector3& playerPosition) {

	Vector3 vec = playerPosition - worldTransform_.translation_;
	vec = Normalize(vec);
	if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
		vec = Vector3(0.0f, 0.0f, 0.0f);
	}
	vec.y = 0.0f;


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
	const float speed = 1.5f;

	worldTransform_.translation_ += -vec * speed;
	if (++count >= 5) {
		isHit_ = false;
		count = 0;
	}

}
