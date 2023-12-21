#include "Easing.h"


Easing::Easing()
{
}

Easing::~Easing()
{
}



float Easing::easing(float& t, float start, float end, float speed, EasingMode easingMode, bool isAdd) {
	if (isAdd == true) {
		t += speed;
	}
	t = clamp(t, 0.0f, 1.0f);

	return((1.0f - easingFunction[easingMode](t)) * start + easingFunction[easingMode](t) * end);
}

float Easing::easing(float t, float start, float end) {

	return((1.0f - t) * start + t * end);
}

Vector2 Easing::easing(float& t, Vector2 start, Vector2 end, float speed, EasingMode easingMode, bool isAdd) {
	if (isAdd == true) {
		t += speed;
	}
	t = clamp(t, 0.0f, 1.0f);
	return{ ((1.0f - easingFunction[easingMode](t)) * start.x + easingFunction[easingMode](t) * end.x) ,((1.0f - easingFunction[easingMode](t)) * start.y + easingFunction[easingMode](t) * end.y) };
}

Vector3 Easing::easing(float& t, Vector3 start, Vector3 end, float speed, EasingMode easingMode, bool isAdd) {
	if (isAdd == true) {
		t += speed;
	}
	t = clamp(t, 0.0f, 1.0f);
	return{ ((1.0f - easingFunction[easingMode](t)) * start.x + easingFunction[easingMode](t) * end.x) ,((1.0f - easingFunction[easingMode](t)) * start.y + easingFunction[easingMode](t) * end.y),((1.0f - easingFunction[easingMode](t)) * start.z + easingFunction[easingMode](t) * end.z) };
}

Vector2 Easing::Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float& t, float speed, EasingMode easingMode, bool isAdd) {
	if (isAdd == true) {
		t += speed;
	}
	t = clamp(t, 0.0f, 1.0f);
	Vector2 tmp0 = easing(t, p0, p1, 0.0f, easingMode, false);
	Vector2 tmp1 = easing(t, p1, p2, 0.0f, easingMode, false);
	Vector2 tmp = easing(t, tmp0, tmp1, 0.0f, easingMode, false);

	return{ tmp };
}

Vector2 Easing::Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float& t, float speed, EasingMode easingMode, bool isAdd) {
	if (isAdd) {
		t += speed;
	}
	t = clamp(t, 0.0f, 1.0f);
	Vector2 tmp0 = easing(t, p0, p1, 0.0f, easingMode, false);
	Vector2 tmp1 = easing(t, p1, p2, 0.0f, easingMode, false);
	Vector2 tmp2 = easing(t, p2, p3, 0.0f, easingMode, false);

	Vector2 tmp3 = easing(t, tmp0, tmp1, 0.0f, easingMode, false);
	Vector2 tmp4 = easing(t, tmp1, tmp2, 0.0f, easingMode, false);

	Vector2 tmp = easing(t, tmp3, tmp4, 0.0f, easingMode, false);

	return tmp;
}

float Easing::Normal(float x) {
	return x;
}

float Easing::InSine(float x) {
	return 1.0f - cosf((x * float(M_PI)) / 2.0f);
}
float Easing::OutSine(float x) {
	return sinf((x * float(M_PI)) / 2.0f);
}
float Easing::InOutSine(float x) {
	return -(cosf(float(M_PI) * x) - 1.0f) / 2.0f;
}
float Easing::InQuad(float x) {
	return x * x;
}
float Easing::OutQuad(float x) {
	return 1.0f - (1.0f - x) * (1.0f - x);
}
float Easing::InOutQuad(float x) {
	return x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}
float Easing::InCubic(float x) {
	return x * x * x;
}
float Easing::OutCubic(float x) {
	return 1.0f - powf(1.0f - x, 3.0f);
}
float Easing::InOutCubic(float x) {
	return x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}
float Easing::InQuart(float x) {
	return x * x * x * x;
}
float Easing::OutQuart(float x) {
	return 1.0f - powf(1.0f - x, 4.0f);
}
float Easing::InOutQuart(float x) {
	return x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;
}
float Easing::InQuint(float x) {
	return x * x * x * x * x;
}
float Easing::OutQuint(float x) {
	return 1.0f - powf(1.0f - x, 5.0f);
}
float Easing::InOutQuint(float x) {
	return x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;
}
float Easing::InExpo(float x) {
	return x = 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f);
}
float Easing::OutExpo(float x) {
	return x = 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);
}
float Easing::InCirc(float x) {
	return 1.0f - sqrtf(1.0f - powf(x, 2.0f));
}
float Easing::OutCirc(float x) {
	return sqrtf(1.0f - powf(x - 1.0f, 2.0f));
}
float Easing::InOutCirc(float x) {
	return x < 0.5f
		? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f
		: (sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}
float Easing::InBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * x * x * x - c1 * x * x;
}
float Easing::OutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * powf(x - 1.0f, 2.0f);
}
float Easing::InOutBack(float x) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f
		? (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
		: (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}
float Easing::InElastic(float x) {
	const float c4 = (2.0f * float(M_PI)) / 3.0f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);
}
float Easing::OutElastic(float x) {
	const float c4 = (2.0f * float(M_PI)) / 3.0f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
}
float Easing::InOutElastic(float x) {
	const float c5 = (2.0f * float(M_PI)) / 4.5f;

	return x == 0.0f
		? 0.0f
		: x == 1.0f
		? 1.0f
		: x < 0.5f
		? -(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f
		: (powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
}
float Easing::InBounce(float x) {
	return 1.0f - OutBounce(1.0f - x);
}
float Easing::OutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.0f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}
float Easing::InOutBounce(float x) {
	return x < 0.5f
		? (1.0f - OutBounce(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + OutBounce(2.0f * x - 1.0f)) / 2.0f;
}

std::function<float(float)> Easing::easingFunction[easeModeNum] = {
	Easing::Normal,
	Easing::InSine,
	Easing::OutSine,
	Easing::InOutSine,
	Easing::InQuad,
	Easing::OutQuad,
	Easing::InOutQuad,
	Easing::InCubic,
	Easing::OutCubic,
	Easing::InOutCubic,
	Easing::InQuart,
	Easing::OutQuart,
	Easing::InOutQuart,
	Easing::InQuint,
	Easing::OutQuint,
	Easing::InOutQuint,
	Easing::InExpo,
	Easing::OutExpo,
	Easing::InCirc,
	Easing::OutCirc,
	Easing::InOutCirc,
	Easing::InBack,
	Easing::OutBack,
	Easing::InOutBack,
	Easing::InElastic,
	Easing::OutElastic,
	Easing::InOutElastic,
	Easing::InBounce,
	Easing::OutBounce,
	Easing::InOutBounce,
};