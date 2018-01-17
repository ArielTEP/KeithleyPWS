#include "stdafx.h"
#include "pws.h"
#include <stdio.h>
#include <stdlib.h>

#define RES "USB::0x05E6::0x2200::1358231::INSTR"
FILE *file;


void main(int argc, _TCHAR* argv[]) 
{
	printf("Iniciando...\n");
	double voltage;
	double current;
	
	// turn output on
	OpenPort();
	SendSCPI("OUTPUT 1"); 
	
	// set output voltage and current
	setMeasure(5.0f, 0.1f);

	// begin measurement
	const int samples = 50;
	double voltageArray[samples];
	double currentArray[samples];
	int i;
	delay(10);
	for (i = 0; i < samples; i++) 
	{
		
		voltageArray[i] = readVoltage();
		currentArray[i] = readCurrent();
		delay(100); // sample time
	}
	
	writeFile(voltageArray, currentArray, samples);

	// end of work
	getch();
	SendSCPI("OUTPUT 0"); //turn output off
	SendSCPI("SYSTEM:LOCAL");
	ClosePort();

	getch();
}

void setMeasure(double voltage, double current) 
{
	printf("Setting voltage(V) & current(A): %f,%f \n", voltage, current);
	setVolatage(voltage);
	setCurrent(current);
}

void setVolatage(double voltage) 
{
	ErrorStatus = viPrintf(PWS4000, "VOLT %f\n", voltage);
	CheckError("Unable to set voltage");
}

void setCurrent(double current) 
{
	ErrorStatus = viPrintf(PWS4000, "CURRENT %f\n", current);
	CheckError("Unable to set current");
}

void writeFile(double* voltageArray, double* currentArray, const int samples)
{
	char string[512];
	char fullpath[64];
	printf("\nNombre del archivo (especifique el formato): ");
	scanf("%s", string);
	fflush(stdin);
	strcpy(fullpath, "C:\\PWSDatabase\\");
	strcat(fullpath, string);
	printf("Creado en: %s\n", fullpath);
	printf("\nPresione intro para continuar...\n");
	getch();

	file = fopen(fullpath, "w");
	int i;
	for (i = 0; i < samples; i++) { // escritura del archivo
		fprintf(file, "%lf,%lf\n", voltageArray[i], currentArray[i]);
		printf("Measured voltage(V) & current(A): %f,%f \n", voltageArray[i], currentArray[i]);
	}
	printf("Listo.\n");
	fclose(file);
}

void identifyResource() 
{
	char Buffer[256];
	OpenPort();
	sprintf(Buffer, "*IDN?");
	SendSCPI(Buffer);
	printf("Instrument identification string:%s \n", Buffer);
	SendSCPI("*RST"); //reset the power supply
	ClosePort();
}

void OpenPort()
{
	//Open communication session with the power supply, and
	// put the power supply in remote
	ErrorStatus = viOpenDefaultRM(&defaultRM);
	ErrorStatus = viOpen(defaultRM, RES, 0, 0, &PWS4000);
	CheckError("La fuente esta desconectada. Conectela y reinicie el programa.");
	SendSCPI("SYSTEM:REMOTE");
}

void SendSCPI(char* pString)
{
	char* pdest;
	strcpy(commandString, pString);
	strcat(commandString, "\n");
	ErrorStatus = viPrintf(PWS4000, commandString);
	CheckError("No se puede escribir la Fuente");
	pdest = strchr(commandString, '?'); //Search for query command
	if (pdest != NULL)
	{
		ErrorStatus = viBufRead(PWS4000,
			(ViBuf)ReadBuffer,
			sizeof(ReadBuffer), VI_NULL);
		CheckError("No se pudo leer nada del Driver");
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

void displayHelp()
{
	printf("************************************\n");
	printf("*            A Y U D A             *\n");
	printf("************************************\n");
	printf("\n\nVerifique que la fuente este encendida y correctamente\n");
	printf("conectada a la PC por medio del cable USB.\n");
	getch();
}

void displayInfo()
{
	printf("************************************\n");
	printf("*       A C E R C A  D E	   *\n");
	printf("************************************\n");
	printf("\n\nEste software es de uso libre, escrito especificamente para\n");
	printf("la fuente programable KEITHLEY 2200-32-3\n");
	printf("Fecha de creacion: 30/05/2017 en SEPI-Telecom ESIME\n");
	printf("Autor: Alumno de Servicio Social\n");
	getch();
}