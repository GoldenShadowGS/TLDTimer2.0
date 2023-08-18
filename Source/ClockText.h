#pragma once
#include "PCH.h"
#include "SevenSegmentText.h"
#include "TimeString.h"

class ClockText
{
public:
	void Init(ID2D1Factory2* pD2DFactory, ID2D1DeviceContext* dc, float size, float skew);
	void Draw(ID2D1DeviceContext* dc, D2D1::Matrix3x2F transform);
	float GetStringWidth();
	BOOL Add(char value);
	BOOL Back();
	BOOL Clear();
	void Set(INT64 ms);
	INT64 GetTime();
private:
	SevenSegmentText m_DigitalText;
	TimeString m_TimeString;
};