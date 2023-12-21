#include "DustParticle.h"
#include "ImGuiManager.h"

DustParticle::DustParticle()
{
	particleBox_ = std::make_unique<ParticleBox>(kParticleNum);
}

void DustParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{

	particleBox_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
}

void DustParticle::Update() {

	emitterWorldTransform_.Update();

	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
					if (emitterWorldTransform_.GetParent()) {
						particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					particles[i].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[i].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 1.0f,1.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void DustParticle::Draw( Vector4 color)
{

	emitterWorldTransform_.Update();

	std::vector<ParticleBox::InstancingBufferData> instancingBufferDatas;
	instancingBufferDatas.reserve(kParticleNum);

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive_) {
			particles[i].worldTransform_.Update();
			instancingBufferDatas.emplace_back(particles[i].worldTransform_.matWorld_);
		}
	}

	if (!instancingBufferDatas.empty()) {
		particleBox_->Draw(instancingBufferDatas, color);
	}
}