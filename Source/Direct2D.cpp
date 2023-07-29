#include "PCH.h"
#include "Direct2D.h"


Direct2DDevice::Direct2DDevice()
{
	InitializeFactory();
}

Direct2DDevice::~Direct2DDevice()
{
	ReleaseGraphicsResources();
	SafeRelease(&pD2DFactory);
}

void Direct2DDevice::InitializeFactory()
{
	if (pD2DFactory == nullptr)
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		if (FAILED(hr))
			throw std::exception("Direct2D Initialization Failed");
	}
}

HRESULT Direct2DDevice::CreateRenderTargetHwnd(HWND hwnd)
{
	HRESULT hr = S_OK;
	if (pRenderTarget == nullptr)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		// Create a Direct2D render target          
		D2D1_RENDER_TARGET_PROPERTIES rendertargetproperties = D2D1::RenderTargetProperties();
		D2D1_PRESENT_OPTIONS options = D2D1_PRESENT_OPTIONS_NONE;
		D2D1_HWND_RENDER_TARGET_PROPERTIES hwnRTP = D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top), options);

		hr = pD2DFactory->CreateHwndRenderTarget(rendertargetproperties, hwnRTP, &pRenderTarget);
	}
	return hr;
}

void Direct2DDevice::ReleaseGraphicsResources()
{
	SafeRelease(&pRenderTarget);
}
