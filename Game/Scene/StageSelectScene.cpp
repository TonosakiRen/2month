#include "StageSelectScene.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "WinApp.h"
#include "Input.h"
#include "GlobalVariables.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "ImGuiManager.h"

StageSelectScene::~StageSelectScene() {
}

void StageSelectScene::Initialize(PointLights* pointLights, SpotLights* spotLights, ShadowSpotLights* shadowSpotLights) {
	shadowSpotLight_ = shadowSpotLights;
	sTextureColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	stageTexture_ = std::make_unique<GameObject>();
	float width = static_cast<int>(WinApp::kWindowWidth) / 2.0f;
	float height = static_cast<int>(WinApp::kWindowHeight) / 2.0f;
	//sTexturePosition_ = Vector2(width, height);
	textureHandle_.resize(kMaxStageNumber_);
	for (uint32_t index = 0u; index < kMaxStageNumber_; index++) {
		textureHandle_[index] = TextureManager::Load(("stageTexture" + std::to_string(index) + ".png").c_str());
	}
	//stageTexture_->Initialize(handle, sTexturePosition_, sTextureColor_);
	stageTexture_->Initialize("plane");
	stageTexture_->SetEnableLighting(true);

	sTextureScaleOffset_ = Vector3(8.0f, 4.5f, 1.0f);
	sTextureTransform_.Initialize();
	sTextureTransform_.scale_ = sTextureScaleOffset_;
	sTextureTransform_.translation_ = Vector3(0.0f, 7.0f, 4.0f);

	camera_ = std::make_unique<SelectCamera>();
	stage_ = std::make_unique<Stage>();
	stage_->Initialize("stageSelect", pointLights, spotLights, shadowSpotLight_);
}

void StageSelectScene::Update() {
	DrawImGui();
	stage_->Update(Vector3());
	StageChange();

	sTextureTransform_.Update();
	camera_->Update();
	cameraState_.position = camera_->GetTransform().GetWorldTranslate();
	cameraState_.rotate = camera_->GetTransform().quaternion_;
}

void StageSelectScene::Draw() {
	stage_->Draw();
	stageTexture_->Draw(sTextureTransform_, textureHandle_[currentStageNumber_]);
}

void StageSelectScene::DrawUI() {
	
}

void StageSelectScene::StageChange() {
	auto key = Input::GetInstance();

	if (key->TriggerKey(DIK_LEFTARROW)) {
		cp_.direction = false;
		cp_.startUp = true;
	}else if (key->TriggerKey(DIK_RIGHTARROW)) {
		cp_.direction = true;
		cp_.startUp = true;
	}

	if (cp_.startUp) {
		ScaleUpdate();
	}

	if (cp_.isSwitch) {
		cp_.isSwitch = false;
		// 右ならば番号を加算する
		if (cp_.direction) {
			currentStageNumber_ += 1;
		}
		// 左ならば番号を減算する
		if (!cp_.direction) {
			currentStageNumber_ -= 1;
		}

		// ステージの最大数以上だと最小値に戻す
		if (currentStageNumber_ >= static_cast<int>(kMaxStageNumber_)) {
			currentStageNumber_ = 0u;
		}
		else if (currentStageNumber_ < 0) {
			currentStageNumber_ = static_cast<int>(kMaxStageNumber_) - 1;
		}
	}

	if (key->TriggerKey(DIK_SPACE)) {
		isChangeScene_ = true;
		shadowSpotLight_->lights_[0].isActive = false;
		shadowSpotLight_->lights_[1].isActive = false;
	}

}

void StageSelectScene::ScaleUpdate() {
	const float oneFrameSize = 15.0f;
	const Vector3 normal = Vector3(0.0f, sTextureScaleOffset_.y / oneFrameSize, 0.0f);
	
	if (cp_.isOnZoom) {
		// 拡大
		sTextureTransform_.scale_ += normal;
		if (/*sTextureTransform_.scale_.x >= sTextureScaleOffset_.x || */sTextureTransform_.scale_.y >= sTextureScaleOffset_.y) {
			sTextureTransform_.scale_ = sTextureScaleOffset_;
			cp_.isOnZoom = false;
			cp_.startUp = false;
		}
	}
	else {
		// 縮小
		sTextureTransform_.scale_ -= normal;
		if (/*sTextureTransform_.scale_.x <= 0.0f || */sTextureTransform_.scale_.y <= 0.0f) {
			sTextureTransform_.scale_.y = 0.0f;
			cp_.isSwitch = true;
			cp_.isOnZoom = true;
		}
	}
}

void StageSelectScene::DrawImGui() {
#ifdef _DEBUG
	static Vector3 rot;
	ImGui::Begin("Texture");
	ImGui::DragFloat3("scale", &sTextureTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rot", &rot.x, Radian(1.0f));
	sTextureTransform_.quaternion_ = MakeFromEulerAngle(rot);
	ImGui::DragFloat3("trans", &sTextureTransform_.translation_.x, 0.1f);

	ImGui::End();
#endif // _DEBUG
}
