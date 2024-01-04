#pragma once
#include "GameObject.h"
#include "Collider.h"
class Wall :
    public GameObject
{
public:

    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
    void Update();
    void Draw();
    void DrawImGui();

public:
    Collider collider_;
};