#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>

#include "application.hpp"


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