#pragma once
#include "PCH.h"
#include "Shape.h"

class Bitmap
{
public:
	void Create(ID2D1DeviceContext* dc, int resource, COLORREF color, D2D1_POINT_2F pivot, float scale);
	void Create32bit(ID2D1DeviceContext* dc, int resource, D2D1_POINT_2F pivot);
	void Draw(ID2D1DeviceContext* dc, D2D1_POINT_2F center);
	void Draw(ID2D1DeviceContext* dc, float angle, D2D1_POINT_2F center);
	void Draw(ID2D1DeviceContext* dc, float angle, D2D1_POINT_2F center, float scale);
private:
	std::vector<BYTE> FileLoader(_In_ int resource, _Out_ UINT& width, _Out_ UINT& height, _Out_ UINT& pitch);
	ComPtr<ID2D1Bitmap> m_Bitmap;
	D2D1_POINT_2F m_Pivot = {};
	D2D1_SIZE_F m_Size = {};
	friend RasterizedShape Load32BitBitmap(ID2D1DeviceContext* dc, int resource);
};