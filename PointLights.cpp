#include "PointLights.h"

void PointLights::Initialize() {
    lights_.push_back(PointLight{});
    lights_[0].constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    Update();
}

void PointLights::Update() {

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.color = lights_[0].color;
    bufferData.position = lights_[0].position;
    bufferData.intensity = lights_[0].intensity;
    bufferData.radius = lights_[0].radius;
    bufferData.decay = lights_[0].decay;

    lights_[0].constBuffer_.Copy(bufferData);
}
