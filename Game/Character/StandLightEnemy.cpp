#include "StandLightEnemy.h"
#include "ModelManager.h"

void StandLightEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"toge", // 親
	};

	BaseInitialize(1, names);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// とりあえず一個だけ
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();

	timer_ = 0u;
	flashCount_ = 3u;

	UpdateTransform();
}

void StandLightEnemy::Update(const Vector3& playerPosition) {
	lightDirection_ = FindDirection(playerPosition);
	Flashing();

	UpdateTransform();
	MoveLight();
}

void StandLightEnemy::Draw() {
	// ライトが点滅しているなら描画
	if (isLightActive_) {
		BaseDraw();
	}
	// 起動しているなら描画
	if (isActive_) {
		//BaseDraw();
	}
}

void StandLightEnemy::EnemyDraw() {
	BaseEnemyDraw();
}

void StandLightEnemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	int time = static_cast<int>(kMaxTime_); ImGui::DragInt("MaxTime", &time, 1); kMaxTime_ = static_cast<uint32_t>(time);
	time = static_cast<int>(kMaxFlashCount_); ImGui::DragInt("MaxFlashCount", &time, 1); kMaxFlashCount_ = static_cast<uint32_t>(time);
	time = static_cast<int>(kInterval_); ImGui::DragInt("Interval", &time, 1); kInterval_ = static_cast<uint32_t>(time);
	if (ImGui::Button("isDraw")) {
		shadowOnly_ = !shadowOnly_;
	}

	UpdateTransform();
#endif // _DEBUG
}

void StandLightEnemy::SetParameters(const uint32_t& maxTime, const uint32_t& MaxFlashCount, const uint32_t& Interval) {
	kMaxTime_ = maxTime;
	kMaxFlashCount_ = MaxFlashCount;
	kInterval_ = Interval;
}

Vector3 StandLightEnemy::FindDirection(const Vector3& playerPosition) {
	Vector3 vec = playerPosition - worldTransform_.translation_;
	vec = Normalize(vec);
	if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
		vec = Vector3(0.0f, 0.0f, 0.0f);
	}

	return vec;
}

void StandLightEnemy::MoveLight() {
	lightDirection_;
}

void StandLightEnemy::Flashing() {
	if (kMaxTime_ < timer_++) {
		
		if ((timer_ - kMaxTime_) % kInterval_ == 0) {
			isLightActive_ = !isLightActive_;
			if (kMaxFlashCount_ <= flashCount_++) {
				// 初期化
				flashCount_ = 0u;
				timer_ = 0u;
			}
		}
	}
}
