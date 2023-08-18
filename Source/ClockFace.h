#pragma once
#include "PCH.h"
#include "Bitmap.h"
#include "Shape.h"

struct ClockAngles
{
	float MinuteAngle = 0.0f;
	float HourAngle = 0.0f;
	float SunAngle = 0.0f;
	BOOL HighlightSun = FALSE;
};

class ClockFace
{
public:
	void Draw(ID2D1DeviceContext* dc, const ClockAngles& clockAngles);
	void Init(ID2D1Factory2* factory, ID2D1DeviceContext* dc);
	float GetCenterX() { return m_Center.x; }
	float GetCenterY() { return m_Center.y; }
	float GetRadius() { return m_Radius; }
private:
	RasterizedShape SunShape;
	RasterizedShape MoonShape;
	RasterizedShape ClockTicksShape;
	D2D1_POINT_2F m_Center = { 220.0f, 120.0f };
	float m_Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap hourhandbitmap;
};