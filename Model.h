#pragma once
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Windows.h>
#include <d3d12.h>
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <wrl.h>
#include "Mymath.h"
#include "DirectionalLights.h"
#include "Material.h"
#include <string>
#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "Mesh.h"

class DirectXCommon;

class Model
{
public:
	enum class RootParameter {
		kWorldTransform, 
		kViewProjection, 
		kTexture,        
		kDirectionalLights, 
		kMaterial,
	};

	static void StaticInitialize();
	static void PreDraw(ID3D12GraphicsCommandList* commandList, const ViewProjection& viewProjection, const DirectionalLights& directionalLight);
	static void PostDraw();

	static void Draw(uint32_t modelHandle,const WorldTransform& worldTransform, const Material& material);

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material, uint32_t textureHadle);

private: 
	static void CreatePipeline();
private:
	static ID3D12GraphicsCommandList* commandList_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static std::unique_ptr<PipelineState> wireFramePipelineState_;
};

