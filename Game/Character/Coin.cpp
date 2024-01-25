#include "Coin.h"
#include "Player.h"
#include "Compute.h"

void Coin::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"coin", // 親
	};

	BaseInitialize(1, names);

	collider_.Initialize(&worldTransform_, "Enemy", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// とりあえず一個だけ
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();
}

void Coin::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);

	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
		return;
	}
	isActive_ = true;

	if (isHit_) {
		CollisionProcess();
	}
	else {
		WaitProcess();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
}

void Coin::OnCollision(Collider& collider, const PlayerDate& date) {
	if (!isActive_) { return; }

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
		if (!isHit_) {
			isHit_ = true;
			UpdateTransform();
		}
	}
}

void Coin::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	BaseDraw();
}

void Coin::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	BaseEnemyDraw();
}

void Coin::CollisionProcess() {
	isAlive_ = false;
}

void Coin::WaitProcess() {
	rotate.y += 2.0f;
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
}


