#include "TextureManager.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cassert>
#include "DirectXCommon.h"
using namespace DirectX;

uint32_t TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}

uint32_t TextureManager::LoadUv(const std::string& fileName , const std::string& filePass) {
	return TextureManager::GetInstance()->LoadUvInternal(fileName , filePass);
}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(std::string directoryPath) {

	device_ = DirectXCommon::GetInstance()->GetDevice();
	directoryPath_ = directoryPath;

	// デスクリプタサイズを取得
	sDescriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t textureHandle) {
	assert(textureHandle < kNumTextures);
	return textures_[textureHandle].resource->GetDesc();
}

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex,uint32_t textureHandle) { // デスクリプタヒープの配列
	assert(textureHandle < kNumTextures);
	// シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(rootParamIndex, textures_[textureHandle].srvHandle);
}

uint32_t TextureManager::LoadInternal(const std::string& fileName) {

	assert(useTextureCount_ < kNumTextures);
	uint32_t handle = useTextureCount_;

	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(),[&](const auto& texture) {return texture.name == fileName;});

	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	// 書き込むテクスチャの参照
	Texture& texture = textures_.at(useTextureCount_);
	texture.name = fileName;

	// ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

	// ユニコード文字列に変換
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WICテクスチャのロード
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(texture.resource.GetAddressOf()));

	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); 
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr,              
			img->pixels,          
			(UINT)img->rowPitch, 
			(UINT)img->slicePitch 
		);
		assert(SUCCEEDED(result));
	}

	// シェーダリソースビュー作成
	texture.srvHandle = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	/*texture.srvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DirectXCommon::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), DirectXCommon::GetInstance()->GetSrvHeapCount(), sDescriptorHandleIncrementSize_);
	texture.gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(DirectXCommon::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart(), DirectXCommon::GetInstance()->GetSrvHeapCount(), sDescriptorHandleIncrementSize_);*/

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; 
	srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;

	device_->CreateShaderResourceView(
		texture.resource, 
		&srvDesc,               
		texture.srvHandle);

	useTextureCount_++;
	return handle;
}

uint32_t TextureManager::LoadUvInternal(const std::string& fileName, const std::string& filePass) {

	assert(useTextureCount_ < kNumTextures);
	uint32_t handle = useTextureCount_;

	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {return texture.name == fileName; });

	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	// 書き込むテクスチャの参照
	Texture& texture = textures_.at(useTextureCount_);
	texture.name = fileName;

	// ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : filePass;

	// ユニコード文字列に変換
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WICテクスチャのロード
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(texture.resource.GetAddressOf()));

	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);
		assert(SUCCEEDED(result));
	}

	// シェーダリソースビュー作成
	texture.srvHandle = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	/*texture.cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(DirectXCommon::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), DirectXCommon::GetInstance()->GetSrvHeapCount(), sDescriptorHandleIncrementSize_);
	texture.gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(DirectXCommon::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart(), DirectXCommon::GetInstance()->GetSrvHeapCount(), sDescriptorHandleIncrementSize_);*/

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;

	device_->CreateShaderResourceView(
		texture.resource,
		&srvDesc,
		texture.srvHandle);

	useTextureCount_++;
	return handle;
}

