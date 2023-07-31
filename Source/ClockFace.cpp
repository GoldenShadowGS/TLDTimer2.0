#include "PCH.h"
#include "ClockFace.h"
#include "Direct2D.h"
#include "Math.h"
#include "Resource.h"

ClockFace::TimeofDayShape::~TimeofDayShape()
{
	SafeRelease(&Geometry);
	SafeRelease(&Sink);
}

D2D1_POINT_2F rotate(D2D1_POINT_2F point, float angle)
{
	float cosa = cos(angle);
	float sina = sin(angle);
	return { point.x * cosa - point.y * sina,point.y * cosa + point.x * sina };
}

D2D1_POINT_2F translate(D2D1_POINT_2F point, D2D1_POINT_2F point2)
{
	return { point.x + point2.x, point.y + point2.y };
}

void ClockFace::InitGeometry(ID2D1Factory* pD2DFactory, float size)
{
	// Sun
	{
		sun.size = size;
		HRESULT hr = pD2DFactory->CreatePathGeometry(&sun.Geometry);
		if (FAILED(hr)) return;

		hr = sun.Geometry->Open(&sun.Sink);
		if (FAILED(hr)) return;
		sun.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float sunradius = size;
		float trianglesize = size * 0.3f;
		float angle = 0.0f;
		D2D1_POINT_2F p1 = { -trianglesize, 0.0f };
		D2D1_POINT_2F p2 = { trianglesize, 0.0f };
		D2D1_POINT_2F p3 = { 0.0f, -trianglesize * 2.0f };
		D2D1_POINT_2F baseoffset = { 0.0f, -size * 1.1f };
		for (int i = 0; i < 8; i++)
		{
			angle = PI2 * ((float)i / 8.0f);
			D2D1_POINT_2F offset = rotate(baseoffset, angle);
			sun.Sink->BeginFigure(translate(rotate(p1, angle), offset), D2D1_FIGURE_BEGIN_FILLED);
			sun.Sink->AddLine(translate(rotate(p2, angle), offset));
			sun.Sink->AddLine(translate(rotate(p3, angle), offset));
			sun.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
		hr = sun.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Moon
	{
		moon.size = size;
		HRESULT hr = pD2DFactory->CreatePathGeometry(&moon.Geometry);
		if (FAILED(hr)) return;

		hr = moon.Geometry->Open(&moon.Sink);
		if (FAILED(hr)) return;
		moon.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);

		float angle1 = 0.5f;
		float angle2 = -0.5f;
		D2D1_POINT_2F p1 = { cos(angle1) * size, sin(angle1) * size };
		D2D1_POINT_2F p2 = { cos(angle2) * size, sin(angle2) * size };
		moon.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		moon.Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(size, size), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		moon.Sink->AddArc(D2D1::ArcSegment(p1, D2D1::SizeF(size * 0.7f, size * 0.35f), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));

		moon.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = moon.Sink->Close();
		if (FAILED(hr)) return;
	}
}

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

	if (pSunbrush == nullptr)
		hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.05f, 0.05f, 0.05f, 0.5f), &pSunbrush);
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
	SafeRelease(&pSunbrush);
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

void ClockFace::DrawSunMoon(ID2D1HwndRenderTarget* pRenderTarget, float angle)
{
	D2D1_POINT_2F rotateoffset = rotate({ 0.0f, Radius * 0.9f }, angle);
	D2D1::Matrix3x2F translationmatrix = D2D1::Matrix3x2F::Translation(CenterX + rotateoffset.x, CenterY + rotateoffset.y);
	pRenderTarget->SetTransform(translationmatrix);
	pRenderTarget->FillGeometry(moon.Geometry, pSunbrush);

	rotateoffset = rotate({ 0.0f, Radius * 0.9f }, angle + PI);
	translationmatrix = D2D1::Matrix3x2F::Translation(CenterX + rotateoffset.x, CenterY + rotateoffset.y);
	D2D1_ELLIPSE buttonellipse = { { 0.0f, 0.0f }, sun.size, sun.size };
	pRenderTarget->SetTransform(translationmatrix);
	pRenderTarget->FillEllipse(buttonellipse, pSunbrush);
	pRenderTarget->FillGeometry(sun.Geometry, pSunbrush);
}