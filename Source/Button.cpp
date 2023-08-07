#include "PCH.h"
#include "Resource.h"
#include "Button.h"
#include "Shape.h"
#include "ComException.h"

void Button::InitShapes(ID2D1Factory2* factory, ID2D1DeviceContext* dc)
{
	const float largesize = 70.0f;
	const float mediumsize = 50.0f;
	const float smallsize = 30.0f;

	D2D1::ColorF shapecolor = D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f);
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_PLAY, shapecolor, largesize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_PAUSE, shapecolor, largesize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_RESET, shapecolor, smallsize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_UP, shapecolor, smallsize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_DOWN, shapecolor, smallsize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_PLUS, shapecolor, mediumsize));
	shapes.emplace_back(CreateShape(factory, dc, SHAPE_MINUS, shapecolor, mediumsize));
	shapes.emplace_back(Load32BitBitmap(dc, BITMAP_BWOOPON));
	shapes.emplace_back(Load32BitBitmap(dc, BITMAP_BWOOPOFF));
	ComPtr<ID2D1SolidColorBrush> Brush;

	// FILL
	D2D1::ColorF color = D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f);
	HR(dc->CreateSolidColorBrush(color, Brush.ReleaseAndGetAddressOf()));
	brushes.push_back(Brush);

	// Outline
	color = D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f);
	HR(dc->CreateSolidColorBrush(color, Brush.ReleaseAndGetAddressOf()));
	brushes.push_back(Brush);

	// Hover
	color = D2D1::ColorF(0.78f, 0.68f, 0.75f, 1.0f);
	HR(dc->CreateSolidColorBrush(color, Brush.ReleaseAndGetAddressOf()));
	brushes.push_back(Brush);

	// Grabbed
	color = D2D1::ColorF(0.78f, 0.57f, 0.71f, 1.0f);
	HR(dc->CreateSolidColorBrush(color, Brush.ReleaseAndGetAddressOf()));
	brushes.push_back(Brush);
}

void Button::Draw(ID2D1DeviceContext* dc, BOOL isTiming, const UIstate& state)
{
	dc->SetTransform(D2D1::Matrix3x2F::Translation(m_Pivot.x, m_Pivot.y));
	BOOL grabbed = (m_ButtonValue == state.GrabbedElementLMB || m_ButtonValue == state.GrabbedElementRMB);
	dc->FillGeometry(Geometry.Get(), (m_ButtonValue == state.HoverElement) ? (grabbed ? brushes[GRABBED].Get() : brushes[HOVER].Get()) : brushes[FILL].Get());
	dc->DrawGeometry(Geometry.Get(), brushes[OUTLINE].Get());

	float scale = (m_ButtonValue == state.HoverElement) ? 1.0f : 0.9f;
	switch (m_ButtonValue)
	{
	case BUTTON_START:
	{
		if (isTiming)
			shapes[PAUSE].DrawScaled(dc, m_Pivot, scale);
		else
			shapes[PLAY].DrawScaled(dc, m_Pivot, scale);
	}
	break;
	case BUTTON_RESET:
	case BUTTON_ZERO1:
	case BUTTON_ZERO2:
	case BUTTON_ZERO3:
	{
		shapes[RESET].DrawScaled(dc, m_Pivot, scale);
	}
	break;
	case BUTTON_ADDTIME1:
	case BUTTON_ADDTIME2:
	case BUTTON_ADDTIME3:
	{
		if (state.GrabbedElementRMB == m_ButtonValue)
			shapes[MINUS].DrawScaled(dc, m_Pivot, scale);
		else
			shapes[PLUS].DrawScaled(dc, m_Pivot, scale);
	}
	break;
	case BUTTON_TIMEOFDAYINC:
	case BUTTON_INCTIME1:
	case BUTTON_INCTIME2:
	case BUTTON_INCTIME3:
	{
		shapes[UP].DrawScaled(dc, m_Pivot, scale);
	}
	break;
	case BUTTON_TIMEOFDAYDEC:
	case BUTTON_DECTIME1:
	case BUTTON_DECTIME2:
	case BUTTON_DECTIME3:
	{
		shapes[DOWN].DrawScaled(dc, m_Pivot, scale);
	}
	break;
	case BUTTON_BWOOP:
	{
		if (state.b_BWOOP)
			shapes[BWOOPON].DrawScaled(dc, m_Pivot, 1.0f);
		else
			shapes[BWOOPOFF].DrawScaled(dc, m_Pivot, 1.0f);
	}
	break;
	}
}

void Button::Create(ID2D1Factory2* factory, ID2D1DeviceContext* dc, int Buttonvalue, int x, int y, int w, int h)
{
	HitTestRect = { x - w / 2, y - h / 2, x + w / 2, y + h / 2 };
	m_ButtonValue = Buttonvalue;
	m_Pivot = { (float)x, (float)y };
	m_Size = { (float)w,(float)h };

	const float halfw = m_Size.width / 2.0f;
	const float halfh = m_Size.height / 2.0f;
	{
		ComPtr<ID2D1GeometrySink> Sink;
		HR(factory->CreatePathGeometry(Geometry.ReleaseAndGetAddressOf()));
		HR(Geometry->Open(Sink.ReleaseAndGetAddressOf()));
		Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		D2D1_POINT_2F p1 = { -halfw, -halfh };
		D2D1_POINT_2F p2 = { halfw, -halfh };
		D2D1_POINT_2F p3 = { halfw, halfh };
		D2D1_POINT_2F p4 = { -halfw, halfh };
		Sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
		Sink->AddLine(p2);
		Sink->AddLine(p3);
		Sink->AddLine(p4);
		Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		HR(Sink->Close());
	}
}

int Button::HitTest(int x, int y)
{
	if (x > HitTestRect.left && x < HitTestRect.right && y > HitTestRect.top && y < HitTestRect.bottom)
		return m_ButtonValue;
	else
		return -1;
}
