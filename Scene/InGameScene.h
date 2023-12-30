#pragma once
#include "Player.h"
#include "Game/Stage/Stage.h"
#include <memory>

class InGameScene {
public:
	InGameScene() = default;
	~InGameScene() = default;

	void Initialize();
	void Update();
	void Draw();

	WorldTransform* GetPlayerTrans() const { return player_.get()->GetWorldTransform(); }

private:
	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;

};
