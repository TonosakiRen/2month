#include "DirectionalLights.h"
#include "DirectXCommon.h"

void DirectionalLights::Initialize() {
    lights_.resize(lightNum);
    for (int i = 0; i < lightNum;i++) {
        lights_[i].shadowMap_.Create(shadowWidth, shadowHeight, DXGI_FORMAT_D32_FLOAT);
        lights_[i].constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
        lights_[i].descriptorHeapIndex = lights_[i].shadowMap_.GetSRV().GetIndex();
    }
    

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

void DirectionalLights::Update() {

    // 定数バッファに書き込み
    std::vector<ConstBufferData> bufferDatas;
    bufferDatas.reserve(lightNum);

    for (int i = 0; i < lightNum; i++) {

        Matrix4x4 viewMatrix = MakeLookRotationMatrix(lights_[i].direction, lights_[i].lockUp);
        viewMatrix.m[3][0] = lights_[i].position.x;
        viewMatrix.m[3][1] = lights_[i].position.y;
        viewMatrix.m[3][2] = lights_[i].position.z;
        viewMatrix = Inverse(viewMatrix);

        ConstBufferData bufferData;
        bufferData.color = lights_[i].color;
        bufferData.direction = Normalize(lights_[i].direction);
        bufferData.intensity = lights_[i].intensity;
        bufferData.viewProjection = viewMatrix * MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
        bufferData.descriptorHeapIndex = lights_[i].descriptorHeapIndex;

        lights_[i].constBuffer_.Copy(bufferData);
        bufferDatas.emplace_back(bufferData);
    }

    structureBuffer_.Copy(bufferDatas.data(), sizeof(bufferDatas[0]) * bufferDatas.size());
}
