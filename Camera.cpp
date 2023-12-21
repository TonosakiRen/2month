#include "Camera.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"

void Camera::Initialize()
{
    ViewProjection::Initialize();
}

void Camera::Initialize(Vector3 position, Quaternion quaternion)
{
    ViewProjection::Initialize();
    translation_ = position;
    quaternion_ = quaternion;
}

void Camera::Update(float playerX)
{
    const float offset = 6.5f;
    savePlayerX = Easing::easing(0.2f, savePlayerX, playerX);
    translation_.x = savePlayerX + offset;
    ViewProjection::Update();
}
