#include "PCH.h"
#include "ClockText.h"

void ClockText::Init(ID2D1Factory2* pD2DFactory, ID2D1DeviceContext* dc, float size, float skew)
{
	m_DigitalText.Init(pD2DFactory, dc, size, skew);
}

void ClockText::Draw(ID2D1DeviceContext* dc, D2D1::Matrix3x2F transform)
{
	m_DigitalText.DrawDigits(dc, m_TimeString, transform);
}

float ClockText::GetStringWidth()
{
	return m_DigitalText.GetStringWidth(m_TimeString);
}

BOOL ClockText::Add(char value)
{
	return m_TimeString.Add(value);
}

BOOL ClockText::Back()
{
	return m_TimeString.Back();
}

BOOL ClockText::Clear()
{
	return m_TimeString.Clear();
}

void ClockText::Set(INT64 ms)
{
	m_TimeString.Set(ms);
}

INT64 ClockText::GetTime()
{
	return m_TimeString.GetTime();
}