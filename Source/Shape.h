#pragma once
#include "PCH.h"

class RasterizedShape
{
public:
	virtual void Draw(ID2D1DeviceContext* dc, D2D1_POINT_2F center);
private:
	void Rasterize(ID2D1DeviceContext* dc, ID2D1PathGeometry* geometry, ID2D1SolidColorBrush* brush);
	void RasterizeClockTicks(ID2D1DeviceContext* dc, ID2D1SolidColorBrush* brush, float radius);
	void RasterizeSunMoon(ID2D1DeviceContext* dc, ID2D1PathGeometry* geometry, ID2D1SolidColorBrush* brush, BOOL sun, float radius);
	ComPtr<ID2D1Bitmap> m_Bitmap;
	D2D1_POINT_2F m_Pivot = {};
	D2D1_SIZE_F m_Size = {};
	friend RasterizedShape CreateShape(ID2D1Factory2* factory, ID2D1DeviceContext* dc, int shapeindex, D2D1::ColorF color, float size);
};

RasterizedShape CreateShape(ID2D1Factory2* factory, ID2D1DeviceContext* dc, int shapeindex, D2D1::ColorF color, float size);
