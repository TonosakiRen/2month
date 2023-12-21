#pragma once
#include "GameObject.h"
#include "Collider.h"
class Floor :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Draw();
public:
    Collider collider_;
};