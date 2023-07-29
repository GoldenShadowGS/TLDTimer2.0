#include "PCH.h"
#include "Sound.h"
#include "Timer.h"

class AppWindow;
class TimerWindow;
class Direct2DDevice;

class Application
{
public:
	Application(HINSTANCE hInstance, LPWSTR lpCmdLine);
	Timer m_Timer;
	SoundManager m_SoundManager;
	WavFile Ratchet1;
	WavFile Ratchet2;
	WavFile Ratchet3;
	WavFile TimerClick;
	WavFile GoClick;
	WavFile StopClick;
	WavFile SplitClick;
	WavFile ResetClick;
	WavFile Alarm;
private:
	std::shared_ptr<AppWindow> m_AppWindow;
	//std::shared_ptr<TimerWindow> m_TimerWindow;
};