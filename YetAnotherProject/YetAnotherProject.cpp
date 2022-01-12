#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>

#include "Application.hpp"
#include "GraphicContext.hpp"
#include "helper.hpp"


int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	try
	{
		Application app(L"best app ever!", 800, 600);

		app.initialize();
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