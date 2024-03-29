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
	textureHandle_.resize(kMaxStageNumber_ + 1u);
	for (uint32_t index = 0u; index < kMaxStageNumber_; index++) {
		textureHandle_[index] = TextureManager::Load(("stageTexture" + std::to_string(index) + ".png").c_str());
	}
	textureHandle_[kMaxStageNumber_] = TextureManager::Load("sandStorm.png");

	stageTexture_.object = std::make_unique<GameObject>();
	stageTexture_.object->Initialize("plane");
	stageTexture_.object->SetEnableLighting(true);

	sTextureScaleOffset_ = Vector3(8.0f, 4.5f, 1.0f);
	stageTexture_.transform.Initialize();
	stageTexture_.transform.scale_ = sTextureScaleOffset_;
	stageTexture_.transform.translation_ = Vector3(0.0f, 7.0f, 4.0f);

	noiseTexture_.object = std::make_unique<GameObject>();
	noiseTexture_.object->Initialize("plane");
	noiseTexture_.object->SetEnableLighting(true);
	noiseTexture_.transform.Initialize();
	noiseTexture_.transform.scale_ = sTextureScaleOffset_;
	noiseTexture_.transform.translation_ = Vector3(0.0f, 7.0f, 3.9f);
	noiseTexture_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

#pragma region Model
	playerHead_.object = std::make_unique<GameObject>();
	playerHead_.object->Initialize("playerHead");
	playerHead_.transform.Initialize();
	playerHead_.transform.translation_ = Vector3(0.0f, 1.7f, 0.0f);
	playerHead_.transform.quaternion_ = MakeFromEulerAngle(Vector3(Radian(-10.0f), 0.0f, 0.0f));

	playerBody_.object = std::make_unique<GameObject>();
	playerBody_.object->Initialize("playerBody");
	playerBody_.transform.Initialize();
	playerBody_.transform.translation_ = Vector3(-7.6f, 0.0f, 0.0f);
	playerBody_.transform.quaternion_ = MakeFromEulerAngle(Vector3(0.0f, Radian(20.0f), 0.0f));

	playerHead_.transform.parent_ = &playerBody_.transform;
#pragma endregion

	camera_ = std::make_unique<SelectCamera>();
	stage_ = std::make_unique<Stage>();
	stage_->Initialize("stageSelect", pointLights, spotLights, shadowSpotLight_);
}

void StageSelectScene::Update() {
	DrawImGui();
	stage_->Update(Vector3());
	StageChange();

	stageTexture_.transform.Update();
	noiseTexture_.transform.Update();
	playerHead_.transform.Update();
	playerBody_.transform.Update();
	camera_->Update();
	cameraState_.position = camera_->GetTransform().GetWorldTranslate();
	cameraState_.rotate = camera_->GetTransform().quaternion_;
}

void StageSelectScene::Draw() {
	stage_->Draw();
	if (isNoise_) {
		noiseTexture_.object->Draw(noiseTexture_.transform, textureHandle_[kMaxStageNumber_], noiseTexture_.color);
	}
	else {
		stageTexture_.object->Draw(stageTexture_.transform, textureHandle_[currentStageNumber_]);
	}
	playerHead_.object->Draw(playerHead_.transform);
	playerBody_.object->Draw(playerBody_.transform);
}

void StageSelectScene::DrawUI() {
	
}

void StageSelectScene::StageChange() {
	auto input = Input::GetInstance();
	bool direction = false;
	bool flag = false;

	float lStic = input->GetLStick().x / SHORT_MAX;
	if (input->TriggerKey(DIK_LEFTARROW) || input->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT) || (lStic <= -0.5f)) {
		direction = false;
		flag = true;
		
	}else if (input->TriggerKey(DIK_RIGHTARROW) || input->TriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT) || (lStic >= 0.5f)) {
		direction = true;
		flag = true;
	}

	if (flag && !cp_.startUp && !isChangeScene_) {
		cp_.startUp = true;
		cp_.direction = direction;
		//camera_->ChangeStage();
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

	if (input->TriggerKey(DIK_SPACE) || input->TriggerButton(XINPUT_GAMEPAD_A)) {
		isChangeScene_ = true;
		//shadowSpotLight_->lights_[0].isActive = false;
		//shadowSpotLight_->lights_[1].isActive = false;
	}

}

void StageSelectScene::ScaleUpdate() {
	const float oneFrameSize = 10.0f;
	const Vector3 normal = Vector3(0.0f, sTextureScaleOffset_.y / oneFrameSize, 0.0f);
	
	//if (cp_.isOnZoom) {
	//	// 拡大
	//	stageTexture_.transform.scale_ += normal;
	//	if (/*sTextureTransform_.scale_.x >= sTextureScaleOffset_.x || */stageTexture_.transform.scale_.y >= sTextureScaleOffset_.y) {
	//		stageTexture_.transform.scale_ = sTextureScaleOffset_;
	//		cp_.isOnZoom = false;
	//		cp_.startUp = false;
	//	}
	//}
	//else {
	//	// 縮小
	//	stageTexture_.transform.scale_ -= normal;
	//	if (/*sTextureTransform_.scale_.x <= 0.0f || */stageTexture_.transform.scale_.y <= 0.0f) {
	//		stageTexture_.transform.scale_.y = 0.0f;
	//		cp_.isSwitch = true;
	//		cp_.isOnZoom = true;
	//	}
	//}

	const uint32_t kMaxCount = 15u;
	if (count_++ > kMaxCount) {
		cp_.startUp = false;
		count_ = 0u;
		isNoise_ = false;
		cp_.isSwitch = true;
		shadowSpotLight_->lights_[0].isActive = true;
		shadowSpotLight_->lights_[1].isActive = true;
	}
	else {
		isNoise_ = true;
		noiseTexture_.object->material_.translation_ = Shake(Vector3(1.0f, 1.0f, 1.0f));
		shadowSpotLight_->lights_[0].isActive = false;
		shadowSpotLight_->lights_[1].isActive = false;
	}

}

void StageSelectScene::DrawImGui() {
#ifdef _DEBUG
	static Vector3 rot;
	ImGui::Begin("SelectStage");
	if (ImGui::TreeNode("Texture")) {
		ImGui::DragFloat3("scale", &stageTexture_.transform.scale_.x, 0.1f);
		ImGui::DragFloat3("rot", &rot.x, Radian(1.0f));
		stageTexture_.transform.quaternion_ = MakeFromEulerAngle(rot);
		ImGui::DragFloat3("trans", &stageTexture_.transform.translation_.x, 0.1f);

		ImGui::ColorEdit4("color", &noiseTexture_.color.x);
		static Vector3 trans = noiseTexture_.object->material_.translation_;
		ImGui::DragFloat3("noiseTrans", &trans.x, 0.1f);
		noiseTexture_.object->material_.translation_ = trans;

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Player")) {

		if (ImGui::TreeNode("Head")) {
			ImGui::DragFloat3("scale", &playerHead_.transform.scale_.x, 0.1f);
			ImGui::DragFloat3("rot", &rot.x, Radian(1.0f));
			playerHead_.transform.quaternion_ = MakeFromEulerAngle(rot);
			ImGui::DragFloat3("trans", &playerHead_.transform.translation_.x, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Body")) {
			ImGui::DragFloat3("scale", &playerBody_.transform.scale_.x, 0.1f);
			ImGui::DragFloat3("rot", &rot.x, Radian(1.0f));
			playerBody_.transform.quaternion_ = MakeFromEulerAngle(rot);
			ImGui::DragFloat3("trans", &playerBody_.transform.translation_.x, 0.1f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	

	ImGui::End();
#endif // _DEBUG
}

Vector3 StageSelectScene::Shake(Vector3 shakeValue) {
	return Vector3(Rand(-shakeValue.x, shakeValue.x), Rand(-shakeValue.y, shakeValue.y), Rand(-shakeValue.z, shakeValue.z));
}