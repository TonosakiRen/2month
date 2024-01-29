#pragma once
#include "GameObject.h"
#include "Collider.h"
class MoveFloor : public GameObject {
public:
    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
    void Update();
    void Draw();
    void DrawImGui();

private:
    void Move();

public:
    Collider collider_;
    Vector3 rotate;


};