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
	stage_->Initialize("test", pointLights, spotLights, shadowSpotLights);
	stage_->SetPlayerRespawn(player_.get());

	enemy_ = std::make_unique<EnemyManager>();
	enemy_->Initialize(pointLights, spotLights, shadowSpotLights);
	enemy_->Load("test");

	g = GlobalVariables::GetInstance();
	g->ChackFiles(fileName_);
	for (auto& i : fileName_) {
		g->CreateGroup(i.c_str());
	}
	fileNumber_ = 0;
	loadSelectName_ = fileName_[fileNumber_].c_str();

}

void CreateStageScene::Update() {
	DrawImGui();
	if (!isPlay) {
		return;
	}

	// 以下通常通りの更新処理
	Player::hitShadowEnemyIndex_ = -1;
	Player::hitCollider_ = nullptr;

	stage_->Update(MakeTranslation(player_->GetWorldTransform()->matWorld_));

	enemy_->Update(player_->GetWorldTransform()->GetWorldTranslate());
	player_->Update();

	enemy_->OnCollisionPlayer(player_->headCollider_, player_->date_);
	stage_->Collision(player_.get());
	stage_->Collision(enemy_.get());
	player_->EnemyCollision();
	player_->EnemyShadowCollision();
}

void CreateStageScene::Draw() {
	stage_->Draw();
	enemy_->Draw();
	player_->Draw();
}

void CreateStageScene::ShadowDraw() {
	stage_->Draw();
	enemy_->ShadowDraw();
	player_->Draw();
}

void CreateStageScene::SpotLightShadowDraw() {
	enemy_->SpotLightShadowDraw();
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
				isPlay = !isPlay;
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
