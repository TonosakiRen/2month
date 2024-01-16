#include "EnemyManager.h"

EnemyManager::EnemyManager() {

}

EnemyManager::~EnemyManager() {

}

void EnemyManager::Initialize() {

	struct Base {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	Base respawn[1];
	respawn[0] = { Vector3(1.0f,1.0f,1.0f),Quaternion(0.0f,0.0f,0.0f,1.0f),Vector3(0.0f,0.0f,0.0f) };

	for (uint32_t index = 0u; index < 1u; index++) {
		auto& enemy = nEnemis_.emplace_back(std::make_unique<NormalEnemy>());
		enemy->Initialize();
	}

}

void EnemyManager::OnCollisionPlayer(Collider& collider) {

}
