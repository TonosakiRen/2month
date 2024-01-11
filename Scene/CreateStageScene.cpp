#include "CreateStageScene.h"
#include "externals/imgui/imgui.h"
#include "GlobalVariables.h"
#include "PointLights.h"
#include "SpotLights.h"


void CreateStageScene::Initialize(PointLights* pointLights, SpotLights* spotLights) {


	player_ = std::make_unique<Player>();
	player_->Initialize("player");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("test", spotLights, pointLights);

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome");

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
	stage_->Update();
	player_->Update();
	skydome_->Update(player_->GetWorldTransform()->translation_);

	for (uint32_t index = 0; index < stage_->GetWalls().size(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
}

void CreateStageScene::Draw() {
	stage_->Draw();
	player_->Draw();
	skydome_->Draw();
}

void CreateStageScene::DrawImGui() {
#ifdef _DEBUG
	g = GlobalVariables::GetInstance();
	ImGui::Begin("CreateStage", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Initialize")) {
			if (ImGui::TreeNode("FileSave")) {
				ImGui::InputText("FileName", itemName_, sizeof(itemName_));
				if (ImGui::Button("Save")) {
					g->CreateGroup(itemName_);
					stage_->Save(itemName_);

					g->SetValue(itemName_, "Player : Scale" + std::string(), player_->GetWorldTransform()->scale_);
					g->SetValue(itemName_, "Player : Rotate" + std::string(), player_->GetWorldTransform()->quaternion_);
					g->SetValue(itemName_, "Player : Translate" + std::string(), player_->GetWorldTransform()->translation_);

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
				}
				ImGui::TreePop();
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("isPlay")) {
			if (ImGui::Button("play")) {
				isPlay = !isPlay;
			}
			ImGui::EndMenu();
		}

		stage_->DrawImGui();
		if (ImGui::BeginMenu("Player")) {
			player_->DrawImGui();
			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}
	ImGui::End();
	
#endif // _DEBUG
}
