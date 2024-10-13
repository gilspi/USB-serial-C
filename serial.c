#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include "serial.h"

void configureComPort(HANDLE hComm, DWORD baudrate,
                      DWORD bytesize, DWORD stopbits,
                      DWORD parity) {
    /*
    Конфигурация порта при запуске приложения.
    baudrate - скорость передачи данных (например, 115200).
    bytesize - размер данных (например, 8 бит).
    stopbits - количество стоп-битов (ONESTOPBIT или TWOSTOPBITS).
    parity - проверка четности (NOPARITY, EVENPARITY, ODDPARITY).
    */
    
    DCB dcb;  // работа с параметрами COM-порта
    SecureZeroMemory(&dcb, sizeof(DCB));  // заполнение области памяти нулями

    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(hComm, &dcb)) {
        perror("Не удалось получить параметры COM порта\n");
        CloseHandle(hComm);
        exit(EXIT_FAILURE);
    } else {
        printf("Параметры COM порта успешно получены\n");
    }

    dcb.BaudRate = baudrate;  // Скорость порта
    dcb.ByteSize = bytesize;  // 8 бит данных
    dcb.StopBits = stopbits;  // Один стоп-бит
    dcb.Parity = parity;    // Без четности

    if (!SetCommState(hComm, &dcb)) {
        perror("Не удалось установить параметры COM порта\n");
        CloseHandle(hComm);
        exit(EXIT_FAILURE);
    } else {
        printf("Параметры COM порта успешно установлены\n");
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;  // максимальный интервал между байтами
    timeouts.ReadTotalTimeoutConstant = 50;  // общий таймаут для операций чтения
    timeouts.WriteTotalTimeoutMultiplier = 10;  // таймаут на каждый байт

    if (!SetCommTimeouts(hComm, &timeouts)) {
        perror("Не удалось установить таймауты порта\n");
        CloseHandle(hComm);
        exit(EXIT_FAILURE);
    } else {
        printf("Таймауты установлены!");
    }

}

void writeToComPort(HANDLE hComm, const char *data) {
    /*
    Функция отправляет команду на порт только один раз после изменения
    состояния кнопки на графическом пользовательском интерфейсе.
    */

    DWORD bytesWritten;  // uint32_t
    if (!WriteFile(hComm, data, strlen(data), &bytesWritten, NULL)) {
        perror("Ошибка при записи в COM порт\n");
        CloseHandle(hComm);
        exit(EXIT_FAILURE);
    } else {
        printf("Данные отправлены: %s\n", data);
    }
}

unsigned __stdcall read_from_port(void *arg) {
    /*
    __stdcall управляет под капотом, как передаются аргументы
    и очищаются стек. Стандарт для взаимодействия с системными функциями
    и библиотеками.
    */

    HANDLE hComm = (HANDLE)arg;  // Преобразуем аргумент обратно в HANDLE
    (void) arg;
    char buffer[256];
    DWORD bytesRead;

    while (1) {
        memset(buffer, 0, sizeof(buffer));  // каждый раз чистим область памяти

        if (ReadFile(hComm, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // завершаем строку
                printf("Прочитано: %s\n", buffer);  // выводим данные
            }
        } else {
            printf("Ошибка при чтении с COM порта\n");
        }

        Sleep(500);  // ждем следующие данные
    }

    return 0;
}
