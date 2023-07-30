#include "PCH.h"
#include "Timer.h"
#include "Resource.h"

Timer::Timer()
{
	m_Timer = this;
}

void Timer::Start()
{
	m_TimerID = SetTimer(nullptr, ID_TIMER, 50, Timerproc);
	m_bStarted = TRUE;
	start = std::chrono::steady_clock::now();
}

void Timer::Stop()
{
	KillTimer(nullptr, m_TimerID);
	m_bStarted = FALSE;
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	INT64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
	m_CurrentMS_Duration = m_CurrentMS_Duration + duration;
}

void Timer::Reset()
{
	KillTimer(nullptr, m_TimerID);
	m_bStarted = FALSE;
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	m_CurrentMS_Duration = 0;
	HWND window = m_Timer->AppWindow;
	m_SplitMS_Duration = 0;
	m_AlarmTime = 0;
	if (window)
	{
		ReDraw(window);
	}
}

INT64 Timer::GetMilliseconds()
{
	if (m_bStarted)
	{
		std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
		INT64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
		return (m_CurrentMS_Duration + duration) / 5;
	}
	else
	{
		return m_CurrentMS_Duration / 5;
	}
}

void Timer::Split()
{
	m_SplitMS_Duration += GetMilliseconds();
	start = std::chrono::steady_clock::now();
	m_CurrentMS_Duration = 0;
}

INT64 Timer::GetSplitMilliseconds()
{
	return m_SplitMS_Duration + GetMilliseconds();
}

void Timer::AddTime(INT64 time)
{
	BOOL IsAlarmSet = (GetAlarmTime() != 0);
	m_CurrentMS_Duration += time * 5;
	if (GetSplitMilliseconds() < 0)
	{
		start = std::chrono::steady_clock::now();
		m_CurrentMS_Duration = 0;
		m_SplitMS_Duration = 0;
	}
	if (!IsAlarmSet)
		m_AlarmTime = GetSplitMilliseconds();
}

void Timer::AdjustAlarm(INT64 ms)
{
	m_AlarmTime += ms;
	if (m_AlarmTime < GetSplitMilliseconds())
	{
		m_AlarmTime = GetSplitMilliseconds();
	}
}

INT64 Timer::GetAlarmTime()
{
	INT64 timeremaining = m_AlarmTime - GetSplitMilliseconds();
	if (timeremaining < 0)
	{
		timeremaining = 0;
		m_AlarmTime = GetSplitMilliseconds();
	}
	return timeremaining;
}

void Timer::Timerproc(HWND hWnd, UINT Param2, UINT_PTR Param3, DWORD Param4)
{
	HWND window = m_Timer->AppWindow;
	if (window)
	{
		ReDraw(window);
	}
	window = m_Timer->TimerWindow;
	if (window)
	{
		ReDraw(window);
	}
}

void Timer::ReDraw(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	InvalidateRect(hwnd, &rc, TRUE);
}