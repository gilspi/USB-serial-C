#include <stdio.h>
#include <stdlib.h>
#include <windows.h>  // для взаимодействия с COM портами
#include <string.h>


int main() {
    HANDLE hSerial = CreateFile(
        "\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        perror("Ошибка при открытии порта...\n");
        exit(EXIT_FAILURE);
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Не удалось получить параметры порта...\n");
        CloseHandle(hSerial);
    }

    CloseHandle(hSerial);
}