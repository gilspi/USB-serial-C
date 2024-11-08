//
// Created by akhremchik_na on 06.11.2024.
//

#ifndef SERIAL_WITH_GUI_UI_H
#define SERIAL_WITH_GUI_UI_H

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void addAvailablePorts(HWND hComboBox);
void handleButtonPress(int buttonID);
void updateAvailablePorts(HWND hComboBox);

#endif //SERIAL_WITH_GUI_UI_H
