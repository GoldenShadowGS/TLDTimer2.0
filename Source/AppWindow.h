#include "PCH.h"
#include "Direct2D.h"
#include "DigitalClock.h"
#include "ClockFace.h"
#include "Math.h"

class Application;
class TimerWindow;
class Timer;

class AppWindow
{
public:
	class Button
	{
	public:
		void Init(ID2D1Factory* pD2DFactory, int Buttonvalue, int x, int y, int w, int h);
		static void InitGeometry(ID2D1Factory* pD2DFactory, float width, float height);
		void Draw(ID2D1HwndRenderTarget* pRenderTarget, BOOL timing, BOOL negative, int hover, int grab);
		int HitTest(int x, int y);
		void DiscardGraphicsResources();
		HRESULT CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget);
		RECT GetRect() { return HitTestRect; }
	private:
		RECT HitTestRect {};
		float centerX = 0.0f;
		float centerY = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		struct Shape
		{
			~Shape();
			ID2D1PathGeometry* Geometry = nullptr;
			ID2D1GeometrySink* Sink = nullptr;
		};
		Shape Border;
		static inline Shape Play;
		static inline Shape Pause;
		static inline Shape Split;
		static inline Shape Reset;
		static inline Shape Increment;
		static inline Shape Decrement;
		static inline Shape Add;
		static inline Shape Sub;
		static inline ID2D1SolidColorBrush* pOutlineBrush = nullptr;
		static inline ID2D1SolidColorBrush* pFillBrush = nullptr;
		static inline ID2D1SolidColorBrush* pHoverBrush = nullptr;
		static inline ID2D1SolidColorBrush* pPressedBrush = nullptr;
		static inline ID2D1SolidColorBrush* pShapeBrush = nullptr;
		int m_ButtonValue = -1;
	};
	AppWindow(HINSTANCE hInstance, Application* app);
	BOOL Create(Timer* timer, int width, int height);
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();
	void Paint();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND GetwindowHandle() { return hWindow; }
	void KillWindow();
	BOOL GetWindowAlive() { return windowAlive; }
	void IncrementTime();
	void DecrementTime();
private:
	ATOM RegisterWindowClass(HINSTANCE hInstance);
	LRESULT CALLBACK ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void SetRepeatTimer();
	void KillRepeatTimer();
	const WCHAR* m_Title = L"TLD StopWatch";
	const WCHAR* m_WindowClass = L"MainWindowsClass";
	HINSTANCE hInst;
	HWND hWindow = nullptr;
	Application* m_App;
	BOOL windowAlive = FALSE;
	Direct2DDevice m_Direct2DDevice;
	DigitalClock m_DigitalClock;
	ClockFace m_ClockFace;
	static const int ButtonCount = 7;
	Button m_Buttons[ButtonCount];
	int HoverElement = -1;
	int GrabbedElement = -1;
	D2D1::Matrix3x2F m_TransformMain{D2D1::Matrix3x2F::Scale(0.15f, 0.15f)* D2D1::Matrix3x2F::Translation(20, 78)};
	D2D1::Matrix3x2F m_TransformSub{D2D1::Matrix3x2F::Scale(0.1f, 0.1f)* D2D1::Matrix3x2F::Translation(46, 28)};
	D2D1::Matrix3x2F m_TransformAddtime{D2D1::Matrix3x2F::Scale(0.1f, 0.1f)* D2D1::Matrix3x2F::Translation(70, 226)};
	INT64 AddTime = 0;
	Timer* m_pTimer;
	BOOL MouseinWindow = FALSE;
	UINT_PTR MouseTimerID {};
};

inline float angleNormalize(float angle)
{
	while (angle < 0.0f)
	{
		angle += PI2;
	}
	while (angle > PI2)
	{
		angle -= PI2;
	}
	return angle;
}

inline float getMouseAngle(int mousex, int mousey, float centerx, float centery, float offset)
{
	float x = (float)mousex - centerx;
	float y = (float)mousey - centery;
	return angleNormalize(atan2(y, x) - offset);
}