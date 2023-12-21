#include "DebugCamera.h"
#include "Input.h"
#include "ImGuiManager.h"

void DebugCamera::Initialize()
{
    ViewProjection::Initialize();
}

void DebugCamera::Initialize(Vector3 position, Vector3 rotation)
{
    translation_ = position;
    debugRotation_ = rotation;
    ViewProjection::Initialize();
}

void DebugCamera::Update()
{
    DebugMode();
    ViewProjection::Update();
}

void DebugCamera::DebugMode()
{
    Input* input = Input::GetInstance();

    Vector2 mouseMove = input->GetMouseMove();
    float wheel = input->GetWheel();


#ifdef _DEBUG
    ImGui::Begin("DebugCamera");
    ImGui::DragFloat3("translation", &translation_.x, 0.01f);
    ImGui::DragFloat3("rotation", &debugRotation_.x, 0.01f);
    ImGui::DragFloat("orthographicValue_", &orthographicValue_, 0.01f);
    orthographicValue_ = clamp(orthographicValue_, 0.01f, FLT_MAX);
    ImGui::End();
#endif // _DEBUG

    Quaternion debugMoveQuaternion{};
    Vector3 rotation{};

    if (input->IsPressMouse(1)) {
        float rot = static_cast<float>(M_PI / 180.0f);
        debugRotation_.x += rot * mouseMove.y * 0.1f;
        debugRotation_.y += rot * mouseMove.x * 0.1f;
    }
    else if (input->IsPressMouse(2)) {
        Matrix4x4 rotMat = MakeRotateXYZMatrix(debugRotation_);
        Vector3 cameraX = GetXAxis(rotMat) * static_cast<float>(-mouseMove.x) * 0.01f;
        Vector3 cameraY = GetYAxis(rotMat) * static_cast<float>(mouseMove.y) * 0.01f;
        translation_ += cameraX + cameraY;
    }
    else if (wheel != 0) {
        Matrix4x4 rotMat = MakeRotateXYZMatrix(debugRotation_);
        Vector3 cameraZ = GetZAxis(rotMat) * (static_cast<float>(wheel / 120) * 0.5f);
        translation_ += cameraZ;
        //orthographicValue_ -= static_cast<float>(wheel / 120) * 0.01f;
        orthographicValue_ = clamp(orthographicValue_, 0.01f, FLT_MAX);
    }

    quaternion_ = MakeFromEulerAngle(debugRotation_);
}
