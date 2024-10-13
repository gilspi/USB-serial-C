#ifndef SERIAL_H
#define SERIAL_H

#include <windows.h>

void configureComPort(HANDLE hComm, DWORD baudrate, DWORD bytesize, DWORD stopbits, DWORD parity);
void writeToComPort(HANDLE hComm, const char *data);
unsigned __stdcall read_from_port(void *arg);  // Поток для чтения

#endif
