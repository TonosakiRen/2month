#pragma once
#include <memory>
#include "Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Character/EnemyManager.h"
#include "Skydome.h"

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
	void ShadowDraw();
	void SpotLightShadowDraw();

	/// <summary>
	/// プレイモードかのフラグを返す
	/// </summary>
	/// <returns>true:debugCamera/false:followCamera</returns>
	bool GetPlay() const { return !isPlay; }
	WorldTransform* GetPlayerTrans() const { return player_.get()->GetWorldTransform(); }

private:
	void DrawImGui();
	
	bool isPlay = false; // 「playerが true:動く/false:止まる

	std::unique_ptr<Player> player_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<EnemyManager> enemy_;

	GlobalVariables* g = nullptr;
	std::vector<std::string> fileName_;
	std::string loadSelectName_;
	char itemName_[256]{};
	int fileNumber_ = 0;

};
