#pragma once
#include "GameObject.h"
#include "Collider.h"
class MoveFloor : public GameObject {
public:
    void Initialize(const Vector3& startPos, const Vector3& endPos, const Quaternion& quaternion, const Vector3& scale, const float& speed);
    void Update(const Vector3& playerWorldPosition);
    void Draw();
    void DrawImGui();

private:
    void Move();
public:
    Collider collider_;

private:
    Vector3 rotate;

    struct MoveParameter {
        Vector3 startPos_;
        Vector3 endPos_;
        float speed_ = 1.0f;
        float T = 0.0f;
    };
    MoveParameter moveParam_;
public:
    MoveParameter GetParam() const { return moveParam_; }
private:
    bool ActiveChack(const Vector3& playerWorldPosition) const;
    bool isActive_ = true;

};