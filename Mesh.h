#pragma once
#include <string>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>
class Mesh
{
	friend class ModelManager;
public:

	D3D12_INDEX_BUFFER_VIEW* GetIbView() {
		return &ibView_;
	}

	uint32_t GetUv() {
		return uvHandle_;
	}

private:
	UploadBuffer indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint32_t> indices_;

	std::string name_;
	uint32_t uvHandle_ = 0;
};