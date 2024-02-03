#include "TitleScene.h"
#include "TextureManager.h"

void TitleScene::Initialize() {
	title_ = std::make_unique<Sprite>();
	uint32_t handle = TextureManager::Load("title.png");
	title_->Initialize(handle, Vector2(960, 540.0f));

	title_->size_ = Vector2(1920.0f, 1080.f);
}

void TitleScene::Update() {

}

void TitleScene::Draw() {
	title_->Draw();
}
