/*
 * esp8266.c
 *
 *  Created on: July 13, 2022
 *      Author: EDISON NGUNJIRI
 */

#include "esp8266.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306.h"


extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;

int Command_Flag=0;
extern Control_Panel_Type panel;
char IP_Address[13];
char RxBuffer[500];
int NetworkStatus=0;
unsigned int CmdFlag=0;




void Esp_Init_Com(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)RxBuffer, 200);
	__HAL_UART_DISABLE_IT(&huart1,DMA_IT_HT);
}



void JoinWifiNewtwork(void)
{
	SendCommand(connectWIFI, "GOT IP");
	HAL_Delay(5000);
}


int SendCommand(char * command,char *Reply)
{
	CmdFlag=1;
	int wait_time=HAL_GetTick();
	char *EspStatus={0};
	ClearBuffer(RxBuffer);
	HAL_UART_Transmit(&huart1, (uint8_t*)command,strlen(command), HAL_MAX_DELAY);
	while(1)
	{
		if(HAL_GetTick()-wait_time<10000)
		{
			EspStatus=strstr(RxBuffer,Reply);
			if(*EspStatus ==Reply[0])
			{
				break;
			}
			EspStatus=strstr(RxBuffer,"ERROR");
			if(*EspStatus =='E')
			{
				return HAL_ERROR;
			}
		}
		else
		{
			break;
		}

	}
	CmdFlag=0;
	HAL_Delay(20);
	return HAL_OK;
}

/**
 * Function to clear a buffer
 * @buffer -The Buffer to be cleared
 */
void ClearBuffer(char *buffer)
{
	memset(RxBuffer,0,strlen(RxBuffer));
}

/**
 * EspTcpClient- Function thats creates the device as TCP client.
 * 1.Close any open connection first by sending AT+CIPCLOE
 * 2.Enable station mode by setting AT+CWMODE=3
 * 3.Disable Multiple connections by sending AT+CIPMUX=0
 * Then start the TCP connection by sending
 * "AT+CIPSTART=\"TCP\",\"*PUT THE SERVER ADDRESS*\",*SERVER PORT*\r\n"
 *
 */
void EspTcpClient(void)
{
	SendCommand("AT\r\n","OK\r\n");
	SendCommand(CloseOpenConnection,"OK\r\n");
	SendCommand(CloseALLConnection,"OK\r\n");
	SendCommand(CheckIP, "OK\r\n");
	GetIpAddress(RxBuffer);
	SendCommand(WIFI_softAP_station_MODE,"OK\r\n");
	SendCommand(MUXMODEOFF, "OK\r\n");
	SendCommand(StartTCP, "CONNECT\r\n\r\nOK\r\n");
}

/**
 * EspUdpServer- Function thats creates the device as UDP client.
 * 1.Close any open connection first by sending AT+CIPCLOE
 * 2.Enable station mode by setting AT+CWMODE=3
 * 3.Enable Multiple connections by sending AT+CIPMUX=1
 * Then start the TCP connection by sending
 * "AT+CIPSTART=4,\"UDP\",\"192.168.101.110\",8080,1112,0\r\n"
 *192.168.101.110->REMOTE IP of the UDP server
 *8080->Remote IP of the UDP server
 *1112->LOCAL PORT of esp8266
 */

void EspUdpServer(void)
{
	SendCommand("AT\r\n","OK\r\n");
	SendCommand(CloseOpenConnection,"OK\r\n");
	SendCommand(CloseALLConnection,"OK\r\n");
	SendCommand(CheckIP, "OK\r\n");
	GetIpAddress(RxBuffer);
	SendCommand(WIFI_softAP_station_MODE,"OK\r\n");
	SendCommand(MUXMODEON, "OK\r\n");
	SendCommand(StartUDP, "OK\r\n");
}


void EspTcpServer(void)
{
	SendCommand("AT\r\n","OK\r\n");
	SendCommand(DisTCPServer, "OK\r\n");
	SendCommand(CloseOpenConnection,"OK\r\n");
	SendCommand(CloseALLConnection,"OK\r\n");
	SendCommand(CheckIP, "OK\r\n");
	GetIpAddress(RxBuffer);
	SendCommand(WIFI_softAP_station_MODE,"OK\r\n");
	SendCommand(MUXMODEON, "OK\r\n");
	SendCommand(EnaTCPserver, "OK\r\n");
}


/** N
 * HAL_UARTEx_RxEventCallback() is called when bytes are received and idle line is detected
 * This function receives data from Servers
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance==USART1)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)RxBuffer, 200);
		__HAL_UART_DISABLE_IT(&huart1,DMA_IT_HT);
		 Esp8266_Application();
	}

}
/**
 * AdddataStack()-Creates a stack for incoming commands
 *
 */

void EspTcpSend(char * data,int len)
{
	SendCommand(InitTransmission,"> ");
	SendCommand(data,"bytes\r\n");
	SendCommand("AT+CIPCLOSE=0\r\n","CLOSED\r\n");
}

/**
 * Esp8266_Application() is the main application
 * This tests the incoming commands from the servers
 *
 */

int Esp8266_Application(void)
{
	char *Cmd={0};
	Cmd=strstr(RxBuffer,"CONNECT\r\n");
	if(*Cmd =='C')
	{
		Command_Flag=1;
		ClearBuffer(RxBuffer);
	}
	Cmd=strstr(RxBuffer,"OFF_LED1");
	if(*Cmd =='O')
	{
		Command_Flag=2;
		ClearBuffer(RxBuffer);
		panel.LED1=0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	}
	Cmd=strstr(RxBuffer,"LED1_ON");
	if(*Cmd =='L')
	{
		Command_Flag=3;
		ClearBuffer(RxBuffer);
		panel.LED1=1;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	}
	Cmd=strstr(RxBuffer,"OFF_LED2");
	if(*Cmd =='O')
	{
		Command_Flag=2;
		ClearBuffer(RxBuffer);
		panel.LED2=0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	}
	Cmd=strstr(RxBuffer,"LED2_ON");
	if(*Cmd =='L')
	{
		Command_Flag=3;
		ClearBuffer(RxBuffer);
		panel.LED2=1;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	}
	Cmd=strstr(RxBuffer,"CLOSED");
	if(*Cmd =='C')
	{
		Command_Flag=4;
		ClearBuffer(RxBuffer);
	}
	return(1);
}

/**
 * This Function is used to get the IP address of esp8266
 * This function should be called after checking the IP Address
 * by using @SendCommand(CheckIP, "OK\r\n");
 *
 */
char *GetIpAddress(char *Str)
{
	char *Pos;
	int i=0;
	int wait_time=HAL_GetTick();
	Pos=strstr(Str,"STAIP,");
	while(HAL_GetTick()-wait_time<10000)
	{
		IP_Address[i]=*(Pos+i+7);
		if(IP_Address[i] =='"')
		{
			break;
		}
		i++;
	}
	IP_Address[i]='\0';
	return(IP_Address);
}


int GetConnectionStatus(void)
{
	char *Pos={0};
	char Gbuf[3]={0};
	SendCommand(GetCIPStatus,"OK\r\n");
	Pos=strstr(RxBuffer,"CIPSTATUS");
	Gbuf[0]=*(Pos+18);
	NetworkStatus=atoi(Gbuf);
	return(NetworkStatus);
}

void Control_Panel(Control_Panel_Type * Ctrl)
{
	if(Ctrl->LED1==0 && Ctrl->LED2==0)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(15,10);
		ssd1306_WriteString("GREEN :OFF", Font_11x18,White);
		ssd1306_SetCursor(15,40);
		ssd1306_WriteString("YELLOW:OFF", Font_11x18,White);
		ssd1306_UpdateScreen(&hi2c1);
	}
	else if(Ctrl->LED1==0 && Ctrl->LED2==1)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(15,10);
		ssd1306_WriteString("GREEN :OFF", Font_11x18,White);
		ssd1306_SetCursor(15,40);
		ssd1306_WriteString("YELLOW:ON", Font_11x18,White);
		ssd1306_UpdateScreen(&hi2c1);
	}
	else if(Ctrl->LED1==1 && Ctrl->LED2==0)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(15,10);
		ssd1306_WriteString("GREEN :ON", Font_11x18,White);
		ssd1306_SetCursor(15,40);
		ssd1306_WriteString("YELLOW:OFF", Font_11x18,White);
		ssd1306_UpdateScreen(&hi2c1);
	}
	else if(Ctrl->LED1==1 && Ctrl->LED2==1)
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(15,10);
		ssd1306_WriteString("GREEN :ON", Font_11x18,White);
		ssd1306_SetCursor(15,40);
		ssd1306_WriteString("YELLOW:ON", Font_11x18,White);
		ssd1306_UpdateScreen(&hi2c1);
	}

}
