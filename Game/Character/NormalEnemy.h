#pragma once
#include "BaseCharacter.h"

class NormalEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) override;
	void Update() override;
	void Draw() override;

private:
	void Move();

	int count = 0;


};

