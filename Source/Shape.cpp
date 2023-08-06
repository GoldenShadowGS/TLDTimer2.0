#include "PCH.h"
#include "Shape.h"
#include "Direct2D.h"
#include "ComException.h"
#include "Math.h"
#include "Resource.h"

RasterizedShape CreateShape(ID2D1Factory2* factory, ID2D1DeviceContext* dc, int shapeindex, D2D1::ColorF color, float size)
{
	RasterizedShape shape;
	ComPtr<ID2D1SolidColorBrush> Brush;
	HR(dc->CreateSolidColorBrush(color, Brush.ReleaseAndGetAddressOf()));
	ComPtr<ID2D1PathGeometry> Geometry;
	ComPtr<ID2D1GeometrySink> Sink;
	HR(factory->CreatePathGeometry(Geometry.ReleaseAndGetAddressOf()));
	HR(Geometry->Open(Sink.ReleaseAndGetAddressOf()));
	Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	switch (shapeindex)
	{
	case SHAPE_PLAY:
	{
		float radius = size * 0.375f;
		float angle = 0.0f;
		D2D1_POINT_2F p1 = { cos(angle) * radius, sin(angle) * radius };
		angle = ThirdPI;
		D2D1_POINT_2F p2 = { cos(angle) * radius, sin(angle) * radius };
		angle = ThirdPI * 2.0f;
		D2D1_POINT_2F p3 = { cos(angle) * radius, sin(angle) * radius };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddLine(p2);
		Sink->AddLine(p3);
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	break;
	case SHAPE_PAUSE:
	{
		float radius = size * 0.3f;
		float width = radius * 0.26f;
		float height = radius * 1.0f;
		float spacing = radius * 0.5f;
		D2D1_POINT_2F p1 = { -width - spacing, -height };
		D2D1_POINT_2F p2 = { width - spacing, -height };
		D2D1_POINT_2F p3 = { width - spacing, height };
		D2D1_POINT_2F p4 = { -width - spacing, height };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddLine(p2);
		Sink->AddLine(p3);
		Sink->AddLine(p4);
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		p1 = { -width + spacing, -height };
		p2 = { width + spacing, -height };
		p3 = { width + spacing, height };
		p4 = { -width + spacing, height };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddLine(p2);
		Sink->AddLine(p3);
		Sink->AddLine(p4);
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	break;
	case SHAPE_RESET:
	{
		float radiusOuter = size / 3;
		float radiusInner = radiusOuter * (2.0f / 3.0f);
		float midradius = (radiusOuter - radiusInner) * 0.5f + radiusInner;
		float arrowwidth = radiusOuter * (4.0f / 9.0f);
		float anglearrow = 0.5f;
		float angle1 = -2.35f;
		float angle2 = 3.2f;
		D2D1_POINT_2F p1 = { cos(angle1) * radiusInner, sin(angle1) * radiusInner };
		D2D1_POINT_2F p2 = { cos(angle2) * radiusInner, sin(angle2) * radiusInner };
		D2D1_POINT_2F p3 = { cos(angle2) * radiusOuter, sin(angle2) * radiusOuter };
		D2D1_POINT_2F p4 = { cos(angle1) * radiusOuter, sin(angle1) * radiusOuter };
		D2D1_POINT_2F p5 = { cos(angle1) * (midradius - arrowwidth), sin(angle1) * (midradius - arrowwidth) };
		D2D1_POINT_2F p6 = { cos(angle1 - anglearrow) * midradius, sin(angle1 - anglearrow) * midradius };
		D2D1_POINT_2F p7 = { cos(angle1) * (midradius + arrowwidth), sin(angle1) * (midradius + arrowwidth) };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(radiusInner, radiusInner), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Sink->AddLine(p3);
		Sink->AddArc(D2D1::ArcSegment(p4, D2D1::SizeF(radiusOuter, radiusOuter), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Sink->AddLine(p5);
		Sink->AddLine(p6);
		Sink->AddLine(p7);
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	break;
	case SHAPE_SUN:
	{
		float sunradius = size / 4.0f;
		float trianglesize = sunradius * 0.3f;
		float angle = 0.0f;
		D2D1_POINT_2F p1 = { -trianglesize, 0.0f };
		D2D1_POINT_2F p2 = { trianglesize, 0.0f };
		D2D1_POINT_2F p3 = { 0.0f, -trianglesize * 2.0f };
		D2D1_POINT_2F baseoffset = { 0.0f, -sunradius * 1.1f };
		for (int i = 0; i < 8; i++)
		{
			angle = PI2 * ((float)i / 8.0f);
			D2D1_POINT_2F offset = rotate(baseoffset, angle);
			Sink->BeginFigure(translate(rotate(p1, angle), offset), D2D1_FIGURE_BEGIN_FILLED);
			Sink->AddLine(translate(rotate(p2, angle), offset));
			Sink->AddLine(translate(rotate(p3, angle), offset));
			Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
	}
	break;
	case SHAPE_MOON:
	{
		float moonradius = size / 4.0f;
		float angle1 = 0.95f;
		float angle2 = -0.95f;
		D2D1_POINT_2F p1 = { cos(angle1) * moonradius, sin(angle1) * moonradius };
		D2D1_POINT_2F p2 = { cos(angle2) * moonradius, sin(angle2) * moonradius };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(moonradius, moonradius), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Sink->AddArc(D2D1::ArcSegment(p1, D2D1::SizeF(moonradius * 0.7f, moonradius * 0.7f), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	break;
	}
	HR(Sink->Close());
	shape.m_Size.width = size;
	shape.m_Size.height = size;
	shape.m_Pivot.x = size / 2.0f;
	shape.m_Pivot.y = size / 2.0f;
	if (shapeindex == SHAPE_SUN || shapeindex == SHAPE_MOON)
		shape.RasterizeSunMoon(dc, Geometry.Get(), Brush.Get(), (shapeindex == SHAPE_SUN), size);
	else if (shapeindex == SHAPE_CLICKTICKS)
		shape.RasterizeClockTicks(dc, Brush.Get(), size / 2.0f);
	else
		shape.Rasterize(dc, Geometry.Get(), Brush.Get());
	return shape;
}

void RasterizedShape::Draw(ID2D1DeviceContext* dc, D2D1_POINT_2F center)
{
	dc->SetTransform(D2D1::Matrix3x2F::Translation(center.x - m_Pivot.x, center.y - m_Pivot.y));
	D2D1_RECT_F	rect = D2D1::RectF(0, 0, m_Size.width, m_Size.height);
	dc->DrawBitmap(m_Bitmap.Get(), rect, 1.0f);
}

void RasterizedShape::Rasterize(ID2D1DeviceContext* dc, ID2D1PathGeometry* geometry, ID2D1SolidColorBrush* brush)
{
	ComPtr<ID2D1BitmapRenderTarget> BitmapRenderTarget;
	HR(dc->CreateCompatibleRenderTarget(m_Size, &BitmapRenderTarget));

	BitmapRenderTarget->BeginDraw();
	BitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(m_Pivot.x, m_Pivot.y));
	BitmapRenderTarget->FillGeometry(geometry, brush);
	HR(BitmapRenderTarget->EndDraw());

	HR(BitmapRenderTarget->GetBitmap(m_Bitmap.ReleaseAndGetAddressOf()));
}

void RasterizedShape::RasterizeClockTicks(ID2D1DeviceContext* dc, ID2D1SolidColorBrush* brush, float radius)
{
	ComPtr<ID2D1BitmapRenderTarget> BitmapRenderTarget;
	HR(dc->CreateCompatibleRenderTarget(m_Size, &BitmapRenderTarget));
	ComPtr<ID2D1SolidColorBrush> BackGroundBrush;
	HR(BitmapRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.75f, 0.75f, 0.75f, 0.75f), BackGroundBrush.ReleaseAndGetAddressOf()));

	BitmapRenderTarget->BeginDraw();
	BitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(m_Pivot.x, m_Pivot.y));

	const float radius1 = radius - 16.0f;
	const float radius2 = radius - 8.0f;
	const float radiusOutside = radius;
	float tickwidth = radius * 0.0363636f;
	for (int i = 0; i < 60; i++)
	{
		float angle = ((float)i / 60.0f) * PI2;
		float ca = cos(angle);
		float sa = sin(angle);
		D2D1_POINT_2F pointa = { ca * radius1, sa * radius1 };
		D2D1_POINT_2F pointb = { ca * radius2, sa * radius2 };
		D2D1_POINT_2F point2 = { ca * radiusOutside, sa * radiusOutside };
		BitmapRenderTarget->DrawLine((i % 5 == 0) ? pointa : pointb, point2, BackGroundBrush.Get(), tickwidth); // 4.0
		BitmapRenderTarget->DrawLine((i % 5 == 0) ? pointa : pointb, point2, brush, tickwidth * 0.625f);
	}
	HR(BitmapRenderTarget->EndDraw());
	HR(BitmapRenderTarget->GetBitmap(m_Bitmap.ReleaseAndGetAddressOf()));
}

void RasterizedShape::RasterizeSunMoon(ID2D1DeviceContext* dc, ID2D1PathGeometry* geometry, ID2D1SolidColorBrush* brush, BOOL sun, float radius)
{
	//ComPtr<ID2D1SolidColorBrush> Brush;
	//HR(dc->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.6f), Brush.ReleaseAndGetAddressOf()));
	ComPtr<ID2D1GradientStopCollection> StopsCollection;
	ComPtr<ID2D1RadialGradientBrush> RadialBackGroundBrush;
	ComPtr<ID2D1BitmapRenderTarget> BitmapRenderTarget;
	HR(dc->CreateCompatibleRenderTarget(m_Size, &BitmapRenderTarget));

	D2D1_GRADIENT_STOP gstop[3] = {};
	gstop[0].color = { 0.8f, 0.2f, 0.8f, 0.6f };
	gstop[0].position = { 0.0f };
	gstop[1].color = { 0.8f, 0.2f, 0.8f, 0.6f };
	gstop[1].position = { 0.5f };
	gstop[2].color = { 0.8f, 0.2f, 0.8f, 0.0f };
	gstop[2].position = { 1.0f };
	HR(BitmapRenderTarget->CreateGradientStopCollection(gstop, 3, StopsCollection.ReleaseAndGetAddressOf()));

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialprops = {};
	radialprops.center = D2D1::Point2F(0.0f, 0.0f);
	radialprops.radiusX = radius / 2.0f;
	radialprops.radiusY = radius / 2.0f;
	radialprops.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
	HR(BitmapRenderTarget->CreateRadialGradientBrush(radialprops, StopsCollection.Get(), RadialBackGroundBrush.ReleaseAndGetAddressOf()));

	BitmapRenderTarget->BeginDraw();

	//draw outline Temp
	//{
	//	BitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	//	D2D1_POINT_2F p1 = { 0, 0 };
	//	D2D1_POINT_2F p2 = { m_Size.width, 0 };
	//	D2D1_POINT_2F p3 = { m_Size.width, m_Size.height };
	//	D2D1_POINT_2F p4 = { 0, m_Size.height };
	//	BitmapRenderTarget->DrawLine(p1, p2, brush, 2.0f);
	//	BitmapRenderTarget->DrawLine(p2, p3, brush, 2.0f);
	//	BitmapRenderTarget->DrawLine(p3, p4, brush, 2.0f);
	//	BitmapRenderTarget->DrawLine(p4, p1, brush, 2.0f);
	//}
	BitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(m_Pivot.x, m_Pivot.y));

	const float bkgRadius = 8.0f;

	D2D1_ELLIPSE bkgellipse = { { 0.0f, 0.0f }, bkgRadius, bkgRadius };
	BitmapRenderTarget->DrawEllipse(bkgellipse, RadialBackGroundBrush.Get(), radius);
	if (sun)
	{
		D2D1_ELLIPSE sunellipse = { { 0.0f, 0.0f }, radius / 4.0f, radius / 4.0f };
		BitmapRenderTarget->FillEllipse(sunellipse, brush);
	}
	BitmapRenderTarget->FillGeometry(geometry, brush);

	//BitmapRenderTarget->FillGeometry(geometry, Brush.Get());
	HR(BitmapRenderTarget->EndDraw());

	HR(BitmapRenderTarget->GetBitmap(m_Bitmap.ReleaseAndGetAddressOf()));
}
