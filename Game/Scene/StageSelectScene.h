#pragma once
#include "Sprite.h"
#include "Model.h"
#include <memory>
#include "Game/Camera/SelectCamera.h"
#include "Game/Stage/Stage.h"
#include "GameObject.h"
#include <vector>

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
	void ScaleUpdate(); // 拡縮演出
	void DrawImGui();

	Vector3 Shake(Vector3 shakeValue);

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
	//std::unique_ptr<Sprite> stageTexture_;
	//std::unique_ptr<GameObject> stageTexture_;
	//WorldTransform sTextureTransform_;
	//Vector4 sTextureColor_;
	Vector3 sTextureScaleOffset_;
	
	struct Object{
		std::unique_ptr<GameObject> object;
		WorldTransform transform;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	};
	Object playerHead_;
	Object playerBody_;
	Object stageTexture_;
	Object noiseTexture_;

	struct ChangeParam {
		bool isSwitch = false; // Stage切り替えをするか
		bool direction = false; // 移動方向 true:右/false:左
		bool startUp = false; // 拡縮演出の起動 true:する/false:しない
		bool isOnZoom = false; // 拡大中か true:拡大/false:縮小
	};
	ChangeParam cp_;

	int currentStageNumber_ = 0u;
	const uint32_t kMaxStageNumber_ = 5u;
	std::unique_ptr<SelectCamera> camera_;
	std::unique_ptr<Stage> stage_;
	ShadowSpotLights* shadowSpotLight_;

	bool isChangeScene_ = false; // シーン切り替え用フラグ
	std::vector<uint32_t> textureHandle_; // ステージテクスチャー用の切り替えハンドル

	uint32_t count_ = 0u;
	bool isNoise_ = false;

};
