#include "CreateStageScene.h"
#include "externals/imgui/imgui.h"

void CreateStageScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize("GoalWell");

	stage_ = std::make_unique<Stage>();
	stage_->initialize();
}

void CreateStageScene::Update() {
	if (!isPlay) {
		DrawImGui();
		return;
	}

	// 以下通常通りの更新処理
	stage_->Update();
	player_->Update();

	for (uint32_t index = 0; index < stage_->GetWallSize(); index++) {
		player_->Collision(stage_->GetWallCollider(index));
	}
}

void CreateStageScene::Draw() {
	stage_->Draw();
	player_->Draw();
}

void CreateStageScene::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("CreateStage", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {

		stage_->DrawImGui();
		if (ImGui::BeginMenu("Player")) {
			player_->DrawImGui();
		}


		ImGui::EndMenu();
	}
	ImGui::End();
	
#endif // _DEBUG
}
