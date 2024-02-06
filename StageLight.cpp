#include "StageLight.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Game/Character/BaseCharacter.h"
#include "ShadowSpotLights.h"
#undef min

void StageLight::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate, ShadowSpotLights* shadowSpotLights,float distance,float shadeDistance,float intensity,float decay) {
	std::string name = "light";
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_, name, modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	//rotate = EulerAngle(worldTransform_.quaternion_);
	//rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y); rotate.z = Degree(rotate.z) - 180.0f;
	shadowSpotLights_ = shadowSpotLights;
	distance_ = distance;
	shadeDistance_ = shadeDistance;
	intensity_ = intensity;
	decay_ = decay;
	//Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
}

StageLight::~StageLight()
{
	if (lightIndex_ != -1) {
		shadowSpotLights_->lights_[lightIndex_].isActive = false;
		lightIndex_ = -1;
	}
}

void StageLight::Update(const Vector3& playerWorldPosition) {

	if (!ActiveChack(playerWorldPosition)) {
		isActive_ = false;
		if (lightIndex_ != -1) {
			shadowSpotLights_->lights_[lightIndex_].isActive = false;
			lightIndex_ = -1;
		}
		return;
	}
	isActive_ = true;

	if (lightIndex_ == -1) {
		for (int i = 0; auto & light : shadowSpotLights_->lights_) {
			if (light.isActive == false) {

				light.isActive = true;
				lightIndex_ = i;
				//lightの初期化
				light.color = {1.0f,1.0,0.58f};
				light.worldTransform.SetInitialize();
				light.worldTransform.parent_ = &worldTransform_;
				light.intensity = intensity_;
				light.direction = MakeDirection(worldTransform_.quaternion_);
				light.distance = distance_;
				light.shadeDistance = shadeDistance_;
				light.decay = decay_;
				light.cosAngle = 0.8f;
				break;
			}
			i++;
		}
	}
	else {
		shadowSpotLights_->lights_[lightIndex_].direction = MakeDirection(worldTransform_.quaternion_);
		shadowSpotLights_->lights_[lightIndex_].distance = distance_;
		shadowSpotLights_->lights_[lightIndex_].shadeDistance = shadeDistance_;
		shadowSpotLights_->lights_[lightIndex_].intensity = intensity_;
		shadowSpotLights_->lights_[lightIndex_].decay = decay_;
	}



	collider_.AdjustmentScale();
	UpdateMatrix();
}

void StageLight::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE

	collider_.Draw();
	GameObject::Draw();
}

void StageLight::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	rotate = { 0.0f,0.0f,0.0f };
	ImGui::DragFloat3("rotate", &rotate.x, 0.02f, -360.0f, 360.0f);
	Quaternion q = MakeFromEulerAngle(rotate);
	//Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z))
	worldTransform_.quaternion_ = worldTransform_.quaternion_ * q;
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat("Distance", &distance_, 0.1f);
	ImGui::DragFloat("ShadeDistance", &shadeDistance_, 0.1f);
	ImGui::DragFloat("intensity", &intensity_, 0.1f);
	ImGui::DragFloat("decay", &decay_, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}

bool StageLight::ActiveChack(const Vector3& playerWorldPosition) const {

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	Vector3 minSize = worldTransform_.GetWorldTranslate() - modelSize;
	Vector3 maxSize = worldTransform_.GetWorldTranslate() + modelSize;

	if ((minSize.x <= playerWorldPosition.x) && (playerWorldPosition.x <= maxSize.x)) {
		// モデルのx軸の間にいるので何もせず終わり
	}
	else {
		float distance = std::min(Distance(playerWorldPosition, minSize), Distance(playerWorldPosition, maxSize));
		// Playerとの距離が一定数以下なら早期リターン
		if (distance > kMaxDistance) {
			return false;
		}
	}
	return true;
}
