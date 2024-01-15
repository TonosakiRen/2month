#include "InGameScene.h"
#include "PointLights.h"
#include "SpotLights.h"

void InGameScene::Initialize(PointLights* pointLights, SpotLights* spotLights) {
	player_ = std::make_unique<Player>();
	player_->Initialize("player");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("test", spotLights, pointLights);
	stage_->SetPlayerRespawn(player_.get());

	auto& enemy = nEnemy_.emplace_back(std::make_unique<NormalEnemy>());
	enemy->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));

}

void InGameScene::Update() {
	stage_->Update();

	player_->Update();

	for (auto& enemy : nEnemy_) {
		enemy->Update();
	}

	for (uint32_t index = 0; index < stage_->GetWalls().size(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
}

void InGameScene::Draw() {
	stage_->Draw();
	player_->Draw();
	for (auto& enemy : nEnemy_) {
		enemy->Draw();
	}
}

void InGameScene::ShadowDraw()
{
	//plyerを最後にして
	player_->Draw();
}
