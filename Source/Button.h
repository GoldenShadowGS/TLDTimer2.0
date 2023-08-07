#pragma once
#include "PCH.h"

class RasterizedShape;

enum ButtonShapes
{
	PLAY = 0,
	PAUSE = 1,
	RESET = 2,
	UP = 3,
	DOWN = 4,
	PLUS = 5,
	MINUS = 6,
	BWOOPON = 7,
	BWOOPOFF = 8
};

enum ButtonBrushes
{
	FILL = 0,
	OUTLINE = 1,
	HOVER = 2,
	GRABBED = 3,
};

struct UIstate
{
	int HoverElement = -1;
	int GrabbedElementLMB = -1;
	int GrabbedElementRMB = -1;
	BOOL GrabLock = FALSE;
	BOOL b_BWOOP = FALSE;
};

class Button
{
public:
	static void InitShapes(ID2D1Factory2* factory, ID2D1DeviceContext* dc);
	void Create(ID2D1Factory2* factory, ID2D1DeviceContext* dc, int Buttonvalue, int x, int y, int w, int h);
	void Draw(ID2D1DeviceContext* dc, BOOL isTiming, const UIstate& state);
	int HitTest(int x, int y);
private:
	static inline std::vector<RasterizedShape> shapes;
	static inline std::vector<ComPtr<ID2D1SolidColorBrush>> brushes;

	RECT HitTestRect {};
	D2D1_POINT_2F m_Pivot = {};
	D2D1_SIZE_F m_Size = {};
	ComPtr<ID2D1PathGeometry> Geometry;
	int m_ButtonValue = -1;
};