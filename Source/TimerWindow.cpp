#include "PCH.h"
#include "TimerWindow.h"
#include "Resource.h"
#include "Direct2D.h"
#include "Math.h"
#include "Application.h"

TimerWindow::TimerWindow(HINSTANCE hInstance, Application* app) : hInst(hInstance), m_App(app)
{
	RegisterWindowClass(hInstance);
	m_Direct2DDevice.InitializeFactory();
}

BOOL TimerWindow::Create(HWND parentWindow, int x, int y, int width, int height)
{
	const DWORD style = WS_POPUP; // WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX     WS_POPUP
	const DWORD exstyle = 0;

	RECT winrect = { 0, 0, width, height };
	AdjustWindowRectEx(&winrect, style, false, exstyle);

	hWindow = CreateWindowExW(
		exstyle,
		m_WindowClass,
		m_Title,
		style,
		x, y,
		winrect.right - winrect.left,
		winrect.bottom - winrect.top,
		parentWindow,
		nullptr,
		hInst,
		nullptr);

	if (!hWindow)
		return FALSE;

	SetWindowLongPtrW(hWindow, GWLP_USERDATA, (INT64)this);
	SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_COLORKEY);
	//SetWindowPos(hMainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	windowAlive = TRUE;

	ShowWindow(hWindow, SW_SHOW);
	UpdateWindow(hWindow);

	return TRUE;
}

HRESULT TimerWindow::CreateGraphicsResources()
{
	HRESULT hr = m_Direct2DDevice.CreateRenderTargetHwnd(hWindow);
	if (FAILED(hr))
		return hr;
	ID2D1HwndRenderTarget* pRenderTarget = m_Direct2DDevice.getD2DRenderTarget();
	hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.02f, 0.02f, 0.02f, 1.0f), &pBlackBrush);
	if (FAILED(hr))
		return hr;
	return hr;
}

void TimerWindow::DiscardGraphicsResources()
{
	m_Direct2DDevice.ReleaseGraphicsResources();
	SafeRelease(&pBlackBrush);
}

void TimerWindow::KillWindow()
{
	windowAlive = FALSE;
	DiscardGraphicsResources();
	DestroyWindow(GetwindowHandle());
}

void TimerWindow::Paint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		ID2D1HwndRenderTarget* pRenderTarget = m_Direct2DDevice.getD2DRenderTarget();
		PAINTSTRUCT ps;
		BeginPaint(hWindow, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

		RECT rect = {};
		GetClientRect(hWindow, &rect);
		float x = (rect.right - rect.left) / 2.0f;
		float y = (rect.bottom - rect.top) / 2.0f;
		D2D1_ELLIPSE ellipse = { { x,  y }, x / 2.0f, y / 2.0f };
		pRenderTarget->FillEllipse(ellipse, pBlackBrush);

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			m_Direct2DDevice.ReleaseGraphicsResources();
		}
		EndPaint(hWindow, &ps);
	}
}

ATOM TimerWindow::RegisterWindowClass(HINSTANCE hInstance)
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

LRESULT CALLBACK TimerWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TimerWindow* window = reinterpret_cast<TimerWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (window)
		return window->ClassWndProc(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK TimerWindow::ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static float PrevX = 0.0f;
	static float PrevY = 0.0f;
	switch (message)
	{
	case WM_PAINT:
	{
		Paint();
		return 0;
	}
	break;
	case WM_KILLFOCUS:
	case WM_MOUSELEAVE:
	{

	}
	break;
	case WM_MOUSEMOVE:
	{
		static float distance = 0.0f;
		float mouseX = (float)GET_X_LPARAM(lParam);
		float mouseY = (float)GET_Y_LPARAM(lParam);

		float newdist = GetPointDist(mouseX, mouseY, PrevX, PrevY);
		float pitch = newdist;
		float vol = min(2.0f, newdist / 4.0f + 0.25f);
		PrevX = mouseX;
		PrevY = mouseY;
		distance += newdist;
		const float clickrate = 100;
		if (distance > clickrate)
		{
			distance -= clickrate;
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
	break;
	case WM_LBUTTONDOWN:
	{

	}
	break;
	case WM_LBUTTONUP:
	{

	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}