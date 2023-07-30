#include "PCH.h"
#include "ClockFace.h"
#include "Direct2D.h"
#include "Math.h"
#include "Resource.h"

ClockFace::~ClockFace()
{
	DiscardGraphicsResources();
}

HRESULT ClockFace::CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget)
{
	HRESULT hr = S_OK;
	if (pBackGroundBrush == nullptr)
		hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f), &pBackGroundBrush);
	if (FAILED(hr))
		return hr;

	float scale = 0.6f;
	//pivot offsets
	float offsetx = 0.0565476f;
	float offsety = 0.486842f;
	hr = minutehandbitmap.CreateGraphicsResources(pRenderTarget, BITMAP_MINUTEHAND, 250, 10, 10, offsetx, offsety, scale);
	if (FAILED(hr))
		return hr;
	offsetx = 0.088462f;
	offsety = 0.48913f;
	hr = hourhandbitmap.CreateGraphicsResources(pRenderTarget, BITMAP_HOURHAND, 30, 50, 30, offsetx, offsety, scale);
	if (FAILED(hr))
		return hr;

	return hr;
}

void ClockFace::DiscardGraphicsResources()
{
	SafeRelease(&pBackGroundBrush);
	minutehandbitmap.DiscardGraphicsResources();
	hourhandbitmap.DiscardGraphicsResources();
}

void ClockFace::DrawBackGround(ID2D1HwndRenderTarget* pRenderTarget)
{
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(CenterX, CenterY));
	{
		const float radius1 = Radius - 16.0f;
		const float radius2 = Radius - 8.0f;
		const float radiusOutside = Radius;
		for (int i = 0; i < 60; i++)
		{
			float angle = ((float)i / 60.0f) * PI2;
			float ca = cos(angle);
			float sa = sin(angle);
			D2D1_POINT_2F pointa = { ca * radius1, sa * radius1 };
			D2D1_POINT_2F pointb = { ca * radius2, sa * radius2 };
			D2D1_POINT_2F point2 = { ca * radiusOutside, sa * radiusOutside };
			pRenderTarget->DrawLine((i % 5 == 0) ? pointa : pointb, point2, pBackGroundBrush, 2.5f);
		}
	}
}

void ClockFace::DrawHands(ID2D1HwndRenderTarget* pRenderTarget, float minAgle, float hourAngle)
{
	hourhandbitmap.Draw(pRenderTarget, hourAngle, CenterX, CenterY);
	minutehandbitmap.Draw(pRenderTarget, minAgle, CenterX, CenterY);
}