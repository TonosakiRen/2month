#pragma once

#include <array>
#include "externals/DirectXTex/d3dx12.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "GPUResource.h"
#include "DescriptorHandle.h"

class TextureManager
{
public:
	static const size_t kNumTextures = 256;

	struct Texture {
		GPUResource resource;
		DescriptorHandle srvHandle;
		std::string name;
	};

	static uint32_t Load(const std::string& fileName);

	static uint32_t LoadUv(const std::string& fileName, const std::string& filePass);

	static TextureManager* GetInstance();

	void Initialize(std::string directoryPath = "Resources/textures/");

	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);

	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	ID3D12Device* device_;
	UINT sDescriptorHandleIncrementSize_ = 0u;
	std::string directoryPath_;
	std::array<Texture, kNumTextures> textures_;
	uint32_t useTextureCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);
	uint32_t LoadUvInternal(const std::string& fileName,const std::string& filePass);
	
};

