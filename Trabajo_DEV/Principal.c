#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"SerialPort.h"


#define MAX_DATA_LENGTH 255

// Funciones prototipo
void autoConnect(SerialPort *arduino,char*);


int main(void)
{
	 //Arduino SerialPort object
	 SerialPort *arduino;
	 
	 // Puerto serie en el que est� Arduino
	 char* portName = "COM3";
	 
	 // Buffer para datos procedentes de Arduino
	 char incomingData[MAX_DATA_LENGTH];
	
	 // Crear estructura de datos del puerto serie
	 arduino = (SerialPort *)malloc(sizeof(SerialPort));
	 
	 // Apertura del puerto serie
	 Crear_Conexion(arduino,portName);
	 autoConnect(arduino,incomingData);
	 
}
void autoConnect(SerialPort *arduino,char *incomingData)
{
	 int readResult;
	 
	
	// Espera la conexi�n con Arduino
	while (!isConnected(arduino))
	{
		Sleep(100);
		Crear_Conexion(arduino,arduino->portName);
	}
	
	 //Comprueba si arduino est� conectado
	if (isConnected(arduino))
	{
		printf ("Conectado con Arduino en el puerto %s\n",arduino->portName);
	}
	
	
	// Bucle de la aplicaci�n
	
	
	while (isConnected(arduino))
	{
		readResult = readSerialPort(arduino,incomingData,MAX_DATA_LENGTH);
		if(readResult!=0)
			printf("%s\n",incomingData);
			
		Sleep(2000);
	}
	
	
	
	
	
	if (!isConnected(arduino))
	 printf ("Se ha perdido la conexi�n con Arduino\n");
}









