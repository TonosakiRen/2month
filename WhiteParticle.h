#pragma once
#include "Particle.h"
class WhiteParticle
{
public:
	static const uint32_t kParticleNum = 300;
	WhiteParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f }, uint32_t textureHandle = 0);
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
	struct deadLineParticle {
		WorldTransform worldTransform_;
		Vector3 direction_;
		Vector3 velocity_;
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	OBB emitBox_;
	deadLineParticle particles[kParticleNum];
	int EmitNum_ = 1;
private:
	float speed_ = 0.05f;
	float scaleSpeed_ = 0.03f;
	bool isEmit_ = false;
	std::unique_ptr<Particle> particle_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
};

