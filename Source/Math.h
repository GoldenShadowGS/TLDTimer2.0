#pragma once
#include "PCH.h"

constexpr float PI = 3.141592f;
constexpr float HalfPI = PI / 2.0f;
constexpr float QuarterPI = PI / 4.0f;
constexpr float PI2 = PI * 2.0f;
constexpr float PI3_4 = PI * 1.5f;
constexpr float ThirdPI = PI2 / 3.0f;
constexpr float Deg2RadFactor = PI / 180.0f;
constexpr float Rad2DegFactor = 180.0f / PI;

inline D2D1_POINT_2F operator+(const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2)
{
	return { p1.x + p2.x, p1.y + p2.y };
}

inline D2D1_POINT_2F operator*(const D2D1_POINT_2F& p1, float scale)
{
	return { p1.x * scale, p1.y * scale };
}

inline D2D1_POINT_2F skew(const D2D1_POINT_2F& p1, float amount)
{
	return { p1.x + amount * p1.y, p1.y };
}

inline float GetPointDist(float x1, float y1, float x2, float y2)
{
	float x3 = x2 - x1;
	float y3 = y2 - y1;
	return sqrt((x3 * x3) + (y3 * y3));
}

inline float angleNormalize(float angle)
{
	while (angle < 0.0f)
	{
		angle += PI2;
	}
	while (angle > PI2)
	{
		angle -= PI2;
	}
	return angle;
}

inline float getMouseAngle(int mousex, int mousey, float centerx, float centery, float offset)
{
	float x = (float)mousex - centerx;
	float y = (float)mousey - centery;
	return angleNormalize(atan2(y, x) - offset);
}

inline float GetAngleDistance(float angle1, float angle2)
{
	float dist = abs(angle1 - angle2);
	if (dist > PI)
		dist = PI2 - dist;
	return dist;
}

inline D2D1_POINT_2F rotate(D2D1_POINT_2F point, float angle)
{
	float cosa = cos(angle);
	float sina = sin(angle);
	return { point.x * cosa - point.y * sina,point.y * cosa + point.x * sina };
}

inline D2D1_POINT_2F translate(D2D1_POINT_2F point, D2D1_POINT_2F point2)
{
	return { point.x + point2.x, point.y + point2.y };
}

//inline float AngleSnap(float angle, float snap)
//{
//	const float normalizedangle = angle / PI2;
//	return (round(normalizedangle * snap) / snap) * PI2;
//}

//inline INT64 RoundAngleToMinute(float angle)
//{
//	const float normalizedangle = angle / PI2;
//	return (INT64)round(normalizedangle * 60.0f);
//}

//inline INT64 RoundThousand(INT64 value)
//{
//	INT64 r = value % 1000;
//	if (r > 500)
//		return value + 1000 - r;
//	else
//		return value - r;
//}

// Debug Output 
//{
//	WCHAR buffer[64];
//	swprintf_s(buffer, 64, L"Redraw() %I64i, %I64i\n", CurrentTime, sleeptime);
//	OutputDebugStringW(buffer);
//}