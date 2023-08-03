#include "PCH.h"
#include "AppWindow.h"
#include "Application.h"
#include "Resource.h"
#include "Timer.h"
#include "Math.h"
#include "ComException.h"

// Problems:
// Add Drop shadow contrast

void AppWindow::Init(HINSTANCE hInstance, Application* app)
{
	hInst = hInstance;
	m_App = app;
	RegisterWindowClass(hInstance);
}

BOOL AppWindow::Create(Timer* timer, int width, int height)
{
	m_pTimer = timer;
	const DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX; // WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX     WS_POPUP
	const DWORD exstyle = WS_EX_NOREDIRECTIONBITMAP;

	RECT winRect = { 0, 0, width, height };
	AdjustWindowRectEx(&winRect, style, false, exstyle);

	hWindow = CreateWindowExW(
		exstyle,
		m_WindowClass,
		m_Title,
		style,
		CW_USEDEFAULT, 0,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		nullptr,
		nullptr,
		hInst,
		this);

	if (!hWindow)
		return FALSE;

	ShowWindow(hWindow, SW_SHOW);
	UpdateWindow(hWindow);

	m_pTimer->SetAppWindow(hWindow);

	return TRUE;
}

void AppWindow::CreateGraphicsResources()
{
	m_DigitalClock.Init(m_Renderer.GetFactory());

	const int startbuttonwidth = 100;
	const int buttonwidth = 50;
	const int buttonheight = 50;
	const int smwidth = 22;
	const int smheight = 22;

	int playX = 75;
	int row1 = 135 + 200;
	int row2 = 185 + 200;
	int splitX = 150;

	int resetx = 24;
	int resety = 28 + 250;

	int zerox = 44;
	int zeroy = 226 + 200;

	int addx = 50;
	int incx = 100;
	int decx = 150;

	int sunincx = 300;
	int sundecx = 350;

	// Make the geometry inits consistent
	AppWindow::Button::InitGeometry(m_Renderer.GetFactory(), buttonwidth, buttonheight);
	m_ClockFace.InitGeometry(m_Renderer.GetFactory(), 12.0);
	m_Buttons[0].Init(m_Renderer.GetFactory(), BUTTON_START, playX, row1, startbuttonwidth, buttonheight);
	m_Buttons[1].Init(m_Renderer.GetFactory(), BUTTON_SPLIT, splitX, row1, buttonwidth, buttonheight);

	m_Buttons[2].Init(m_Renderer.GetFactory(), BUTTON_RESET, resetx, resety, smwidth, smheight);

	m_Buttons[3].Init(m_Renderer.GetFactory(), BUTTON_ADDTIME, addx, row2, buttonwidth, buttonheight);
	m_Buttons[4].Init(m_Renderer.GetFactory(), BUTTON_INCTIME, incx, row2, buttonwidth, buttonheight);
	m_Buttons[5].Init(m_Renderer.GetFactory(), BUTTON_DECTIME, decx, row2, buttonwidth, buttonheight);
	m_Buttons[6].Init(m_Renderer.GetFactory(), BUTTON_ZERO, zerox, zeroy, smwidth, smheight);

	m_Buttons[7].Init(m_Renderer.GetFactory(), BUTTON_TIMEOFDAYINC, sunincx, row2, buttonwidth, buttonheight);
	m_Buttons[8].Init(m_Renderer.GetFactory(), BUTTON_TIMEOFDAYDEC, sundecx, row2, buttonwidth, buttonheight);

	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	m_DigitalClock.CreateGraphicsResources(dc);
	m_ClockFace.CreateGraphicsResources(dc);
	AppWindow::Button::CreateButtonGraphicsResources(dc);
}

void AppWindow::Paint()
{
	static INT64 previousAlarmms = 0;
	static INT64 previousHours = 0;
	// Update minuteHandAngle to match clock
	if (!GrabLock)
	{
		minuteHandangle = getMinuteAngleRad(m_pTimer->GetAlarmTime());
	}

	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	dc->BeginDraw();
	dc->Clear();

	dc->SetTransform(D2D1::Matrix3x2F::Identity());
	for (int i = 0; i < ButtonCount; i++)
	{
		m_Buttons[i].Draw(dc, m_pTimer->isStarted(), (GrabbedElementRMB == BUTTON_ADDTIME), HoverElement, GrabbedElementLMB);
	}

	INT64 ms = m_pTimer->GetMilliseconds();
	INT64 absms = abs(ms);
	INT64 splitms = m_pTimer->GetSplitMilliseconds();

	INT64 alarmms = m_pTimer->GetAlarmTime();
	if (alarmms == 0 && previousAlarmms > 0 && !GrabLock && !Reseting)
	{
		m_App->m_SoundManager.Play(m_App->Alarm, 1.0f, 1.0f);
	}
	previousAlarmms = alarmms;
	{
		INT64 hours = GetHours(absms);
		if (hours > previousHours && !Adding)
			m_App->m_SoundManager.Play(m_App->Bwoop, 1.0f, 1.0f);
		previousHours = hours;
	}

	m_ClockFace.DrawSunMoon(dc, getDayAngleRad(splitms) + timeofDayOffset);

	m_ClockFace.DrawBackGround(dc);

	m_ClockFace.DrawHands(dc, minuteHandangle * Rad2DegFactor - 90, getHourAngleDeg(alarmms));

	m_DigitalClock.Draw(dc, m_TransformMain, (ms < 0), (absms && (HoverElement == BUTTON_RESET || HoverElement == BUTTON_SPLIT)), TRUE, GetDays(absms), GetHours(absms), GetMinutes(absms), GetTenths(absms));
	m_DigitalClock.Draw(dc, m_TransformSub, FALSE, (splitms && (HoverElement == BUTTON_RESET)), TRUE, GetDays(splitms), GetHours(splitms), GetMinutes(splitms), GetTenths(splitms));
	INT64 absTime = abs(AddTime);
	m_DigitalClock.Draw(dc, m_TransformAddtime, FALSE, (AddTime && (HoverElement == BUTTON_ZERO)), FALSE, GetDays(absTime), GetHours(absTime), GetMinutes(absTime), GetTenths(absTime));
	if (alarmms)
		m_DigitalClock.Draw(dc, m_TransformAlarm, FALSE, FALSE, TRUE, GetDays(alarmms), GetHours(alarmms), GetMinutes(alarmms), GetTenths(alarmms));

	HR(dc->EndDraw());
	HR(m_Renderer.GetSwapChain()->Present(1, 0));
	Reseting = FALSE;
	Adding = FALSE;
	PAINTSTRUCT ps;
	BeginPaint(hWindow, &ps);
	EndPaint(hWindow, &ps);
}

ATOM AppWindow::RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = m_WindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALLICON));

	return RegisterClassExW(&wcex);
}

void AppWindow::SetRepeatTimer(HWND hWnd)
{
	MouseTimerID = SetTimer(hWnd, ID_REPEATTIMER, 50, nullptr);
}

void AppWindow::KillRepeatTimer(HWND hWnd)
{
	KillTimer(hWnd, MouseTimerID);
}

void AppWindow::IncrementTime()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime += 144000000;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		AddTime += 60000;
	else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime += 1440000;
	else
		AddTime += 1000;
}

void AppWindow::DecrementTime()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime -= 144000000;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		AddTime -= 60000;
	else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime -= 1440000;
	else
		AddTime -= 1000;
	if (AddTime < 0)
		AddTime = 0;
}

BOOL AppWindow::CheckMouseHand(int mousex, int mousey)
{
	float dist = GetPointDist((float)mousex, (float)mousey, m_ClockFace.GetCenterX(), m_ClockFace.GetCenterY());
	if (dist < m_ClockFace.GetRadius() && dist > 18.0f)
	{
		mouseAngle = getMouseAngle(mousex, mousey, m_ClockFace.GetCenterX(), m_ClockFace.GetCenterY(), -HalfPI);
		float mouseHandAngle = GetAngleDistance(mouseAngle, minuteHandangle);
		float radialratio = dist / m_ClockFace.GetRadius();
		float angulardist = mouseHandAngle * radialratio;
		return angulardist < 0.15f;
	}
	return FALSE;
}

void AppWindow::MouseAdjustAlarm(int mousex, int mousey)
{
	mouseAngle = getMouseAngle(mousex, mousey, m_ClockFace.GetCenterX(), m_ClockFace.GetCenterY(), -HalfPI);
	float previousAngle = minuteHandangle;
	minuteHandangle = mouseAngle;
	float angledif = angleNormalize(minuteHandangle - previousAngle);
	if (angledif > PI)
		angledif -= PI2;
	INT64 timeadjustment = AngleToTime(angledif);
	m_pTimer->AdjustAlarm(timeadjustment);
	INT64 newalarmtime = m_pTimer->GetAlarmTime();
	if (newalarmtime == 0)
	{
		if (minuteHandangle < 6.0f && minuteHandangle > PI)
		{
			GrabLock = FALSE;
			m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
		}
		minuteHandangle = 0.0f;
	}
	else
	{
		static INT64 accumlatedTime = 0;
		static int minticks = 0;
		minticks++;
		accumlatedTime += abs(timeadjustment);
		if (accumlatedTime > 5000 && minticks > 4)
		{
			minticks = 0;
			accumlatedTime -= 5000;
			if (accumlatedTime > 5000)
				accumlatedTime = 0;
			float speed = abs(angledif);
			float pitch = min(1.0f, max(0.7f, speed * 5.0f));
			float vol = min(1.0f, max(0.5f, speed * 5.0f + 0.25f));

			static int index = 0;
			index++;
			if (index > 2)
				index = 0;
			switch (index)
			{
			case 0:
				m_App->m_SoundManager.Play(m_App->Ratchet1, vol, pitch);
				break;
			case 1:
				m_App->m_SoundManager.Play(m_App->Ratchet2, vol, pitch);
				break;
			case 2:
				m_App->m_SoundManager.Play(m_App->Ratchet3, vol, pitch);
				break;
			}
		}
	}
}

void AppWindow::AdjustTimeofDay(BOOL Forward)
{
	if (Forward)
		timeofdayvalue++;
	else
		timeofdayvalue--;
	if (timeofdayvalue > 191)
		timeofdayvalue = 0;
	else if (timeofdayvalue < 0)
		timeofdayvalue = 191;
	timeofDayOffset = (float(timeofdayvalue) / 192.0f) * PI2;

	WCHAR buffer[64];
	swprintf_s(buffer, 64, L"%f", timeofDayOffset);
	SetWindowTextW(hWindow, buffer);
}

LRESULT CALLBACK AppWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static AppWindow* window = nullptr;
	if (window)
		return window->ClassWndProc(hWnd, message, wParam, lParam);
	else
	{
		if (message == WM_CREATE)
		{
			window = reinterpret_cast<AppWindow*>(((CREATESTRUCTW*)lParam)->lpCreateParams);
			window->m_Renderer.Init(hWnd);
			window->CreateGraphicsResources();
			return 0;
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}

LRESULT CALLBACK AppWindow::ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int MouseDelay = 0;
	switch (message)
	{
	case WM_PAINT:
	{
		//if (m_Renderer.isValid())
		Paint();
		//else
		//	throw;


	}
	break;
	case WM_KILLFOCUS:
	case WM_MOUSELEAVE:
	{
		MouseinWindow = FALSE;
		HoverElement = -1;
		GrabbedElementLMB = -1;
		GrabLock = FALSE;
		RECT rc;
		GetClientRect(hWnd, &rc);
		InvalidateRect(hWnd, &rc, TRUE);
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (!MouseinWindow)
		{
			TRACKMOUSEEVENT mouseEvent = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, HOVER_DEFAULT };
			TrackMouseEvent(&mouseEvent);
			MouseinWindow = TRUE;
		}
		int OldHoverElement = HoverElement;
		HoverElement = -1;
		if (GrabLock)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
			MouseAdjustAlarm(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		else
		{
			for (int i = 0; i < ButtonCount; i++)
			{
				int button = m_Buttons[i].HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				if (button > 0)
				{
					HoverElement = button;
				}
			}
			if (OldHoverElement != HoverElement)
			{
				GrabbedElementLMB = -1;
				RECT rc;
				GetClientRect(hWnd, &rc);
				InvalidateRect(hWnd, &rc, TRUE);
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		MouseDelay = 0;
		SetRepeatTimer(hWnd);


		mouseAngle = getMouseAngle(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), m_ClockFace.GetCenterX(), m_ClockFace.GetCenterY(), -HalfPI);
		float mouseHandAngle = GetAngleDistance(mouseAngle, minuteHandangle);

		if (CheckMouseHand(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
		{
			MouseAdjustAlarm(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			GrabbedElementLMB = CLOCK_HANDLE;
			GrabLock = TRUE;
			SetCapture(hWnd);
		}
		else
		{
			if (GrabbedElementRMB == -1)
				GrabbedElementLMB = HoverElement;
		}
		RECT rc;
		GetClientRect(hWnd, &rc);
		InvalidateRect(hWnd, &rc, TRUE);
	}
	break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		GrabLock = FALSE;
		if (HoverElement == GrabbedElementLMB)
		{
			switch (GrabbedElementLMB)
			{
			case BUTTON_START:
			{
				if (!m_pTimer->isStarted())
				{
					m_pTimer->Start();
					m_App->m_SoundManager.Play(m_App->GoClick, 1.0f, 1.0f);
				}
				else
				{
					m_pTimer->Stop();
					m_App->m_SoundManager.Play(m_App->StopClick, 1.0f, 1.0f);
				}
			}
			break;
			case BUTTON_SPLIT:
				m_pTimer->Split();
				m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_RESET:
				Reseting = TRUE;
				m_pTimer->Reset();
				m_App->m_SoundManager.Play(m_App->ResetClick, 1.0f, 1.0f);
				break;
			case BUTTON_INCTIME:
				IncrementTime();
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_DECTIME:
				DecrementTime();
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ADDTIME:
				m_pTimer->AddTime(AddTime);
				Adding = TRUE;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ZERO:
				AddTime = 0;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_TIMEOFDAYINC:
			{
				AdjustTimeofDay(TRUE);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			break;
			case BUTTON_TIMEOFDAYDEC:
			{
				AdjustTimeofDay(FALSE);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			break;
			}
		}
		GrabbedElementLMB = -1;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (GrabbedElementLMB == -1)
			GrabbedElementRMB = HoverElement;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_RBUTTONUP:
	{
		if (HoverElement == GrabbedElementRMB)
		{
			if (GrabbedElementRMB == BUTTON_ADDTIME)
			{
				m_pTimer->AddTime(-AddTime);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
		}
		GrabbedElementRMB = -1;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_TIMER:
	{
		if (MouseDelay > 6)
		{
			if (GrabbedElementLMB == BUTTON_INCTIME)
				IncrementTime();
			else if (GrabbedElementLMB == BUTTON_DECTIME)
				DecrementTime();
			else if (GrabbedElementLMB == BUTTON_TIMEOFDAYINC)
				AdjustTimeofDay(TRUE);
			else if (GrabbedElementLMB == BUTTON_TIMEOFDAYDEC)
				AdjustTimeofDay(FALSE);
			else
				KillRepeatTimer(hWnd);
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
		MouseDelay++;
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
