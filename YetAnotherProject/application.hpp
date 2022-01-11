#pragma once

#include <string>
#include "WindowClassType.hpp"
#include "GraphicContext.hpp"

struct Application
{
	Application(const std::string& title, int width, int height, DWORD dwStyle = WS_OVERLAPPEDWINDOW, DWORD dwExStyle = 0);
	void initialize();
	void runApplication();
	HWND windowHandle;

private:
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Win32::WindowClassType<Application, &WndProc> wct;

	GraphicContext _gc;
};