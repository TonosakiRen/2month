#pragma once
#include "GameObject.h"
#include "Collider.h"

class ControlPoint : public GameObject {
public:
    void Initialize(Quaternion quaternion, Vector3 translate);
    void Update();
    void Draw();
    void DrawImGui();

public:
    Collider collider_;
    Vector3 rotate;
};
