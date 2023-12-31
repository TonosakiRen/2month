#pragma once

#include <array>
#include "CommandContext.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include <vector>
#include "Mesh.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"

class ModelManager
{
public:
	static const size_t kNumModels = 256;

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct ModelIndex {
		std::string name;
		std::vector<Mesh> meshes;
		UploadBuffer vertexBuffer_;
		std::vector<VertexData> vertices_;
		D3D12_VERTEX_BUFFER_VIEW vbView_{};
		//model全体のsize
		Vector3 modelSize{};
		Vector3 modelCenter{};
		uint32_t meshNum = 0;
	};


	static uint32_t Load(const std::string& fileName);

	static void CreateMeshes(ModelIndex& modelIndex);

	static ModelManager* GetInstance();

	Vector3 GetModelSize(uint32_t modelHandle);

	Vector3 GetModelCenter(uint32_t modelHandle);

	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle);
	void DrawInstanced(CommandContext* commandContext,uint32_t modelHandle,UINT textureRootParamterIndex);
	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex,uint32_t textureHandle);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	std::array<ModelIndex, kNumModels> models_;
	uint32_t useModelCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);

};

