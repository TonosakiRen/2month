#include "TitleScene.h"
#include "TextureManager.h"

void TitleScene::Initialize() {
	title_ = std::make_unique<Sprite>();
	uint32_t handle = TextureManager::Load("title.png");
	title_->Initialize(handle, Vector2(640.0f, 360.0f));

}

void TitleScene::Update() {

}

void TitleScene::Draw() {
	title_->Draw();
}
