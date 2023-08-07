#pragma once
#include "PCH.h"
#include "Direct2D.h"
#include "DigitalClock.h"
#include "ClockFace.h"
#include "Shape.h"
#include "Button.h"

class Application;
class TimerWindow;
class Timer;

struct Savedstate
{
	INT64 m_SavedDuration = 0;
	INT64 AddTime[3] = {};
	INT64 m_AlarmTime = 0;
	int timeofdayvalue = 0;
	BOOL Bwoop = FALSE;
};

class AppWindow
{
public:
	void Init(HINSTANCE hInstance, Application* app);
	BOOL Create(Timer* timer, int width, int height);
	void CreateGraphicsResources();
	void Paint();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND GetwindowHandle() { return hWindow; }
	Renderer m_Renderer;
private:
	UIstate m_UIstate;
	ATOM RegisterWindowClass(HINSTANCE hInstance);
	LRESULT CALLBACK ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void SetRepeatTimer(HWND hWnd);
	void KillRepeatTimer(HWND hWnd);
	BOOL IncrementTime(UINT index);
	BOOL DecrementTime(UINT index);
	BOOL CheckMouseHand(int mousex, int mousey);
	void MouseAdjustAlarm(int mousex, int mousey);
	void AdjustTimeofDay(int amount);
	void SaveFile();
	void Loadfile();
	const WCHAR* m_Title = L"TLD StopWatch";
	const WCHAR* m_WindowClass = L"MainWindowsClass";
	HINSTANCE hInst = nullptr;
	HWND hWindow = nullptr;
	Application* m_App = nullptr;
	DigitalClock m_DigitalClock;
	ClockFace m_ClockFace;
	static const int ButtonCount = 17;
	Button m_Buttons[ButtonCount];
	BOOL Reseting = FALSE;
	BOOL Adding = FALSE;
	float mouseAngle = 0.0f;
	float minuteHandangle = 0.0f;
	D2D1::Matrix3x2F m_TransformAddtime[3] = {};
	D2D1::Matrix3x2F m_TransformMain{D2D1::Matrix3x2F::Scale(0.15f, 0.15f)* D2D1::Matrix3x2F::Translation(24, 200)};
	D2D1::Matrix3x2F m_TransformAlarm{D2D1::Matrix3x2F::Scale(0.1f, 0.1f)* D2D1::Matrix3x2F::Translation(260, 150 )};
	float timeofDayOffset = 0.0f;
	int timeofdayvalue = 0;
	INT64 AddTime[3] = {};
	Timer* m_pTimer = nullptr;
	BOOL MouseinWindow = FALSE;
	//BOOL b_Bwoop = FALSE;
	UINT_PTR MouseTimerID {};
	ComPtr<ID2D1SolidColorBrush> BKGBrush;
	ComPtr<ID2D1SolidColorBrush> BorderBrush;
};
