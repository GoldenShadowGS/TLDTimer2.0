#include "PCH.h"
#include "Direct2D.h"

class Application;

class TimerWindow
{
public:
	TimerWindow(HINSTANCE hInstance, Application* app);
	BOOL Create(HWND parentWindow, int x, int y, int width, int height);
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();
	void Paint();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND GetwindowHandle() { return hWindow; }
	void KillWindow();
	BOOL GetWindowAlive() { return windowAlive; }
private:
	ATOM RegisterWindowClass(HINSTANCE hInstance);
	LRESULT CALLBACK ClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	const WCHAR* m_Title = L"Timer";
	const WCHAR* m_WindowClass = L"TimerWindowsClass";
	HINSTANCE hInst;
	HWND hWindow = nullptr;
	Application* m_App;
	BOOL windowAlive = FALSE;
	Direct2DDevice m_Direct2DDevice;
	int HoverElement = -1;
	int GrabbedElement = -1;
	ID2D1SolidColorBrush* pBlackBrush = nullptr;
};