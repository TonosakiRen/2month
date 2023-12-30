#pragma once
#include "GameObject.h"
class Skydome :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update(const Vector3& translate = Vector3(0.0f,0.0f,0.0f));

    void SetParentTranslate(WorldTransform* transform) { parentTranslate_ = transform; }
    const WorldTransform* parentTranslate_;

};