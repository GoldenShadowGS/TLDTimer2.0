#pragma once
#include "PCH.h"
#include "Math.h"

inline INT64 GetTenths(INT64 ms)
{
	return ms / 100;
}

inline INT64 GetMinutes(INT64 ms)
{
	return ms / 1000;
}

inline INT64 GetHours(INT64 ms)
{
	return ms / 60000;
}

inline INT64 GetDays(INT64 ms)
{
	return ms / 1440000;
}

inline float getMinuteAngleDeg(INT64 ms)
{
	float minutes = float(ms) / 1000.0f;
	return fmod(minutes, 60.0f) / 60.0f * 360.0f - 90;
}

inline float getHourAngleDeg(INT64 ms)
{
	float minutes = float(ms) / 60000.0f;
	return fmod(minutes, 12.0f) / 12.0f * 360.0f - 90;
}

inline float getDayAngleRad(INT64 ms)
{
	float hours = float(ms) / 60000.0f;
	return fmod(hours, 24.0f) / 24.0f * PI2 - HalfPI;
}

inline float getMinuteAngleRad(INT64 ms)
{
	float minutes = float(ms) / 1000.0f;
	return (fmod(minutes, 60.0f) / 60.0f) * PI2;
}

inline INT64 AngleToTime(float radians)
{
	return INT64((radians / (double)PI2) * 60000.0);
}

class Timer
{
public:
	Timer();
	void Start();
	void Stop();
	void Reset();
	BOOL isStarted() { return m_bStarted; }
	INT64 GetMilliseconds();
	void SetAppWindow(HWND hwnd) { AppWindow = hwnd; }
	void Split();
	INT64 GetSplitMilliseconds();
	void AddTime(INT64 time);
	void AdjustAlarm(INT64 ms);
	INT64 GetAlarmTime();
private:
	HWND AppWindow = nullptr;
	static inline Timer* m_Timer = nullptr;
	static void Timerproc(HWND hWnd, UINT Param2, UINT_PTR Param3, DWORD Param4);
	static void ReDraw(HWND hwnd);
	BOOL m_bStarted = FALSE;
	UINT_PTR m_TimerID {};
	std::chrono::time_point<std::chrono::steady_clock> start;
	INT64 m_AlarmTime = 0;
	INT64 m_CurrentMS_Duration = 0;
	INT64 m_SplitMS_Duration = 0;
};