#pragma once
#include "PCH.h"
#include "Math.h"

struct Savedstate;

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
	~Timer();
	void Start();
	void Stop();
	void Reset();
	BOOL isStarted() { return m_bStarted; }
	INT64 GetMilliseconds();
	void SetAppWindow(HWND hwnd) { AppWindow = hwnd; }
	void AddTime(INT64 time);
	void AdjustAlarm(INT64 ms);
	INT64 GetAlarmTime();
	void MouseGrab();
	void MouseRelease();
	void LoadTime(const Savedstate& ss);
	void SaveTime(Savedstate& ss);
private:
	HWND AppWindow = nullptr;
	void ReDraw();
	void ClockThreadMain();

	class ActiveTimer
	{
	public:
		void ResetStart()
		{
			const std::lock_guard<std::mutex> lock(start_Mutex);
			start = std::chrono::steady_clock::now();
		}
		INT64 GetActiveDuration()
		{
			std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - GetStartTime()).count();
		}
	private:
		std::chrono::time_point<std::chrono::steady_clock> GetStartTime()
		{
			const std::lock_guard<std::mutex> lock(start_Mutex);
			return start;
		}
		std::mutex start_Mutex;
		std::chrono::time_point<std::chrono::steady_clock> start;
	};
	ActiveTimer m_ActiveTimer;
	INT64 m_AlarmTime = 0;
	INT64 m_AlarmStartTime = 0;
	BOOL m_MouseGrabbed = FALSE;
	class SavedDuration
	{
	public:
		INT64 GetSavedDuration()
		{
			const std::lock_guard<std::mutex> lock(saved_duration_Mutex);
			return m_SavedDuration;
		}
		void IncrementDuration(INT64 duration)
		{
			const std::lock_guard<std::mutex> lock(saved_duration_Mutex);
			m_SavedDuration += duration;
		}
		void SetDuration(INT64 duration)
		{
			const std::lock_guard<std::mutex> lock(saved_duration_Mutex);
			m_SavedDuration = duration;
		}
		void Reset()
		{
			const std::lock_guard<std::mutex> lock(saved_duration_Mutex);
			m_SavedDuration = 0;
		}
	private:
		std::mutex saved_duration_Mutex;
		INT64 m_SavedDuration = 0;
	};
	SavedDuration m_SavedDuration;
	HANDLE m_EventHandle = {};
	std::jthread m_ClockThread;
	std::atomic<BOOL> m_bStarted = FALSE;
	std::atomic<BOOL> m_bTimerThreadRunning = FALSE;
};