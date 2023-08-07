#pragma once
#include "PCH.h"
#include "Bitmap.h"
#include "Resource.h"
#include "ResourceLoader.h"
#include "Direct2D.h"
#include "ComException.h"

std::vector<BYTE> FlipImage8bit(std::vector<BYTE>& Image, int pitch, int height)
{
	std::vector<BYTE> FlippedPixels;
	FlippedPixels.resize(Image.size());
	int pixelpitch = pitch / 4;
	for (UINT64 row = 0; row < height; row++)
	{
		BYTE* SrcRow = Image.data() + (row * pixelpitch);
		BYTE* DestRow = FlippedPixels.data() + (((height - 1LL) - row) * pixelpitch);
		memcpy(DestRow, SrcRow, pixelpitch);
	}
	return FlippedPixels;
}

std::vector<BYTE> FlipImage32bit(std::vector<BYTE>& Image, int pitch, int height)
{
	std::vector<BYTE> FlippedPixels;
	FlippedPixels.resize(Image.size());
	for (UINT64 row = 0; row < height; row++)
	{
		BYTE* SrcRow = Image.data() + (row * pitch);
		BYTE* DestRow = FlippedPixels.data() + (((height - 1LL) - row) * pitch);
		memcpy(DestRow, SrcRow, pitch);
	}
	return FlippedPixels;
}

void Bitmap::Create(ID2D1DeviceContext* dc, int resource, COLORREF color, D2D1_POINT_2F pivot, float scale)
{
	UINT PixelWidth;
	UINT PixelHeight;
	UINT Pitch;
	std::vector<BYTE> Pixels = FileLoader(resource, PixelWidth, PixelHeight, Pitch);
	std::vector<BYTE> FlippedPixels = FlipImage8bit(Pixels, Pitch, PixelHeight);
	std::vector<int> Pixels32Bit(FlippedPixels.size());
	for (UINT64 i = 0; i < FlippedPixels.size(); i++)
	{
		const float alpha = (255 - FlippedPixels[i]) / 255.0f;
		Pixels32Bit[i] =
			BYTE(GetRValue(color) * alpha) +
			(BYTE(GetGValue(color) * alpha) << 8) +
			(BYTE(GetBValue(color) * alpha) << 16) +
			(BYTE(alpha * 255.0f) << 24);
	}

	D2D1_SIZE_U bitmapsize = {};
	bitmapsize.width = PixelWidth;
	bitmapsize.height = PixelHeight;
	m_Size = { (float)PixelWidth * scale, (float)PixelHeight * scale };

	// m_Pivot is the Center of the bitmap for positioning and rotation
	m_Pivot = D2D1::Point2F(pivot.x * m_Size.width, pivot.y * m_Size.height);

	// Resource Bitmap
	D2D1_BITMAP_PROPERTIES bitmapprops = {};
	bitmapprops.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapprops.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapprops.dpiX = 96.0f;
	bitmapprops.dpiY = 96.0f;

	// Create a temporary bitmap 
	ComPtr<ID2D1Bitmap> bitmapA;
	HR(dc->CreateBitmap(bitmapsize, Pixels32Bit.data(), Pitch, bitmapprops, bitmapA.ReleaseAndGetAddressOf()));

	// Create Render Target of the scaled bitmap
	ComPtr<ID2D1BitmapRenderTarget> BitmapRenderTarget;
	HR(dc->CreateCompatibleRenderTarget(m_Size, BitmapRenderTarget.ReleaseAndGetAddressOf()));

	// Draw Scaled Bitmap
	BitmapRenderTarget->BeginDraw();

	D2D1_RECT_F BitmapRect = { 0.0f, 0.0f, m_Size.width, m_Size.height };
	BitmapRenderTarget->DrawBitmap(bitmapA.Get(), BitmapRect);
	HR(BitmapRenderTarget->EndDraw());

	//After Drawing, retrieve the scaled bitmap from the render target and store it in the class member ComPtr
	HR(BitmapRenderTarget->GetBitmap(m_Bitmap.ReleaseAndGetAddressOf()));
}

void Bitmap::Create32bit(ID2D1DeviceContext* dc, int resource, D2D1_POINT_2F pivot)
{
	UINT PixelWidth;
	UINT PixelHeight;
	UINT Pitch;
	std::vector<BYTE> Pixels32Bit = FileLoader(resource, PixelWidth, PixelHeight, Pitch);
	std::vector<BYTE> FlippedPixels32Bit = FlipImage32bit(Pixels32Bit, Pitch, PixelHeight);
	D2D1_SIZE_U bitmapsize = {};
	bitmapsize.width = PixelWidth;
	bitmapsize.height = PixelHeight;
	m_Size = { (float)PixelWidth, (float)PixelHeight };

	// m_Pivot is the Center of the bitmap for positioning and rotation
	m_Pivot = D2D1::Point2F(pivot.x * m_Size.width, pivot.y * m_Size.height);

	// Resource Bitmap
	D2D1_BITMAP_PROPERTIES bitmapprops = {};
	bitmapprops.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapprops.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapprops.dpiX = 96.0f;
	bitmapprops.dpiY = 96.0f;

	HR(dc->CreateBitmap(bitmapsize, FlippedPixels32Bit.data(), Pitch, bitmapprops, m_Bitmap.ReleaseAndGetAddressOf()));
}

void Bitmap::Draw(ID2D1DeviceContext* dc, D2D1_POINT_2F center)
{
	dc->SetTransform(D2D1::Matrix3x2F::Translation(center.x - m_Pivot.x, center.y - m_Pivot.y));
	D2D1_RECT_F	rect = D2D1::RectF(0, 0, m_Size.width, m_Size.height);
	dc->DrawBitmap(m_Bitmap.Get(), rect, 1.0f);
}

void Bitmap::Draw(ID2D1DeviceContext* dc, float angle, D2D1_POINT_2F center)
{
	dc->SetTransform(D2D1::Matrix3x2F::Rotation(angle, m_Pivot) * D2D1::Matrix3x2F::Translation(center.x - m_Pivot.x, center.y - m_Pivot.y));
	D2D1_RECT_F	rect = D2D1::RectF(0, 0, m_Size.width, m_Size.height);
	dc->DrawBitmap(m_Bitmap.Get(), rect, 1.0f);
}

void Bitmap::Draw(ID2D1DeviceContext* dc, float angle, D2D1_POINT_2F center, float scale)
{
	const D2D1_POINT_2F ScaledPivot = { m_Pivot.x * scale,m_Pivot.y * scale };
	dc->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale) * D2D1::Matrix3x2F::Rotation(angle, ScaledPivot) * D2D1::Matrix3x2F::Translation(center.x - ScaledPivot.x, center.y - ScaledPivot.y));
	D2D1_RECT_F	rect = D2D1::RectF(0, 0, m_Size.width, m_Size.height);
	dc->DrawBitmap(m_Bitmap.Get(), rect, 1.0f);
}

inline UINT Align(UINT uLocation, UINT uAlign)
{
	return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
}

std::vector<BYTE> Bitmap::FileLoader(_In_ int resource, _Out_ UINT& width, _Out_ UINT& height, _Out_ UINT& pitch)
{
	std::vector<BYTE> Pixels8Bit;
	struct BMPHeader
	{
		char bm[2];
		unsigned int bmpfilesize;
		short appspecific1;
		short appspecific2;
		unsigned int pixeldataoffset;
	} bmpHeader = {};
	struct DIBHeader
	{
		unsigned int dibSize;
		unsigned int width;
		unsigned int height;
		short colorplane;
		short bitsperpixel;
		unsigned int pixelcompression;
		unsigned int rawPixelDataSize;
	} dibHeader = {};

	ResourceLoader resLoader(resource, L"BITMAPDATA");
	resLoader.Read(bmpHeader.bm, 2);
	if (!(bmpHeader.bm[0] == 'B' && bmpHeader.bm[1] == 'M'))
		throw std::exception("Bad Bitmap");
	resLoader.Read(&bmpHeader.bmpfilesize, sizeof(bmpHeader.bmpfilesize));
	resLoader.Read(&bmpHeader.appspecific1, sizeof(bmpHeader.appspecific1));
	resLoader.Read(&bmpHeader.appspecific2, sizeof(bmpHeader.appspecific2));
	resLoader.Read(&bmpHeader.pixeldataoffset, sizeof(bmpHeader.pixeldataoffset));
	resLoader.Read(&dibHeader.dibSize, sizeof(dibHeader.dibSize));
	resLoader.Read(&dibHeader.width, sizeof(dibHeader.width));
	resLoader.Read(&dibHeader.height, sizeof(dibHeader.height));
	resLoader.Read(&dibHeader.colorplane, sizeof(dibHeader.colorplane));
	resLoader.Read(&dibHeader.bitsperpixel, sizeof(dibHeader.bitsperpixel));
	resLoader.Read(&dibHeader.pixelcompression, sizeof(dibHeader.pixelcompression));
	resLoader.Read(&dibHeader.rawPixelDataSize, sizeof(dibHeader.rawPixelDataSize));

	width = dibHeader.width;
	height = dibHeader.height;
	pitch = Align(dibHeader.width * 4, 16);

	const size_t buffersize = dibHeader.rawPixelDataSize;
	Pixels8Bit.resize(buffersize);
	resLoader.Seek(bmpHeader.pixeldataoffset);
	resLoader.Read(Pixels8Bit.data(), buffersize);

	return Pixels8Bit;
}