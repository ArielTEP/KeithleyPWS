#pragma once
#include "visa.h"
#include <time.h>
#include <conio.h>
#include <string.h>

ViSession defaultRM; //Resource manager id
ViSession PWS4000; //Identifies the power supply

long ErrorStatus;
char commandString[256];
char ReadBuffer[256];

void OpenPort();
void SendSCPI(char* pString);
void CheckError(char* pMessage);
void delay(clock_t wait);
double readVoltage();
double readCurrent();
void setMeasure(double voltage, double current);
void writeFile(double* voltageArray, double* currentArray, const int samples);
void ClosePort();
