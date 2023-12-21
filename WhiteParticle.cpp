#include "WhiteParticle.h"
#include "ImGuiManager.h"

WhiteParticle::WhiteParticle()
{
	particle_ = std::make_unique<Particle>(kParticleNum);
}

void WhiteParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{

	particle_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
	emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);
	emitBox_.size = { 1.0f,1.0f,1.0f };
}

void WhiteParticle::Update() {


	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;

					emitterWorldTransform_.Update();
					emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);

					if (emitterWorldTransform_.GetParent()) {
						particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					particles[i].worldTransform_.translation_ = MakeRandVector3(emitBox_);
					particles[i].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(2.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 0.0f,0.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			particles[i].worldTransform_.Update();
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void WhiteParticle::Draw(Vector4 color, uint32_t textureHandle)
{

	emitterWorldTransform_.Update();

	std::vector<Particle::InstancingBufferData> instancingBufferDatas;
	instancingBufferDatas.reserve(kParticleNum);

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive_) {
			instancingBufferDatas.emplace_back(particles[i].worldTransform_.matWorld_);
		}
	}

	if (!instancingBufferDatas.empty()) {
		particle_->Draw(instancingBufferDatas, color, textureHandle);
	}
}
