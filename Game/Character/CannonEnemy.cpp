#include "CannonEnemy.h"
#include "Player.h"
#include "Compute.h"
#include "Audio.h"

void CannonEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"gomibako_body", // 親
	};
	BaseInitialize(static_cast<uint32_t>(names.size()), names);
	// 大砲の衝突判定
	collider_.Initialize(&worldTransform_, "Cannon", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// 親 大砲
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();


	UpdateTransform();
}

void CannonEnemy::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
	}
	else {
		isActive_ = true;
	}

	if (isHit_) {
		CollisionProcess();
	}

	Attack(playerPosition);

	collider_.AdjustmentScale();

	UpdateTransform();
}

void CannonEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	// 本体
	if (isActive_) {
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
			Player::hitShadowEnemyIndex_ = kNumber_;
			Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
			isColl = true;
		}

		if (isColl) {
			if (!isHit_ && date.isAttack_ && (id_ != date.id)) {
				// 当たった瞬間
				isHit_ = true;
				id_ = date.id;
				hp_ -= date.damage_;

				Audio* audio = Audio::GetInstance();
				size_t handle = audio->SoundLoadWave("hit.wav");
				size_t hitHandle = audio->SoundPlayWave(handle);
				audio->SetValume(hitHandle, 1.0f);
			}
			else {
				Player::hitCollider_ = &collider_;
			}
			UpdateTransform();
		}
	}
	// 弾

	for (auto& bullet : bullets_) {
		bullet->OnCollision(collider);
	}

}

void CannonEnemy::OnCollision(Collider& collider) {
	for (auto& bullet : bullets_) {
		bullet->OnCollision(collider);
	}
}

void CannonEnemy::Draw() {
	bool flag = false;
#ifndef _DEBUG
	flag = true;
#endif // RELEASE
	if (isActive_) {
		collider_.Draw();
		BaseDraw();
	}
	else if (!flag) {
		collider_.Draw();
		BaseDraw();
	}
	for (auto& bullet : bullets_) {
		bullet->collider_.Draw();
		bullet->Draw();
	}
}

void CannonEnemy::EnemyDraw() {
	bool flag = false;
#ifndef _DEBUG
	flag = true;
#endif // RELEASE
	if (isActive_) {
		BaseEnemyDraw();
	}
	else if (!flag) {
		BaseEnemyDraw();
	}

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
	if (ImGui::Button("isDraw")) {
		shadowOnly_ = !shadowOnly_;
	}

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
			//Vector3 vec = playerPosition - worldTransform_.translation_; // playerの方向に発射
			Vector3 vec(-1.0f, 0.0f, 0.0f); // 一定ベクトルに向けて発射
			vec = Normalize(vec);
			if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
				vec = Vector3(1.0f, 0.0f, 0.0f);
			}

			// 弾を登録する
			auto& bullet = bullets_.emplace_back(std::make_unique<Bullet>());
			bullet->Initialize(worldTransform_.translation_, vec, bulletSpeed_); // world座標を渡してあげる
			bullet->shadowOnly_ = this->shadowOnly_;
			timer_ = 0u;
		}
	}

	for (auto& bullet : bullets_) {
		bullet->Update(playerPosition);
	}

}

void CannonEnemy::CollisionProcess() {
	// 当たった時の処理。体力が0になったらisActiv_をfalseにする
	// 死亡した時
	if (hp_ <= 0) {
		DownAnimation();
		return;
	}else isHit_ = false;

	// 通常ダメージ処理

}

CannonEnemy::Bullet::Bullet() {
	std::vector<std::string> names = {
		"can", // 親
	};
	BaseInitialize(static_cast<uint32_t>(names.size()), names);
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// 親 大砲の弾
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();
	isDead_ = false;

	collider_.Initialize(&worldTransform_, "CannonBullet", models_.at(0).modelHandle_);
}

void CannonEnemy::Bullet::Initialize(const Vector3& translate, const Vector3& shotVec, const float& speed) {
	this->worldTransform_.translation_ = Vector3(translate.x, translate.y + 2.0f, translate.z);
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

void CannonEnemy::Bullet::OnCollision(Collider& collider) {
	bool isColl = false;
	Vector3 pushBackVector;
	if (this->collider_.Collision(collider, pushBackVector) && !shadowOnly_) {
		Player::hitReaction_ = Player::damage;
		Player::hitCollider_ = &collider_;
		Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
		isColl = true;
	}
	uint32_t* shadowDate = static_cast<uint32_t*>(Compute::GetData());
	if (shadowDate[kNumber_] == 1) {
		Player::hitShadowEnemyIndex_ = kNumber_;
		Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
		Player::hitReaction_ = Player::damage;
		isColl = true;
	}

	if (isColl) {
		Player::hitCollider_ = &this->collider_;
		isDead_ = true; // playerと当たった時に弾を消している。無敵があれば消さなくてもいい
	}
}

void CannonEnemy::Bullet::Draw() {
	collider_.Draw();
	BaseDraw();
}

void CannonEnemy::Bullet::EnemyDraw() {
	BaseEnemyDraw();
}

void CannonEnemy::DownAnimation() {
	worldTransform_.translation_.y += 0.5f;
	Vector3 vec = knockBackVector_;
	vec = Normalize(vec);
	const float speed = 1.2f;
	worldTransform_.translation_ += -vec * speed;
	if (60.0f < count++) {
		isAlive_ = false;
	}
}