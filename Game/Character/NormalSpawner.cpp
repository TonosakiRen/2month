#include "NormalSpawner.h"
#include "NormalEnemy.h"
#include <memory>
#include "ModelManager.h"

void NormalSpawner::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"woodbox", // 親
	};

	BaseInitialize(1, names);

	collider_.Initialize(&worldTransform_, "Spawner", models_.at(0).modelHandle_);

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
}

void NormalSpawner::Update(const Vector3& playerPosition) {
	isSpawn_ = false;
	if (isActive_) {
		isSpawn_ = true;
		isActive_ = false;
	}
}

void NormalSpawner::OnCollision(Collider& collider, const PlayerDate& date) {

}

bool NormalSpawner::EnemySpawn() {
	if (!isSpawn_) { return false; }

	respawnPoint_.scale = Vector3(1.0f, 1.0f, 1.0f);
	respawnPoint_.rotate = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	respawnPoint_.translate = worldTransform_.GetWorldTranslate();
	isSpawn_ = false;
	return true;
}
