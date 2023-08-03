#pragma once
#include "PCH.h"

class DigitalClock
{
public:
	void Init(ID2D1Factory2* pD2DFactory);
	void CreateGraphicsResources(ID2D1DeviceContext* pRenderTarget);
	void Draw(ID2D1DeviceContext* pRenderTarget, D2D1::Matrix3x2F transform, BOOL negative, BOOL Highlighted, BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths);
private:
	float GetWidth(BOOL negative, BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths);
	void DrawDot(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* Brush);
	void DrawColon(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* Brush);
	class SevenSegment
	{
	public:
		void InitGeometry(ID2D1Factory2* pD2DFactory);
		void Draw(ID2D1DeviceContext* pRenderTarget, int value, ID2D1SolidColorBrush* pFullBrush);
	private:
		class Segment
		{
		public:
			void Init(ID2D1Factory2* pD2DFactory, BOOL vertical, float longdist, float shortdist, float halfwidth, float sk, D2D1_POINT_2F offset);
			void DrawSegment(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* pBrush);
			ComPtr<ID2D1PathGeometry> Geometry;
			ComPtr<ID2D1GeometrySink> Sink;
		};
		Segment Seg[7];
	} m_SevenSegment;
	ComPtr<ID2D1SolidColorBrush> pNormalBrush;
	ComPtr<ID2D1SolidColorBrush> pHighlightBrush;
	ComPtr<ID2D1SolidColorBrush> pBackgroundBrush;
};