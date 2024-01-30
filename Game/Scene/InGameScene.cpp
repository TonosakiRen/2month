#include "InGameScene.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"

void InGameScene::Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights) {
	player_ = std::make_unique<Player>();
	player_->Initialize("playerBody");

	std::string filePath = "test";

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(filePath, pointLights, spotLights, shadowSpotLights);
	stage_->SetPlayerRespawn(player_.get());

	enemy_ = std::make_unique<EnemyManager>();
	enemy_->Initialize(pointLights, spotLights, shadowSpotLights);
	enemy_->Load(filePath);

}

void InGameScene::Update() {

	Player::hitShadowEnemyIndex_ = -1;
	Player::hitCollider_ = nullptr;

	stage_->Update(MakeTranslation(player_->GetWorldTransform()->matWorld_));

	enemy_->Update(MakeTranslation(player_->GetWorldTransform()->matWorld_));
	player_->Update();

	enemy_->OnCollisionPlayer(player_->headCollider_, player_->date_);
	stage_->Collision(player_.get());
	stage_->Collision(enemy_.get());
	player_->EnemyCollision();
	player_->EnemyShadowCollision();
}

void InGameScene::Draw() {
	stage_->Draw();
	enemy_->Draw();
	player_->Draw();
}

void InGameScene::StageDepthDraw()
{
	stage_->Draw();
}

void InGameScene::DrawUI()
{
	player_->DrawUI();
}

void InGameScene::ShadowDraw() {
	stage_->Draw();
	enemy_->ShadowDraw();
	//plyerを最後にして
	player_->Draw();
}


void InGameScene::SpotLightShadowDraw() {
	enemy_->SpotLightShadowDraw();
	player_->Draw();
	stage_->SpotLightShadowDraw();
}
