#include "PCH.h"
#include "DigitalClock.h"
#include "Direct2D.h"
#include "Math.h"
#include "ComException.h"
#include "Timer.h"

void DigitalClock::SevenSegment::Segment::Init(ID2D1Factory2* pD2DFactory, BOOL vertical, float longdist, float shortdist, float halfwidth, float sk, D2D1_POINT_2F offset)
{
	HR(pD2DFactory->CreatePathGeometry(Geometry.ReleaseAndGetAddressOf()));

	HR(Geometry->Open(Sink.ReleaseAndGetAddressOf()));

	Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	D2D1_POINT_2F p1 = { -longdist, 0 };
	D2D1_POINT_2F p2 = { -shortdist,-halfwidth };
	D2D1_POINT_2F p3 = { shortdist  ,-halfwidth };
	D2D1_POINT_2F p4 = { longdist, 0 };
	D2D1_POINT_2F p5 = { shortdist, halfwidth };
	D2D1_POINT_2F p6 = { -shortdist, halfwidth };
	if (vertical)
	{
		p1 = { 0, -longdist };
		p2 = { -halfwidth, -shortdist };
		p3 = { -halfwidth, shortdist };
		p4 = { 0, longdist };
		p5 = { halfwidth, shortdist };
		p6 = { halfwidth , -shortdist };
	}
	Sink->BeginFigure(skew(p1 + offset, sk), D2D1_FIGURE_BEGIN_FILLED);
	Sink->AddLine(skew(p2 + offset, sk));
	Sink->AddLine(skew(p3 + offset, sk));
	Sink->AddLine(skew(p4 + offset, sk));
	Sink->AddLine(skew(p5 + offset, sk));
	Sink->AddLine(skew(p6 + offset, sk));
	Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	HR(Sink->Close());
}

void DigitalClock::SevenSegment::Segment::DrawSegment(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	pRenderTarget->FillGeometry(Geometry.Get(), pBrush);
}

void DigitalClock::SevenSegment::InitGeometry(ID2D1Factory2* pD2DFactory)
{
	const float longdist = 31.0f;
	const float shortdist = 19.0f;
	const float halfwidth = 12.0f;
	const float sk = -0.2f;
	const float space = 1.5f;
	D2D1_POINT_2F segoffset = { 0.0f, -longdist * 2.0f };
	D2D1_POINT_2F segmentSpacing = { 0.0f, -space * 2.0f };
	D2D1_POINT_2F offset = segoffset + segmentSpacing;
	Seg[0].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { -longdist, -longdist };
	segmentSpacing = { -space, -space };
	offset = segoffset + segmentSpacing;
	Seg[1].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { longdist, -longdist };
	segmentSpacing = { space, -space };
	offset = segoffset + segmentSpacing;
	Seg[2].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { 0.0f, 0.0f };
	segmentSpacing = { 0.0f, 0.0f };
	offset = segoffset + segmentSpacing;
	Seg[3].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { -longdist, longdist };
	segmentSpacing = { -space, space };
	offset = segoffset + segmentSpacing;
	Seg[4].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { longdist, longdist };
	segmentSpacing = { space, space };
	offset = segoffset + segmentSpacing;
	Seg[5].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { 0.0f, longdist * 2.0f };
	segmentSpacing = { 0.0f, space * 2.0f };
	offset = segoffset + segmentSpacing;
	Seg[6].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
}

void DigitalClock::CreateGraphicsResources(ID2D1DeviceContext* pRenderTarget)
{
	D2D1::ColorF color = D2D1::ColorF(0.02f, 0.02f, 0.02f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pNormalBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.8f, 0.04f, 0.04f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pHighlightBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.75f, 0.75f, 0.75f, 0.75f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pBackgroundBrush.ReleaseAndGetAddressOf()));
}

void DigitalClock::SevenSegment::Draw(ID2D1DeviceContext* pRenderTarget, int value, ID2D1SolidColorBrush* pFullBrush)
{
	switch (value)
	{
	case 0:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 1:
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 2:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 3:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 4:
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 5:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 6:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 7:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 8:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 9:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case -1:
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 'd':
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
	}
}

void DigitalClock::Init(ID2D1Factory2* pD2DFactory)
{
	m_SevenSegment.InitGeometry(pD2DFactory);
}

float DigitalClock::GetWidth(BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths)
{
	const float DigitSpacing = 100.0f;
	const float DotSpacing = 40.0f;
	float spacing = 0.0f;

	if (days > 9999)
		spacing += DigitSpacing;
	if (days > 999)
		spacing += DigitSpacing;
	if (days > 99)
		spacing += DigitSpacing;
	if (days > 9)
		spacing += DigitSpacing;
	if (days > 0)
	{
		spacing += DigitSpacing;
		spacing += DigitSpacing + DigitSpacing;
	}
	if (hours > 9)
		spacing += DigitSpacing;
	if (hours > 0)
		spacing += DigitSpacing + DotSpacing;
	if (mins > 9)
		spacing += DigitSpacing;
	if (bTenths)
		spacing += DigitSpacing + DotSpacing;
	return spacing;
}

void DigitalClock::Draw(ID2D1DeviceContext* dc, D2D1::Matrix3x2F transform, BOOL Highlighted, BOOL bTenths, INT64 ms)
{
	assert(ms >= 0);
	DrawInternal(dc, transform, Highlighted, bTenths, GetDays(ms), GetHours(ms), GetMinutes(ms), GetTenths(ms));
}

void DigitalClock::DrawInternal(ID2D1DeviceContext* dc, D2D1::Matrix3x2F transform, BOOL Highlighted, BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths)
{
	const float width = GetWidth(bTenths, days, hours, mins, tenths);
	const float DigitSpacing = 100.0f;
	const float DotSpacing = 40.0f;
	D2D1_ROUNDED_RECT roundedRect = { {-DigitSpacing * 0.75f, -100.0f, width + DigitSpacing * 0.75f, 100.0f}, 50.0f, 50.0f };
	dc->SetTransform(transform);
	dc->FillRoundedRectangle(roundedRect, pBackgroundBrush.Get());
	float spacing = 0.0f;
	D2D1::Matrix3x2F DigitTransform = D2D1::Matrix3x2F::Identity();
	ID2D1SolidColorBrush* pBrush = pNormalBrush.Get();
	if (Highlighted)
		pBrush = pHighlightBrush.Get();

	// 5 digits of days
	if (days > 9999)
	{
		int digit = (days / 10000) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 999)
	{
		int digit = (days / 1000) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 99)
	{
		int digit = (days / 100) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 9)
	{
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		int digit = (days / 10) % 10;
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 0)
	{
		int digit = days % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		//DrawColon(pRenderTarget);
		spacing += DigitSpacing;
		// d for Days
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, 'd', pBrush);
		spacing += DigitSpacing + DigitSpacing;
	}
	// Draw Hours
	int clippedhours = hours % 24;
	if (hours > 9)
	{
		int digit = clippedhours / 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (hours > 0)
	{
		int digit = clippedhours % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		DrawColon(dc, pBrush);
		spacing += DigitSpacing + DotSpacing;
	}
	// Draw Minutes
	int clippedminutes = mins % 60;
	if (mins > 9)
	{
		int digit = clippedminutes / 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
		spacing += DigitSpacing;
	}
	{
		int digit = clippedminutes % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
	}
	// Draw Tenths
	if (bTenths)
	{
		DrawDot(dc, pBrush);
		spacing += DigitSpacing + DotSpacing;
		int digit = tenths % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		dc->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(dc, digit, pBrush);
	}
}

void DigitalClock::DrawDot(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* Brush)
{
	D2D1_ELLIPSE ellipse = { { 70 + -0.2f * 70,  70 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
}

void DigitalClock::DrawColon(ID2D1DeviceContext* pRenderTarget, ID2D1SolidColorBrush* Brush)
{
	D2D1_ELLIPSE ellipse = { { 70 + -0.2f * 40,  40 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
	ellipse = { { 70 + -0.2f * -40, -40 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
}