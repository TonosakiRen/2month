#include "CreateStageScene.h"
#include "externals/imgui/imgui.h"
#include "GlobalVariables.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"


void CreateStageScene::Initialize(PointLights* pointLights, SpotLights* spotLights,ShadowSpotLights* shadowSpotLights) {

	player_ = std::make_unique<Player>();
	player_->Initialize("playerBody");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("ki-biju", pointLights, spotLights, shadowSpotLights);
	stage_->SetPlayerRespawn(player_.get());

	enemy_ = std::make_unique<EnemyManager>();
	enemy_->Initialize(pointLights, spotLights, shadowSpotLights);
	enemy_->Load("ki-biju");

	g = GlobalVariables::GetInstance();
	g->ChackFiles(fileName_);
	for (auto& i : fileName_) {
		g->CreateGroup(i.c_str());
	}
	fileNumber_ = 0;
	loadSelectName_ = fileName_[fileNumber_].c_str();
	//isPlay = true;

	followCamera_ = std::make_shared<FollowCamera>();
	railCamera_ = std::make_shared<RailCamera>();
	fixedCamera_ = std::make_shared<FixedCamera>();
}

void CreateStageScene::Update() {
	DrawImGui();
	if (!isPlay) {
		return;
	}

	// 以下通常通りの更新処理
	Player::hitShadowEnemyIndex_ = -1;
	Player::hitCollider_ = nullptr;

	player_->Update();
	enemy_->Update(player_->GetWorldTransform()->GetWorldTranslate());

	stage_->Update(MakeTranslation(player_->GetWorldTransform()->matWorld_));
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
		Vector3 centerPos = Vector3(stage_->GetParam().centerPosX_, stage_->GetParam().centerPosY_,0.0f);
		fixedCamera_->Initialize(centerPos, railCamera_->GetTransform());
	}
	// 定点終了初期化処理
	else if(stage_->GetParam().isMomentActivation_ && stage_->GetParam().isBreaked_) {
		iscamera = true;
		followCamera_->Inisialize(fixedCamera_->GetTransform());
		railCamera_->Initialize(fixedCamera_->GetTransform());
	}

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
	// カメラの更新
	
	cameraState_.rotate = handle.rotate;
	cameraState_.position = handle.position;

	stage_->PostUpdate();
}

void CreateStageScene::Draw() {
	stage_->Draw();
	enemy_->Draw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->Draw();
		}
	}
	player_->Draw();
}

void CreateStageScene::ParticleDraw() {
	player_->DrawParticle();
}

void CreateStageScene::ParticleBoxDraw() {
	player_->DrawParticleBox();
}

void CreateStageScene::ShadowDraw() {
	stage_->ShadowDraw();
	enemy_->ShadowDraw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->ShadowDraw();
		}
	}
	player_->Draw();
}

void CreateStageScene::SpotLightShadowDraw() {
	enemy_->SpotLightShadowDraw();
	if (isTrapped_) {
		if (houseEnemy_) {
			houseEnemy_->SpotLightShadowDraw();
		}
	}
	player_->Draw();
	stage_->SpotLightShadowDraw();
}

void CreateStageScene::StageDepthDraw()
{
	stage_->Draw();
}

void CreateStageScene::DrawUI()
{
	player_->DrawUI();
}

void CreateStageScene::DrawImGui() {
#ifdef _DEBUG
	g = GlobalVariables::GetInstance();
	ImGui::Begin("CreateStage", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Initialize")) {
			if (ImGui::Button("play")) {
				if (!isPlay) {
					isPlay = true;
					stage_->Save("temporary");
					enemy_->Save("temporary");
				}
				else {
					isPlay = false;
					stage_->Load("temporary");
					enemy_->Load("temporary");
					//stage_->SetPlayerRespawn(player_.get());
				}
			}
			if (ImGui::TreeNode("FileSave")) {
				ImGui::InputText("FileName", itemName_, sizeof(itemName_));
				if (ImGui::Button("Save")) {
					g->CreateGroup(itemName_);
					stage_->Save(itemName_);

					g->SetValue(itemName_, "Player : Scale" + std::string(), player_->GetWorldTransform()->scale_);
					g->SetValue(itemName_, "Player : Rotate" + std::string(), player_->GetWorldTransform()->quaternion_);
					g->SetValue(itemName_, "Player : Translate" + std::string(), player_->GetWorldTransform()->translation_);

					enemy_->Save(itemName_);

					g->SaveFile(itemName_);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("FileLoad")) {
				for (size_t i = 0; i < fileName_.size(); i++) {
					if (ImGui::RadioButton(fileName_[i].c_str(), &fileNumber_, static_cast<int>(i))) {
						loadSelectName_ = fileName_[fileNumber_].c_str();
					}
				}
				if (ImGui::Button("Load")) {
					stage_->Load(loadSelectName_);
					player_->GetWorldTransform()->scale_ = g->GetVector3Value(loadSelectName_, "Player : Scale");
					player_->GetWorldTransform()->quaternion_ = g->GetQuaternionValue(loadSelectName_, "Player : Rotate");
					player_->GetWorldTransform()->translation_ = g->GetVector3Value(loadSelectName_, "Player : Translate");

					enemy_->Load(loadSelectName_);
				}
				ImGui::TreePop();
			}
			
			ImGui::EndMenu();
		}
		
		stage_->DrawImGui();
		if (ImGui::BeginMenu("Player")) {
			player_->DrawImGui();
			ImGui::EndMenu();
		}
		enemy_->DrawImGui();

		ImGui::EndMenuBar();
	}
	ImGui::End();
	
#endif // _DEBUG
}
