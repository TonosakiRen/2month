#include "InGameScene.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"

void InGameScene::Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights) {
	player_ = std::make_unique<Player>();
	player_->Initialize("playerBody");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("test", pointLights, spotLights, shadowSpotLights);
	stage_->SetPlayerRespawn(player_.get());

	enemy_ = std::make_unique<EnemyManager>();
	enemy_->Initialize(pointLights, spotLights, shadowSpotLights);

}

void InGameScene::Update() {
	stage_->Update();

	enemy_->Update(player_->GetWorldTransform()->translation_);
	
	player_->Update();

	for (uint32_t index = 0; index < stage_->GetWalls().size(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
	stage_->Collision(player_.get());
	enemy_->OnCollisionPlayer(player_->headCollider_, player_->date_);

}

void InGameScene::Draw() {
	stage_->Draw();
	enemy_->Draw();
	player_->Draw();
}

void InGameScene::ShadowDraw() {
	enemy_->ShadowDraw();
	//plyerを最後にして
	player_->Draw();
}

void InGameScene::SpotLightShadowDraw() {
	enemy_->SpotLightShadowDraw();
	//plyerを最後にして
	player_->Draw();
}
