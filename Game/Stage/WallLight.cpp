#include "WallLight.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"

void WallLight::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	GameObject::Initialize("wallLight");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	// light y + 1.0f z + 0.4f

}

void WallLight::Update() {
	UpdateMatrix();
}

void WallLight::Draw() {
	GameObject::Draw();
}

void WallLight::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}

Vector3& WallLight::GetLightPos() const {
	Vector3 result = worldTransform_.translation_;
	result += Vector3(0.0f, 1.0f, -0.4f);
	return result;
}