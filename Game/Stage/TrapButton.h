#pragma once
#include "GameObject.h"
#include "Collider.h"

// モンスターハウスを呼び出すためのオブジェクト
class TrapButton : public GameObject {
public:
	TrapButton();
	~TrapButton();
	void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
	void Update(const Vector3& playerWorldPosition);
	void Draw();
	void DrawImGui();
	const uint32_t& GetNumber() const { return kNumber_; }
	const Vector3& GetWorldPosition() const;
public:
	Collider collider_;
	Vector3 rotate;
	uint32_t kNumber_ = 0u;
private:
	bool ActiveChack(const Vector3& playerWorldPosition) const;
	bool isActive_ = true;


};

