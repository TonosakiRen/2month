#include "Heart.h"
#include "Player.h"
#include "Compute.h"
#include "Audio.h"

void Heart::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"Heart", // 親
	};

	audio_ = Audio::GetInstance();

	BaseInitialize(1, names);

	collider_.Initialize(&worldTransform_, "Heart", models_.at(0).modelHandle_);

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

void Heart::Update(const Vector3& playerPosition) {
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

void Heart::OnCollision(Collider& collider, const PlayerDate& date) {
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

void Heart::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	BaseDraw();
}

void Heart::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	BaseHeartDraw();
}

void Heart::CollisionProcess() {

	isAlive_ = false;

	size_t handle = audio_->SoundLoadWave("coin.wav");
	size_t catchHandle = audio_->SoundPlayWave(handle);
	audio_->SetValume(catchHandle, 0.1f);
}

void Heart::WaitProcess() {
	rotate.y += 2.0f;
	if (rotate.y >= 360.0f) {
		rotate.y -= 360.0f;
	}

	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
}


