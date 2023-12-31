#include "SpotLights.h"

void SpotLights::Initialize() {
    lights_.push_back(SpotLight{});
    lights_[0].constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    Update();
}

void SpotLights::Update() {

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.color = lights_[0].color;
    bufferData.position = lights_[0].position;
    bufferData.intensity = lights_[0].intensity;
    bufferData.direction = lights_[0].direction;
    bufferData.distance = lights_[0].distance;
    bufferData.decay = lights_[0].decay;
    bufferData.cosAngle = lights_[0].cosAngle;
    lights_[0].constBuffer_.Copy(bufferData);
}
