#pragma once
#include "GameObject.h"
#include "Collider.h"
class MoveFloor : public GameObject {
public:
    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
    void Initialize(const Vector3& startPos, const Vector3& endPos, const Quaternion& quaternion, const Vector3& scale, const float& speed);
    void Update();
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

};