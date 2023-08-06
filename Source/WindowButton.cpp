#include "PCH.h"
#include "AppWindow.h"
#include "Application.h"
#include "Resource.h"
#include "Timer.h"
#include "Math.h"
#include "ComException.h"

void AppWindow::Button::Init(ID2D1Factory2* pD2DFactory, int Buttonvalue, int x, int y, int w, int h)
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
		HRESULT hr = pD2DFactory->CreatePathGeometry(Border.Geometry.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return;

		hr = Border.Geometry->Open(Border.Sink.ReleaseAndGetAddressOf());
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

void AppWindow::Button::InitGeometry(ID2D1Factory2* pD2DFactory, float width, float height)
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
		//HRESULT hr = pD2DFactory->CreatePathGeometry(&Split.Geometry);
		//if (FAILED(hr)) return;

		//hr = Split.Geometry->Open(&Split.Sink);
		//if (FAILED(hr)) return;
		//Split.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		//float radiusInner = 11;
		//float radiusOuter = 17;
		//float angle1 = 0.00001f;
		//float angle2 = 0.0f;
		//{
		//	D2D1_POINT_2F p1 = { cos(angle1) * radiusInner, sin(angle1) * radiusInner };
		//	D2D1_POINT_2F p2 = { cos(angle2) * radiusInner, sin(angle2) * radiusInner };
		//	D2D1_POINT_2F p3 = { cos(angle2) * radiusOuter, sin(angle2) * radiusOuter };
		//	D2D1_POINT_2F p4 = { cos(angle1) * radiusOuter, sin(angle1) * radiusOuter };

		//	Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		//	Split.Sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(radiusInner, radiusInner), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		//	Split.Sink->AddLine(p3);
		//	Split.Sink->AddArc(D2D1::ArcSegment(p4, D2D1::SizeF(radiusOuter, radiusOuter), 0.0f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
		//	Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//}
		//{
		//	float handwidth = 1.0f;
		//	float handlength = 11.0f;
		//	D2D1_POINT_2F p1 = { -handwidth, 0 };
		//	D2D1_POINT_2F p2 = { -handwidth, -handlength };
		//	D2D1_POINT_2F p3 = { handwidth, -handlength };
		//	D2D1_POINT_2F p4 = { handwidth, 0 };

		//	Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		//	Split.Sink->AddLine(p2);
		//	Split.Sink->AddLine(p3);
		//	Split.Sink->AddLine(p4);
		//	Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//}
		//{
		//	float handwidth = 1.0f;
		//	float handlength = 8.0f;
		//	D2D1_POINT_2F p1 = { 0, -handwidth };
		//	D2D1_POINT_2F p2 = { handlength, -handwidth };
		//	D2D1_POINT_2F p3 = { handlength, handwidth };
		//	D2D1_POINT_2F p4 = { 0, handwidth };

		//	Split.Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		//	Split.Sink->AddLine(p2);
		//	Split.Sink->AddLine(p3);
		//	Split.Sink->AddLine(p4);
		//	Split.Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//}
		//hr = Split.Sink->Close();
		//if (FAILED(hr)) return;
	}
	// Reset Button
	{
		HRESULT hr = pD2DFactory->CreatePathGeometry(&Reset.Geometry);
		if (FAILED(hr)) return;

		hr = Reset.Geometry->Open(&Reset.Sink);
		if (FAILED(hr)) return;
		Reset.Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		float radiusOuter = 9.0f;
		float radiusInner = radiusOuter * (2.0f/3.0f);
		float midradius = (radiusOuter - radiusInner) * 0.5f + radiusInner;
		float arrowwidth = radiusOuter * (4.0f / 9.0f);
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

void AppWindow::Button::Draw(ID2D1DeviceContext* pRenderTarget, BOOL timing, BOOL negative, int hover, int grab)
{
	float scale = (m_ButtonValue == hover) ? 1.2f : 1.0f;
	D2D1::Matrix3x2F scalematrix = D2D1::Matrix3x2F::Scale(scale, scale);
	D2D1::Matrix3x2F translationmatrix = D2D1::Matrix3x2F::Translation(centerX, centerY);
	pRenderTarget->SetTransform(translationmatrix);
	pRenderTarget->FillGeometry(Border.Geometry.Get(), (m_ButtonValue == grab) ? pPressedBrush.Get() : ((m_ButtonValue == hover) ? pHoverBrush.Get() : pFillBrush.Get()));
	pRenderTarget->DrawGeometry(Border.Geometry.Get(), pOutlineBrush.Get());
	pRenderTarget->SetTransform(scalematrix * translationmatrix);
	switch (m_ButtonValue)
	{
	case BUTTON_START:
		pRenderTarget->FillGeometry(timing ? Pause.Geometry.Get() : Play.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_RESET:
		pRenderTarget->FillGeometry(Reset.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_ADDTIME:
		pRenderTarget->FillGeometry(negative ? Sub.Geometry.Get() : Add.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_INCTIME:
		pRenderTarget->FillGeometry(Increment.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_DECTIME:
		pRenderTarget->FillGeometry(Decrement.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_ZERO:
		pRenderTarget->FillGeometry(Reset.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_TIMEOFDAYINC:
		pRenderTarget->FillGeometry(Increment.Geometry.Get(), pShapeBrush.Get());
		break;
	case BUTTON_TIMEOFDAYDEC:
		pRenderTarget->FillGeometry(Decrement.Geometry.Get(), pShapeBrush.Get());
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

void AppWindow::Button::CreateButtonGraphicsResources(ID2D1DeviceContext* pRenderTarget)
{
	D2D1::ColorF color = D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pOutlineBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pFillBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pHoverBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.7f, 0.7f, 0.7f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pPressedBrush.ReleaseAndGetAddressOf()));

	color = D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f);
	HR(pRenderTarget->CreateSolidColorBrush(color, pShapeBrush.ReleaseAndGetAddressOf()));
}