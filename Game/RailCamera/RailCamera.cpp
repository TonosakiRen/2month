#include "RailCamera.h"
#include <algorithm>
#include "Game/Camera/FollowCamera.h"

void RailCamera::Initialize(const WorldTransform& transform) {
	start_.translate = transform.GetWorldTranslate();
	start_.rotate = transform.quaternion_;
	count_ = 0.0f;
	isEase_ = true;
}

void RailCamera::Update(const Vector3& playerTranslate, std::vector <std::shared_ptr<ControlPoint>> points) {
	float T = 0.0f;
	std::vector<Vector3> positions;
	for (auto& point : points) {
		Vector3 trans = point->GetWorldTransform()->GetWorldTranslate();
		trans.y = playerTranslate.y;
		trans.z = playerTranslate.z;
		positions.push_back(trans);
	}
	int closestSegment = FindClosestSegment(playerTranslate, positions, T);
	
	int p0 = closestSegment - 1;
	if (p0 <= 0) { p0 = 0; }
	int p2 = closestSegment + 1;
	if (p2 >= static_cast<int>(points.size())) { p2 = static_cast<int>(points.size()) - 1; }
	int p3 = closestSegment + 2;
	if (p3 >= static_cast<int>(points.size())) { p3 = static_cast<int>(points.size()) - 1; }


	// 座標を取得
	worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
	
	worldTransform_.translation_ = CatmullRomSpline(T, points.at(p0)->GetWorldTransform()->GetWorldTranslate(), points.at(closestSegment)->GetWorldTransform()->GetWorldTranslate(),
			points.at(p2)->GetWorldTransform()->GetWorldTranslate(), points.at(p3)->GetWorldTransform()->GetWorldTranslate());
	prePosition_ = Lerp(prePosition_, worldTransform_.translation_, 0.95f);
	worldTransform_.translation_ = prePosition_;


	worldTransform_.quaternion_ = Slerp(T, points.at(closestSegment)->GetWorldTransform()->quaternion_, points.at(p2)->GetWorldTransform()->quaternion_);
	worldTransform_.Update();


	if (isEase_) {
		float MaxFrame = 20.0f;
		Vector3 shake(0.5f, 0.5f, 0.0f);
		shake = FollowCamera::Shake(shake);
		float Time = count_ / MaxFrame;
		Time = std::clamp(Time, 0.0f, 1.0f);

		worldTransform_.translation_ = Lerp(start_.translate, worldTransform_.translation_, Time) + shake;
		worldTransform_.quaternion_ = Slerp(Time, start_.rotate, worldTransform_.quaternion_);
		worldTransform_.Update();
		if (count_++ >= MaxFrame) {
			count_ = MaxFrame;
			isEase_ = false;
		}
	}

}

float RailCamera::DistanceToSegment(const Vector3& point, const Vector3& p1, const Vector3& p2, float& t) {
	Vector3 segment = p2 - p1;
	//segment.z = 0.0f;
	Vector3 toPoint = point - p1;
	//toPoint.z = 0.0f;
	float segmentLengthSquared = Dot(segment, segment); // 線分の長さの二乗
	if (segmentLengthSquared == 0.0f) {
		// P1とP2が同じ点の場合（稀なケース）
		t = 0.0f;
		return Distance(point, p1);
	}

	// 線分上の最も近い点を決定するためのt（補間変数）
	t = Dot(toPoint, segment) / segmentLengthSquared;
	t = std::clamp(t, 0.0f, 1.0f); // tは0から1の範囲にクランプ

	// 線分上の最近傍の点を計算
	Vector3 projection = p1 + t * segment;

	// 点と最近傍の点との距離を計算
	return Distance(point, projection);
}

int RailCamera::FindClosestSegment(const Vector3& playerPosition, const std::vector<Vector3>& controlPoints, float& tOnSegment) {
	int closestSegment = -1;
	float minDistance = D3D12_FLOAT32_MAX;

	// 各セグメントをチェック
	for (int32_t i = 0; i < controlPoints.size() - 1; ++i) {
		float t = 0.0f; // セグメント内の相対位置
		float lDistance = DistanceToSegment(playerPosition, controlPoints[i], controlPoints[i + 1], t);

		// 最も近いセグメントを記録
		if (lDistance < minDistance && t >= 0.0f && t <= 1.0f + 0.05f) {
			minDistance = lDistance;
			closestSegment = i;
			tOnSegment = t; // 最も近いセグメント内での相対位置を記録
		}
	}

	return closestSegment;
}
