#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>

#include "WindowClassType.hpp"

struct Application
{
	Application(const std::string& title, int bufferWidth, int bufferHeight, DWORD dwStyle = WS_OVERLAPPEDWINDOW, DWORD dwExStyle = 0);
	void runApplication();
	HWND windowHandle;

private:
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Win32::WindowClassType<Application, &WndProc> wct;
};

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application app("best app ever!", 800, 600);
		app.runApplication();
	}
	catch (std::exception& e)
	{
		std::cout << "error: " << e.what() << std::endl;
#ifdef _WIN32
		OutputDebugStringA((std::string(e.what()) + '\n').c_str());
#endif
		return -1;
	}
	catch (...)
	{
		std::cout << "unknow exception\n" << std::flush;
#ifdef _WIN32
		OutputDebugStringA("unknow exception\n");
#endif
		return -1;
	}

	return 0;
}

void Application::runApplication() {
	bool bRun = true;
	MSG msg;

	DWORD frameticks = 0;
	float frametime = 0.0f;

	// rc.UpdateProjectionMatrix(ProjeMatrix(180, 800.0f / 600.0f, 1.0f, 100.0f));

	while (bRun)
	{
		frameticks = GetTickCount();
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				bRun = false;
			}
		}

		/*FLOAT color[] = {1.0f, 0.0f, 0.0f, 1.0f};
		ID3D11Buffer* vertexBuffers[] = { sr.buffer };
		UINT stride = sizeof(Vertex), offset = 0;
		gc.context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
		gc.context->IASetInputLayout(defIL);

		ID3D11RenderTargetView* render_target_views[] = { gc.rtv };
		gc.context->OMSetRenderTargets(1, render_target_views, nullptr);

		gc.context->PSSetShader(defPS, nullptr, 0U);
		gc.context->VSSetShader(defVS, nullptr, 0U);
		gc.context->ClearRenderTargetView(gc.rtv, color);

		gc.swapchain->Present(0, 0);*/

		frameticks = (GetTickCount() - frameticks);
		frametime = static_cast<float>(frameticks / 1000.0f);
	}
}

Win32::WindowClassType<Application, &Application::WndProc> Application::wct("windowclassname", 0, 0, 0, 0);

Application::Application(const std::string& title, int bufferWidth, int bufferHeight, DWORD dwStyle, DWORD dwExStyle)
	: windowHandle(wct.createWindow(*this, dwExStyle, title.c_str(), dwStyle, bufferWidth, bufferHeight))
{
	ShowWindow(windowHandle, 1);
	UpdateWindow(windowHandle);
}

LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(windowHandle);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(windowHandle, msg, wParam, lParam);
	}

	return 0;
}