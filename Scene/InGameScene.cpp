#include "InGameScene.h"

void InGameScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize("player");

	stage_ = std::make_unique<Stage>();
	stage_->initialize("test");
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
