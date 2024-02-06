#pragma once
#include "Sprite.h"
#include "GameObject.h"
#include <memory>

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

	std::unique_ptr<Sprite> title_;
	GameObject titleModel_;
	GameObject floor_;
	ShadowSpotLights* spotLights_;
};