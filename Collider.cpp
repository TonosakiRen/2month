#include "Collider.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "Input.h"

bool Collider::isDrawCollider = false;

void Collider::SwitchIsDrawCollider()
{
#ifdef _DEBUG
	ImGui::Begin("Collider");
	if (ImGui::Button("SwitchColliderDraw")) {
		if (Collider::isDrawCollider == false) {
			Collider::isDrawCollider = true;
		}
		else {
			Collider::isDrawCollider = false;
		}
	}
	ImGui::End();
#endif 
}

void Collider::Initialize(WorldTransform* objectWorldTransform, const std::string name, Vector3 initialScale, Vector3 initialPos)
{
	worldTransform_.Initialize();
	worldTransform_.SetParent(objectWorldTransform);
	worldTransform_.scale_ = initialScale;
	worldTransform_.translation_ = initialPos;
	name_ = name;
	cube_.Initialize("box1x1");
}

void Collider::Initialize(WorldTransform* objectWorldTransform, const std::string name,uint32_t modelHandle)
{
	worldTransform_.Initialize();
	worldTransform_.SetParent(objectWorldTransform);
	worldTransform_.scale_ = ModelManager::GetInstance()->GetModelSize(modelHandle);
	worldTransform_.translation_ = ModelManager::GetInstance()->GetModelCenter(modelHandle);
	name_ = name;
	cube_.Initialize("box1x1");
}

void Collider::AdjustmentScale()
{
#ifdef _DEBUG

	const std::string col = "collider";
	ImGui::Begin((name_ + col).c_str());
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::End();
#endif

	MatrixUpdate();
}

void Collider::Initialize(const std::string name)
{
	worldTransform_.Initialize();
	name_ = name;
	cube_.Initialize("box1x1");
	cube_.SetEnableLighting(false);
}

bool Collider::Collision(Collider& colliderB, Vector3& minAxis, float& minOverlap)
{

	MatrixUpdate();
	colliderB.MatrixUpdate();
	OBB obb1;
	OBB obb2;
	obb1.center = MakeTranslation(worldTransform_.matWorld_);
	obb2.center = MakeTranslation(colliderB.worldTransform_.matWorld_);
	obb1.size = MakeScale(worldTransform_.matWorld_) / 2.0f;
	obb2.size = MakeScale(colliderB.worldTransform_.matWorld_) / 2.0f;
	if (!SphereCollision(obb1.center, obb1.size, obb2.center, obb2.size)) {
		minAxis = { 0.0f,0.0f,0.0f };
		minOverlap = 0.0f;
		return false;
	}
	Matrix4x4 rotateMatrix1 = NormalizeMakeRotateMatrix(worldTransform_.matWorld_);
	Matrix4x4 rotateMatrix2 = NormalizeMakeRotateMatrix(colliderB.worldTransform_.matWorld_);
	obb1.orientations[0] = GetXAxis(rotateMatrix1);
	obb1.orientations[1] = GetYAxis(rotateMatrix1);
	obb1.orientations[2] = GetZAxis(rotateMatrix1);
	obb2.orientations[0] = GetXAxis(rotateMatrix2);
	obb2.orientations[1] = GetYAxis(rotateMatrix2);
	obb2.orientations[2] = GetZAxis(rotateMatrix2);

	return ObbCollision(obb1, obb2, minAxis, minOverlap);
}

bool Collider::Collision(Collider& colliderB, Vector3& pushBuckVector)
{
	MatrixUpdate();
	colliderB.MatrixUpdate();
	OBB obb1;
	OBB obb2;
	obb1.center = MakeTranslation(worldTransform_.matWorld_);
	obb2.center = MakeTranslation(colliderB.worldTransform_.matWorld_);
	obb1.size = MakeScale(worldTransform_.matWorld_) / 2.0f;
	obb2.size = MakeScale(colliderB.worldTransform_.matWorld_) / 2.0f;
	if (!SphereCollision(obb1.center, obb1.size, obb2.center, obb2.size)) {
 		pushBuckVector = { 0.0f,0.0f,0.0f };
		return false;
	}
	Matrix4x4 rotateMatrix1 = NormalizeMakeRotateMatrix(worldTransform_.matWorld_);
	Matrix4x4 rotateMatrix2 = NormalizeMakeRotateMatrix(colliderB.worldTransform_.matWorld_);
	obb1.orientations[0] = GetXAxis(rotateMatrix1);
	obb1.orientations[1] = GetYAxis(rotateMatrix1);
	obb1.orientations[2] = GetZAxis(rotateMatrix1);
	obb2.orientations[0] = GetXAxis(rotateMatrix2);
	obb2.orientations[1] = GetYAxis(rotateMatrix2);
	obb2.orientations[2] = GetZAxis(rotateMatrix2);

	Vector3 minAxis = { 0.0f,0.0f,0.0f };
	float minOverlap = 0.0f;

	if (ObbCollision(obb1, obb2, minAxis, minOverlap)) {
		float dot = Dot(MakeTranslation(colliderB.worldTransform_.matWorld_) - MakeTranslation(this->worldTransform_.matWorld_), minAxis);
		if (dot > 0.0f) {
			minOverlap = -minOverlap;
		}
		pushBuckVector = Normalize(minAxis) * minOverlap;
		return true;
	}
	else {
		pushBuckVector = { 0.0f,0.0f,0.0f };
		return false;
	}
	return false;
}

bool Collider::SphereCollision(Vector3 position1, Vector3 size1, Vector3 position2, Vector3 size2)
{
	float radius1 = Length(size1) ;
	float radius2 = Length(size2) ;
	float distance = Length(position1 - position2);
	if (distance < radius2 + radius1) {
		return true;
	}
	return false;
}

bool Collider::Collision(Collider& colliderB)
{

	MatrixUpdate();
	colliderB.MatrixUpdate();
	OBB obb1;
	OBB obb2;
	obb1.center = MakeTranslation(worldTransform_.matWorld_);
	obb2.center = MakeTranslation(colliderB.worldTransform_.matWorld_);
	obb1.size = MakeScale(worldTransform_.matWorld_) / 2.0f;
	obb2.size = MakeScale(colliderB.worldTransform_.matWorld_) / 2.0f;
	if (!SphereCollision(obb1.center, obb1.size, obb2.center, obb2.size)) {
		return false;
	}
	Matrix4x4 rotateMatrix1 = NormalizeMakeRotateMatrix(worldTransform_.matWorld_);
	Matrix4x4 rotateMatrix2 = NormalizeMakeRotateMatrix(colliderB.worldTransform_.matWorld_);
	obb1.orientations[0] = GetXAxis(rotateMatrix1);
	obb1.orientations[1] = GetYAxis(rotateMatrix1);
	obb1.orientations[2] = GetZAxis(rotateMatrix1);
	obb2.orientations[0] = GetXAxis(rotateMatrix2);
	obb2.orientations[1] = GetYAxis(rotateMatrix2);
	obb2.orientations[2] = GetZAxis(rotateMatrix2);

	return ObbCollision(obb1, obb2);
}

void Collider::MatrixUpdate()
{
	// スケール、回転、平行移動行列の計算
	Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransform_.scale_);

	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	rotateMatrix = MakeRotateMatrix(worldTransform_.quaternion_);
	Matrix4x4 translationMatrix = MakeTranslateMatrix(worldTransform_.translation_);


	// 親行列の指定がある場合は、掛け算する
	if (worldTransform_.GetParent()) {

		// ワールド行列の合成
		worldTransform_.matWorld_ = MakeIdentity4x4();
		worldTransform_.matWorld_ *= scaleMatrix;
		worldTransform_.matWorld_ *= rotateMatrix;
		worldTransform_.matWorld_ *= translationMatrix;

		worldTransform_.matWorld_ *= worldTransform_.GetParent()->matWorld_;
	}
	else {
		// ワールド行列の合成
		worldTransform_.matWorld_ = MakeIdentity4x4();
		worldTransform_.matWorld_ *= scaleMatrix;
		worldTransform_.matWorld_ *= rotateMatrix;
		worldTransform_.matWorld_ *= translationMatrix;

	}

	// 定数バッファに書き込み
	worldTransform_.ConstUpdate();
}

void Collider::Draw(Vector4 color)
{

	if (isDrawCollider) {
		MatrixUpdate();
		cube_.Draw(worldTransform_ ,color);
	}

}

bool Collider::ObbCollision(const OBB& obb1, const OBB& obb2)
{
	Vector3 separationAxisCandidate[15];
	// 各軸
	separationAxisCandidate[0] = obb1.orientations[0];
	separationAxisCandidate[1] = obb1.orientations[1];
	separationAxisCandidate[2] = obb1.orientations[2];
	separationAxisCandidate[3] = obb2.orientations[0];
	separationAxisCandidate[4] = obb2.orientations[1];
	separationAxisCandidate[5] = obb2.orientations[2];
	// 各辺のクロス積
	separationAxisCandidate[6] = Cross(obb1.orientations[0], obb2.orientations[0]);
	separationAxisCandidate[7] = Cross(obb1.orientations[0], obb2.orientations[1]);
	separationAxisCandidate[8] = Cross(obb1.orientations[0], obb2.orientations[2]);
	separationAxisCandidate[9] = Cross(obb1.orientations[1], obb2.orientations[0]);
	separationAxisCandidate[10] = Cross(obb1.orientations[1], obb2.orientations[1]);
	separationAxisCandidate[11] = Cross(obb1.orientations[1], obb2.orientations[2]);
	separationAxisCandidate[12] = Cross(obb1.orientations[2], obb2.orientations[0]);
	separationAxisCandidate[13] = Cross(obb1.orientations[2], obb2.orientations[1]);
	separationAxisCandidate[14] = Cross(obb1.orientations[2], obb2.orientations[2]);
	// 頂点
	Matrix4x4 obb1WorldMatrix = {
		obb1.orientations[0].x, obb1.orientations[0].y, obb1.orientations[0].z, 0.0f,
		obb1.orientations[1].x, obb1.orientations[1].y, obb1.orientations[1].z, 0.0f,
		obb1.orientations[2].x, obb1.orientations[2].y, obb1.orientations[2].z, 0.0f,
		obb1.center.x,          obb1.center.y,          obb1.center.z,          1.0f };
	Matrix4x4 obb2WorldMatrix = {
		obb2.orientations[0].x, obb2.orientations[0].y, obb2.orientations[0].z, 0.0f,
		obb2.orientations[1].x, obb2.orientations[1].y, obb2.orientations[1].z, 0.0f,
		obb2.orientations[2].x, obb2.orientations[2].y, obb2.orientations[2].z, 0.0f,
		obb2.center.x,          obb2.center.y,          obb2.center.z,          1.0f };

	Vector3 vertices1[] = {
		-obb1.size,
		{obb1.size.x,  -obb1.size.y, -obb1.size.z},
		{obb1.size.x,  -obb1.size.y, obb1.size.z },
		{-obb1.size.x, -obb1.size.y, obb1.size.z },
		{-obb1.size.x, obb1.size.y,  -obb1.size.z},
		{obb1.size.x,  obb1.size.y,  -obb1.size.z},
		obb1.size,
		{-obb1.size.x, obb1.size.y,  obb1.size.z }
	};

	Vector3 vertices2[] = {
		-obb2.size,
		{obb2.size.x,  -obb2.size.y, -obb2.size.z},
		{obb2.size.x,  -obb2.size.y, obb2.size.z },
		{-obb2.size.x, -obb2.size.y, obb2.size.z },
		{-obb2.size.x, obb2.size.y,  -obb2.size.z},
		{obb2.size.x,  obb2.size.y,  -obb2.size.z},
		obb2.size,
		{-obb2.size.x, obb2.size.y,  obb2.size.z }
	};

	for (int i = 0; i < 8; i++) {
		vertices1[i] = vertices1[i] * obb1WorldMatrix;
		vertices2[i] = vertices2[i] * obb2WorldMatrix;
	}

	float minOverlap = FLT_MAX;
	int axisIndex = 0;

	// 各軸
	for (int i = 0; i < 15; i++) {
		// 影の長さの合計
		float sumSpan;
		// 2つの影の両端の差分
		float longSpan;
		// 射影した最大値最小値
		float max1, min1, max2, min2;
		// 差分が形状を分離軸に射影した長さ
		float L1, L2;
		// すべての頂点を射影した値
		float Dot1[8];
		float Dot2[8];
		// 各頂点
		for (int j = 0; j < 8; j++) {
			Dot1[j] = Dot(separationAxisCandidate[i], vertices1[j]);
		}
		for (int k = 0; k < 8; k++) {
			Dot2[k] = Dot(separationAxisCandidate[i], vertices2[k]);
		}
		max1 = (std::max)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		min1 = (std::min)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		L1 = max1 - min1;
		max2 = (std::max)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		min2 = (std::min)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		L2 = max2 - min2;

		sumSpan = L1 + L2;
		longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

		if (sumSpan < longSpan) {
			return false;
		}
		float overlap = sumSpan - longSpan;
		if (overlap < minOverlap) {
			minOverlap = overlap;
			axisIndex = i;
		}
	}


	return true;
}

bool Collider::ObbCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap)
{
	Vector3 separationAxisCandidate[15];
	// 各軸
	separationAxisCandidate[0] = obb1.orientations[0];
	separationAxisCandidate[1] = obb1.orientations[1];
	separationAxisCandidate[2] = obb1.orientations[2];
	separationAxisCandidate[3] = obb2.orientations[0];
	separationAxisCandidate[4] = obb2.orientations[1];
	separationAxisCandidate[5] = obb2.orientations[2];
	// 各辺のクロス積
	separationAxisCandidate[6] = Normalize(Cross(obb1.orientations[0], obb2.orientations[0]));
	separationAxisCandidate[7] = Normalize(Cross(obb1.orientations[0], obb2.orientations[1]));
	separationAxisCandidate[8] = Normalize(Cross(obb1.orientations[0], obb2.orientations[2]));
	separationAxisCandidate[9] = Normalize(Cross(obb1.orientations[1], obb2.orientations[0]));
	separationAxisCandidate[10] = Normalize(Cross(obb1.orientations[1], obb2.orientations[1]));
	separationAxisCandidate[11] = Normalize(Cross(obb1.orientations[1], obb2.orientations[2]));
	separationAxisCandidate[12] = Normalize(Cross(obb1.orientations[2], obb2.orientations[0]));
	separationAxisCandidate[13] = Normalize(Cross(obb1.orientations[2], obb2.orientations[1]));
	separationAxisCandidate[14] = Normalize(Cross(obb1.orientations[2], obb2.orientations[2]));
	// 頂点
	Matrix4x4 obb1WorldMatrix = {
		obb1.orientations[0].x, obb1.orientations[0].y, obb1.orientations[0].z, 0.0f,
		obb1.orientations[1].x, obb1.orientations[1].y, obb1.orientations[1].z, 0.0f,
		obb1.orientations[2].x, obb1.orientations[2].y, obb1.orientations[2].z, 0.0f,
		obb1.center.x,          obb1.center.y,          obb1.center.z,          1.0f };
	Matrix4x4 obb2WorldMatrix = {
		obb2.orientations[0].x, obb2.orientations[0].y, obb2.orientations[0].z, 0.0f,
		obb2.orientations[1].x, obb2.orientations[1].y, obb2.orientations[1].z, 0.0f,
		obb2.orientations[2].x, obb2.orientations[2].y, obb2.orientations[2].z, 0.0f,
		obb2.center.x,          obb2.center.y,          obb2.center.z,          1.0f };

	Vector3 vertices1[] = {
		-obb1.size,
		{obb1.size.x,  -obb1.size.y, -obb1.size.z},
		{obb1.size.x,  -obb1.size.y, obb1.size.z },
		{-obb1.size.x, -obb1.size.y, obb1.size.z },
		{-obb1.size.x, obb1.size.y,  -obb1.size.z},
		{obb1.size.x,  obb1.size.y,  -obb1.size.z},
		obb1.size,
		{-obb1.size.x, obb1.size.y,  obb1.size.z }
	};

	Vector3 vertices2[] = {
		-obb2.size,
		{obb2.size.x,  -obb2.size.y, -obb2.size.z},
		{obb2.size.x,  -obb2.size.y, obb2.size.z },
		{-obb2.size.x, -obb2.size.y, obb2.size.z },
		{-obb2.size.x, obb2.size.y,  -obb2.size.z},
		{obb2.size.x,  obb2.size.y,  -obb2.size.z},
		obb2.size,
		{-obb2.size.x, obb2.size.y,  obb2.size.z }
	};

	for (int i = 0; i < 8; i++) {
		vertices1[i] = vertices1[i] * obb1WorldMatrix;
		vertices2[i] = vertices2[i] * obb2WorldMatrix;
	}

	float minoverlap = FLT_MAX;
	int axisIndex = 0;

	// 各軸
	for (int i = 0; i < 15; i++) {
		if (separationAxisCandidate[i].x == 0.0f && separationAxisCandidate[i].y == 0.0f && separationAxisCandidate[i].z == 0.0f) {
			continue;
		}
		// 影の長さの合計
		float sumSpan;
		// 2つの影の両端の差分
		float longSpan;
		// 射影した最大値最小値
		float max1, min1, max2, min2;
		// 差分が形状を分離軸に射影した長さ
		float L1, L2;
		// すべての頂点を射影した値
		float Dot1[8];
		float Dot2[8];
		// 各頂点
		for (int j = 0; j < 8; j++) {
			Dot1[j] = Dot(separationAxisCandidate[i], vertices1[j]);
		}
		for (int k = 0; k < 8; k++) {
			Dot2[k] = Dot(separationAxisCandidate[i], vertices2[k]);
		}
		max1 = (std::max)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		min1 = (std::min)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		L1 = max1 - min1;
		max2 = (std::max)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		min2 = (std::min)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		L2 = max2 - min2;

		sumSpan = L1 + L2;
		longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

		if (sumSpan < longSpan) {
			return false;
		}
		float overlap = sumSpan - longSpan;
		if (overlap < minoverlap) {
			minoverlap = overlap;
			axisIndex = i;
		}
	}

	minAxis = separationAxisCandidate[axisIndex];
	minOverlap = minoverlap;

	return true;
}