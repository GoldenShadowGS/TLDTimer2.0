#pragma once
#include "PCH.h"

//template <class T> void SafeRelease(T** ppT)
//{
//	if (*ppT)
//	{
//		(*ppT)->Release();
//		*ppT = NULL;
//	}
//}
//
//class Direct2DDevice
//{
//public:
//	Direct2DDevice();
//	~Direct2DDevice();
//	static void InitializeFactory();
//	HRESULT CreateRenderTargetHwnd(HWND hwnd);
//	void ReleaseGraphicsResources();
//	static ID2D1Factory* getD2DFactory() { return pD2DFactory; }
//	ID2D1HwndRenderTarget* getD2DRenderTarget() { return pRenderTarget; }
//private:
//	static inline ID2D1Factory* pD2DFactory = nullptr;
//	ID2D1HwndRenderTarget* pRenderTarget = nullptr;
//};

class Renderer
{
public:
	void Init(HWND hWnd);
	ComPtr<ID2D1SolidColorBrush> CreateBrush(const D2D1_COLOR_F& color);
	ID2D1DeviceContext* GetDeviceContext() { return dc.Get(); }
	IDXGISwapChain1* GetSwapChain() { return swapChain.Get(); }
	ID2D1Factory2* GetFactory() { return d2Factory.Get(); }
private:
	ComPtr<ID3D11Device> direct3dDevice;
	ComPtr<IDXGIDevice> dxgiDevice;
	ComPtr<IDXGIFactory2> dxFactory;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID2D1Factory2> d2Factory;
	ComPtr<ID2D1Device1> d2Device;
	ComPtr<ID2D1DeviceContext> dc;
	ComPtr<IDXGISurface2> surface;
	ComPtr<ID2D1Bitmap1> bitmap;
	ComPtr<IDCompositionDevice> dcompDevice;
	ComPtr<IDCompositionTarget> target;
	ComPtr<IDCompositionVisual> visual;
};