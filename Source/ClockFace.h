#pragma once
#include "PCH.h"
#include "Bitmap.h"

class ClockFace
{
public:
	void DrawBackGround(ID2D1HwndRenderTarget* pRenderTarget);
	void LoadBitmaps(ID2D1HwndRenderTarget* pRenderTarget);
	void DrawHands(ID2D1HwndRenderTarget* pRenderTarget, float minAgle, float hourAngle);
	HRESULT CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget);
	void DiscardGraphicsResources();
private:
	ID2D1SolidColorBrush* pBackGroundBrush = nullptr;
	float CenterX = 320;
	float CenterY = 120;
	float Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap minutehandhighlightedbitmap;
	Bitmap hourhandbitmap;
	Bitmap minutehandbitmapShadow;
	Bitmap hourhandbitmapShadow;
};