#pragma once
#include "PCH.h"

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class Direct2DDevice
{
public:
	Direct2DDevice();
	~Direct2DDevice();
	static void InitializeFactory();
	HRESULT CreateRenderTargetHwnd(HWND hwnd);
	void ReleaseGraphicsResources();
	static ID2D1Factory* getD2DFactory() { return pD2DFactory; }
	ID2D1HwndRenderTarget* getD2DRenderTarget() { return pRenderTarget; }
private:
	static inline ID2D1Factory* pD2DFactory = nullptr;
	ID2D1HwndRenderTarget* pRenderTarget = nullptr;
};