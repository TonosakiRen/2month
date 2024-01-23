#include "EnemyManager.h"
#include "SpotLights.h"
#include "PointLights.h"
#include "ShadowSpotLights.h"

EnemyManager::EnemyManager() {

}

EnemyManager::~EnemyManager() {

}

void EnemyManager::Initialize(PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowSpotLight) {

	pointLights_ = pointLight;
	spotLights_ = spotLight;
	shadowSpotLights_ = shadowSpotLight;

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

	/*const uint32_t kMaxNormalLightEnemyCount = 1u;
	for (uint32_t index = 0u; index < kMaxNormalLightEnemyCount; index++) {
		auto& enemy = nLightEnemis_.emplace_back(std::make_unique<NormalLightEnemy>());
		enemy->Initialize(respawn[index].scale, respawn[index].rotate, respawn[index].translate);
		enemy->SetLight(shadowSpotLights_->lights_[index]);
	}*/

}

void EnemyManager::Update(const Vector3& playerPosition) {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Update(playerPosition);
	}
}

void EnemyManager::OnCollisionPlayer(Collider& collider, const PlayerDate& date) {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->OnCollision(collider, date);
	}
}

void EnemyManager::Draw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
}

void EnemyManager::ShadowDraw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
}

void EnemyManager::SpotLightShadowDraw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
}
