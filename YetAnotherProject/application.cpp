#include "application.hpp"

void Application::initialize()
{
	_gc.initialize();
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

Application::Application(const std::string& title, int width, int height, DWORD dwStyle, DWORD dwExStyle)
	: windowHandle(wct.createWindow(*this, dwExStyle, title.c_str(), dwStyle, width, height)), _gc(windowHandle, width, height)
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