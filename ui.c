#include <process.h>
#include <stdio.h>
#include "ui.h"
#include "serial.h"
#include "utils.h"

/* идентификаторы кнопок */
#define ID_BUTTON_CONNECT 1
#define ID_COMBOBOX 2
#define ID_BUTTON_SEND_ONCE 3
#define ID_BUTTON_SEND_CONT 4
#define ID_BUTTON_SEND_STOP 5
#define ID_BUTTON_SEND_SPED 6


extern HANDLE hSerial;


void addAvailablePorts(HWND hComboBox) {
    for (int i = 1; i <= 256; i++) {
        char portName[10];
        snprintf(portName, sizeof(portName), "COM%d", i);
        HANDLE hPort = CreateFile(portName,
                                  GENERIC_READ | GENERIC_WRITE,
                                  0,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL);
        if (hPort != INVALID_HANDLE_VALUE) {
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)portName);
            CloseHandle(hPort);
        }
    }
}


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
    }
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hComboBox, hConnectButton;

    switch (msg) {
        case WM_CREATE:
            hComboBox = CreateWindow("COMBOBOX",
                                     NULL,
                                     WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                     50, 50, 200, 200, hwnd, (HMENU)ID_COMBOBOX, NULL, NULL);
            addAvailablePorts(hComboBox);

            hConnectButton = CreateWindow("BUTTON",
                                          "CONNECT",
                                          WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                          50, 100, 100, 30, hwnd, (HMENU)ID_BUTTON_CONNECT, NULL, NULL);

            CreateWindow("BUTTON",
                         "<MAonce>",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 150, 100, 30, hwnd, (HMENU)ID_BUTTON_SEND_ONCE, NULL, NULL);
            CreateWindow("BUTTON",
                         "<MAcont>",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 200, 100, 30, hwnd, (HMENU)ID_BUTTON_SEND_CONT, NULL, NULL);
            CreateWindow("BUTTON",
                         "<MAstop>",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 250, 100, 30, hwnd, (HMENU)ID_BUTTON_SEND_STOP, NULL, NULL);
            CreateWindow("BUTTON",
                         "<MAsped>",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         50, 300, 100, 30, hwnd, (HMENU)ID_BUTTON_SEND_SPED, NULL, NULL);
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BUTTON_CONNECT) {
                int itemIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                char portName[10];
                SendMessage(hComboBox, CB_GETLBTEXT, itemIndex, (LPARAM)portName);

                if (hSerial == INVALID_HANDLE_VALUE) {
                    hSerial = openSerialPort(portName);
                    if (hSerial) {
                        _beginthreadex(NULL, 0, &read_from_port, hSerial, 0, NULL);
                        SetWindowText(GetDlgItem(hwnd, ID_BUTTON_CONNECT),
                                      "DISCONNECT");
                    }
                } else {
                    if (hSerial != INVALID_HANDLE_VALUE){
                        CloseHandle(hSerial);

                        MessageBoxW(NULL,
                                   L"Разрыв соединения с COM",
                                   L"Информация",
                                   MB_OK | MB_ICONINFORMATION);
                    }
                    hSerial = INVALID_HANDLE_VALUE;
                    SetWindowText(GetDlgItem(hwnd, ID_BUTTON_CONNECT), "CONNECT");
                    SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)-1, 0);
                    clearConsole();
                }
            } else {
                handleButtonPress(LOWORD(wParam));
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

