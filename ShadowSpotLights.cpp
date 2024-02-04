#include "ShadowSpotLights.h"
#include "DirectXCommon.h"
#include "Player.h"
#include "ImGuiManager.h"
void ShadowSpotLights::Initialize() {
    lights_.resize(lightNum);

    for (int i = 0; i < lightNum; i++) {
        lights_[i].shadowMap_.Create(shadowWidth, shadowHeight, DXGI_FORMAT_D32_FLOAT);
        lights_[i].constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
        lights_[i].collisionData.Create(shadowWidth, shadowHeight, DXGI_FORMAT_R32G32_FLOAT);
        lights_[i].shadowDescriptorHeapIndex = lights_[i].shadowMap_.GetSRV().GetIndex();
        lights_[i].collisionDescriptorHeapIndex = lights_[i].collisionData.GetSRV().GetIndex();
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

    globalVariables_ = GlobalVariables::GetInstance();
    SetGlobalVariable();

    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(structureBuffer_, &srvDesc, srvHandle_);
}

void ShadowSpotLights::SetGlobalVariable()
{
    for (int i = 0; auto & light : lights_) {
        std::string name = "ShadowSpotLight" +  std::to_string(i);
        globalVariables_->CreateGroup(name);
        globalVariables_->AddItem(name, "translation", light.worldTransform.translation_);
        globalVariables_->AddItem(name, "direction", light.direction);
        globalVariables_->LoadFile(name);
        i++;
    }

    ApplyGlobalVariable();
}

void ShadowSpotLights::ApplyGlobalVariable()
{
    for (int i = 0; auto & light : lights_) {
        std::string name = "ShadowSpotLight" + std::to_string(i);
        light.worldTransform.translation_ = globalVariables_->GetVector3Value(name, "translation");
        light.direction = Normalize(globalVariables_->GetVector3Value(name, "direction"));
        i++;
    }
}

void ShadowSpotLights::Update() {

    //ApplyGlobalVariable();

    /*for (int i = 0; i < lightNum; i++) {
        lights_[i].playerDistance = Length(Player::playerPos_ - lights_[i].worldTransform.translation_);
        if (lights_[i].playerDistance >= 40.0f) {
            lights_[i].isActive = false;
        }
        else {
            lights_[i].isActive = true;
        }
       
    }*/

    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);

    for (int i = 0; i < lightNum; i++) {

        lights_[i].worldTransform.Update();

        Matrix4x4 viewMatrix = MakeLookRotationMatrix(lights_[i].direction, lights_[i].lockUp);
        Vector3 position = MakeTranslation(lights_[i].worldTransform.matWorld_);
        viewMatrix.m[3][0] = position.x;
        viewMatrix.m[3][1] = position.y;
        viewMatrix.m[3][2] = position.z;
        viewMatrix = Inverse(viewMatrix);

        ConstBufferData data;
        data.color = lights_[i].color;
        data.position = position;
        data.intensity = lights_[i].intensity;
        data.direction = lights_[i].direction;
        data.distance = lights_[i].distance;
        data.decay = lights_[i].decay;
        data.cosAngle = lights_[i].cosAngle;
        data.isActive = static_cast<float>(lights_[i].isActive);

        float fov = (231.0f - data.cosAngle * 100.0f * 2.0f) * std::numbers::pi_v <float> / 180.0f;

        data.viewProjection = viewMatrix * MakePerspectiveFovMatrix(fov, aspectRatio_, nearZ_, lights_[i].distance + lights_[i].shadeDistance);
        data.shadowDescriptorHeapIndex = lights_[i].shadowDescriptorHeapIndex;
        data.collisionDescriptorHeapIndex = lights_[i].collisionDescriptorHeapIndex;

        lights_[i].constBuffer_.Copy(data);
        bufferData.emplace_back(data);
    }
    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
