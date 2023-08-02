#pragma once
#include "PCH.h"
#include "Bitmap.h"

class ClockFace
{
public:
	void CreateGraphicsResources(ID2D1DeviceContext* pRenderTarget);
	void DrawBackGround(ID2D1DeviceContext* pRenderTarget);
	void DrawHands(ID2D1DeviceContext* pRenderTarget, float minAgle, float hourAngle);
	void InitGeometry(ID2D1Factory2* pD2DFactory, float size);
	void DrawSunMoon(ID2D1DeviceContext* pRenderTarget, float angle);
	float GetCenterX() { return CenterX; }
	float GetCenterY() { return CenterY; }
	float GetRadius() { return Radius; }
private:
	ComPtr<ID2D1SolidColorBrush> pBackGroundBrush;
	ComPtr<ID2D1SolidColorBrush> pSunbrush;
	float CenterX = 320;
	float CenterY = 120;
	float Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap hourhandbitmap;
	struct TimeofDayShape
	{
		float size = 0.0f;
		ComPtr<ID2D1PathGeometry> Geometry;
		ComPtr<ID2D1GeometrySink> Sink;
	};
	TimeofDayShape sun;
	TimeofDayShape moon;
};