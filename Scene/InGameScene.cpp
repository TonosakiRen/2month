#include "InGameScene.h"
#include "PointLights.h"
#include "SpotLights.h"

void InGameScene::Initialize(PointLights* pointLights, SpotLights* spotLights) {
	player_ = std::make_unique<Player>();
	player_->Initialize("player");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("test", spotLights, pointLights);
	stage_->SetPlayerRespawn(player_.get());
}

void InGameScene::Update() {
	stage_->Update();

	player_->Update();

	for (uint32_t index = 0; index < stage_->GetWalls().size(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
}

void InGameScene::Draw() {
	stage_->Draw();
	player_->Draw();
}
