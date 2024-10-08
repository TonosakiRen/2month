#include "InGameScene.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "Input.h"

void InGameScene::Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights, const uint32_t& respawnPoint, const uint32_t& stageNumber) {
	uint32_t point = respawnPoint;
	uint32_t stageNum = stageNumber;
	player_ = std::make_unique<Player>();
	player_->Initialize("playerBody");

	std::string filePath = "1_Stage";
	switch (stageNum) {
	case 0:
		filePath = "1_Stage";
		break;
	case 1:
		filePath = "2_Stage";
		break;
	}

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(filePath, pointLights, spotLights, shadowSpotLights, point);
	stage_->SetPlayerRespawn(player_.get());

	enemy_ = std::make_unique<EnemyManager>();
	enemy_->Initialize(pointLights, spotLights, shadowSpotLights);
	enemy_->Load(filePath);

	g = GlobalVariables::GetInstance();
	g->ChackFiles(fileName_);
	for (auto& i : fileName_) {
		g->CreateGroup(i.c_str());
	}
	fileNumber_ = 0;
	loadSelectName_ = fileName_[fileNumber_].c_str();

	WorldTransform trans;

	followCamera_ = std::make_shared<FollowCamera>();
	//followCamera_->Inisialize(trans);
	fixedCamera_ = std::make_shared<FixedCamera>();
	railCamera_ = std::make_shared<RailCamera>();

	isEndClearAnimation_ = false;
	isEndDeadAnimation_ = false;
}

void InGameScene::Update() {


	// 以下通常通りの更新処理
	Player::hitShadowEnemyIndex_ = -1;
	Player::hitCollider_ = nullptr;

	stage_->Update(MakeTranslation(player_->GetWorldTransform()->matWorld_));

	player_->Update();
	enemy_->Update(player_->GetWorldTransform()->GetWorldTranslate());
	// trapが起動されたら
	if (isTrapped_) {
		// 敵を生成する
		if (isTrappedInitialize) {
			houseEnemy_ = std::make_unique<EnemyManager>();
			houseEnemy_->Load("house" + std::to_string(stage_->GetParam().trapNumber_));
			houseEnemy_->HousePopInitialize(Vector3(stage_->GetParam().centerPosX_, stage_->GetParam().centerPosY_, 0.0f));
			isTrappedInitialize = false;
		}

		// 更新処理
		if (houseEnemy_) {
			houseEnemy_->Update(player_->GetWorldTransform()->GetWorldTranslate());
			houseEnemy_->OnCollisionPlayer(player_->headCollider_, player_->date_);

			// 敵が全員倒されたかのフラグを取得
			if (!houseEnemy_->Exists()) {
				houseEnemy_.reset();
				stage_->SetTrapFinish();
				isTrapped_ = false;
			}
		}
	}

	enemy_->OnCollisionPlayer(player_->headCollider_, player_->date_);
	stage_->Collision(player_.get());
	stage_->Collision(enemy_.get());
	player_->EnemyCollision();
	player_->EnemyShadowCollision();

	// 定点開始初期化処理
	if (stage_->GetParam().isMomentActivation_ && stage_->GetParam().isFalled_) {
		iscamera = false;
		isTrappedInitialize = true;
		Vector3 centerPos = Vector3(stage_->GetParam().centerPosX_, stage_->GetParam().centerPosY_, 0.0f);
		fixedCamera_->Initialize(centerPos, followCamera_->GetTransform());
	}
	// 定点終了初期化処理
	else if (stage_->GetParam().isMomentActivation_ && stage_->GetParam().isBreaked_) {
		iscamera = true;
		followCamera_->Inisialize(fixedCamera_->GetTransform());
		railCamera_->Initialize(fixedCamera_->GetTransform());
	}

	// カメラの更新
	if (!player_->isClear_) {
		RT handle;
		if (iscamera) {
			followCamera_->Update(player_->GetWorldTransform()->GetWorldTranslate());
			railCamera_->Update(player_->GetWorldTransform()->GetWorldTranslate(), stage_->GetControlPoints());
			handle.rotate = railCamera_->GetTransform().quaternion_;
			handle.position = railCamera_->GetTransform().GetWorldTranslate();
		}
		else {
			fixedCamera_->Update();
			isTrapped_ = !fixedCamera_->GetMove(); // カメラが動いていないとトラップが起動された状態
			handle.rotate = fixedCamera_->GetTransform().quaternion_;
			handle.position = fixedCamera_->GetTransform().GetWorldTranslate();
		}

		cameraState_.rotate = handle.rotate;
		cameraState_.position = handle.position;
	}

	stage_->PostUpdate();
	if (player_->isEndClearAnimation) {
		isEndClearAnimation_ = true;
	}
	if (player_->isEndDeadAnimation_) {
		isEndDeadAnimation_ = true;
	}

}

void InGameScene::Draw() {
	stage_->Draw();
	enemy_->Draw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->Draw();
		}
	}
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
	stage_->ShadowDraw();
	enemy_->ShadowDraw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->ShadowDraw();
		}
	}
	player_->Draw();
}

void InGameScene::ParticleDraw()
{
	player_->DrawParticle();
}

void InGameScene::ParticleBoxDraw()
{
	player_->DrawParticleBox();
}


void InGameScene::SpotLightShadowDraw() {
	enemy_->SpotLightShadowDraw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->SpotLightShadowDraw();
		}
	}
	player_->Draw();
	stage_->SpotLightShadowDraw();
}
