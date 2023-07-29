#include "PCH.h"
#include "Application.h"
#include "AppWindow.h"
#include "TimerWindow.h"
#include "Direct2D.h"
#include "Resource.h"


Application::Application(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	m_SoundManager.Init(32);
	Ratchet1.Load(SOUND_RATCHET1);
	Ratchet2.Load(SOUND_RATCHET2);
	Ratchet3.Load(SOUND_RATCHET3);
	TimerClick.Load(SOUND_TIMER);
	GoClick.Load(SOUND_GOCLICK);
	StopClick.Load(SOUND_STOPCLICK);
	SplitClick.Load(SOUND_SPLIT);
	ResetClick.Load(SOUND_RESET);
	Alarm.Load(SOUND_ALARM);
	m_AppWindow = std::make_shared<AppWindow>(hInstance, this);
	//m_TimerWindow = std::make_shared<TimerWindow>(hInstance, this);

	if (!m_AppWindow->Create(&m_Timer, 440, 240))
		throw std::exception("Window Creation Failed");
}
