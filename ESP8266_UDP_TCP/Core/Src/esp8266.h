/*
 * esp8266.h
 *
 *  Created on: Jul 13, 2022
 *      Author: EDISON NGUNJIRI
 */

#ifndef ESP8266_H_
#define ESP8266_H_


#define IsOK                      "OK"
#define ESPReset                  "AT+RESTORE\r\n"
#define ESPRestart                "AT+RST\r\n"
#define espTest                   "AT\r\n"
#define espVersion                 "AT+GMR\r\n"
#define SearchWIFI                "AT+CWLAP\r\n"
#define DisconnectAP              "AT+ CWQAP\r\n"
#define connectWIFI               "AT+CWJAP_DEF=\"SUPERHUMAN\",\"Edie!1994@@\"\r\n"
//#define connectWIFI               "AT+CWJAP_DEF=\"Superhuman\",\"00000004\"\r\n"
#define CheckWIFIMODE             "AT+CWMODE=?\r\n"
#define WIFI_station_MODE         "AT+CWMODE=1\r\n"                     //station mode
#define ConnectServer             "AT+CIPSTART=\"TCP\",\"192.168.4.1\",8080\r\n"
#define WIFI_softAPMODE           "AT+CWMODE=2\r\n"                       //softAP mode
#define WIFI_softAP_station_MODE  "AT+CWMODE=3\r\n"              //softAP + station mode
#define Set_SoftAP_name           "AT+CWSAP=\"pos_softap\"," ",11,0,3\r\n"
#define CheckMuxMODE              "AT+CIPMUX?\r\n"                            //enable multi connection mode
#define MUXMODEON                 "AT+CIPMUX=1\r\n"                                //enable multi connection mode
#define MUXMODEOFF                "AT+CIPMUX=0\r\n"                                //enable multi connection mode
#define EnaTCPserver              "AT+CIPSERVER=1,8080\r\n"                     //Enable the TCP server with port 80,
#define DisTCPServer              "AT+CIPSERVER=0\r\n"
#define CheckIP                   "AT+CIFSR\r\n"
#define CUSTOMURL                 "HELLO WORLD,My name is server\r\n"
#define InitTransmission          "AT+CIPSEND=0,9\r\n"
#define GetCIPStatus              "AT+CIPSTATUS\r\n"
#define CloseConnection           "AT+CIPCLOSE=0\r\n"//  once file sent, close connection
#define GetWifiMode               "AT+CWMODE=?\r\n"
#define setTimeout                "AT+CIPSTO=120\r\n"
#define StartTCP                  "AT+CIPSTART=\"TCP\",\"192.168.0.101\",8080\r\n"
#define StartUDP                  "AT+CIPSTART=4,\"UDP\",\"192.168.0.104\",8080,1112,0\r\n"
#define EspSent                   "OK\r\n"
#define CloseOpenConnection       "AT+CIPCLOSE\r\n"
#define CloseALLConnection        "AT+CIPCLOSE=5\r\n"


typedef struct Control_Panel_Type
{
	unsigned LED1:1;
	unsigned LED2:2;
	unsigned LED3:3;
	unsigned LED4:4;
}Control_Panel_Type;

void Esp_Init_Com(void);
void JoinWifiNewtwork();
void ClearBuffer(char *buffer);
int SendCommand(char * command,char *Reply);
void EspTcpClient(void);
int Esp8266_Application(void);
void EspTcpServer(void);
void EspUdpServer(void);
void EspTcpSend(char * data,int len);
char *GetIpAddress(char *Str);
int GetConnectionStatus(void);
void Control_Panel(Control_Panel_Type * Ctrl);
#endif /* ESP8266_H_ */
