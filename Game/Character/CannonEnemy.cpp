#include "CannonEnemy.h"

void CannonEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"enemy01", // 親
	};
	BaseInitialize(static_cast<uint32_t>(names.size()), names);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// 親 大砲
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();

	// 大砲の衝突判定
	collider_.Initialize(&worldTransform_, "Cannon", models_.at(0).modelHandle_);

	UpdateTransform();
}

void CannonEnemy::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);
	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
	}
	else {
		isActive_ = true;
	}

	Attack(playerPosition);

	collider_.AdjustmentScale();

	UpdateTransform();
}

void CannonEnemy::OnCollision(Collider& collider, const PlayerDate& date) {

}

void CannonEnemy::Draw() {
	collider_.Draw();
	BaseDraw();
	for (auto& bullet : bullets_) {
		bullet->collider_.Draw();
		bullet->Draw();
	}

}

void CannonEnemy::EnemyDraw() {
	BaseEnemyDraw();
	for (auto& bullet : bullets_) {
		bullet->EnemyDraw();
	}
}

void CannonEnemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 rotHandle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(rotHandle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	int handle = static_cast<int>(kInterval_); ImGui::DragInt("interval", &handle, 1); kInterval_ = static_cast<uint32_t>(handle);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed_, 0.01f);
	ImGui::Text("bulletCount : %d", static_cast<int>(bullets_.size()));
	
	UpdateTransform();
#endif // _DEBUG
}

void CannonEnemy::Attack(const Vector3& playerPosition) {

	// デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) {
		return bullet->isDead_;
		});

	if (isActive_) {
		if (kInterval_ < timer_++) {
			Vector3 vec = playerPosition - worldTransform_.translation_;
			vec = Normalize(vec);
			if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
				vec = Vector3(1.0f, 0.0f, 0.0f);
			}

			// 弾を登録する
			auto& bullet = bullets_.emplace_back(std::make_unique<Bullet>());
			bullet->Initialize(worldTransform_.translation_, vec, bulletSpeed_); // world座標を渡してあげる
			timer_ = 0u;
		}
	}

	for (auto& bullet : bullets_) {
		bullet->Update(playerPosition);
	}

}

CannonEnemy::Bullet::Bullet() {
	std::vector<std::string> names = {
		"toge", // 親
	};
	BaseInitialize(static_cast<uint32_t>(names.size()), names);
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// 親 大砲の弾
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();
	isDead_ = false;

	collider_.Initialize(&worldTransform_, "CannonBullet", models_.at(0).modelHandle_);
}

void CannonEnemy::Bullet::Initialize(const Vector3& translate, const Vector3& shotVec, const float& speed) {
	this->worldTransform_.translation_ = translate;
	this->shotVec_ = shotVec;
	this->speed_ = speed;
}

void CannonEnemy::Bullet::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);
	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isDead_ = true;
		return;
	}

	this->worldTransform_.translation_ += (shotVec_ * speed_);
	this->UpdateTransform();
}

void CannonEnemy::Bullet::OnCollision(Collider& collider, const PlayerDate& date) {

}

void CannonEnemy::Bullet::Draw() {
	collider_.Draw();
	BaseDraw();
}

void CannonEnemy::Bullet::EnemyDraw() {
	BaseEnemyDraw();
}
