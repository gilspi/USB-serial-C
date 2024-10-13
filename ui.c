#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "serial.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*
Используется для обработки сообщений, поступающих в окно.
Получает: дескриптор (окно, которое принимает сообщение), 
          идентификатор сообщений (создание, уничтожение, перерисовка, обработка команды пользователя (нажатие кнопки)),
          доп. парам клавиатуры (Код нажатой клавиши, ID кнопки), 
          доп. парам. окна (координаты курсора, размер окна) 
*/

// Идентификаторы кнопок
#define ID_BUTTON_SEND_ONCE 1
#define ID_BUTTON_SEND_CONT 2
#define ID_BUTTON_SEND_STOP 3
#define ID_BUTTON_SEND_SPED 4

HANDLE hSerial;
const char *comPort = "\\\\.\\COM5";  // указываем нужный COM-порт


// Функция для обработки кнопок
void handleButtonPress(int buttonID) {
    if (hSerial != INVALID_HANDLE_VALUE) {
        switch (buttonID) {
            case ID_BUTTON_SEND_ONCE:
                writeToComPort(hSerial, "<MAonce>");
                break;
            case ID_BUTTON_SEND_CONT:
                writeToComPort(hSerial, "<MAcont>");
                break;
            case ID_BUTTON_SEND_STOP:
                writeToComPort(hSerial, "<MAstop>");
                break;
            case ID_BUTTON_SEND_SPED:
                writeToComPort(hSerial, "<MAsped>");
                break;
        }
    } else {
        printf("COM порт не открыт!\n");
    }
}

// Основная функция окна
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void) hPrevInstance;
    (void) lpCmdLine;

    // Открываем COM порт
    hSerial = CreateFile(comPort,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        MessageBoxW(NULL, L"Не удалось открыть COM порт", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    configureComPort(hSerial, CBR_115200, 8, ONESTOPBIT, NOPARITY);  // настраиваем COM порт

    _beginthreadex(NULL, 0, &read_from_port, hSerial, 0, NULL);  // запускаем поток для чтения

    // Создаем окно и кнопки
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ComPortWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0,
                              "ComPortWindowClass",
                              "MA GUI",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              220,
                              320, NULL, NULL, hInstance, NULL
    );
    ShowWindow(hwnd, nCmdShow);

    // Запуск цикла обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hSerial);  // закрываем порт после завершения программы
    return 0;
}

// Обработчик сообщений окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CREATE:
            CreateWindow("BUTTON", "<MAonce>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 50, 100, 30, hwnd, (HMENU) ID_BUTTON_SEND_ONCE, NULL, NULL);
            CreateWindow("BUTTON", "<MAcont>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 100, 100, 30, hwnd, (HMENU) ID_BUTTON_SEND_CONT, NULL, NULL);
            CreateWindow("BUTTON", "<MAstop>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 150, 100, 30, hwnd, (HMENU) ID_BUTTON_SEND_STOP, NULL, NULL);
            CreateWindow("BUTTON", "<MAsped>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 200, 100, 30, hwnd, (HMENU) ID_BUTTON_SEND_SPED, NULL, NULL);
            return 0;

        case WM_COMMAND:
            handleButtonPress(LOWORD(wParam));  // Обработка нажатия кнопок
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}