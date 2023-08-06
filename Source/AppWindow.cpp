#include "PCH.h"
#include "AppWindow.h"
#include "Application.h"
#include "Resource.h"
#include "Timer.h"
#include "Math.h"
#include "ComException.h"

// TODO Egg Timer stop ticking down when it is being held by the mouse

void AppWindow::Init(HINSTANCE hInstance, Application* app, BOOL isBwoop)
{
	hInst = hInstance;
	m_App = app;
	b_Bwoop = isBwoop;
	RegisterWindowClass(hInstance);
}

BOOL AppWindow::Create(Timer* timer, int width, int height)
{
	m_pTimer = timer;
	Loadfile();
	const DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
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

	AppWindow::Button::InitGeometry(m_Renderer.GetFactory(), buttonwidth, buttonheight);
	m_ClockFace.InitGeometry(m_Renderer.GetFactory(), 12.0);
	m_Buttons[0].Init(m_Renderer.GetFactory(), BUTTON_START, playX, row1, startbuttonwidth, buttonheight);

	m_Buttons[1].Init(m_Renderer.GetFactory(), BUTTON_RESET, resetx, resety, smwidth, smheight);

	m_Buttons[2].Init(m_Renderer.GetFactory(), BUTTON_ADDTIME, addx, row2, buttonwidth, buttonheight);
	m_Buttons[3].Init(m_Renderer.GetFactory(), BUTTON_INCTIME, incx, row2, buttonwidth, buttonheight);
	m_Buttons[4].Init(m_Renderer.GetFactory(), BUTTON_DECTIME, decx, row2, buttonwidth, buttonheight);
	m_Buttons[5].Init(m_Renderer.GetFactory(), BUTTON_ZERO, zerox, zeroy, smwidth, smheight);

	m_Buttons[6].Init(m_Renderer.GetFactory(), BUTTON_TIMEOFDAYINC, sunincx, row2, buttonwidth, buttonheight);
	m_Buttons[7].Init(m_Renderer.GetFactory(), BUTTON_TIMEOFDAYDEC, sundecx, row2, buttonwidth, buttonheight);

	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	m_DigitalClock.CreateGraphicsResources(dc);
	m_ClockFace.CreateGraphicsResources(dc);
	AppWindow::Button::CreateButtonGraphicsResources(dc);
	shapesun = CreateShape(m_Renderer.GetFactory(), dc, SHAPE_SUN, D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.6f), 100.0f);
	shapemoon = CreateShape(m_Renderer.GetFactory(), dc, SHAPE_MOON, D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.6f), 100.0f);
	shapeclockticks = CreateShape(m_Renderer.GetFactory(), dc, SHAPE_CLICKTICKS, D2D1::ColorF(0.1f, 0.1f, 0.1f, 0.75f), 220.0f);
	AdjustTimeofDay(0);
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

	INT64 maintime = m_pTimer->GetMilliseconds();

	INT64 alarmTime = m_pTimer->GetAlarmTime();

	if (alarmTime == 0 && previousAlarmms > 0 && !GrabLock && !Reseting)
	{
		m_App->m_SoundManager.Play(m_App->Alarm, 1.0f, 1.0f);
	}
	previousAlarmms = alarmTime;
	{
		INT64 hours = GetHours(maintime);
		if (hours > previousHours && !Adding && b_Bwoop)
			m_App->m_SoundManager.Play(m_App->Bwoop, 1.0f, 1.0f);
		previousHours = hours;
	}


	m_ClockFace.DrawBackGround(dc, getDayAngleRad(maintime) + timeofDayOffset);

	m_ClockFace.DrawHands(dc, minuteHandangle * Rad2DegFactor - 90, getHourAngleDeg(alarmTime));


	m_DigitalClock.Draw(dc, m_TransformMain, (maintime && (HoverElement == BUTTON_RESET)), TRUE, maintime);

	m_DigitalClock.Draw(dc, m_TransformAddtime, (AddTime && (HoverElement == BUTTON_ZERO)), FALSE, AddTime);
	if (alarmTime)
		m_DigitalClock.Draw(dc, m_TransformAlarm, FALSE, TRUE, alarmTime);
	//shapesun.Draw(dc, { 120.0f, 120.0f });
	//shapemoon.Draw(dc, { 120.0f, 220.0f });
	//shapeclockticks.Draw(dc, { 130.0f, 130.0f });
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
			m_pTimer->MouseRelease();
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

void AppWindow::AdjustTimeofDay(int amount)
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		amount *= 12;
	constexpr int interval = 288;
	timeofdayvalue += amount;
	if (timeofdayvalue > 287)
		timeofdayvalue -= interval;
	else if (timeofdayvalue < 0)
		timeofdayvalue += interval;
	timeofDayOffset = (float(timeofdayvalue) / (float)interval) * PI2;
}

void AppWindow::SaveFile()
{
	Savedstate ss;
	m_pTimer->SaveTime(ss);
	ss.timeofdayvalue = timeofdayvalue;
	ss.AddTime1 = AddTime;
	std::ofstream file;
	file.open("TLDClock.data", std::ios::binary);
	if (file.is_open())
	{
		file.write(reinterpret_cast<char*>(&ss), sizeof(Savedstate));
		file.close();
	}
}

void AppWindow::Loadfile()
{
	Savedstate ss = {};
	std::ifstream is;
	is.open("TLDClock.data", std::ios::binary);
	if (is.is_open())
	{
		is.read(reinterpret_cast<char*>(&ss), sizeof(Savedstate));
		is.close();
		m_pTimer->LoadTime(ss);
		timeofdayvalue = ss.timeofdayvalue;
		AddTime = ss.AddTime1;
		Adding = TRUE;
	}
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
		Paint();
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
			m_pTimer->MouseGrab();
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
			case BUTTON_RESET:
				Reseting = TRUE;
				m_pTimer->Reset();
				timeofdayvalue = 0;
				AdjustTimeofDay(0);
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
				AdjustTimeofDay(1);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			break;
			case BUTTON_TIMEOFDAYDEC:
			{
				AdjustTimeofDay(-1);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			break;
			}
		}
		GrabbedElementLMB = -1;
		m_pTimer->MouseRelease();
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
				AdjustTimeofDay(1);
			else if (GrabbedElementLMB == BUTTON_TIMEOFDAYDEC)
				AdjustTimeofDay(-1);
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
		SaveFile();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
