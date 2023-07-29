#pragma once
#include "PCH.h"

class DigitalClock
{
public:
	void Init(ID2D1Factory* pD2DFactory);
	void DiscardGraphicsResources();
	HRESULT CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget);
	void Draw(ID2D1HwndRenderTarget* pRenderTarget, D2D1::Matrix3x2F transform, BOOL negative, BOOL Highlighted, BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths);
private:
	void DrawDot(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* Brush);
	void DrawColon(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* Brush);
	class SevenSegment
	{
	public:
		void InitGeometry(ID2D1Factory* pD2DFactory);
		void Draw(ID2D1HwndRenderTarget* pRenderTarget, int value, ID2D1SolidColorBrush* pFullBrush);
	private:
		class Segment
		{
		public:
			void Init(ID2D1Factory* pD2DFactory, BOOL vertical, float longdist, float shortdist, float halfwidth, float sk, D2D1_POINT_2F offset);
			~Segment();
			void DrawSegment(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush);
			ID2D1PathGeometry* Geometry = nullptr;
			ID2D1GeometrySink* Sink = nullptr;
		};
		Segment Seg[7];
	} m_SevenSegment;
	ID2D1SolidColorBrush* pNormalBrush = nullptr;
	ID2D1SolidColorBrush* pHighlightBrush = nullptr;
};