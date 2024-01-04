#include "WallLight.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"

void WallLight::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	GameObject::Initialize("wallLight");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();

}

void WallLight::Update() {
	UpdateMatrix();
}

void WallLight::Draw() {
	GameObject::Draw();
}

void WallLight::DrawImGui() {
#ifdef _DEBUG
	static Vector3 rotate;
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}
