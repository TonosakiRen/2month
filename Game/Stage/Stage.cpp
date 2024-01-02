#include "Stage.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"
#include <string>
#include "GlobalVariables.h"

void Stage::initialize() {
	uint32_t wallModelHandle_ = ModelManager::Load("scene");
	float sizeX = ModelManager::GetInstance()->ModelManager::GetModelSize(wallModelHandle_).x;

	const uint8_t wallSize = 3;
	Vector3 wallIniPos[wallSize]{
		Vector3(-sizeX ,-2.0f,0.0f),
		Vector3(0.0f,-2.0f,0.0f),
		Vector3(+sizeX,-2.0f,0.0f),
	};

	for (uint8_t index = 0u; index < wallSize; index++) {
		walls_.emplace_back(std::make_unique<Wall>())->Initialize(wallIniPos[index]);
	}
}

void Stage::Update() {
	for (auto& wall : walls_) {
		wall->Update();
	}
}

void Stage::Draw() {
	for (auto& wall : walls_) {
		wall->Draw();
	}
}

void Stage::DrawImGui() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("Walls")) {
		if (ImGui::Button("Create")) {
			walls_.emplace_back(std::make_unique<Wall>())->Initialize(Vector3(0.0f,0.0f,0.0f));
		}
		// 要素数確認
		ImGui::Text("ElementCount = %d", walls_.size());
		for (int i = 0; i < walls_.size(); i++) {
			if (ImGui::TreeNode(("WallNumber : " + std::to_string(i)).c_str())) {
				walls_.at(i)->DrawImGui();
				if (ImGui::Button("Delete")) {
					walls_.erase(walls_.begin() + i);
				}
				ImGui::TreePop();
			}
		}
		ImGui::EndMenu();
	}
	
#endif // _DEBUG
}

void Stage::Load(const std::filesystem::path& loadFile) {
	GlobalVariables* global = GlobalVariables::GetInstance();
	std::string selectName = loadFile.string();
	global->LoadFile(selectName);

	int num = global->GetIntValue(selectName, "WallConfirmation");
	walls_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 trans = global->GetVector3Value(selectName, ("WallNumber : " + std::to_string(i) + " : Translate").c_str());
		Vector3 scal = global->GetVector3Value(selectName, ("WallNumber : " + std::to_string(i) + " : Scale").c_str());
		auto& wall = walls_.emplace_back(std::make_unique<Wall>());
		wall->GetWorldTransform()->translation_ = trans;
		wall->GetWorldTransform()->scale_ = scal;
		wall->Initialize(trans);
	}
}
