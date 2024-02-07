#pragma once
#include "Sprite.h"
#include "GameObject.h"
#include <memory>
#include "Game/Camera/TitleCamera.h"
#include "Mymath.h"
class ShadowSpotLights;

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene();

	void Initialize(ShadowSpotLights* spotLights);
	void Update();
	void Draw();
	void TatemonoDraw();
	void DepthDraw();
	void ModelDraw();
private:
	void FlashColor();
	struct RT {
		Quaternion rotate;
		Vector3 position;
	};
	RT cameraState_;

public:
	const RT& GetCameraState() const { return cameraState_; }
private:

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<Sprite> pressA_;
	GameObject titleModel_;
	GameObject floor_;
	GameObject tatemono_;
	ShadowSpotLights* spotLights_;
	std::unique_ptr<TitleCamera> titleCamera_;

	// 点滅用
	bool isInOut_ = false;
	Vector4 pressAColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	float sinf = 0;
};