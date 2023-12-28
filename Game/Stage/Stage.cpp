#include "Stage.h"
#include "ModelManager.h"

void Stage::initialize() {
	uint32_t wallModelHandle_ = ModelManager::Load("wall");
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
