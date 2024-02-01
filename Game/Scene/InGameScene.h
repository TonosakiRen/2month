#pragma once
#include "Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Character/EnemyManager.h"
#include <memory>
#include <vector>
class PointLights;
class SpotLights;
class ShadowSpotLights;

class InGameScene {
public:
	InGameScene() = default;
	~InGameScene() = default;

	void Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights);
	void Update();
	void Draw();
	void StageDepthDraw();
	void DrawUI();
	void ShadowDraw();
	void SpotLightShadowDraw();

	WorldTransform* GetPlayerTrans() const { return player_.get()->GetWorldTransform(); }
private:
	struct RT {
		Quaternion rotate;
		Vector3 position;
	};
	RT cameraState_;

public:
	const RT& GetCameraState() const { return cameraState_; }

private:
	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;
	//std::vector<std::unique_ptr<NormalEnemy>> nEnemy_;
	std::unique_ptr<EnemyManager> enemy_;

};
