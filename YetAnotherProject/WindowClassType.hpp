#pragma once

// by dot <3

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


namespace Win32
{

    template <class T, LRESULT(T::* MsgProc)(HWND, UINT, WPARAM, LPARAM)>
    class WindowClassType
    {
    private:
        ATOM cls;

        static LRESULT CALLBACK BootstrapWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_CREATE)
            {
                T* obj = reinterpret_cast<T*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(obj));
                SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc));
                return (obj->*MsgProc)(hWnd, msg, wParam, lParam);
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            T* obj = reinterpret_cast<T*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            return (obj->*MsgProc)(hWnd, msg, wParam, lParam);
        }

    public:
        WindowClassType(LPCTSTR lpszClassName,
            UINT style,
            HICON hIcon,
            HICON hIconSm,
            HCURSOR hCursor,
            HBRUSH hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
            LPCTSTR lpszMenuName = 0)
        {
            WNDCLASSEX wnd_cls;
            wnd_cls.cbSize = sizeof(wnd_cls);
            wnd_cls.style = style;
            wnd_cls.lpfnWndProc = &BootstrapWndProc;
            wnd_cls.cbClsExtra = 0;
            wnd_cls.cbWndExtra = 0;
            wnd_cls.hInstance = GetModuleHandle(0);
            wnd_cls.hIcon = hIcon;
            wnd_cls.hCursor = hCursor;
            wnd_cls.hbrBackground = hbrBackground;
            wnd_cls.lpszMenuName = lpszMenuName;
            wnd_cls.lpszClassName = lpszClassName;
            wnd_cls.hIconSm = hIconSm;
            cls = RegisterClassEx(&wnd_cls);
        }

        ~WindowClassType()
        {
            UnregisterClass((LPCTSTR)cls, GetModuleHandle(0));
        }

        HWND createWindow(T& obj,
            DWORD dwExStyle,
            LPCTSTR lpWindowName,
            DWORD dwStyle,
            int nWidth = CW_USEDEFAULT,
            int nHeight = CW_USEDEFAULT,
            int X = CW_USEDEFAULT,
            int Y = CW_USEDEFAULT,
            HWND hWndParent = 0,
            HMENU hMenu = 0)
        {
            return CreateWindowEx(dwExStyle,
                MAKEINTATOM(cls),
                lpWindowName,
                dwStyle,
                X,
                Y,
                nWidth,
                nHeight,
                hWndParent,
                hMenu,
                GetModuleHandle(0),
                static_cast<void*>(&obj));
        }

    };
}