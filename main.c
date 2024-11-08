//
// Created by akhremchik_na on 06.11.2024.
//

#include <windows.h>
#include <process.h>
#include <locale.h>
#include "serial.h"
#include "ui.h"

HANDLE hSerial = INVALID_HANDLE_VALUE;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ComPortWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClass(&wc)) return FALSE;

    HWND hwnd = CreateWindowEx(0,
                               "ComPortWindowClass",
                               "MA GUI",
                               WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               300,
                               400,
                               NULL,
                               NULL,
                               hInstance,
                               NULL);
    if (!hwnd) return FALSE;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // _beginthreadex(NULL, 0, &monitorPorts, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial);
    return (int)msg.wParam;
}

