#include "StageSelectScene.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "Input.h"

StageSelectScene::StageSelectScene() {
	sTextureColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	stageTexture_ = std::make_unique<Sprite>();
	float width = static_cast<int>(WinApp::kWindowWidth) / 2.0f;
	float height = static_cast<int>(WinApp::kWindowHeight) / 2.0f;
	sTexturePosition_ = Vector2(width, height);
	uint32_t handle = TextureManager::Load("title.png");
	stageTexture_->Initialize(handle, sTexturePosition_, sTextureColor_);
	stageTexture_->size_ = Vector2(width, height);

}

void StageSelectScene::Update() {

}

void StageSelectScene::Draw() {
	
}

void StageSelectScene::DrawUI() {
	stageTexture_->Draw();
}

void StageSelectScene::StageChange() {
	auto key = Input::GetInstance();

	if (key->TriggerKey(DIK_LEFTARROW)) {
		cp_.direction = false;
		cp_.isSwitch = true;
	}else if (key->TriggerKey(DIK_LEFTARROW)) {
		cp_.direction = false;
		cp_.isSwitch = true;
	}

	if (cp_.isSwitch) {
		// 右ならば番号を加算する
		if (cp_.direction) {

		}
		// 左ならば番号を加算する
		if (!cp_.direction) {

		}
	}

}
