#include "TitleScene.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "ShadowSpotLights.h"

TitleScene::~TitleScene()
{
	spotLights_->lights_[0].isActive = false;
}

void TitleScene::Initialize(ShadowSpotLights* spotLights) {
	title_ = std::make_unique<Sprite>();
	uint32_t handle = TextureManager::Load("title.png");
	title_->Initialize(handle, Vector2(960, 540.0f));

	title_->size_ = Vector2(1920.0f, 1080.f);
	titleModel_.Initialize("Title");
	titleModel_.worldTransform_.translation_ = { 0.0f,7.7f,0.0f };
	titleModel_.worldTransform_.scale_ = { 7.0f,7.0f,7.0f };

	floor_.Initialize("stagefloor");
	floor_.worldTransform_.scale_ = { 3.0f,1.0f,3.0f };
	floor_.worldTransform_.translation_ = { 0.0f,3.0f,0.0f };
	spotLights_ = spotLights;
	spotLights_->lights_[0].worldTransform.translation_ = {0.0f,24.0f,26.0f};
	spotLights_->lights_[0].color = {1.0f,1.0f,0.58f};
	spotLights_->lights_[0].intensity = 3.0f;
	spotLights_->lights_[0].direction = { 0.0f,-0.703f,-0.711f };
	spotLights_->lights_[0].distance = 52.3f;
	spotLights_->lights_[0].decay = 1.0f;
	spotLights_->lights_[0].cosAngle = 0.785f;

	titleCamera_ = std::make_unique<TitleCamera>();
	titleCamera_->Inisialize();
}

void TitleScene::Update() {

	spotLights_->lights_[0].worldTransform.translation_ = { 0.0f,24.0f,26.0f };
	spotLights_->lights_[0].color = { 1.0f,1.0f,0.58f };
	spotLights_->lights_[0].intensity = 3.0f;
	spotLights_->lights_[0].direction = { 0.0f,-0.703f,-0.711f };
	spotLights_->lights_[0].distance = 52.3f;
	spotLights_->lights_[0].decay = 1.0f;
	spotLights_->lights_[0].cosAngle = 0.785f;

	titleModel_.UpdateMatrix();
	spotLights_->lights_[0].isActive = true;
#ifdef _DEBUG

	ImGui::Begin("spotLight");
	ImGui::DragFloat3("lightPosition", &spotLights_->lights_[0].worldTransform.translation_.x, 0.01f);
	ImGui::DragFloat3("lightColor", &spotLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
	ImGui::DragFloat("intensity", &spotLights_->lights_[0].intensity, 0.01f, 0.0f);
	ImGui::DragFloat3("direction", &spotLights_->lights_[0].direction.x, 0.01f, 0.0f);
	spotLights_->lights_[0].direction = Normalize(spotLights_->lights_[0].direction);
	ImGui::DragFloat("distance", &spotLights_->lights_[0].distance, 0.01f, 0.0f);
	ImGui::DragFloat("decay", &spotLights_->lights_[0].decay, 0.01f, 0.0f);
	ImGui::DragFloat("cosAngle", &spotLights_->lights_[0].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
	ImGui::End();

	ImGui::DragFloat3("title", &titleModel_.worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("titleSca", &titleModel_.worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("stagefloor", &floor_.worldTransform_.translation_.x, 0.1f);
#endif // _DEBUG
	floor_.UpdateMatrix();

	// カメラの更新
	titleCamera_->Update();
	cameraState_.position = titleCamera_->GetTransform().GetWorldTranslate();
	cameraState_.rotate = titleCamera_->GetTransform().quaternion_;

}

void TitleScene::Draw() {
	title_->Draw();
}

void TitleScene::DepthDraw() {
	floor_.Draw();
}

void TitleScene::ModelDraw()
{
	titleModel_.Draw();
	floor_.Draw();
}
