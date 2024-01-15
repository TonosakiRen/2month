#pragma once
#include "Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Character/NormalEnemy.h"
#include <memory>
#include <vector>
class PointLights;
class SpotLights;

class InGameScene {
public:
	InGameScene() = default;
	~InGameScene() = default;

	void Initialize(PointLights* pointLights, SpotLights* spotLights);
	void Update();
	void Draw();
	void ShadowDraw();

	WorldTransform* GetPlayerTrans() const { return player_.get()->GetWorldTransform(); }

private:
	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;
	std::vector<std::unique_ptr<NormalEnemy>> nEnemy_;

};
