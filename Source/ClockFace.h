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
	float GetCenterX() { return CenterX; }
	float GetCenterY() { return CenterY; }
	float GetRadius() { return Radius; }
private:
	ID2D1SolidColorBrush* pBackGroundBrush = nullptr;
	float CenterX = 320;
	float CenterY = 120;
	float Radius = 110.0f;
	Bitmap minutehandbitmap;
	Bitmap hourhandbitmap;
};