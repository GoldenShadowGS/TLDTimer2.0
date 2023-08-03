#include "PCH.h"
#include "ClockFace.h"
#include "Direct2D.h"
#include "Math.h"
#include "Resource.h"
#include "ComException.h"

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

void ClockFace::InitGeometry(ID2D1Factory2* pD2DFactory, float size)
{
	// Sun
	{
		sun.size = size;
		HRESULT hr = pD2DFactory->CreatePathGeometry(sun.Geometry.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return;

		hr = sun.Geometry->Open(sun.Sink.ReleaseAndGetAddressOf());
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

void ClockFace::CreateGraphicsResources(ID2D1DeviceContext* pRenderTarget)
{
	D2D1_GRADIENT_STOP gstop[3] = {};
	gstop[0].color = { 0.8f, 0.2f, 0.8f, 0.6f };
	gstop[0].position = { 0.0f };
	gstop[1].color = { 0.8f, 0.2f, 0.8f, 0.6f };
	gstop[1].position = { 0.5f };
	gstop[2].color = { 0.8f, 0.2f, 0.8f, 0.0f };
	gstop[2].position = { 1.0f };
	HR(pRenderTarget->CreateGradientStopCollection(gstop, 3, pStopsCollection.ReleaseAndGetAddressOf()));

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialprops = {};
	radialprops.center = D2D1::Point2F(0.0f, 0.0f);
	radialprops.radiusX = 25;
	radialprops.radiusY = 25;
	radialprops.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
	HR(pRenderTarget->CreateRadialGradientBrush(radialprops, pStopsCollection.Get(), pRadialBackGroundBrush.ReleaseAndGetAddressOf()));

	//pRadialBackGroundBrush->SetGradientOriginOffset();
	//HR(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.75f, 0.75f, 0.75f, 0.75f), pRadialBackGroundBrush.ReleaseAndGetAddressOf()));
	HR(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.75f, 0.75f, 0.75f, 0.75f), pBackGroundBrush.ReleaseAndGetAddressOf()));
	HR(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f), pTickBrush.ReleaseAndGetAddressOf()));
	HR(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.95f), pSunbrush.ReleaseAndGetAddressOf()));

	float scale = 0.6f;
	//pivot offsets
	float offsetx = 0.0565476f;
	float offsety = 0.486842f;
	HR(minutehandbitmap.CreateGraphicsResources(pRenderTarget, BITMAP_MINUTEHAND, 220, 10, 220, offsetx, offsety, scale));

	offsetx = 0.088462f;
	offsety = 0.48913f;
	HR(hourhandbitmap.CreateGraphicsResources(pRenderTarget, BITMAP_HOURHAND, 140, 10, 140, offsetx, offsety, scale));

}

void ClockFace::DrawBackGround(ID2D1DeviceContext* pRenderTarget, float angle)
{
	// Sun and moon
	{
		angle += HalfPI;
		const float bkgRadius = 8.0f;
		D2D1_POINT_2F rotateoffset = rotate({ 0.0f, Radius * 0.5f }, angle);
		D2D1::Matrix3x2F translationmatrix = D2D1::Matrix3x2F::Translation(CenterX + rotateoffset.x, CenterY + rotateoffset.y);
		pRenderTarget->SetTransform(translationmatrix);
		D2D1_ELLIPSE bkgellipse = { { 0.0f, 0.0f }, bkgRadius, bkgRadius };
		pRenderTarget->DrawEllipse(bkgellipse, pRadialBackGroundBrush.Get(), 35.0f);
		pRenderTarget->FillGeometry(moon.Geometry.Get(), pSunbrush.Get());

		rotateoffset = rotate({ 0.0f, Radius * 0.5f }, angle + PI);
		translationmatrix = D2D1::Matrix3x2F::Translation(CenterX + rotateoffset.x, CenterY + rotateoffset.y);
		D2D1_ELLIPSE sunellipse = { { 0.0f, 0.0f }, sun.size, sun.size };
		pRenderTarget->SetTransform(translationmatrix);
		pRenderTarget->DrawEllipse(bkgellipse, pRadialBackGroundBrush.Get(), 35.0f);
		pRenderTarget->FillEllipse(sunellipse, pSunbrush.Get());
		pRenderTarget->FillGeometry(sun.Geometry.Get(), pSunbrush.Get());
	}
	// Ticks
	{
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(CenterX, CenterY));
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
			pRenderTarget->DrawLine((i % 5 == 0) ? pointa : pointb, point2, pBackGroundBrush.Get(), 4.0f);
			pRenderTarget->DrawLine((i % 5 == 0) ? pointa : pointb, point2, pTickBrush.Get(), 2.5f);
		}
	}
}

void ClockFace::DrawHands(ID2D1DeviceContext* pRenderTarget, float minAgle, float hourAngle)
{
	hourhandbitmap.Draw(pRenderTarget, hourAngle, CenterX, CenterY);
	minutehandbitmap.Draw(pRenderTarget, minAgle, CenterX, CenterY);
}
