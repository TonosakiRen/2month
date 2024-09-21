#pragma once
#include <memory>
#include "Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Character/EnemyManager.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/FixedCamera.h"
#include "Game/RailCamera/RailCamera.h"


class GlobalVariables;
class PointLights;
class SpotLights;
class ShadowSpotLights;


class CreateStageScene {
public:
	CreateStageScene() = default;
	~CreateStageScene() = default;

	void Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights);
	void Update();
	void Draw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void DrawUI();
	void ShadowDraw();
	void SpotLightShadowDraw();
	void StageDepthDraw();


	/// <summary>
	/// プレイモードかのフラグを返す
	/// </summary>
	/// <returns>true:debugCamera/false:followCamera</returns>
	bool GetPlay() const { return !isPlay; }
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
	void DrawImGui();
	
	bool isPlay = false; // 「playerが true:動く/false:止まる

	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<EnemyManager> enemy_;

	std::unique_ptr<EnemyManager> houseEnemy_; // モンスターハウス用の敵たち

	GlobalVariables* g = nullptr;
	std::vector<std::string> fileName_;
	std::string loadSelectName_;
	char itemName_[256]{};
	int fileNumber_ = 0;

	std::shared_ptr<FollowCamera> followCamera_;
	std::shared_ptr<RailCamera> railCamera_;
	std::shared_ptr<FixedCamera> fixedCamera_;

	bool isTrapped_ = false; // true:起動している/false:していない
	bool isTrappedInitialize = true;
	bool iscamera = true;

};
