#include "PCH.h"
#include "Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);

	try
	{
		Application clockapp;
		clockapp.Init(hInstance, lpCmdLine);

		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (int)msg.wParam;
	}
	catch (...)
	{
		return 0;
	}
	//catch (std::exception& e)
	//{
	//	// Catch Errors and Exit
	//	UNREFERENCED_PARAMETER(e);
	//	return 0;
	//}
}

