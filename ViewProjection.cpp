#include "ViewProjection.h"
#include "WinApp.h"
#include "Input.h"
#include "ImGuiManager.h"


bool ViewProjection::isUseDebugCamera = false;

void ViewProjection::SwitchIsUseDebugCamera()
{
#ifdef _DEBUG
    ImGui::Begin("switchDebugCamera");
    if (ImGui::Button("SwitchDebugDraw")) {
        if (ViewProjection::isUseDebugCamera == false) {
            ViewProjection::isUseDebugCamera = true;
        }
        else {
            ViewProjection::isUseDebugCamera = false;
        }
    }
    ImGui::End();
#endif 
}

void ViewProjection::Initialize() {
    constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    Update();
}

void ViewProjection::Update() {

    // ビュー行列の生成
    Vector3 tranlation = translation_ + Vector3{ Rand(-shakeValue_.x,shakeValue_.x),Rand(-shakeValue_.y,shakeValue_.y) ,Rand(-shakeValue_.z,shakeValue_.z) };
    matView = MakeViewMatirx(quaternion_, tranlation);

    // 透視投影による射影行列の生成
    matProjection = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
    //matProjection = MakeOrthograohicmatrix(-16.0f * orthographicValue_, 9.0f * orthographicValue_,16.0f * orthographicValue_, -9.0f * orthographicValue_, 1.0f, farZ_);

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    Matrix4x4 viewProjection = matView * matProjection;
    bufferData.viewProjection = viewProjection;
    bufferData.inverseViewProjection = Inverse(viewProjection);
    bufferData.viewPosition = translation_;

    constBuffer_.Copy(bufferData);
}

bool ViewProjection::Shake(Vector3 shakeValue, int& frame)
{
    frame--;
    if (frame > 0) {
        shakeValue_ = shakeValue;
        return true;
    }
    frame = 0;
    shakeValue_ = { 0.0f,0.0f,0.0f };
    return false;
}