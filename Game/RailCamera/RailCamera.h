#pragma once
#include "Mymath.h"
#include "ControlPoint.h"
#include <vector>
#include <memory>

class RailCamera {
public:
	RailCamera() = default;
	~RailCamera() = default;

	void Initialize();

	void Update(const Vector3& playerTranslate, std::vector<std::shared_ptr<ControlPoint>> points);
	WorldTransform GetTransform() const { return worldTransform_; }

private:
	// segmentの距離とTを求める
	float DistanceToSegment(const Vector3& point, const Vector3& p1, const Vector3& p2, float& t);

	// 最近接点から最も近いsegmentを求める
	int FindClosestSegment(const Vector3& playerPosition, const std::vector<Vector3>& controlPoints, float& tOnSegment);

	WorldTransform worldTransform_;

	Vector3 oldPosition_;
	Vector3 prePosition_;

};
