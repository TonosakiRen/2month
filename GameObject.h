#pragma once
#include "ViewProjection.h"
#include "Material.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "ViewProjection.h"
#include <string>
class GameObject
{
public:
	void Initialize(const std::string name);
	void UpdateMatrix();
	void UpdateMaterial(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(uint32_t textureHandle ,Vector4 color = { 1.0f,1.0f,1.0f,1.0f } );
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(const WorldTransform& worldTransform, uint32_t textureHandle, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(const WorldTransform& worldTransform, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	void SetEnableLighting(bool enableLighting) {
		material_.enableLighting_ = enableLighting;
	}
	void SetParent(WorldTransform* parent) {
		worldTransform_.SetParent(parent);
	}
	void SetScale(Vector3 scale) {
		worldTransform_.scale_ = scale;
	}
	void SetPosition(Vector3 position) {
		worldTransform_.translation_ = position;
	}
	WorldTransform* GetWorldTransform() {
		return &worldTransform_;
	}
protected:
	uint32_t modelHandle_;
	WorldTransform worldTransform_;
	Material material_;
};