#pragma once
#include "Sprite.h"
#include <memory>
#include "Game/Camera/SelectCamera.h"
#include "Game/Stage/Stage.h"

class ShadowSpotLights;
class StageSelectScene {
public:
	StageSelectScene() = default;
	~StageSelectScene();

	void Initialize(class PointLights* pointLights, class SpotLights* spotLights, ShadowSpotLights* shadowSpotLights);
	void Update();
	void Draw();
	void DrawUI();

private:
	void StageChange();

private:
	struct RT {
		Quaternion rotate;
		Vector3 position;
	};
	RT cameraState_;

public:
	const RT& GetCameraState() const { return cameraState_; }
	const uint32_t& GetStageNumber() const { return currentStageNumber_; }
	const bool& SceneChange() const { return isChangeScene_; }
private:
	std::unique_ptr<Sprite> stageTexture_;
	Vector2 sTexturePosition_;
	Vector4 sTextureColor_;
	
	struct ChangeParam {
		bool isSwitch = false; // Stage切り替えをするか
		bool direction = false; // 移動方向 true:右/false:左
	};
	ChangeParam cp_;

	uint32_t currentStageNumber_ = 0u;
	uint32_t kMaxStageNumber_ = 10u;
	std::unique_ptr<SelectCamera> camera_;
	std::unique_ptr<Stage> stage_;
	ShadowSpotLights* shadowSpotLight_;

	bool isChangeScene_ = false; // シーン切り替え用フラグ

};
