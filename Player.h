#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Collider.h"
class Player :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Collision(Collider& otherCollider);
    void Draw();
public:
    Collider collider_;
private:
    WorldTransform modelWorldTransform_;
    Vector3 velocisity_;
    Vector3 acceleration_;
    Vector3 modelSize_;
    Input* input_;
};