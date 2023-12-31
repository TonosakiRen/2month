#include "InGameScene.h"

void InGameScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize("player");

	stage_ = std::make_unique<Stage>();
	stage_->initialize();
}

void InGameScene::Update() {
	stage_->Update();

	player_->Update();

	for (uint32_t index = 0; index < stage_->GetWallSize(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
}

void InGameScene::Draw() {
	stage_->Draw();
	player_->Draw();
}
