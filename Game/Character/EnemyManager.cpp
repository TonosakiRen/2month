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
	const uint32_t kMaxNormalEnemyCount = 1u;
	Base respawn[kMaxNormalEnemyCount] = {
		{ Vector3(1.0f,1.0f,1.0f),Quaternion(0.0f,0.0f,0.0f,1.0f),Vector3(0.0f,2.0f,0.0f) },
	};

	for (uint32_t index = 0u; index < kMaxNormalEnemyCount; index++) {
		auto& enemy = nEnemis_.emplace_back(std::make_unique<NormalEnemy>());
		enemy->Initialize(respawn[index].scale, respawn[index].rotate, respawn[index].translate);
	}

}

void EnemyManager::Update(const Vector3& playerPosition) {
	for (auto& enemy : nEnemis_) {
		enemy->Update(playerPosition);
	}
}

void EnemyManager::OnCollisionPlayer(Collider& collider, const PlayerDate& date) {
	for (auto& enemy : nEnemis_) {
		enemy->OnCollision(collider, date);
	}
}

void EnemyManager::Draw() {
	for (auto& enemy : nEnemis_) {
		enemy->Draw();
	}
}

void EnemyManager::ShadowDraw() {
	for (auto& enemy : nEnemis_) {
		enemy->Draw();
	}
}

void EnemyManager::SpotLightShadowDraw() {
	for (auto& enemy : nEnemis_) {
		enemy->EnemyDraw();
	}
}
