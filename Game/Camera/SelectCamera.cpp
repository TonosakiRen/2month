#include "SelectCamera.h"
#include "ImGuiManager.h"

SelectCamera::SelectCamera() {
    Vector3 initializeCameraPos = { 0.0f,8.0f,-20.0f };
    Vector3 initializeCameraRotate = { Radian(2.0f),0.0f,0.0f };
    transform_.translation_ = initializeCameraPos;
    transform_.quaternion_ = MakeFromEulerAngle(initializeCameraRotate);
    transform_.Update();
}

void SelectCamera::Update() {
    //DrawImGui();
    transform_.Update();
}

void SelectCamera::DrawImGui() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
    Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
    transform_.quaternion_ = MakeFromEulerAngle(handle);
    ImGui::DragFloat3("translate", &transform_.translation_.x, 0.1f);
    /*if (ImGui::Button("SelectCameraCameraSave")) {
        auto global = GlobalVariables::GetInstance();
        global->SetValue("SelectCamera", "SelectCameraOffset : Translate", transform_.GetWorldTranslate());
        global->SetValue("SelectCamera", "SelectCameraOffset : Rotate", transform_.quaternion_);
        global->SaveFile("SelectCamera");
    }*/
    ImGui::End();
    transform_.Update();
#endif // _DEBUG
}
