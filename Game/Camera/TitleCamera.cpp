#include "TitleCamera.h"
#include "ImGuiManager.h"
#include "GlobalVariables.h"

void TitleCamera::Inisialize() {
    Vector3 initializeCameraPos = { 0.13f,14.7f,-34.0f };
    Vector3 initializeCameraRotate = { 0.21f,0.0f,0.0f };
    transform_.translation_ = initializeCameraPos;
    transform_.quaternion_ = MakeFromEulerAngle(initializeCameraRotate);
    transform_.Update();
}

void TitleCamera::Update() {
    transform_.Update();
}

void TitleCamera::DrawImGui() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
    Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
    transform_.quaternion_ = MakeFromEulerAngle(handle);
    ImGui::DragFloat3("translate", &transform_.translation_.x, 0.1f);
    if (ImGui::Button("TitleCameraCameraSave")) {
        auto global = GlobalVariables::GetInstance();
        global->SetValue("TitleCamera", "TitleCameraOffset : Translate", transform_.GetWorldTranslate());
        global->SetValue("TitleCamera", "TitleCameraOffset : Rotate", transform_.quaternion_);
        global->SaveFile("TitleCamera");
    }
    ImGui::End();
    transform_.Update();
#endif // _DEBUG
}
