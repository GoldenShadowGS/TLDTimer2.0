#include "PCH.h"
#include "ClockFace.h"
#include "Direct2D.h"
#include "Math.h"
#include "Resource.h"
#include "ComException.h"

void ClockFace::Init(ID2D1Factory2* factory, ID2D1DeviceContext* dc)
{
	SunShape = CreateShape(factory, dc, SHAPE_SUN, D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.8f), 50.0f);
	MoonShape = CreateShape(factory, dc, SHAPE_MOON, D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.8f), 50.0f);
	ClockTicksShape = CreateShape(factory, dc, SHAPE_CLICKTICKS, D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.75f), 220.0f);

	float scale = 0.6f;
	D2D1_POINT_2F pivot = D2D1::Point2F(0.0565476f, 0.486842f);
	minutehandbitmap.Create(dc, BITMAP_MINUTEHAND, RGB(220, 10, 220), pivot, scale);

	pivot = D2D1::Point2F(0.088462f, 0.48913f);
	hourhandbitmap.Create(dc, BITMAP_HOURHAND, RGB(140, 10, 140), pivot, scale);
}

void ClockFace::Draw(ID2D1DeviceContext* dc, const ClockAngles& clockAngles)
{
	float opacity = clockAngles.HighlightSun ? 1.0f : 0.75f;
	ClockTicksShape.Draw(dc, m_Center);
	const float halfradius = m_Radius / 2.0f;
	D2D1_POINT_2F offset = { halfradius, 0.0f };
	SunShape.Draw(dc, m_Center + rotate(offset, clockAngles.SunAngle), opacity);
	offset = { -halfradius, 0.0f };
	MoonShape.Draw(dc, m_Center + rotate(offset, clockAngles.SunAngle), opacity);
	hourhandbitmap.Draw(dc, clockAngles.HourAngle, m_Center);
	minutehandbitmap.Draw(dc, clockAngles.MinuteAngle, m_Center);
}
