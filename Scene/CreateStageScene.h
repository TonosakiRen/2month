#pragma once
#include <memory>
#include "Player.h"
#include "Game/Stage/Stage.h"

class CreateStageScene {
public:
	CreateStageScene() = default;
	~CreateStageScene() = default;

	void Initialize();
	void Update();
	void Draw();

private:
	void DrawImGui();
	
	bool isPlay = false; // 「playerが true:動く/false:止まる

	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;

};
