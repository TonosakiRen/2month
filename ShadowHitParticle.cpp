#include "ShadowHitParticle.h"
#include "ImGuiManager.h"

ShadowHitParticle::ShadowHitParticle()
{
	particle_ = std::make_unique<Particle>(kParticleNum);
}

void ShadowHitParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{

	particle_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.scale_ = { 4.0f,1.0f,1.0f };
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
	emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);
	emitBox_.size = { 0.1f,0.1f,0.0f };
	particle_->material_.enableLighting_ = false;
}

void ShadowHitParticle::Update() {


	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t j = 0; j < kParticleNum; j++) {
				if (particles[j].isActive_ == false) {
					particles[j].isActive_ = true;

					emitterWorldTransform_.Update();
					emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);

					if (emitterWorldTransform_.GetParent()) {
						particles[j].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[j].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					Vector3 randVec = MakeRandVector3(emitBox_);
					particles[j].worldTransform_.translation_ = randVec;
					particles[j].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[j].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					particles[j].aliveFrame_ = 60;
					break;
				}
			}
		}
		isEmit_ = false;
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(2.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
		//	particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 0.0f,0.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].aliveFrame_--;
			particles[i].worldTransform_.Update();
			if (particles[i].aliveFrame_ <= 0) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void ShadowHitParticle::Draw( uint32_t textureHandle)
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
		particle_->Draw(instancingBufferDatas, particle_->material_.color_, textureHandle);
	}
}
