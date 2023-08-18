#include "PCH.h"
#include "AppWindow.h"
#include "Application.h"
#include "Resource.h"
#include "Timer.h"
#include "Math.h"
#include "ComException.h"

void AppWindow::Init(HINSTANCE hInstance, Application* app)
{
	hInst = hInstance;
	m_App = app;
	RegisterWindowClass(hInstance);
}

BOOL AppWindow::Create(Timer* timer, int width, int height)
{
	WindowWidth = (float)width;
	WindowHeight = (float)height;
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
	ID2D1Factory2* factory = m_Renderer.GetFactory();
	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	HR(dc->CreateSolidColorBrush(D2D1::ColorF(0.79f, 0.75f, 0.75f, 1.0f), BKGBrush.ReleaseAndGetAddressOf()));
	HR(dc->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f), BorderBrush.ReleaseAndGetAddressOf()));
	Button::InitShapes(factory, dc);
	m_DigitalClock.Init(factory, dc);
	m_ClockFace.Init(factory, dc);

	m_ClockText.Init(factory, dc, 20, -8.0f);

	TomaTroll = Load32BitBitmap(dc, BITMAP_TOMATROLL);

	const int lrgSize = 60;
	const int medSize = 30;
	const int smSize = 22;

	// Clusters
	int AddX = 4;
	int AddY = 0;
	int UpX = 50;
	int UpY = -15;
	int DownX = 50;
	int DownY = 15;
	int ZeroX = -38;
	int ZeroY = 50;
	int TimeX = -16;
	int TimeY = 50;

	m_Buttons[0].Create(factory, dc, BUTTON_START, 136, 390, 145, 74);
	m_Buttons[1].Create(factory, dc, BUTTON_RESET, 22, 390, smSize, smSize);
	m_Buttons[2].Create(factory, dc, BUTTON_TIMEOFDAYINC, 300, 390, medSize, medSize);
	m_Buttons[3].Create(factory, dc, BUTTON_TIMEOFDAYDEC, 340, 390, medSize, medSize);
	int XINCREMENT = 144;
	// Offsets
	int OffsetX = 60;
	int OffsetY = 470;
	for (int i = 0; i < 3; i++)
	{
		m_Buttons[4 + i * 4].Create(factory, dc, BUTTON_ADDTIME1 + i * 4, AddX + OffsetX, AddY + OffsetY, lrgSize, lrgSize);
		m_Buttons[5 + i * 4].Create(factory, dc, BUTTON_INCTIME1 + i * 4, UpX + OffsetX, UpY + OffsetY, medSize, medSize);
		m_Buttons[6 + i * 4].Create(factory, dc, BUTTON_DECTIME1 + i * 4, DownX + OffsetX, DownY + OffsetY, medSize, medSize);
		m_Buttons[7 + i * 4].Create(factory, dc, BUTTON_ZERO1 + i * 4, ZeroX + OffsetX, ZeroY + OffsetY, smSize, smSize);
		m_TransformAddtime[i] = D2D1::Matrix3x2F::Scale(0.1175f, 0.1175f) * D2D1::Matrix3x2F::Translation((float)TimeX + OffsetX, (float)TimeY + OffsetY);
		OffsetX += XINCREMENT;
	}
	m_Buttons[16].Create(factory, dc, BUTTON_BWOOP, 400, 390, 64, 64);
	m_ClockText.Set(m_pTimer->GetMilliseconds());
	AdjustTimeofDay(0);
}

void AppWindow::Paint()
{
	static INT64 previousAlarmms = 0;
	static INT64 previousHours = 0;
	// Update minuteHandAngle to match clock
	if (!m_UIstate.GrabLock)
	{
		minuteHandangle = getMinuteAngleRad(m_pTimer->GetAlarmTime());
	}

	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	dc->BeginDraw();
	dc->Clear();

	INT64 maintime = m_pTimer->GetMilliseconds();
	int trolltime = maintime % 60000;
	if (trolltime >= 0 && trolltime < 250 && maintime >= 60000 && m_UIstate.b_BWOOP)
	{
		FastRender = TRUE;
		TomaTroll.DrawScaled(dc, { 200, 220.0f }, 0.75f);
	}
	else
		FastRender = FALSE;

	D2D_RECT_F UI_Rect = { 0.0f, 340.0f, 440.0f, 540.0f };
	D2D1_POINT_2F p1 = { 0.0f, 340.0f };
	D2D1_POINT_2F p2 = { 440.0f, 340.0f };
	dc->SetTransform(D2D1::Matrix3x2F::Identity());
	dc->FillRectangle(UI_Rect, BKGBrush.Get());
	dc->DrawLine(p1, p2, BorderBrush.Get(), 1.0f);
	for (int i = 0; i < ButtonCount; i++)
	{
		m_Buttons[i].Draw(dc, m_pTimer->isStarted(), m_UIstate);
	}


	INT64 alarmTime = m_pTimer->GetAlarmTime();

	if (alarmTime == 0 && previousAlarmms > 0 && !m_UIstate.GrabLock && !Reseting)
	{
		m_App->m_SoundManager.Play(m_App->Alarm, 1.0f, 1.0f);
	}
	previousAlarmms = alarmTime;
	{
		INT64 hours = GetHours(maintime);
		if (hours > previousHours && !Adding && m_UIstate.b_BWOOP)
			m_App->m_SoundManager.Play(m_App->Bwoop, 1.0f, 1.0f);
		previousHours = hours;
	}

	ClockAngles clockAngle =
	{
		minuteHandangle * Rad2DegFactor - 90,
		getHourAngleDeg(alarmTime),
		getDayAngleRad(maintime) + timeofDayOffset,
		(m_UIstate.HoverElement == BUTTON_TIMEOFDAYINC || m_UIstate.HoverElement == BUTTON_TIMEOFDAYDEC)
	};

	m_ClockFace.Draw(dc, clockAngle);

	//m_DigitalClock.Draw(dc, m_TransformMain, (maintime && (m_UIstate.HoverElement == BUTTON_RESET)), TRUE, maintime);

	BOOL highlighted = (m_UIstate.HoverElement == BUTTON_INCTIME1 || m_UIstate.HoverElement == BUTTON_DECTIME1) ||
		(AddTime[0] && m_UIstate.HoverElement >= BUTTON_ADDTIME1 && m_UIstate.HoverElement <= BUTTON_ZERO1);

	m_DigitalClock.Draw(dc, m_TransformAddtime[0], highlighted, FALSE, AddTime[0]);

	highlighted = (m_UIstate.HoverElement == BUTTON_INCTIME2 || m_UIstate.HoverElement == BUTTON_DECTIME2) ||
		(AddTime[1] && m_UIstate.HoverElement >= BUTTON_ADDTIME2 && m_UIstate.HoverElement <= BUTTON_ZERO2);

	m_DigitalClock.Draw(dc, m_TransformAddtime[1], highlighted, FALSE, AddTime[1]);

	highlighted = (m_UIstate.HoverElement == BUTTON_INCTIME3 || m_UIstate.HoverElement == BUTTON_DECTIME3) ||
		(AddTime[2] && m_UIstate.HoverElement >= BUTTON_ADDTIME3 && m_UIstate.HoverElement <= BUTTON_ZERO3);

	m_DigitalClock.Draw(dc, m_TransformAddtime[2], highlighted, FALSE, AddTime[2]);

	if (alarmTime)
		m_DigitalClock.Draw(dc, m_TransformAlarm, (m_UIstate.GrabLock), TRUE, alarmTime);

	//m_ClockText.Set(maintime);
	float stringwidth = m_ClockText.GetStringWidth();
	D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Translation(WindowWidth * 0.5f - stringwidth * 0.5f, 240);
	m_ClockText.Draw(dc, transform);


	HR(dc->EndDraw());
	HR(m_Renderer.GetSwapChain()->Present(1, 0));
	Reseting = FALSE;
	Adding = FALSE;
	if (!FastRender)
	{
		PAINTSTRUCT ps;
		BeginPaint(hWindow, &ps);
		EndPaint(hWindow, &ps);
	}
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

BOOL AppWindow::IncrementTime(UINT index)
{
	constexpr int maxtime = 14400000 * 100 - 1;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime[index] += 144000000;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		AddTime[index] += 60000;
	else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime[index] += 1440000;
	else
		AddTime[index] += 1000;
	if (AddTime[index] > maxtime)
	{
		AddTime[index] = maxtime;
		return FALSE;
	}
	return TRUE;
}

BOOL AppWindow::DecrementTime(UINT index)
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime[index] -= 144000000;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		AddTime[index] -= 60000;
	else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		AddTime[index] -= 1440000;
	else
		AddTime[index] -= 1000;
	if (AddTime[index] < 0)
	{
		AddTime[index] = 0;
		return FALSE;
	}
	return TRUE;
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
			m_UIstate.GrabLock = FALSE;
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
	ss.Bwoop = m_UIstate.b_BWOOP;
	for (int i = 0; i < 3; i++)
	{
		ss.AddTime[i] = AddTime[i];
	}
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
		m_UIstate.b_BWOOP = ss.Bwoop;
		for (int i = 0; i < 3; i++)
		{
			AddTime[i] = ss.AddTime[i];
		}
		Adding = TRUE;
	}
}

void AppWindow::KeyPressed(ClockText& timeString, char key)
{
	if (key >= '0' && key <= '9')
	{
		if (timeString.Add(key))
		{
			Adding = TRUE;
			m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
		}
	}
	else if (key == 8) // 8 BackSpace
	{
		if (timeString.Back())
		{
			Adding = TRUE;
			m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
		}
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
		m_UIstate.HoverElement = -1;
		m_UIstate.GrabbedElementLMB = -1;
		m_UIstate.GrabLock = FALSE;
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
		int OldHoverElement = m_UIstate.HoverElement;
		m_UIstate.HoverElement = -1;
		if (m_UIstate.GrabLock)
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
					m_UIstate.HoverElement = button;
				}
			}
			if (OldHoverElement != m_UIstate.HoverElement)
			{
				m_UIstate.GrabbedElementLMB = -1;
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
			m_UIstate.GrabbedElementLMB = CLOCK_HANDLE;
			m_UIstate.GrabLock = TRUE;
			SetCapture(hWnd);
		}
		else
		{
			if (m_UIstate.GrabbedElementRMB == -1)
				m_UIstate.GrabbedElementLMB = m_UIstate.HoverElement;
		}
		RECT rc;
		GetClientRect(hWnd, &rc);
		InvalidateRect(hWnd, &rc, TRUE);
	}
	break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		m_UIstate.GrabLock = FALSE;
		if (m_UIstate.HoverElement == m_UIstate.GrabbedElementLMB)
		{
			switch (m_UIstate.GrabbedElementLMB)
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
			case BUTTON_INCTIME1:
				if (IncrementTime(0))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_DECTIME1:
				if (DecrementTime(0))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_ADDTIME1:
				m_pTimer->AddTime(AddTime[0]);
				m_ClockText.Set(m_pTimer->GetMilliseconds());
				Adding = TRUE;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ZERO1:
				AddTime[0] = 0;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;

			case BUTTON_INCTIME2:
				if (IncrementTime(1))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_DECTIME2:
				if (DecrementTime(1))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_ADDTIME2:
				m_pTimer->AddTime(AddTime[1]);
				m_ClockText.Set(m_pTimer->GetMilliseconds());
				Adding = TRUE;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ZERO2:
				AddTime[1] = 0;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;

			case BUTTON_INCTIME3:
				if (IncrementTime(2))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_DECTIME3:
				if (DecrementTime(2))
					m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				else
					m_App->m_SoundManager.Play(m_App->SplitClick, 1.0f, 1.0f);
				break;
			case BUTTON_ADDTIME3:
				m_pTimer->AddTime(AddTime[2]);
				m_ClockText.Set(m_pTimer->GetMilliseconds());
				Adding = TRUE;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ZERO3:
				AddTime[2] = 0;
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
			case BUTTON_BWOOP:
			{
				m_UIstate.b_BWOOP = !m_UIstate.b_BWOOP;
				if (m_UIstate.b_BWOOP)
					m_App->m_SoundManager.Play(m_App->Bwoop, 0.5f, 1.0f);
			}
			break;
			}
		}
		m_UIstate.GrabbedElementLMB = -1;
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
		if (m_UIstate.GrabbedElementLMB == -1 && (m_UIstate.HoverElement == BUTTON_ADDTIME1 || m_UIstate.HoverElement == BUTTON_ADDTIME2 || m_UIstate.HoverElement == BUTTON_ADDTIME3))
			m_UIstate.GrabbedElementRMB = m_UIstate.HoverElement;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_RBUTTONUP:
	{
		if (m_UIstate.HoverElement == m_UIstate.GrabbedElementRMB)
		{
			if (m_UIstate.GrabbedElementRMB == BUTTON_ADDTIME1)
			{
				m_pTimer->AddTime(-AddTime[0]);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			else if (m_UIstate.GrabbedElementRMB == BUTTON_ADDTIME2)
			{
				m_pTimer->AddTime(-AddTime[1]);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
			else if (m_UIstate.GrabbedElementRMB == BUTTON_ADDTIME3)
			{
				m_pTimer->AddTime(-AddTime[2]);
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
			}
		}
		m_UIstate.GrabbedElementRMB = -1;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_CHAR:
	{
		Adding = TRUE;
		if (!m_pTimer->isStarted())
		{
			KeyPressed(m_ClockText, (char)wParam);
			m_pTimer->SetTime(m_ClockText.GetTime());
		}
		Paint();
	}
	break;
	case WM_APP + 1:
	{
		m_ClockText.Set(m_pTimer->GetMilliseconds());
		Paint();
	}
	break;
	case WM_TIMER:
	{
		if (MouseDelay > 6)
		{
			if (m_UIstate.GrabbedElementLMB == BUTTON_INCTIME1)
				IncrementTime(0);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_DECTIME1)
				DecrementTime(0);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_INCTIME2)
				IncrementTime(1);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_DECTIME2)
				DecrementTime(1);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_INCTIME3)
				IncrementTime(2);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_DECTIME3)
				DecrementTime(2);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_TIMEOFDAYINC)
				AdjustTimeofDay(1);
			else if (m_UIstate.GrabbedElementLMB == BUTTON_TIMEOFDAYDEC)
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
