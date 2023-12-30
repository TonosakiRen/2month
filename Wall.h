#pragma once
#include "GameObject.h"
#include "Collider.h"
class Wall :
    public GameObject
{
public:

    void Initialize(Vector3 position);
    void Update();
    void Draw();
    void DrawImGui();

public:
    Collider collider_;
};