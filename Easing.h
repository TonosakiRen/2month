#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Mymath.h"
#include<functional>
#include<iostream>
class Easing
{
public:
	enum EasingMode {
		easeNormal,
		easeInSine,
		easeOutSine,
		easeInOutSine,
		easeInQuad,
		easeOutQuad,
		easeInOutQuad,
		easeInCubic,
		easeOutCubic,
		easeInOutCubic,
		easeInQuart,
		easeOutQuart,
		easeInOutQuart,
		easeInQuint,
		easeOutQuint,
		easeInOutQuint,
		easeInExpo,
		easeOutExpo,
		easeInCirc,
		easeOutCirc,
		easeInOutCirc,
		easeInBack,
		easeOutBack,
		easeInOutBack,
		easeInElastic,
		easeOutElastic,
		easeInOutElastic,
		easeInBounce,
		easeOutBounce,
		easeInOutBounce,
		easeModeNum
	};


	static std::function<float(float)> easingFunction[easeModeNum];

	static float Normal(float x);
	static float InSine(float x);
	static float OutSine(float x);
	static float InOutSine(float x);
	static float InQuad(float x);
	static float OutQuad(float x);
	static float InOutQuad(float x);
	static float InCubic(float x);
	static float OutCubic(float x);
	static float InOutCubic(float x);
	static float InQuart(float x);
	static float OutQuart(float x);
	static float InOutQuart(float x);
	static float InQuint(float x);
	static float OutQuint(float x);
	static float InOutQuint(float x);
	static float InExpo(float x);
	static float OutExpo(float x);
	static float InCirc(float x);
	static float OutCirc(float x);
	static float InOutCirc(float x);
	static float InBack(float x);
	static float OutBack(float x);
	static float InOutBack(float x);
	static float InElastic(float x);
	static float OutElastic(float x);
	static float InOutElastic(float x);
	static float InBounce(float x);
	static float OutBounce(float x);
	static float InOutBounce(float x);

	/// <summary>
	/// イージング
	/// </summary>
	/// <param name="t">t</param>
	/// <param name="start">スタート</param>
	/// <param name="end">終わり</param>
	/// <param name="speed">tのスピード</param>
	/// <param name="mode">イージングモード</param>
	/// <param name="isAdd">tを加算するか</param>
	/// <returns></returns>
	static float easing(float& t, float start, float end, float speed, EasingMode easingMode, bool isAdd = true);

	/// <summary>
	/// イージングで数がほしいとき
	/// </summary>
	/// <param name="t">t</param>
	/// <param name="start">スタート</param>
	/// <param name="end">終わり</param>
	/// <returns></returns>
	static float easing(float t, float start, float end);

	/// <summary>
	/// イージング
	/// </summary>
	/// <param name="t">t</param>
	/// <param name="start">スタート</param>
	/// <param name="end">終わり</param>
	/// <param name="speed">tのスピード</param>
	/// <param name="mode">イージングモード</param>
	/// <param name="isAdd">tを加算するか</param>
	/// <returns></returns>
	static Vector2 easing(float& t, Vector2 start, Vector2 end, float speed, EasingMode easingMode, bool isAdd = true);

	/// <summary>
	/// イージング
	/// </summary>
	/// <param name="t">t</param>
	/// <param name="start">スタート</param>
	/// <param name="end">終わり</param>
	/// <param name="speed">tのスピード</param>
	/// <param name="mode">イージングモード</param>
	/// <param name="isAdd">tを加算するか</param>
	/// <returns></returns>
	static Vector3 easing(float& t, Vector3 start, Vector3 end, float speed, EasingMode easingMode, bool isAdd = true);

	static Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float& t, float speed, EasingMode easingMode, bool isAdd = true);

	static Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float& t, float speed, EasingMode easingMode, bool isAdd = true);


private:
	Easing();
	~Easing();



};