#pragma once
#include "visa.h"
#include <time.h>
#include <conio.h>
#include <string.h>

void OpenPort();
void SendSCPI(char* pString);
void CheckError(char* pMessage);
void delay(clock_t wait);
double readVoltage();
double readCurrent();
void ClosePort();