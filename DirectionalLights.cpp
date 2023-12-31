#include "DirectionalLights.h"

void DirectionalLights::Initialize() {
    lights_.push_back(DirectionalLight{});
    lights_[0].constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    lights_[0].shadowMap_.Create(1024, 1024, DXGI_FORMAT_D32_FLOAT);
    Update();
}

void DirectionalLights::Update() {

    Matrix4x4 viewMatrix = MakeLookRotationMatrix(lights_[0].direction, lights_[0].lockUp);
    viewMatrix.m[3][0] = lights_[0].position.x;
    viewMatrix.m[3][1] = lights_[0].position.y;
    viewMatrix.m[3][2] = lights_[0].position.z;
    viewMatrix = Inverse(viewMatrix);
   
    lights_[0].viewProjection = viewMatrix * MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);


    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.color = lights_[0].color;
    bufferData.direction = Normalize(lights_[0].direction);
    bufferData.intensity = lights_[0].intensity;
    bufferData.viewProjection = lights_[0].viewProjection;

    lights_[0].constBuffer_.Copy(bufferData);
}
