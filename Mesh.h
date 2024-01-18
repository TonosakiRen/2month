#pragma once
#include <string>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>
class Mesh
{
	friend class ModelManager;
public:

	const D3D12_INDEX_BUFFER_VIEW* GetIbView() const {
		return &ibView_;
	}

	const uint32_t GetUv() const {
		return uvHandle_;
	}

private:
	UploadBuffer indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint32_t> indices_;

	std::string name_;
	uint32_t uvHandle_ = 0;
};