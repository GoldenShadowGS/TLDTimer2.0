#include "PCH.h"
#include "Direct2D.h"
#include "ComException.h"


//Direct2DDevice::Direct2DDevice()
//{
//	InitializeFactory();
//}
//
//Direct2DDevice::~Direct2DDevice()
//{
//	ReleaseGraphicsResources();
//	SafeRelease(&pD2DFactory);
//}
//
//void Direct2DDevice::InitializeFactory()
//{
//	if (pD2DFactory == nullptr)
//	{
//		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
//		if (FAILED(hr))
//			throw std::exception("Direct2D Initialization Failed");
//	}
//}

//HRESULT Direct2DDevice::CreateRenderTargetHwnd(HWND hwnd)
//{
//	HRESULT hr = S_OK;
//	if (pRenderTarget == nullptr)
//	{
//		RECT rc;
//		GetClientRect(hwnd, &rc);
//
//		// Create a Direct2D render target          
//		D2D1_RENDER_TARGET_PROPERTIES rendertargetproperties = D2D1::RenderTargetProperties();
//		D2D1_PRESENT_OPTIONS options = D2D1_PRESENT_OPTIONS_NONE;
//		D2D1_HWND_RENDER_TARGET_PROPERTIES hwnRTP = D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top), options);
//
//		hr = pD2DFactory->CreateHwndRenderTarget(rendertargetproperties, hwnRTP, &pRenderTarget);
//	}
//	return hr;
//}

//void Direct2DDevice::ReleaseGraphicsResources()
//{
//	SafeRelease(&pRenderTarget);
//}

void Renderer::Init(HWND hWnd)
{
	HR(D3D11CreateDevice(nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&direct3dDevice,
		nullptr,    // Actual feature level
		nullptr));  // Device context

	HR(direct3dDevice.As(&dxgiDevice));
#ifdef _DEBUG
	UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT flags = 0;
#endif
	HR(CreateDXGIFactory2(flags, __uuidof(dxFactory), reinterpret_cast<void**>(dxFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 description = {};
	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

	RECT rect = {};
	GetClientRect(hWnd, &rect);
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	HR(dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(), &description, nullptr, swapChain.GetAddressOf()));

	// Create a single-threaded Direct2D factory with debugging information
#ifdef _DEBUG
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
#else
	D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_NONE };
#endif
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, d2Factory.GetAddressOf()));

	// Create the Direct2D device that links back to the Direct3D device
	HR(d2Factory->CreateDevice(dxgiDevice.Get(), d2Device.GetAddressOf()));

	// Create the Direct2D device context that is the actual render target
	// and exposes drawing commands
	HR(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, dc.GetAddressOf()));

	// Retrieve the swap chain's back buffer
	HR(swapChain->GetBuffer(0, __uuidof(surface), reinterpret_cast<void**>(surface.GetAddressOf())));

	// Create a Direct2D bitmap that points to the swap chain surface
	D2D1_BITMAP_PROPERTIES1 properties = {};
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

	HR(dc->CreateBitmapFromDxgiSurface(surface.Get(), properties, bitmap.GetAddressOf()));

	// Point the device context to the bitmap for rendering
	dc->SetTarget(bitmap.Get());

	HR(DCompositionCreateDevice(dxgiDevice.Get(), __uuidof(dcompDevice), reinterpret_cast<void**>(dcompDevice.GetAddressOf())));

	HR(dcompDevice->CreateTargetForHwnd(hWnd, true, target.GetAddressOf()));

	HR(dcompDevice->CreateVisual(visual.GetAddressOf()));
	HR(visual->SetContent(swapChain.Get()));
	HR(target->SetRoot(visual.Get()));
	HR(dcompDevice->Commit());
}

ComPtr<ID2D1SolidColorBrush> Renderer::CreateBrush(const D2D1_COLOR_F& color)
{
	ComPtr<ID2D1SolidColorBrush> brush;
	D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f, 0.55f, 0.34f, 0.75f);
	HR(dc->CreateSolidColorBrush(brushColor, brush.ReleaseAndGetAddressOf()));
	return brush;
}
