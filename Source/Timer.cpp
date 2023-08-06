#include "PCH.h"
#include "Timer.h"
#include "Resource.h"
#include "AppWindow.h"

Timer::Timer()
{
	m_EventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_EventHandle == INVALID_HANDLE_VALUE)
		throw std::exception("Invalid Event Handle");
}

Timer::~Timer()
{
	Stop();
	if (m_EventHandle)
		CloseHandle(m_EventHandle);
}

void Timer::Start()
{
	if (!m_bStarted)
	{
		m_ActiveTimer.ResetStart();
		m_bStarted = TRUE;
		if (!m_bTimerThreadRunning)
		{
			m_bTimerThreadRunning = TRUE;
			m_ClockThread = std::jthread(&Timer::ClockThreadMain, this);
		}
	}
}

void Timer::Stop()
{
	if (m_bStarted)
	{
		m_bStarted = FALSE;
		SetEvent(m_EventHandle);
		m_SavedDuration.IncrementDuration(m_ActiveTimer.GetActiveDuration());
	}
}

void Timer::Reset()
{
	Stop();
	m_SavedDuration.Reset();
	m_AlarmTime = 0;
	ReDraw();
}

INT64 Timer::GetMilliseconds()
{
	if (m_bStarted)
		return (m_SavedDuration.GetSavedDuration() + m_ActiveTimer.GetActiveDuration()) / 5;
	else
		return m_SavedDuration.GetSavedDuration() / 5;
}

void Timer::AddTime(INT64 time)
{
	BOOL IsAlarmSet = (GetAlarmTime() != 0);
	m_SavedDuration.IncrementDuration(time * 5);
	if (GetMilliseconds() < 0)
	{
		m_ActiveTimer.ResetStart();
		m_SavedDuration.Reset();
	}
	if (!IsAlarmSet)
		m_AlarmTime = GetMilliseconds();
}

void Timer::AdjustAlarm(INT64 ms)
{
	m_AlarmTime += ms;
}

INT64 Timer::GetAlarmTime()
{
	INT64 timeremaining = m_MouseGrabbed ? m_AlarmTime - m_AlarmStartTime : m_AlarmTime - GetMilliseconds();
	if (timeremaining < 0)
	{
		timeremaining = 0;
	}
	return timeremaining;
}

void Timer::MouseGrab()
{
	m_MouseGrabbed = TRUE;
	m_AlarmStartTime = GetMilliseconds();
}

void Timer::MouseRelease()
{
	if (m_MouseGrabbed)
	{
		m_MouseGrabbed = FALSE;
		m_AlarmTime += GetMilliseconds() - m_AlarmStartTime;
		if ((m_AlarmTime - GetMilliseconds()) < 0)
		{
			m_AlarmTime = GetMilliseconds();
		}
	}
}

void Timer::ReDraw()
{
	if (AppWindow)
	{
		RECT rc;
		GetClientRect(AppWindow, &rc);
		InvalidateRect(AppWindow, &rc, TRUE);
	}
}

void Timer::LoadTime(const Savedstate& ss)
{
	m_AlarmTime = ss.m_AlarmTime;
	m_SavedDuration.SetDuration(ss.m_SavedDuration);
}

void Timer::SaveTime(Savedstate& ss)
{
	if (isStarted())
		Stop();
	ss.m_AlarmTime = m_AlarmTime;
	ss.m_SavedDuration = m_SavedDuration.GetSavedDuration();
}

void Timer::ClockThreadMain()
{
	const UINT SleepAmount = 500;
	while (m_bStarted)
	{
		if (m_bStarted)
		{
			INT64 CurrentTime = m_SavedDuration.GetSavedDuration() + m_ActiveTimer.GetActiveDuration();
			INT64 difference = CurrentTime - ((CurrentTime / SleepAmount) * SleepAmount);
			INT64 sleeptime = SleepAmount - difference;
			WaitForSingleObject(m_EventHandle, (UINT)sleeptime);
			ReDraw();
		}
	}
	m_bTimerThreadRunning = FALSE;
}
