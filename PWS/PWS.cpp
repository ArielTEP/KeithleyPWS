#include "stdafx.h"
#include "pws.h"
#include <stdio.h>
#include <stdlib.h>

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
void ClosePort();


void main(int argc, _TCHAR* argv[]) 
{
	printf("Iniciando...\n");
	double voltage;
	char Buffer[256];
	double current;
	OpenPort();
	//Query the power supply id, read response and print it
	sprintf(Buffer, "*IDN?");
	SendSCPI(Buffer);
	printf("Instrument identification string:%s \n", Buffer);
	SendSCPI("*RST"); //reset the power supply
	SendSCPI("OUTPUT 1"); // turn output on

	voltage = 3.0;
	current = 0.1;
	printf("Setting voltage(V) & current(A): %f,%f \n",	voltage, current);
	ErrorStatus = viPrintf(PWS4000, "VOLT %f\n", voltage); //set the output voltage
	CheckError("Unable to set voltage");
	ErrorStatus = viPrintf(PWS4000, "CURRENT %f\n", current);
	//set the output current
	CheckError("Unable to set current");

	// begin measurement 
	delay(100);
	voltage = readVoltage();
	current = readCurrent();
	printf("Measured voltage(V) & current(A): %f,%f \n", voltage, current);
	
	getch();
	SendSCPI("OUTPUT 0"); //turn output off
	SendSCPI("SYSTEM:LOCAL");
	ClosePort();

	getch();
}

void OpenPort()
{
	//Open communication session with the power supply, and
	// put the power supply in remote
	ErrorStatus = viOpenDefaultRM(&defaultRM);
	ErrorStatus = viOpen(defaultRM,
		"USB::0x05E6::0x2200::1358231::INSTR", 0, 0, &PWS4000);
	CheckError("Unable to open the port");
	SendSCPI("SYSTEM:REMOTE");
}

void SendSCPI(char* pString)
{
	char* pdest;
	strcpy(commandString, pString);
	strcat(commandString, "\n");
	ErrorStatus = viPrintf(PWS4000, commandString);
	CheckError("Can't Write to Power Supply");
	pdest = strchr(commandString, '?'); //Search for query command
	if (pdest != NULL)
	{
		ErrorStatus = viBufRead(PWS4000,
			(ViBuf)ReadBuffer,
			sizeof(ReadBuffer), VI_NULL);
		CheckError("Can't read from driver");
		strcpy(pString, ReadBuffer);
	}
}

void ClosePort()
{
	viClose(PWS4000);
	viClose(defaultRM);
}

void CheckError(char* pMessage)
{
	if (ErrorStatus != VI_SUCCESS)
	{
		printf("\n %s", pMessage);
		ClosePort();
		getch();
		exit(0);
	}
}

double readCurrent() {
	double current = -1;
	ErrorStatus = viPrintf(PWS4000, "MEASURE:CURRENT?\n");
	//medir la corriente de salida
	CheckError("No se pudo escribir en el dispositivo");
	ErrorStatus = viScanf(PWS4000, "%lf", &current); //lectura de voltaje
	CheckError("No se pudo leer la corriente\n");
	return current;
}

double readVoltage() {
	double voltage = -1;
	ErrorStatus = viPrintf(PWS4000, "MEASURE:VOLTAGE?\n");
	//medir el voltaje de salida
	CheckError("No se pudo escribir en el dispositivo");
	ErrorStatus = viScanf(PWS4000, "%lf", &voltage); //lectura de voltaje
	CheckError("No se pudo leer el voltaje\n");
	return voltage;
}

void delay(clock_t wait) 
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock());
}