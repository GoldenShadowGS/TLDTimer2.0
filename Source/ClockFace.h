#pragma once
#include "PCH.h"
#include "Bitmap.h"

class ClockFace
{
public:
	void CreateGraphicsResources(ID2D1DeviceContext* pRenderTarget);
	void DrawBackGround(ID2D1DeviceContext* pRenderTarget, float angle);
	void DrawHands(ID2D1DeviceContext* pRenderTarget, float minAgle, float hourAngle);
	void InitGeometry(ID2D1Factory2* pD2DFactory, float size);
	float GetCenterX() { return m_Center.x; }
	float GetCenterY() { return m_Center.y; }
	float GetRadius() { return m_Radius; }
private:
	ComPtr<ID2D1SolidColorBrush> pBackGroundBrush;
	ComPtr<ID2D1GradientStopCollection> pStopsCollection;
	ComPtr<ID2D1RadialGradientBrush> pRadialBackGroundBrush;
	ComPtr<ID2D1SolidColorBrush> pTickBrush;
	ComPtr<ID2D1SolidColorBrush> pSunbrush;
	D2D1_POINT_2F m_Center = { 320.0f, 120.0f};
	float m_Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap hourhandbitmap;
	struct TimeofDayShape
	{
		float size = 0.0f;
		ComPtr<ID2D1PathGeometry> Geometry;
		//ComPtr<ID2D1GeometrySink> Sink;
	};
	TimeofDayShape sun;
	TimeofDayShape moon;
};