#include <Windows.h>
#include <cstring>
#include <fileapi.h>
#include <iostream>
#include <minwindef.h>
#include <winnt.h>
#include <winuser.h>
#include "utils.h"

std::string default_file_name = "data.dat";
char character = '*';
int gap = 20;
int timer_id = 1;
int timer_interval = 500;

std::string file_name;

#pragma comment(lib, "gdi32")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            file_name = default_file_name;

            LPCTSTR Message = ("File " + file_name + " does not exist!").c_str();
            LPCTSTR Title = "Error";
            
            if(!file_exists(file_name))
            {
                MessageBox(hwnd, Message, Title, MB_OK);
                //PostQuitMessage(1);
            }

            return 0; 
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HANDLE h_file = CreateFile(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, NULL);

            int* numbers;
            int length;

            file_to_numbers(h_file, numbers, length);
           
            //int numbers[] = {2 , 4, 1, 3, 5};
            //int length = 5;

            HDC hdc = BeginPaint(hwnd, &ps);

            for (int i = 0; i < length; i++)
            {
                TextOut(hdc, 0, i * gap, get_characters(character, numbers[i]).c_str(), numbers[i]);
            }
            
            SetTimer(hwnd, timer_id, timer_interval, NULL);
            EndPaint(hwnd, &ps);

            return 0;
        }

        case WM_TIMER:
        {
            if (wParam == timer_id)
            {
                InvalidateRect(hwnd, NULL, TRUE);
            }

            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SomeWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
    {
        return 1;
    }

    HWND hwnd = CreateWindowEx(
            0,
            wc.lpszClassName,
            "Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            500,
            300,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    if (hwnd == NULL)
    {
        std::cerr << "Error creating the window!" << std::endl;
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {0};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
