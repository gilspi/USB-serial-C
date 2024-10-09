#include <stdio.h>
#include <stdlib.h>
#include <windows.h>  // для взаимодействия с COM портами
#include <string.h>
#include <process.h>  // потоки


HANDLE hSerial;  // дескриптор COM порта


unsigned __stdcall write_to_com(void *arg) {
    (void) arg;
    char *data = "<MAonce>";  // данные для отправки
    DWORD bytes_written;  // uint32_t

    while(1) {
        if (!WriteFile(hSerial, data, strlen(data), &bytes_written, NULL)) {
            perror("Ошибка при записи в COM-порт\n");
            exit(EXIT_FAILURE);
        } else {
            printf("ДАнные записаны в COM порт\n");
        }

        printf("Отправлено: %s\n", data);
        Sleep(2000);  // задержка между отправками
    }

    return 0;
}


unsigned __stdcall read_from_port(void *arg) {
    (void) arg;
    /*__stdcall управляет под капотом, как передаются аргументы
    и очищаются стек. Стандарт для взаимодействия с системными функциями
    и библиотеками.*/
    char buffer[256];
    DWORD bytesRead;  // uint32_t
    while(1) {
        memset(buffer, 0, sizeof(buffer));

        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // конец строки
                printf("Прочитано %s\n", buffer);
            }
        } else {
            perror("Ошибка при чтении с COM-порта\n");
            exit(EXIT_FAILURE);
        }

        Sleep(500);  // ждем новые данные
    }

    return 0;
}


int main() {
    hSerial = CreateFile(
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
    } else {
        printf("Соединение установлено!\n");
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        perror("Не удалось получить параметры порта...\n");
        CloseHandle(hSerial);
        exit(EXIT_FAILURE);
    } else {
        printf("Параметры порта получены.\n");
    }

    dcbSerialParams.BaudRate = CBR_115200;  // скорость 115200
    dcbSerialParams.ByteSize = 8;  // 8 бит данных
    dcbSerialParams.StopBits = ONESTOPBIT;  // один стоп-бит
    dcbSerialParams.Parity = NOPARITY;  // без четности

    // применяем новые настройки порта 
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        perror("Не удалось установить параметры порта\n");
        CloseHandle(hSerial);
        exit(EXIT_FAILURE);
    } else {
        printf("Параметры для порта - установлены\n");
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;  // максимальный интервал между байтами
    timeouts.ReadTotalTimeoutConstant = 50;  // общий таймаут для операций чтения
    timeouts.WriteTotalTimeoutMultiplier = 10;  // таймаут на каждый байт

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        perror("Не удалось установить таймауты порта\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Таймауты установлены!");
    }

    _beginthreadex(NULL, 0, &write_to_com, NULL, 0, NULL);  // создание нового потока на WIN для отправки
    _beginthreadex(NULL, 0, &read_from_port, NULL, 0, NULL);

    while(1) {
        Sleep(1000);  // просто ждем окончания
    }

    CloseHandle(hSerial);

    return 0;
}