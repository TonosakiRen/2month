#pragma once
#include "GameObject.h"
#include <string>
class Collider
{
public:

	static bool isDrawCollider;
	static bool isDrawSphere;
	static void SwitchIsDrawCollider();

	enum Shape {
		kOBB,
		kSphere
	};

	void Initialize(WorldTransform* objectWorldTransform, const std::string name, Vector3 initialScale = { 1.0f,1.0f,1.0f }, Vector3 initialPos = { 0.0f,0.0f,0.0f });
	void Initialize(WorldTransform* objectWorldTransform, const std::string name,uint32_t modelHandle);
	void Initialize(WorldTransform* objectWorldTransform, const std::string name, uint32_t modelHandle,Vector3 initialPos);
	//ただの四角形用
	void Initialize(const std::string name);
	bool Collision(Collider& colliderB,Shape shape = kOBB);
	bool Collision(Collider& colliderB, Vector3& pushBuckVector, Shape shape = kOBB);

	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	std::string GetName() const { return name_; }

	void AdjustmentScale();
	void MatrixUpdate();

private:
	bool BoxSphereCollision(Vector3 position1,Vector3 size1, Vector3 position2, Vector3 size2);
	bool SphereCollision(Collider& colliderB);
	bool SphereCollision(Collider& colliderB ,Vector3& pushBuckVector);

	bool ObbCollision(const OBB& obb1, const OBB& obb2);
	bool ObbCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap);

public:
	WorldTransform worldTransform_;
private:
	GameObject cube_;
	GameObject sphere_;
	std::string name_;

};