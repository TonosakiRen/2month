#include "DirectionalLights.h"

void DirectionalLights::Initialize() {
    constBuffer_.Create((sizeof(DirectionalLight) + 0xff) & ~0xff);
    lights_.push_back(DirectionalLight{});
    Update();
}

void DirectionalLights::Update() {

    // 定数バッファに書き込み
    DirectionalLight bufferData;
    bufferData.color = lights_[0].color;
    bufferData.direction = Normalize(lights_[0].direction);
    bufferData.intensity = lights_[0].intensity;

    constBuffer_.Copy(bufferData);
}