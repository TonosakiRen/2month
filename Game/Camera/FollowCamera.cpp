#include "FollowCamera.h"
#include "Easing.h"
#include "ImGuiManager.h"
#include <algorithm>
#include "GlobalVariables.h"

FollowCamera::FollowCamera() {
    auto global = GlobalVariables::GetInstance();
    Vector3 trans = global->GetVector3Value("Player", "FollowCameraOffset : Translate");
    Quaternion rot = global->GetQuaternionValue("Player", "FollowCameraOffset : Rotate");

    transform_.translation_ = trans;
    transform_.quaternion_ = rot;

    end_.translate = Vector3(0.0f, 8.45f, -26.0f);
    end_.rotate = MakeFromEulerAngle(Vector3(Radian(9.0f), 0.0f, 0.0f));
}

void FollowCamera::Inisialize(const WorldTransform& transform) {
    start_.translate = transform.GetWorldTranslate();
    start_.rotate = transform.quaternion_;
    count_ = 0.0f;
    isEase_ = true;
}

void FollowCamera::Update(float playerX) {
    //DrawImGui();

    const float offset = 6.5f;
    if (isEase_) {
        float MaxFrame = 20.0f;
        Vector3 shake(0.5f, 0.5f, 0.0f);
        shake = Shake(shake);
        float T = count_ / MaxFrame;
        T = std::clamp(T, 0.0f, 1.0f);
        end_.translate.x = playerX + offset;
        transform_.quaternion_ = Slerp(T, start_.rotate, end_.rotate);
        transform_.translation_ = Lerp(start_.translate, end_.translate, T) + shake;
        if (count_++ >= MaxFrame) { 
            count_ = MaxFrame;
            isEase_ = false;
            savePlayerX_ = playerX;
        }
    }
    else {
        savePlayerX_ = Easing::easing(0.2f, savePlayerX_, playerX);
        transform_.translation_.x = savePlayerX_ + offset;
    }
    transform_.Update();
    
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
    static Vector3 rotate;
    ImGui::Begin("Camera");
    ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
    Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
    transform_.quaternion_ = MakeFromEulerAngle(handle);
    ImGui::DragFloat3("translate", &transform_.translation_.x, 0.1f);
    if (ImGui::Button("FollowCameraSave")) {
        auto global = GlobalVariables::GetInstance();
        global->SetValue("Player", "FollowCameraOffset : Translate", transform_.GetWorldTranslate());
        global->SetValue("Player", "FollowCameraOffset : Rotate", transform_.quaternion_);
        global->SaveFile("Player");
    }
    ImGui::End();
    transform_.Update();
#endif // _DEBUG
}

Vector3 FollowCamera::Shake(Vector3 shakeValue) {
    return Vector3(Rand(-shakeValue.x, shakeValue.x), Rand(-shakeValue.y, shakeValue.y), Rand(-shakeValue.z, shakeValue.z));
}
