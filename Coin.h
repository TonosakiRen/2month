#pragma once
#include "GameObject.h"
#include "Collider.h"
class Coin :
    public GameObject
{
public:

    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
    void Update();
    void Draw();
    void DrawImGui();

public:
    Collider collider_;
    Vector3 rotate;
};