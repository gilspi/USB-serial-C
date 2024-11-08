//
// Created by akhremchik_na on 06.11.2024.
//
#include <windows.h>
#include "serial.h"


void clearConsole() {
    system("cls");
}


void disconnect() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        clearConsole();
    }
}