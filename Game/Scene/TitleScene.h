#pragma once
#include "Sprite.h"
#include "GameObject.h"
#include <memory>
#include "Game/Camera/TitleCamera.h"

class ShadowSpotLights;

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene();

	void Initialize(ShadowSpotLights* spotLights);
	void Update();
	void Draw();
	void DepthDraw();
	void ModelDraw();
private:
	struct RT {
		Quaternion rotate;
		Vector3 position;
	};
	RT cameraState_;

public:
	const RT& GetCameraState() const { return cameraState_; }
private:

	std::unique_ptr<Sprite> title_;
	GameObject titleModel_;
	GameObject floor_;
	ShadowSpotLights* spotLights_;
	std::unique_ptr<TitleCamera> titleCamera_;
};