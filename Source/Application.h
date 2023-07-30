#include "PCH.h"
#include "Sound.h"
#include "Timer.h"
#include "AppWindow.h"

class AppWindow;
class TimerWindow;
class Direct2DDevice;

class Application
{
public:
	void Init(HINSTANCE hInstance, LPWSTR lpCmdLine);
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
	WavFile Bwoop;
private:
	AppWindow m_AppWindow;
};