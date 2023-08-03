#include "PCH.h"
#include "Application.h"
#include "AppWindow.h"
#include "Direct2D.h"
#include "Resource.h"


void Application::Init(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	m_SoundManager.Init(64);
	Ratchet1.Load(SOUND_RATCHET1);
	Ratchet2.Load(SOUND_RATCHET2);
	Ratchet3.Load(SOUND_RATCHET3);
	TimerClick.Load(SOUND_TIMER);
	GoClick.Load(SOUND_GOCLICK);
	StopClick.Load(SOUND_STOPCLICK);
	SplitClick.Load(SOUND_SPLIT);
	ResetClick.Load(SOUND_RESET);
	Alarm.Load(SOUND_ALARM);
	Bwoop.Load(SOUND_BWOOP);
	std::wstring cmdline = lpCmdLine;
	BOOL isBwoop = cmdline.compare(L"bwoop") == 0;
	m_AppWindow.Init(hInstance, this, isBwoop);
	if (!m_AppWindow.Create(&m_Timer, 440, 440))
		throw std::exception("Window Creation Failed");
}
