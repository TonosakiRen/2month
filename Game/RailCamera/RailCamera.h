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
	float DistanceToSegment(const Vector3& point, const Vector3& p1, const Vector3& p2, float& t);

	int FindClosestSegment(const Vector3& playerPosition, const std::vector<Vector3>& controlPoints, float& tOnSegment);

	WorldTransform worldTransform_;

};