#pragma once
#include "GameObject.h"
#include "Collider.h"

// 中間地点用のオブジェクト
class SavePoint : public GameObject {
public:
	SavePoint();
	~SavePoint();
	void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
	void Update(const Vector3& playerWorldPosition);
	void Draw();
	void DrawImGui();
	const uint32_t& GetNumber() const { return kNumber_; }
public:
	Collider collider_;
	Vector3 rotate;
	static uint32_t masterNumber_;
private:
	bool ActiveChack(const Vector3& playerWorldPosition) const;
	bool isActive_ = true;
	const uint32_t kNumber_ = 0u;

};

