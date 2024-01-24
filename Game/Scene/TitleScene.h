#pragma once
#include "Sprite.h"
#include <memory>

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene() = default;

	void Initialize();
	void Update();
	void Draw();

private:

	std::unique_ptr<Sprite> title_;

};