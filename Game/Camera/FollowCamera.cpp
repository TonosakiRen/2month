#include "FollowCamera.h"
#include "Easing.h"
#include "ImGuiManager.h"
#include <algorithm>
#include "GlobalVariables.h"

FollowCamera::FollowCamera() {
    auto global = GlobalVariables::GetInstance();
    global->LoadFile("FollowCamera");
    Vector3 trans = global->GetVector3Value("FollowCamera", "FollowCameraOffset : Translate");
    Quaternion rot = global->GetQuaternionValue("FollowCamera", "FollowCameraOffset : Rotate");

    end_.translate = trans;
    end_.rotate = rot;

    transform_.translation_ = end_.translate;
    transform_.quaternion_ = end_.rotate;

    rotate = EulerAngle(transform_.quaternion_);
    rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;
}

void FollowCamera::Inisialize(const WorldTransform& transform) {
    start_.translate = transform.GetWorldTranslate();
    start_.rotate = transform.quaternion_;
    count_ = 0.0f;
    isEase_ = true;
}

void FollowCamera::Update(const Vector3& playerPosition) {
    DrawImGui();

    float offset = 6.5f;
    if (isEase_) {
        float MaxFrame = 20.0f;
        Vector3 shake(0.5f, 0.5f, 0.0f);
        shake = Shake(shake);
        float T = count_ / MaxFrame;
        T = std::clamp(T, 0.0f, 1.0f);
        end_.translate.x = playerPosition.x + offset;
        transform_.quaternion_ = Slerp(T, start_.rotate, end_.rotate);
        transform_.translation_ = Lerp(start_.translate, end_.translate, T) + shake;
        if (count_++ >= MaxFrame) { 
            count_ = MaxFrame;
            isEase_ = false;
            savePlayerX_ = playerPosition.x;
        }
    }
    else {
        savePlayerX_ = Easing::easing(0.2f, savePlayerX_, playerPosition.x);
        transform_.translation_.x = savePlayerX_ + offset;

        if (playerPosition.y > end_.translate.y) {
            offset = end_.translate.y + playerPosition.y;
        }
        else {
            offset = end_.translate.y;
        }
        //savePlayerY_ = Easing::easing(0.2f, savePlayerY_, offset);
        savePlayerY_ = savePlayerY_ + (offset - savePlayerY_) * 0.2f;
        transform_.translation_.y = savePlayerY_;
    }
    transform_.Update();
    
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
    Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
    transform_.quaternion_ = MakeFromEulerAngle(handle);
    ImGui::DragFloat3("translate", &transform_.translation_.x, 0.1f);
    if (ImGui::Button("FollowCameraSave")) {
        auto global = GlobalVariables::GetInstance();
        global->SetValue("FollowCamera", "FollowCameraOffset : Translate", transform_.GetWorldTranslate());
        global->SetValue("FollowCamera", "FollowCameraOffset : Rotate", transform_.quaternion_);
        global->SaveFile("FollowCamera");
    }
    ImGui::End();
    transform_.Update();
#endif // _DEBUG
}

Vector3 FollowCamera::Shake(Vector3 shakeValue) {
    return Vector3(Rand(-shakeValue.x, shakeValue.x), Rand(-shakeValue.y, shakeValue.y), Rand(-shakeValue.z, shakeValue.z));
}
