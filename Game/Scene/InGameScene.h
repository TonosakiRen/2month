#pragma once
#include "Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Character/EnemyManager.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/FixedCamera.h"
#include <memory>
#include <vector>
class PointLights;
class SpotLights;
class ShadowSpotLights;

class InGameScene {
public:
	InGameScene() = default;
	~InGameScene() = default; 

	void Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights, const uint32_t& respawnPoint, const uint32_t& stageNumber);
	void Update();
	void Draw();
	void StageDepthDraw();
	void DrawUI();
	void ShadowDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void SpotLightShadowDraw();

	WorldTransform* GetPlayerTrans() const { return player_.get()->GetWorldTransform(); }
	uint32_t GetPoint() const { return stage_->savePointIndex_; }
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
	std::unique_ptr<EnemyManager> houseEnemy_; // モンスターハウス用の敵たち

	std::shared_ptr<FollowCamera> followCamera_;
	std::shared_ptr<FixedCamera> fixedCamera_;

	bool isTrapped_ = false; // true:起動している/false:していない
	bool isTrappedInitialize = true;
	bool iscamera = true;

	GlobalVariables* g = nullptr;
	std::vector<std::string> fileName_;
	std::string loadSelectName_;
	char itemName_[256]{};
	int fileNumber_ = 0;

	// クリア用
	bool isEndClearAnimation_ = false;
	bool isEndDeadAnimation_ = false;
	// チェックポイント用
	uint32_t chackPointNum_ = 0u;
public:
	const bool& GetEndClearAnimation() const { return isEndClearAnimation_; }
	const bool& GetEndDeadAnimation() const { return isEndDeadAnimation_; }

};
