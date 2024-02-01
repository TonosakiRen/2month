#include "FixedCamera.h"
#include "Easing.h"
#include <algorithm>

FixedCamera::FixedCamera() {
	end_.translate = Vector3(0.0f, 10.0f, -30.0f);
	end_.rotate = MakeFromEulerAngle(Vector3(Radian(6.0f), 0.0f, 0.0f));
}

void FixedCamera::Initialize(const float& trapPosition, const WorldTransform& transform) {
	end_.translate.x = trapPosition;
	start_.translate = transform.translation_;
	start_.rotate = transform.quaternion_;
	count_ = 0.0f;
	isMove_ = true;
}

void FixedCamera::Update() {
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

Vector3 FixedCamera::Shake(Vector3 shakeValue) {
	return Vector3(Rand(-shakeValue.x, shakeValue.x), Rand(-shakeValue.y, shakeValue.y), Rand(-shakeValue.z, shakeValue.z));
}