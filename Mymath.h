#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <numbers>
#include<cstdlib>
#include<ctime>

#pragma region 数学関係関数
inline float clamp(float num, float min, float max) {
	if (num < min) {
		return min;
	}
	if (num > max) {
		return max;
	}
	return num;
}

inline bool closeValue(float& num, float goal, float speed) {
	if (std::fabs(num - goal) < std::fabs(speed)) {
		num = goal;
		return true;
	}
	if (num < goal) {
		num += speed;
	}
	if (num > goal) {
		num -= speed;
	}
	return false;
}

inline float Radian(float degree) { return degree * std::numbers::pi_v<float> / 180.0f; }
inline float Degree(float radian) { return radian * 180.0f / std::numbers::pi_v<float>; }

inline int Rand(int min, int max) {
	if (min == 0 && max == 0) {
		return 0;
	}
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}
inline float Rand(float min, float max) {
	if (min == 0 && max == 0) {
		return 0;
	}
	int kmin = int(min * 10000);
	int kmax = int(max * 10000);

	float result = (kmin + (int)(rand() * (kmax - kmin + 1.0) / (1.0 + RAND_MAX))) / 10000.0f;
	return result;
}
inline bool Rand() { return bool(0 + (int)(rand() * (1 - 0 + 1.0) / (1.0 + RAND_MAX))); }

inline void SRAND() {
	srand((unsigned)time(NULL));
}
#pragma endregion

#pragma region	struct定義

struct Vector2 {
	float x = 0.0f;
	float y = 0.0f;
};
struct Vector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
struct Vector4 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};
struct Matrix4x4 {
	float m[4][4];
};
struct Quaternion {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};
struct Line {
	Vector3 origin{}; //始点
	Vector3 diff{};//終点への差分ベクトル
};
struct Ray {
	Vector3 origin{}; //始点
	Vector3 diff{};//終点への差分ベクトル
};
struct Segment {
	Vector3 origin{}; //始点
	Vector3 diff{};//終点への差分ベクトル
};
struct OBB {
	Vector3 center{};          // 中心点
	Vector3 orientations[3]{}; // 座標軸、正規化、直交座標
	Vector3 size{};            // 座標軸方向の長さの半分。中心から面までの距離
};
#pragma endregion

#pragma region	Vector2
#pragma region 演算子のオーバーロード
#pragma endregion
inline float Cross(Vector2 v1, Vector2 v2) {
	return v1.x * v2.y - v1.y * v2.x;
}
#pragma endregion
#pragma region	Vector3
#pragma region 演算子のオーバーロード
inline Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 tmp;
	tmp.x = v1.x + v2.x;
	tmp.y = v1.y + v2.y;
	tmp.z = v1.z + v2.z;
	return tmp;
}
inline Vector3 operator +(const Vector3& v1, const Vector3& v2) {
	return{ v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
}
inline Vector3 operator +(const Vector3& v1, const float& scalar) {
	return{ v1.x + scalar,v1.y + scalar,v1.z + scalar };
}
//減算
inline Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 tmp;
	tmp.x = v1.x - v2.x;
	tmp.y = v1.y - v2.y;
	tmp.z = v1.z - v2.z;
	return tmp;
}
inline Vector3 operator -(const Vector3& v1, const Vector3& v2) {
	return{ v1.x - v2.x,v1.y - v2.y,v1.z - v2.z };
}
inline Vector3 operator -(const Vector3& v1, const float& scalar) {
	return{ v1.x - scalar,v1.y - scalar,v1.z - scalar };
}

//スカラー倍
inline Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 tmp;
	tmp.x = v.x * scalar;
	tmp.y = v.y * scalar;
	tmp.z = v.z * scalar;
	return tmp;
}
inline Vector3 operator *(const Vector3& v, const float& scalar) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar };
}
inline Vector3 operator *(const float& scalar, const Vector3& v) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar };
}
//スカラー割り算
inline Vector3 operator /(const Vector3& v, const float& scalar) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar };
}
inline Vector3 operator /(const float& scalar, const Vector3& v) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar };
}
inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}
inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

inline Vector3 operator -(const Vector3& v1) {
	return{ -v1.x,-v1.y,-v1.z };
}
#pragma endregion

//内積
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return{ v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
}
//長さ
inline float Length(const Vector3& v) {
	float tmp = v.x * v.x + v.y * v.y + v.z * v.z;
	tmp = sqrtf(tmp);
	return tmp;
}
//距離
inline float Distance(const Vector3& start, const Vector3& end) noexcept {
	return Length((end - start));
}
//正規化
inline Vector3 Normalize(const Vector3& v) {
	float tmp = v.x * v.x + v.y * v.y + v.z * v.z;
	tmp = sqrtf(tmp);
	return { v.x / tmp, v.y / tmp, v.z / tmp };
}
//外積
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return { (v1.y * v2.z - v1.z * v2.y),(v1.z * v2.x - v1.x * v2.z),(v1.x * v2.y - v1.y * v2.x) };
}


inline Vector3 Project(const Vector3& v1, const Vector3 v2) {
	Vector3 norm = Normalize(v2);
	Vector3 result = norm * (Dot(v1, norm));
	return result;
}

inline Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result = segment.origin + Project(point - segment.origin, segment.diff);
	return result;
}

inline float Angle(const Vector3& from, const Vector3& to) {
	float dot = Dot(Normalize(from), Normalize(to));
	if (dot >= 1.0f) {
		return 0.0f;
	}
	if (dot <= -1.0f) {
		return Radian(180.0f);
	}
	return std::acosf(dot);
}

inline float SignedAngle(const Vector3& from, const Vector3& to, const Vector3& axis) {
	return Dot(Cross(from, to), axis) < 0.0f ? Angle(from, to) : -Angle(from, to);
}


// 線形補間
inline Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result = v1 + (v2 - v1) * t;
	return result;
}
// 球面線形補間
inline Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {

	const float epsilon = 0.0001f;

	Vector3 a = Normalize(v1);
	Vector3 b = Normalize(v2);

	float dot = Dot(a, b);

	if (std::abs(dot - 1.0f) < epsilon) {
		return a;
	}
	else if (std::abs(dot + 1.0f) < epsilon) {
		return Lerp(v1, v2, t);
	}

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float factorA = std::sin((1.0f - t) * theta) / sinTheta;
	float factorB = std::sin(t * theta) / sinTheta;

	return Vector3{
		factorA * a.x + factorB * b.x, factorA * a.y + factorB * b.y,
		factorA * a.z + factorB * b.z };
}

inline Vector3 CatmullRomSpline(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	return
		0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * std::pow(t, 3.0f)
			+ (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * std::pow(t, 2.0f)
			+ (-p0 + p2) * t
			+ 2.0f * p1);
}

inline Vector3 QuadraticBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	float s = 1.0f - t;
	return
		s * s * p0 +
		2.0f * t * s * p1 +
		t * t * p2;
}

inline Vector3 CubicBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	float s = 1.0f - t;
	return
		(s * s * s * p0) +
		(3.0f * s * s * t * p1) +
		(3.0f * s * t * t * p2) +
		t * t * t * p3;
}


#pragma endregion
#pragma region	Vector4
#pragma endregion
#pragma region	Matrix4x4
#pragma region 演算子のオーバーロード

inline Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 tmp;
	tmp.m[0][0] = 1.0f;
	tmp.m[0][1] = 0.0f;
	tmp.m[0][2] = 0.0f;
	tmp.m[0][3] = 0.0f;

	tmp.m[1][0] = 0.0f;
	tmp.m[1][1] = 1.0f;
	tmp.m[1][2] = 0.0f;
	tmp.m[1][3] = 0.0f;

	tmp.m[2][0] = 0.0f;
	tmp.m[2][1] = 0.0f;
	tmp.m[2][2] = 1.0f;
	tmp.m[2][3] = 0.0f;

	tmp.m[3][0] = 0.0f;
	tmp.m[3][1] = 0.0f;
	tmp.m[3][2] = 0.0f;
	tmp.m[3][3] = 1.0f;

	return tmp;
}

inline Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] + m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] + m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] + m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] + m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] + m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] + m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] + m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] + m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] + m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] + m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] + m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] + m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] + m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] + m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] + m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return tmp;

}
inline Matrix4x4 operator +(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] + m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] + m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] + m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] + m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] + m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] + m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] + m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] + m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] + m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] + m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] + m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] + m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] + m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] + m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] + m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return tmp;
}
inline Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] - m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] - m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] - m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] - m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] - m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] - m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] - m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] - m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] - m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] - m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] - m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] - m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] - m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] - m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] - m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return tmp;
}
inline Matrix4x4 operator -(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] - m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] - m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] - m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] - m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] - m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] - m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] - m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] - m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] - m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] - m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] - m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] - m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] - m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] - m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] - m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return tmp;
}
inline Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return tmp;
}
inline Matrix4x4 operator *(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return tmp;
}
inline Matrix4x4 Inverse(const Matrix4x4& m) {
	float lal = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
		+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
		+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
		- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
		- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
		- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
		- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
		+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
		+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
		+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
		- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
		- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
		- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
		+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
		+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 tmp;
	tmp.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) / lal;

	tmp.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / lal;


	tmp.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / lal;

	tmp.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) / lal;


	tmp.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) / lal;

	tmp.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / lal;

	tmp.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) / lal;

	tmp.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / lal;


	tmp.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / lal;

	tmp.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) / lal;

	tmp.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / lal;

	tmp.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) / lal;

	return tmp;
}
inline Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m.m[0][0];
	tmp.m[0][1] = m.m[1][0];
	tmp.m[0][2] = m.m[2][0];
	tmp.m[0][3] = m.m[3][0];

	tmp.m[1][0] = m.m[0][1];
	tmp.m[1][1] = m.m[1][1];
	tmp.m[1][2] = m.m[2][1];
	tmp.m[1][3] = m.m[3][1];

	tmp.m[2][0] = m.m[0][2];
	tmp.m[2][1] = m.m[1][2];
	tmp.m[2][2] = m.m[2][2];
	tmp.m[2][3] = m.m[3][2];

	tmp.m[3][0] = m.m[0][3];
	tmp.m[3][1] = m.m[1][3];
	tmp.m[3][2] = m.m[2][3];
	tmp.m[3][3] = m.m[3][3];

	return tmp;
}

inline Vector3 operator *(const Vector3& v, const Matrix4x4& m) {

	Vector3 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

inline Matrix4x4& operator*=(Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
		m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
		m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
		m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
		m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
		m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
		m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
		m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
		m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
		m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
		m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
		m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
		m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
		m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
		m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
		m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
		m1.m[3][3] * m2.m[3][3];
	m1 = tmp;
	return m1;


}

#pragma endregion
#pragma region Vector3算出
inline Vector3 GetXAxis(Matrix4x4 m) {

	return { m.m[0][0],m.m[0][1],m.m[0][2] };
}
inline Vector3 GetYAxis(Matrix4x4 m) {

	return { m.m[1][0],m.m[1][1],m.m[1][2] };
}
inline Vector3 GetZAxis(Matrix4x4 m) {

	return { m.m[2][0],m.m[2][1],m.m[2][2] };
}

inline void SetXAxis(Matrix4x4& m, Vector3 v) {
	m.m[0][0] = v.x;
	m.m[0][1] = v.y;
	m.m[0][2] = v.z;
}
inline void SetYAxis(Matrix4x4& m, Vector3 v) {
	m.m[1][0] = v.x;
	m.m[1][1] = v.y;
	m.m[1][2] = v.z;
}
inline void SetZAxis(Matrix4x4& m, Vector3 v) {
	m.m[2][0] = v.x;
	m.m[2][1] = v.y;
	m.m[2][2] = v.z;
}

inline Vector3 MakeScale(const Matrix4x4& matrix) {
	Vector3 scaleX = { matrix.m[0][0],matrix.m[0][1],matrix.m[0][2] };
	Vector3 scaleY = { matrix.m[1][0],matrix.m[1][1],matrix.m[1][2] };
	Vector3 scaleZ = { matrix.m[2][0],matrix.m[2][1],matrix.m[2][2] };
	Vector3 result;
	result.x = Length(scaleX);
	result.y = Length(scaleY);
	result.z = Length(scaleZ);
	return result;
}

inline Vector3 MakeTranslation(const Matrix4x4& matrix) {
	Vector3 result;
	result.x = matrix.m[3][0];
	result.y = matrix.m[3][1];
	result.z = matrix.m[3][2];
	return result;
}
inline Vector3 MakeEulerAngle(const Matrix4x4& matrix) {
	Vector3 angles;

	// Extract yaw (Y軸回りの回転)
	angles.y = atan2(matrix.m[2][0], matrix.m[2][2]);

	// Extract pitch (X軸回りの回転)
	float sinPitch = -matrix.m[2][1];
	if (fabs(sinPitch) >= 1.0f) {
		angles.x = copysign(3.14159265f / 2.0f, sinPitch);
	}
	else {
		angles.x = asin(sinPitch);
	}

	// Extract roll (Z軸回りの回転)
	angles.z = atan2(matrix.m[0][1], matrix.m[1][1]);

	return angles;
}
inline Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;

}
#pragma endregion
#pragma region ファクトリ関数
//回転行列算出
inline Matrix4x4 NormalizeMakeRotateMatrix(const Matrix4x4& matrix) {
	Vector3 xAxis = Normalize(GetXAxis(matrix)); // [0][?]
	Vector3 yAxis = Normalize(GetYAxis(matrix)); // [1][?]
	Vector3 zAxis = Normalize(GetZAxis(matrix)); // [2][?]
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = xAxis.x;
	result.m[0][1] = xAxis.y;
	result.m[0][2] = xAxis.z;
	result.m[1][0] = yAxis.x;
	result.m[1][1] = yAxis.y;
	result.m[1][2] = yAxis.z;
	result.m[2][0] = zAxis.x;
	result.m[2][1] = zAxis.y;
	result.m[2][2] = zAxis.z;

	return result;
}

inline Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 tmp;
	tmp.m[0][0] = 1;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = 1;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = translate.x;
	tmp.m[3][1] = translate.y;
	tmp.m[3][2] = translate.z;
	tmp.m[3][3] = 1;

	return tmp;

}
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 tmp;
	tmp.m[0][0] = scale.x;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = scale.y;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = scale.z;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}

inline Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = 1;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = std::cos(radian);
	tmp.m[1][2] = std::sin(radian);
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = -std::sin(radian);
	tmp.m[2][2] = std::cos(radian);
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}
inline Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = std::cos(radian);
	tmp.m[0][1] = 0;
	tmp.m[0][2] = -std::sin(radian);
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = 1;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = std::sin(radian);
	tmp.m[2][1] = 0;
	tmp.m[2][2] = std::cos(radian);
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}

inline Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = std::cos(radian);
	tmp.m[0][1] = std::sin(radian);
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = -sinf(radian);
	tmp.m[1][1] = std::cos(radian);
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}

inline Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian) {

	return MakeRotateXMatrix(radian.x) * MakeRotateYMatrix(radian.y) * MakeRotateZMatrix(radian.z);;
}

inline Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result;
	result.m[0][0] = quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[0][1] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[0][2] = 2 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	result.m[0][3] = 0;

	result.m[1][0] = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	result.m[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[1][2] = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	result.m[1][3] = 0;

	result.m[2][0] = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	result.m[2][1] = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	result.m[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

inline Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 normalizeFrom = Normalize(from);
	Vector3 normalizeTo = Normalize(to);

	Vector3 cross = Cross(normalizeFrom, normalizeTo);
	float cos = Dot(normalizeFrom, normalizeTo);
	float sin = Length(cross);

	Vector3 n = { 0.0f,0.0f,0.0f };

	if (Length(cross) > 0) {
		n = Normalize(cross);
	}
	else if (normalizeFrom.x != 0.0f || normalizeFrom.z != 0.0f) {
		n.x = normalizeFrom.y;
		n.y = -normalizeFrom.x;
		n.z = 0.0f;
	}
	else if (normalizeFrom.x != 0.0f || normalizeFrom.y != 0.0f) {
		n.x = normalizeFrom.z;
		n.y = 0.0f;
		n.z = -normalizeFrom.x;

	}

	Matrix4x4 RotateMatrix = MakeIdentity4x4();
	RotateMatrix.m[0][0] = n.x * n.x * (1.0f - cos) + cos;
	RotateMatrix.m[0][1] = n.x * n.y * (1.0f - cos) + n.z * sin;
	RotateMatrix.m[0][2] = n.x * n.z * (1.0f - cos) - n.y * sin;

	RotateMatrix.m[1][0] = n.x * n.y * (1.0f - cos) - n.z * sin;
	RotateMatrix.m[1][1] = n.y * n.y * (1.0f - cos) + cos;
	RotateMatrix.m[1][2] = n.y * n.z * (1.0f - cos) + n.x * sin;

	RotateMatrix.m[2][0] = n.x * n.z * (1.0f - cos) + n.y * sin;
	RotateMatrix.m[2][1] = n.y * n.z * (1.0f - cos) - n.x * sin;
	RotateMatrix.m[2][2] = n.z * n.z * (1.0f - cos) + cos;

	return RotateMatrix;
}

inline Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 RotateMatrix = MakeIdentity4x4();
	RotateMatrix.m[0][0] = axis.x * axis.x * (1.0f - std::cosf(angle)) + std::cosf(angle);
	RotateMatrix.m[0][1] = axis.x * axis.y * (1.0f - std::cosf(angle)) + axis.z * std::sinf(angle);
	RotateMatrix.m[0][2] = axis.x * axis.z * (1.0f - std::cosf(angle)) - axis.y * std::sinf(angle);

	RotateMatrix.m[1][0] = axis.x * axis.y * (1.0f - std::cosf(angle)) - axis.z * std::sinf(angle);
	RotateMatrix.m[1][1] = axis.y * axis.y * (1.0f - std::cosf(angle)) + std::cosf(angle);
	RotateMatrix.m[1][2] = axis.y * axis.z * (1.0f - std::cosf(angle)) + axis.x * std::sinf(angle);

	RotateMatrix.m[2][0] = axis.x * axis.z * (1.0f - std::cosf(angle)) + axis.y * std::sinf(angle);
	RotateMatrix.m[2][1] = axis.y * axis.z * (1.0f - std::cosf(angle)) - axis.x * std::sinf(angle);
	RotateMatrix.m[2][2] = axis.z * axis.z * (1.0f - std::cosf(angle)) + std::cosf(angle);

	return RotateMatrix;
}

inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 tmp = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

	return tmp;
}
inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 tmp = scaleMatrix * rotateMatrix * translateMatrix;

	return tmp;
}

// 透視投影行列
inline Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 tmp;
	float cot = 1.0f / std::tan(fovY / 2.0f);
	tmp.m[0][0] = (1.0f / aspectRatio) * cot;
	tmp.m[0][1] = 0.0f;
	tmp.m[0][2] = 0.0f;
	tmp.m[0][3] = 0.0f;
	tmp.m[1][0] = 0.0f;
	tmp.m[1][1] = cot;
	tmp.m[1][2] = 0.0f;
	tmp.m[1][3] = 0.0f;
	tmp.m[2][0] = 0.0f;
	tmp.m[2][1] = 0.0f;
	tmp.m[2][2] = farClip / (farClip - nearClip);
	tmp.m[2][3] = 1.0f;
	tmp.m[3][0] = 0.0f;
	tmp.m[3][1] = 0.0f;
	tmp.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	tmp.m[3][3] = 0.0f;

	return tmp;
}
//正射影行列
inline Matrix4x4 MakeOrthograohicmatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 tmp;
	tmp.m[0][0] = 2.0f / (right - left);
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;
	tmp.m[1][0] = 0;
	tmp.m[1][1] = 2.0f / (top - bottom);
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;
	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1.0f / (farClip - nearClip);
	tmp.m[2][3] = 0;
	tmp.m[3][0] = (left + right) / (left - right);
	tmp.m[3][1] = (top + bottom) / (bottom - top);
	tmp.m[3][2] = nearClip / (nearClip - farClip);
	tmp.m[3][3] = 1.0f;

	return tmp;

}
//ビューポート変換行列
inline Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 tmp;
	tmp.m[0][0] = width / 2.0f;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;
	tmp.m[1][0] = 0;
	tmp.m[1][1] = -height / 2.0f;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;
	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = maxDepth - minDepth;
	tmp.m[2][3] = 0;
	tmp.m[3][0] = left + (width / 2.0f);
	tmp.m[3][1] = top + (height / 2.0f);
	tmp.m[3][2] = minDepth;
	tmp.m[3][3] = 1;
	return tmp;
}
//View行列
inline Matrix4x4 MakeViewMatirx(const Vector3& rotate, const Vector3& tranlate) {
	Matrix4x4 cameraWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, tranlate);

	return Inverse(cameraWorldMatrix);
}

inline Matrix4x4 MakeViewMatirx(const Quaternion& q, const Vector3& tranlate) {
	Matrix4x4 cameraWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, q, tranlate);
	return Inverse(cameraWorldMatrix);
}

inline Matrix4x4 MakeLookRotationMatrix(const Vector3& direction, const Vector3& up = {0.0f,1.0f,0.0f})  {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(Normalize(up), z));
	Vector3 y = Cross(z, x);
	return {
		x.x, x.y, x.z, 0.0f,
		y.x, y.y, y.z, 0.0f,
		z.x, z.y, z.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
}

#pragma endregion
#pragma endregion
#pragma region	Quaternion
inline float Norm(const Quaternion& q) {
	float result = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	return result;
}

inline Quaternion Conjugate(const Quaternion& q) {
	Quaternion result{ -q.x,-q.y,-q.z,q.w };
	return result;
}

inline Quaternion Inverse(const Quaternion& q) {
	float norm = Norm(q);
	norm = norm * norm;
	Quaternion conjugate = Conjugate(q);
	Quaternion result;
	result.x = conjugate.x / norm;
	result.y = conjugate.y / norm;
	result.z = conjugate.z / norm;
	result.w = conjugate.w / norm;
	return result;
}

#pragma region 演算子のオーバーロード
inline Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion result{
	lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
	lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
	lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
	lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z };

	return result;
}

inline Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion result{
	lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
	lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
	lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
	lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z };

	return result;
}

inline Quaternion& operator *=(Quaternion& lhs, const Quaternion& rhs) {
	lhs = lhs * rhs;
	return lhs;
}

inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
	return Quaternion{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

inline Quaternion operator*(const Quaternion& lhs, float rhs) {
	return Quaternion{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}

inline Quaternion operator*(float lhs, const Quaternion& rhs) {
	return Quaternion{ lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}

inline Vector3 operator *(const Vector3& v, const Quaternion& q) {
	Quaternion vq = { v.x,v.y,v.z, 0 };
	Quaternion inverseQ = Inverse(q);
	Quaternion resultQ = q * vq * inverseQ;
	Vector3 result = { resultQ.x,resultQ.y,resultQ.z };
	return result;
}
#pragma endregion
#pragma region ファクトリ関数
inline Quaternion IdentityQuaternion() {
	Quaternion result{ 0.0f,0.0f,0.0f,1.0f };
	return result;
}

inline Quaternion MakeForXAxis(float angle) {
	return Quaternion(std::sin(angle / 2.0f), 0.0f, 0.0f, std::cos(angle / 2.0f));
}
inline Quaternion MakeForYAxis(float angle) {
	return Quaternion(0.0f, std::sin(angle / 2.0f), 0.0f, std::cos(angle / 2.0f));
}
inline Quaternion MakeForZAxis(float angle) {
	return Quaternion(0.0f, 0.0f, std::sin(angle / 2.0f), std::cos(angle / 2.0f));
}

inline Quaternion MakeXAxisFromQuaternion(Quaternion q) {
	return Quaternion(q.x, 0.0f, 0.0f, q.w);
}
inline Quaternion MakeYAxisFromQuaternion(Quaternion q) {
	return Quaternion(0.0f, q.y, 0.0f, q.w);
}
inline Quaternion MakeZAxisFromQuaternion(Quaternion q) {
	return Quaternion(0.0f, 0.0f, q.z, q.w);
}

inline Quaternion MakeFromAngleAxis(const Vector3& axis, float angle) {
	Vector3 v = Normalize(axis) * std::sinf(angle * 0.5f);
	Quaternion result;
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w = std::cosf(angle * 0.5f);
	return result;
}

inline Quaternion MakeFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z) {
	float trace = x.x + y.y + z.z;
	if (trace > 0.0f) {
		float s = std::sqrtf(trace + 1.0f) * 0.5f;
		Quaternion result{};
		result.w = s;
		s = 0.25f / s;
		result.x = (y.z - z.y) * s;
		result.y = (z.x - x.z) * s;
		result.z = (x.y - y.x) * s;
		return result;
	}
	else if (x.x > y.y && x.x > z.z) {
		float s = std::sqrtf(1.0f + x.x - y.y - z.z) * 0.5f;
		Quaternion result{};
		result.x = s;
		s = 0.25f / s;
		result.y = (x.y + y.x) * s;
		result.z = (z.x + x.z) * s;
		result.w = (y.z - z.y) * s;
		return result;
	}
	else if (y.y > z.z) {
		float s = std::sqrtf(1.0f - x.x + y.y - z.z) * 0.5f;
		Quaternion result{};
		result.y = s;
		s = 0.25f / s;
		result.x = (x.y + y.x) * s;
		result.z = (y.z + z.y) * s;
		result.w = (z.x - x.z) * s;
		return result;
	}
	Quaternion result{};
	float s = std::sqrtf(1.0f - x.x - y.y + z.z) * 0.5f;
	result.z = s;
	s = 0.25f / s;
	result.x = (z.x + x.z) * s;
	result.y = (y.z + z.y) * s;
	result.w = (x.y - y.x) * s;
	return result;
}

inline Quaternion MakeFromOrthonormal(const Matrix4x4& m) {
	Vector3 x = GetXAxis(m);
	Vector3 y = GetYAxis(m);
	Vector3 z = GetZAxis(m);

	float trace = x.x + y.y + z.z;
	if (trace > 0.0f) {
		float s = std::sqrtf(trace + 1.0f) * 0.5f;
		Quaternion result{};
		result.w = s;
		s = 0.25f / s;
		result.x = (y.z - z.y) * s;
		result.y = (z.x - x.z) * s;
		result.z = (x.y - y.x) * s;
		return result;
	}
	else if (x.x > y.y && x.x > z.z) {
		float s = std::sqrtf(1.0f + x.x - y.y - z.z) * 0.5f;
		Quaternion result{};
		result.x = s;
		s = 0.25f / s;
		result.y = (x.y + y.x) * s;
		result.z = (z.x + x.z) * s;
		result.w = (y.z - z.y) * s;
		return result;
	}
	else if (y.y > z.z) {
		float s = std::sqrtf(1.0f - x.x + y.y - z.z) * 0.5f;
		Quaternion result{};
		result.y = s;
		s = 0.25f / s;
		result.x = (x.y + y.x) * s;
		result.z = (y.z + z.y) * s;
		result.w = (z.x - x.z) * s;
		return result;
	}
	Quaternion result{};
	float s = std::sqrtf(1.0f - x.x - y.y + z.z) * 0.5f;
	result.z = s;
	s = 0.25f / s;
	result.x = (z.x + x.z) * s;
	result.y = (y.z + z.y) * s;
	result.w = (x.y - y.x) * s;
	return result;
}

inline Quaternion MakeFromTwoVector(const Vector3& from, const Vector3& to) {
	Vector3 axis = Cross(from, to);
	float angle = Angle(from, to);
	return MakeFromAngleAxis(axis, angle);
}

inline Quaternion RotateMatrixToQuaternion(Matrix4x4 m) {
	float px = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f;
	float py = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f;
	float pz = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f;
	float pw = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f;

	int selected = 0;
	float max = px;
	if (max < py) {
		selected = 1;
		max = py;
	}
	if (max < pz) {
		selected = 2;
		max = pz;
	}
	if (max < pw) {
		selected = 3;
		max = pw;
	}

	if (selected == 0) {
		float x = std::sqrt(px) * 0.5f;
		float d = 1.0f / (4.0f * x);
		return Quaternion(
			x,
			(m.m[1][0] + m.m[0][1]) * d,
			(m.m[0][2] + m.m[2][0]) * d,
			(m.m[2][1] - m.m[1][2]) * d
		);
	}
	else if (selected == 1) {
		float y = std::sqrt(py) * 0.5f;
		float d = 1.0f / (4.0f * y);
		return Quaternion(
			(m.m[1][0] + m.m[0][1]) * d,
			y,
			(m.m[2][1] + m.m[1][2]) * d,
			(m.m[0][2] - m.m[2][0]) * d
		);
	}
	else if (selected == 2) {
		float z = std::sqrt(pz) * 0.5f;
		float d = 1.0f / (4.0f * z);
		return Quaternion(
			(m.m[0][2] + m.m[2][0]) * d,
			(m.m[2][1] + m.m[1][2]) * d,
			z,
			(m.m[1][0] - m.m[0][1]) * d
		);
	}
	else if (selected == 3) {
		float w = std::sqrtf(pw) * 0.5f;
		float d = 1.0f / (4.0f * w);
		return Quaternion(
			(m.m[2][1] - m.m[1][2]) * d,
			(m.m[0][2] - m.m[2][0]) * d,
			(m.m[1][0] - m.m[0][1]) * d,
			w
		);
	}
	assert(false);
	return Quaternion{};
}

inline Quaternion MakeFromEulerAngle(const Vector3& euler) {
	Vector3 s = Vector3(std::sin(euler.x * 0.5f), std::sin(euler.y * 0.5f), std::sin(euler.z * 0.5f));
	Vector3 c = Vector3(std::cos(euler.x * 0.5f), std::cos(euler.y * 0.5f), std::cos(euler.z * 0.5f));
	return Quaternion{
		s.x * c.y * c.z - c.x * s.y * s.z,
		c.x * s.y * c.z + s.x * c.y * s.z,
		c.x * c.y * s.z - s.x * s.y * c.z,
		c.x * c.y * c.z + s.x * s.y * s.z };
}

inline Quaternion MakeLookRotation(const Vector3& direction, const Vector3& up = { 0.0f,1.0f,0.0f }) {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(up, z));
	Vector3 y = Cross(z, x);
	return MakeFromOrthonormal(x, y, z);
}
#pragma endregion

inline Vector3 EulerAngle(const Quaternion& q) {
	Vector3 euler{};
	euler.x = std::atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
	euler.y = std::asin(2.0f * (q.w * q.y - q.z * q.x));
	euler.z = std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
	return euler;
}

inline Quaternion Normalize(const Quaternion& q) {
	float tmp = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
	tmp = sqrtf(tmp);
	return { q.x / tmp, q.y / tmp, q.z / tmp, q.w / tmp };
}


inline Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion vq = { v.x,v.y,v.z, 0 };
	Quaternion inverseQ = Inverse(q);
	Quaternion resultQ = q * vq * inverseQ;
	Vector3 result = { resultQ.x,resultQ.y,resultQ.z };
	return result;
}


inline float Dot(const Quaternion& lhs, const Quaternion& rhs) noexcept {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

inline Quaternion Lerp(float t, const Quaternion& start, const Quaternion& end) noexcept {
	return Quaternion{
		start.x + t * (end.x - start.x),
		start.y + t * (end.y - start.y),
		start.z + t * (end.z - start.z),
		start.w + t * (end.w - start.w) };
}

inline Quaternion Slerp(float t, const Quaternion& start, const Quaternion& end) noexcept {
	Quaternion s = start;
	float dot = Dot(start, end);
	if (dot > 0.9999f) {
		return Lerp(t, start, end);
	}
	// q1,q2が反対向きの場合
	if (dot < 0) {
		s.x = -s.x;
		s.y = -s.y;
		s.z = -s.z;
		s.w = -s.w;
		dot = -dot;
	}
	// 球面線形補間の計算
	float theta = std::acos(dot);
	return (std::sin((1.0f - t) * theta) * s + std::sin(t * theta) * end) * (1.0f / std::sin(theta));
}
#pragma endregion
#pragma region	OBB
inline OBB MakeOBB(Vector3 pos, Matrix4x4 rotation, Vector3 size) {
	OBB result;
	result.center = pos;
	result.orientations[0] = GetXAxis(rotation);
	result.orientations[1] = GetYAxis(rotation);
	result.orientations[2] = GetZAxis(rotation);
	result.size = size;

	return result;
}

inline OBB MakeOBB(Matrix4x4 worldMatrix) {
	OBB result;
	result.center = MakeTranslation(worldMatrix);

	Matrix4x4 rotation = MakeIdentity4x4();
	rotation = NormalizeMakeRotateMatrix(worldMatrix);
	result.orientations[0] = GetXAxis(rotation);
	result.orientations[1] = GetYAxis(rotation);
	result.orientations[2] = GetZAxis(rotation);

	result.size = MakeScale(worldMatrix);

	return result;
}

inline Vector3 MakeRandVector3(OBB box) {
	Vector3 result;
	result = { Rand(-box.size.x ,box.size.x),Rand(-box.size.y ,box.size.y) ,Rand(-box.size.z,box.size.z) };
	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	SetXAxis(rotateMatrix, box.orientations[0]);
	SetYAxis(rotateMatrix, box.orientations[1]);
	SetZAxis(rotateMatrix, box.orientations[2]);
	result = result * rotateMatrix;
	result = result + box.center;
	return result;
}

#pragma endregion


