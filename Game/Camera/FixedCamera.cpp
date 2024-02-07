#include "FixedCamera.h"
#include "Easing.h"
#include <algorithm>
#include "GlobalVariables.h"
#include "ImGuiManager.h"

FixedCamera::FixedCamera() {
	auto global = GlobalVariables::GetInstance();
	global->LoadFile("FixedCamera");
	offsetTrans = global->GetVector3Value("FixedCamera", "FixedCameraOffset : Translate");
	Quaternion rot = global->GetQuaternionValue("FixedCamera", "FixedCameraOffset : Rotate");
	end_.translate = offsetTrans;
	end_.rotate = rot;

	rotate = EulerAngle(transform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;
}

void FixedCamera::Initialize(const Vector3& trapPosition, const WorldTransform& transform) {
	end_.translate.x = trapPosition.x;
	end_.translate.y = trapPosition.y + offsetTrans.y;
	start_.translate = transform.translation_;
	start_.rotate = transform.quaternion_;
	count_ = 0.0f;
	isMove_ = true;
}

void FixedCamera::Update() {
	DrawImGui();
	if (!isMove_) { return; }

	float MaxFrame = 120.0f;
	Vector3 shake(0.5f, 0.5f, 0.0f);
	shake = Shake(shake);
	if (++count_ >= MaxFrame) { 
		count_ = MaxFrame;
		shake = Vector3(0.0f, 0.0f, 0.0f);
		isMove_ = false;
	}
	float T = count_ / MaxFrame;
	T = std::clamp(T, 0.0f, 1.0f);
	transform_.quaternion_ = Slerp(T, start_.rotate, end_.rotate);
	transform_.translation_ = Lerp(start_.translate, end_.translate, T) + shake;
	transform_.Update();
}

void FixedCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	transform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &transform_.translation_.x, 0.1f);
	if (ImGui::Button("FixedCameraSave")) {
		auto global = GlobalVariables::GetInstance();
		global->SetValue("FixedCamera", "FixedCameraOffset : Translate", transform_.GetWorldTranslate());
		global->SetValue("FixedCamera", "FixedCameraOffset : Rotate", transform_.quaternion_);
		global->SaveFile("FixedCamera");
	}
	ImGui::End();
	transform_.Update();
#endif // _DEBUG
}

Vector3 FixedCamera::Shake(Vector3 shakeValue) {
	return Vector3(Rand(-shakeValue.x, shakeValue.x), Rand(-shakeValue.y, shakeValue.y), Rand(-shakeValue.z, shakeValue.z));
}