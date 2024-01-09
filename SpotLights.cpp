#include "SpotLights.h"
#include "DirectXCommon.h"

void SpotLights::Initialize() {
    lights_.resize(lightNum);
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(sizeof(ConstBufferData) * lightNum);
    structureBuffer_.Create(sizeINB);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = lightNum;
    srvDesc.Buffer.StructureByteStride = sizeof(ConstBufferData);

    srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(structureBuffer_, &srvDesc, srvHandle_);
    Update();
}

void SpotLights::Update() {

    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);
    for (int i = 0; i < lightNum; i++) {
        ConstBufferData data;
        data.color = lights_[i].color;
        lights_[i].worldTransform.Update();
        data.position = MakeTranslation(lights_[i].worldTransform.matWorld_);
        data.intensity = lights_[i].intensity;
        data.direction = lights_[i].direction;
        data.distance = lights_[i].distance;
        data.decay = lights_[i].decay;
        data.cosAngle = lights_[i].cosAngle;
        data.isActive = static_cast<float>(lights_[i].isActive);
        bufferData.emplace_back(data);
    }
    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
