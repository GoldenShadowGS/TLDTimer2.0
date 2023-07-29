#include "PCH.h"
#include "AppWindow.h"
#include "Application.h"
#include "Timerwindow.h"
#include "Resource.h"
#include "Timer.h"
#include "Math.h"

AppWindow::Button::Shape::~Shape()
{
	SafeRelease(&Geometry);
	SafeRelease(&Sink);
}

void AppWindow::Button::Init(ID2D1Factory* pD2DFactory, int Buttonvalue, int x, int y, int w, int h)
{
	HitTestRect = { x - w / 2, y - h / 2, x + w / 2, y + h / 2 };
	m_ButtonValue = Buttonvalue;
	centerX = (float)x;
	centerY = (float)y;
	width = (float)w;
	height = (float)h;

	// Border
	const float halfw = width / 2.0f;
	const float halfh = height / 2.0f;
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Border.Geometry);
		if (FAILED(hr)) return;

		hr = Border.Geometry->Open(&Border.Sink);
		if (FAILED(hr)) return;
		Border.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		D2D1_POINT_2F p1 = { -halfw, -halfh };
		D2D1_POINT_2F p2 = { halfw, -halfh };
		D2D1_POINT_2F p3 = { halfw, halfh };
		D2D1_POINT_2F p4 = { -halfw, halfh };
		Border.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Border.Sink->AddLine(p2);
		Border.Sink->AddLine(p3);
		Border.Sink->AddLine(p4);
		Border.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = Border.Sink->Close();
		if (FAILED(hr)) return;
	}
}

void AppWindow::Button::InitGeometry(ID2D1Factory* pD2DFactory, float width, float height)
{
	const float halfw = width / 2.0f;
	const float halfh = height / 2.0f;
	// Shape Triangle
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Play.Geometry);
		if (FAILED(hr)) return;

		hr = Play.Geometry->Open(&Play.Sink);
		if (FAILED(hr)) return;

		Play.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float radius = halfh * 0.75f;
		float angle = 0.0f;
		D2D1_POINT_2F p1 = { cos(angle) * radius, sin(angle) * radius };
		angle = ThirdPI;
		D2D1_POINT_2F p2 = { cos(angle) * radius, sin(angle) * radius };
		angle = ThirdPI * 2.0f;
		D2D1_POINT_2F p3 = { cos(angle) * radius, sin(angle) * radius };
		Play.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Play.Sink->AddLine(p2);
		Play.Sink->AddLine(p3);
		Play.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = Play.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Pause Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Pause.Geometry);
		if (FAILED(hr)) return;

		hr = Pause.Geometry->Open(&Pause.Sink);
		if (FAILED(hr)) return;
		Pause.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float radius = height * 0.3f;
		float width = radius * 0.26f;
		float height = radius * 1.0f;
		float spacing = radius * 0.5f;
		D2D1_POINT_2F p1 = { -width - spacing, -height };
		D2D1_POINT_2F p2 = { width - spacing, -height };
		D2D1_POINT_2F p3 = { width - spacing, height };
		D2D1_POINT_2F p4 = { -width - spacing, height };
		Pause.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Pause.Sink->AddLine(p2);
		Pause.Sink->AddLine(p3);
		Pause.Sink->AddLine(p4);
		Pause.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		p1 = { -width + spacing, -height };
		p2 = { width + spacing, -height };
		p3 = { width + spacing, height };
		p4 = { -width + spacing, height };
		Pause.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Pause.Sink->AddLine(p2);
		Pause.Sink->AddLine(p3);
		Pause.Sink->AddLine(p4);
		Pause.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = Pause.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Split Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Split.Geometry);
		if (FAILED(hr)) return;

		hr = Split.Geometry->Open(&Split.Sink);
		if (FAILED(hr)) return;
		Split.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float radiusInner = 11;
		float radiusOuter = 17;
		float angle1 = 0.00001f;
		float angle2 = 0.0f;
		{
			D2D1_POINT_2F p1 = { cos(angle1) * radiusInner, sin(angle1) * radiusInner };
			D2D1_POINT_2F p2 = { cos(angle2) * radiusInner, sin(angle2) * radiusInner };
			D2D1_POINT_2F p3 = { cos(angle2) * radiusOuter, sin(angle2) * radiusOuter };
			D2D1_POINT_2F p4 = { cos(angle1) * radiusOuter, sin(angle1) * radiusOuter };

			Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
			Split.Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(radiusInner, radiusInner), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
			Split.Sink->AddLine(p3);
			Split.Sink->AddArc(D2D1::ArcSegment(p4, D2D1::SizeF(radiusOuter, radiusOuter), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
			Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
		{
			float handwidth = 1.0f;
			float handlength = 11.0f;
			D2D1_POINT_2F p1 = { -handwidth, 0 };
			D2D1_POINT_2F p2 = { -handwidth, -handlength };
			D2D1_POINT_2F p3 = { handwidth, -handlength };
			D2D1_POINT_2F p4 = { handwidth, 0 };

			Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
			Split.Sink->AddLine(p2);
			Split.Sink->AddLine(p3);
			Split.Sink->AddLine(p4);
			Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
		{
			float handwidth = 1.0f;
			float handlength = 8.0f;
			D2D1_POINT_2F p1 = { 0, -handwidth };
			D2D1_POINT_2F p2 = { handlength, -handwidth };
			D2D1_POINT_2F p3 = { handlength, handwidth };
			D2D1_POINT_2F p4 = { 0, handwidth };

			Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
			Split.Sink->AddLine(p2);
			Split.Sink->AddLine(p3);
			Split.Sink->AddLine(p4);
			Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
		hr = Split.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Reset Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Reset.Geometry);
		if (FAILED(hr)) return;

		hr = Reset.Geometry->Open(&Reset.Sink);
		if (FAILED(hr)) return;
		Reset.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float radiusInner = 6;
		float radiusOuter = 9;
		float midradius = (radiusOuter - radiusInner) * 0.5f + radiusInner;
		float arrowwidth = 4.0f;
		float anglearrow = 0.5f;
		float angle1 = -2.35f;
		float angle2 = 3.2f;
		D2D1_POINT_2F p1 = { cos(angle1) * radiusInner, sin(angle1) * radiusInner };
		D2D1_POINT_2F p2 = { cos(angle2) * radiusInner, sin(angle2) * radiusInner };
		D2D1_POINT_2F p3 = { cos(angle2) * radiusOuter, sin(angle2) * radiusOuter };
		D2D1_POINT_2F p4 = { cos(angle1) * radiusOuter, sin(angle1) * radiusOuter };
		D2D1_POINT_2F p5 = { cos(angle1) * (midradius - arrowwidth), sin(angle1) * (midradius - arrowwidth) };
		D2D1_POINT_2F p6 = { cos(angle1 - anglearrow) * midradius, sin(angle1 - anglearrow) * midradius };
		D2D1_POINT_2F p7 = { cos(angle1) * (midradius + arrowwidth), sin(angle1) * (midradius + arrowwidth) };
		Reset.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Reset.Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(radiusInner, radiusInner), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Reset.Sink->AddLine(p3);
		Reset.Sink->AddArc(D2D1::ArcSegment(p4, D2D1::SizeF(radiusOuter, radiusOuter), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		Reset.Sink->AddLine(p5);
		Reset.Sink->AddLine(p6);
		Reset.Sink->AddLine(p7);
		Reset.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = Reset.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Increment Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Increment.Geometry);
		if (FAILED(hr)) return;

		hr = Increment.Geometry->Open(&Increment.Sink);
		if (FAILED(hr)) return;
		Increment.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float width = 10.0f;
		float height = 8.0f;
		D2D1_POINT_2F p1 = { -width, height };
		D2D1_POINT_2F p2 = { 0.0f, -height };
		D2D1_POINT_2F p3 = { width, height };
		Increment.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Increment.Sink->AddLine(p2);
		Increment.Sink->AddLine(p3);
		Increment.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = Increment.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Decrement Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Decrement.Geometry);
		if (FAILED(hr)) return;

		hr = Decrement.Geometry->Open(&Decrement.Sink);
		if (FAILED(hr)) return;
		Decrement.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float width = 10.0f;
		float height = 8.0f;
		D2D1_POINT_2F p1 = { -width, -height };
		D2D1_POINT_2F p2 = { 0.0f, height };
		D2D1_POINT_2F p3 = { width, -height };
		Decrement.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Decrement.Sink->AddLine(p2);
		Decrement.Sink->AddLine(p3);
		Decrement.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = Decrement.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Add Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Add.Geometry);
		if (FAILED(hr)) return;

		hr = Add.Geometry->Open(&Add.Sink);
		if (FAILED(hr)) return;
		Add.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float lgLength = 15.0f;
		float smLength = 4.0f;
		D2D1_POINT_2F p1 = { -lgLength, -smLength };
		D2D1_POINT_2F p2 = { -smLength, -smLength };
		D2D1_POINT_2F p3 = { -smLength, -lgLength };
		D2D1_POINT_2F p4 = { smLength, -lgLength };

		D2D1_POINT_2F p5 = { smLength, -smLength };
		D2D1_POINT_2F p6 = { lgLength, -smLength };
		D2D1_POINT_2F p7 = { lgLength, smLength };
		D2D1_POINT_2F p8 = { smLength, smLength };

		D2D1_POINT_2F p9 = { smLength, lgLength };
		D2D1_POINT_2F p10 = { -smLength, lgLength };
		D2D1_POINT_2F p11 = { -smLength, smLength };
		D2D1_POINT_2F p12 = { -lgLength, smLength };

		Add.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Add.Sink->AddLine(p2);
		Add.Sink->AddLine(p3);
		Add.Sink->AddLine(p4);
		Add.Sink->AddLine(p5);
		Add.Sink->AddLine(p6);
		Add.Sink->AddLine(p7);
		Add.Sink->AddLine(p8);
		Add.Sink->AddLine(p9);
		Add.Sink->AddLine(p10);
		Add.Sink->AddLine(p11);
		Add.Sink->AddLine(p12);
		Add.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = Add.Sink->Close();
		if (FAILED(hr)) return;
	}
	// Minus Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Sub.Geometry);
		if (FAILED(hr)) return;

		hr = Sub.Geometry->Open(&Sub.Sink);
		if (FAILED(hr)) return;
		Sub.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float lgLength = 15.0f;
		float smLength = 4.0f;
		D2D1_POINT_2F p1 = { -lgLength, -smLength };
		D2D1_POINT_2F p2 = { lgLength, -smLength };
		D2D1_POINT_2F p3 = { lgLength, smLength };
		D2D1_POINT_2F p4 = { -lgLength, smLength };

		Sub.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sub.Sink->AddLine(p2);
		Sub.Sink->AddLine(p3);
		Sub.Sink->AddLine(p4);
		Sub.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = Sub.Sink->Close();
		if (FAILED(hr)) return;
	}
}

void AppWindow::Button::Draw(ID2D1HwndRenderTarget* pRenderTarget, BOOL timing, BOOL negative, int hover, int grab)
{
	float scale = (m_ButtonValue == hover) ? 1.2f : 1.0f;
	D2D1::Matrix3x2F scalematrix = D2D1::Matrix3x2F::Scale(scale, scale);
	D2D1::Matrix3x2F translationmatrix = D2D1::Matrix3x2F::Translation(centerX, centerY);
	pRenderTarget->SetTransform(translationmatrix);
	pRenderTarget->FillGeometry(Border.Geometry, (m_ButtonValue == grab) ? pPressedBrush : ((m_ButtonValue == hover) ? pHoverBrush : pFillBrush));
	pRenderTarget->DrawGeometry(Border.Geometry, pOutlineBrush);
	pRenderTarget->SetTransform(scalematrix * translationmatrix);
	switch (m_ButtonValue)
	{
	case BUTTON_START:
		pRenderTarget->FillGeometry(timing ? Pause.Geometry : Play.Geometry, pShapeBrush);
		break;
	case BUTTON_SPLIT:
		pRenderTarget->FillGeometry(Split.Geometry, pShapeBrush);
		break;
	case BUTTON_RESET:
		pRenderTarget->FillGeometry(Reset.Geometry, pShapeBrush);
		break;
	case BUTTON_ADDTIME:
		pRenderTarget->FillGeometry(negative ? Sub.Geometry : Add.Geometry, pShapeBrush);
		break;
	case BUTTON_INCTIME:
		pRenderTarget->FillGeometry(Increment.Geometry, pShapeBrush);
		break;
	case BUTTON_DECTIME:
		pRenderTarget->FillGeometry(Decrement.Geometry, pShapeBrush);
		break;
	case BUTTON_ZERO:
		pRenderTarget->FillGeometry(Reset.Geometry, pShapeBrush);
		break;
	}
}

int AppWindow::Button::HitTest(int x, int y)
{
	if (x > HitTestRect.left && x < HitTestRect.right && y > HitTestRect.top && y < HitTestRect.bottom)
		return m_ButtonValue;
	else
		return -1;
}

void AppWindow::Button::DiscardGraphicsResources()
{
	SafeRelease(&pOutlineBrush);
	SafeRelease(&pFillBrush);
	SafeRelease(&pHoverBrush);
	SafeRelease(&pPressedBrush);
	SafeRelease(&pShapeBrush);
}

HRESULT AppWindow::Button::CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget)
{
	HRESULT hr = S_OK;
	D2D1::ColorF color = D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f);
	if (pOutlineBrush == nullptr)
		hr = pRenderTarget->CreateSolidColorBrush(color, &pOutlineBrush);
	if (FAILED(hr))
		return hr;

	color = D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f);
	if (pFillBrush == nullptr)
	hr = pRenderTarget->CreateSolidColorBrush(color, &pFillBrush);
	if (FAILED(hr))
		return hr;

	color = D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f);
	if (pHoverBrush == nullptr)
	hr = pRenderTarget->CreateSolidColorBrush(color, &pHoverBrush);
	if (FAILED(hr))
		return hr;

	color = D2D1::ColorF(0.7f, 0.7f, 0.7f, 1.0f);
	if (pPressedBrush == nullptr)
	hr = pRenderTarget->CreateSolidColorBrush(color, &pPressedBrush);
	if (FAILED(hr))
		return hr;

	color = D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f);
	if (pShapeBrush == nullptr)
	hr = pRenderTarget->CreateSolidColorBrush(color, &pShapeBrush);
	if (FAILED(hr))
		return hr;

	return hr;
}


AppWindow::AppWindow(HINSTANCE hInstance, Application* app) : hInst(hInstance), m_App(app)
{
	RegisterWindowClass(hInstance);
	m_Direct2DDevice.InitializeFactory();
	m_DigitalClock.Init(m_Direct2DDevice.getD2DFactory());

	const int startbuttonwidth = 100;
	const int buttonwidth = 50;
	const int buttonheight = 50;
	const int smwidth = 22;
	const int smheight = 22;


	int playX = 75;
	int row1 = 135;
	int row2 = 185;
	int splitX = 150;

	int resetx = 24;
	int resety = 28;

	int zerox = 44;
	int zeroy = 226;

	int addx = 50;
	int incx = 100;
	int decx = 150;

	AppWindow::Button::InitGeometry(m_Direct2DDevice.getD2DFactory(), buttonwidth, buttonheight);
	m_Buttons[0].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_START, playX, row1, startbuttonwidth, buttonheight);
	m_Buttons[1].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_SPLIT, splitX, row1, buttonwidth, buttonheight);

	m_Buttons[2].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_RESET, resetx, resety, smwidth, smheight);

	m_Buttons[3].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_ADDTIME, addx, row2, buttonwidth, buttonheight);
	m_Buttons[4].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_INCTIME, incx, row2, buttonwidth, buttonheight);
	m_Buttons[5].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_DECTIME, decx, row2, buttonwidth, buttonheight);
	m_Buttons[6].Init(m_Direct2DDevice.getD2DFactory(), BUTTON_ZERO, zerox, zeroy, smwidth, smheight);
}

BOOL AppWindow::Create(Timer* timer, int width, int height)
{
	m_pTimer = timer;
	const DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX; // WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX     WS_POPUP
	const DWORD exstyle = 0;

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
		nullptr);

	if (!hWindow)
		return FALSE;

	SetWindowLongPtrW(hWindow, GWLP_USERDATA, (INT64)this);
	//SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_COLORKEY);

	windowAlive = TRUE;

	ShowWindow(hWindow, SW_SHOW);
	UpdateWindow(hWindow);

	m_pTimer->SetAppWindow(hWindow);

	return TRUE;
}

HRESULT AppWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;

	hr = m_Direct2DDevice.CreateRenderTargetHwnd(hWindow);
	if (FAILED(hr))
		return hr;
	ID2D1HwndRenderTarget* pRenderTarget = m_Direct2DDevice.getD2DRenderTarget();
	hr = m_DigitalClock.CreateGraphicsResources(pRenderTarget);
	if (FAILED(hr))
		return hr;
	hr = m_ClockFace.CreateGraphicsResources(pRenderTarget);
	if (FAILED(hr))
		return hr;

	for (int i = 0; i < ButtonCount; i++)
	{
		hr = m_Buttons[i].CreateGraphicsResources(pRenderTarget);
		if (FAILED(hr))
			return hr;
	}
	m_ClockFace.CreateGraphicsResources(m_Direct2DDevice.getD2DRenderTarget());

	return hr;
}

void AppWindow::DiscardGraphicsResources()
{
	m_Direct2DDevice.ReleaseGraphicsResources();
	m_DigitalClock.DiscardGraphicsResources();
	m_ClockFace.DiscardGraphicsResources();
	for (int i = 0; i < ButtonCount; i++)
	{
		m_Buttons[i].DiscardGraphicsResources();
	}
}

void AppWindow::KillWindow()
{
	windowAlive = FALSE;
	DestroyWindow(GetwindowHandle());
}

void AppWindow::Paint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		ID2D1HwndRenderTarget* pRenderTarget = m_Direct2DDevice.getD2DRenderTarget();
		PAINTSTRUCT ps;
		BeginPaint(hWindow, &ps);

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));


		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		for (int i = 0; i < ButtonCount; i++)
		{
			m_Buttons[i].Draw(pRenderTarget, m_pTimer->isStarted(), (AddTime < 0), HoverElement, GrabbedElement);
		}

		INT64 ms = m_pTimer->GetMilliseconds();
		INT64 splitms = m_pTimer->GetSplitMilliseconds();

		m_ClockFace.DrawBackGround(pRenderTarget);

		float minangle = getMinuteAngleDeg(ms);
		float hourangle = getHourAngleDeg(ms);
		m_ClockFace.DrawHands(pRenderTarget, getMinuteAngleDeg(ms), getHourAngleDeg(ms));

		m_DigitalClock.Draw(pRenderTarget, m_TransformMain, FALSE, (ms && (HoverElement == BUTTON_RESET || HoverElement == BUTTON_SPLIT)), TRUE, GetDays(ms), GetHours(ms), GetMinutes(ms), GetTenths(ms));
		m_DigitalClock.Draw(pRenderTarget, m_TransformSub, FALSE, (splitms && (HoverElement == BUTTON_RESET)), TRUE, GetDays(splitms), GetHours(splitms), GetMinutes(splitms), GetTenths(splitms));
		INT64 absTime = abs(AddTime);
		m_DigitalClock.Draw(pRenderTarget, m_TransformAddtime, (AddTime < 0), (AddTime && (HoverElement == BUTTON_ZERO)), FALSE, GetDays(absTime), GetHours(absTime), GetMinutes(absTime), GetTenths(absTime));

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			m_Direct2DDevice.ReleaseGraphicsResources();
		}
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

void AppWindow::SetRepeatTimer()
{
	MouseTimerID = SetTimer(hWindow, ID_REPEATTIMER, 50, nullptr);
}

void AppWindow::KillRepeatTimer()
{
	KillTimer(hWindow, MouseTimerID);
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
}

LRESULT CALLBACK AppWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AppWindow* window = reinterpret_cast<AppWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (window)
		return window->ClassWndProc(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK AppWindow::ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int MouseDelay = 0;
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
		MouseinWindow = FALSE;
		HoverElement = -1;
		GrabbedElement = -1;
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
		for (int i = 0; i < ButtonCount; i++)
		{
			int button = m_Buttons[i].HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (button > 0)
			{
				HoverElement = button;
			}
		}
		if (GrabbedElement != HoverElement)
		{
			GrabbedElement = -1;
		}
		if (OldHoverElement != HoverElement)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		MouseDelay = 0;
		SetRepeatTimer();
		GrabbedElement = HoverElement;
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, TRUE);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (HoverElement == GrabbedElement)
		{
			switch (GrabbedElement)
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
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			case BUTTON_ZERO:
				AddTime = 0;
				m_App->m_SoundManager.Play(m_App->TimerClick, 1.0f, 1.0f);
				break;
			}
		}
		GrabbedElement = -1;
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
			if (GrabbedElement == BUTTON_INCTIME)
				IncrementTime();
			else if (GrabbedElement == BUTTON_DECTIME)
				DecrementTime();
			else
				KillRepeatTimer();
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
