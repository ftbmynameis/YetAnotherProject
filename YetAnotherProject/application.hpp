#pragma once

#include <string>
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