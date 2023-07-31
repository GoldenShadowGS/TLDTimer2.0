#pragma once
#include "PCH.h"
#include "Bitmap.h"

class ClockFace
{
public:
	~ClockFace();
	HRESULT CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget);
	void DiscardGraphicsResources();
	void DrawBackGround(ID2D1HwndRenderTarget* pRenderTarget);
	void DrawHands(ID2D1HwndRenderTarget* pRenderTarget, float minAgle, float hourAngle);
	void InitGeometry(ID2D1Factory* pD2DFactory, float size);
	void DrawSunMoon(ID2D1HwndRenderTarget* pRenderTarget, float angle);
	float GetCenterX() { return CenterX; }
	float GetCenterY() { return CenterY; }
	float GetRadius() { return Radius; }
private:
	ID2D1SolidColorBrush* pBackGroundBrush = nullptr;
	ID2D1SolidColorBrush* pSunbrush = nullptr;
	float CenterX = 320;
	float CenterY = 120;
	float Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap hourhandbitmap;
	struct TimeofDayShape
	{
		~TimeofDayShape();
		float size = 0.0f;
		ID2D1PathGeometry* Geometry = nullptr;
		ID2D1GeometrySink* Sink = nullptr;
	};
	TimeofDayShape sun;
	TimeofDayShape moon;
};