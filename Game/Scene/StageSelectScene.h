#pragma once
#include "Sprite.h"
#include <memory>

class StageSelectScene {
public:
	StageSelectScene();
	~StageSelectScene() = default;

	void Update();
	void Draw();
	void DrawUI();

private:
	void StageChange();

private:
	std::unique_ptr<Sprite> stageTexture_;
	Vector2 sTexturePosition_;
	Vector4 sTextureColor_;
	
	struct ChangeParam {
		bool isSwitch = false; // Stage切り替えをするか
		bool direction = false; // 移動方向 true:右/false:左

	};
	ChangeParam cp_;


};
