#pragma once
#include "ParticleBox.h"
class HitParticle
{
public:
	static const uint32_t kParticleNum = 100;
	HitParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw();
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetSpeed(float speed) {
		speed_ = speed;
	}
	void SetScaleSpeed(float scaleSpeed) {
		scaleSpeed_ = scaleSpeed;
	}
	struct Particle {
		WorldTransform worldTransform_;
		Vector3 direction_;
		Vector3 velocity_;
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	Particle particles[kParticleNum];
	int EmitNum_ = 4;
	std::unique_ptr<ParticleBox> particleBox_;
private:
	float speed_ = 0.2f;
	float scaleSpeed_ = 0.02f;
	bool isEmit_ = false;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	uint32_t modelHandle_;
};