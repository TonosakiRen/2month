#pragma once
#include "Player.h"
#include "Game/Stage/Stage.h"
#include <memory>
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

};
