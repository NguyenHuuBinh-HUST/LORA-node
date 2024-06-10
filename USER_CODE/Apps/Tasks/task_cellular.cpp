#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "task_cellular.h"
//#include "mdm_hal.h"
//#include "mdm_socket_hal.h"
//#include "main_loadconfig.h"
#include "debug_hal.h"
#include "systick_driver.h"
//#include "base85.h"
//#include "rtc_driver.h"
#include "Serial_Debug.h"

//#include "Circular_Buffer.h"
#include "task_LoRa.h"
#include "./cfg/LoRaMac_Node_Cfg.h"
//#include "user_string.h"
//uint8_t rxCelbuffer[256];    // Length of rx buffer

//uint8_t txCelbuffer[256];    // Length of tx buffer

uint8_t sendBuffer[256];

/* Private typedef ----------------------------------------------------------*/
enum MODEM_RUNNING_STATE
{
	MODEM_REG_NETWORK = 0, 		//Startup
	MODEM_CREAT_SOCKET_HANDLER,//Creat socket handler
	MODEM_CREAT_CONNECTION,   //Connect to server
	MODEM_SEND_PACKAGE,				//Send Data
	MODEM_CLOSE_SOCKET				//Close socket
};

enum comtype
{
	LORA = 0,
	GSM,
	ETHERNET,
};

enum PACKAGE_TYPE
{
	AUTHENTICATE = 0,				//Authentication if need
	CLOCK_SYNC,						//Synchronous time from server
	NORMAL_DATA						//Normal data package
};

ExtBoard_t detector_board = {
	&huart5, \
	{Eb_Stt_Port,Eb_Stt_Pin}, \
};

/* Private define -----------------------------------------------------------*/
#define MAXSIZE_ENCRYPT_BASE85 256
#define MAXSIZE_SOCKET_RECV    128

//MDM_DeviceSerial thanhntMDM;
extern SystemInformation systemInfo;
extern ServerConfig serverCfg;
extern SystemTime timeSystemCr;
extern Cycle_TimeData timeDataCtl;
extern volatile bool LTE_flag;
extern LoRaMacMessageData_t tx_msg;
extern xQueueHandle xQueue1;
DataPackage gprsDataPackage;

//extern CircularBuffer_t cir_buf;

xSemaphoreHandle sem_Modem_Bus = 0;

//unsigned char en_base85_buff[MAXSIZE_ENCRYPT_BASE85], en_base85_data_send[MAXSIZE_ENCRYPT_BASE85];
char socket_recv_buff[MAXSIZE_SOCKET_RECV];
long socket_Create;
//int EN_BASE85_LEN = 0;
//NetStatus netStatus_MDM;
bool UpdatedTime = false;

/* Private macro ------------------------------------------------------------*/

int Send_DataPakage_Normal(char command, long socket_created, uint8_t *data);
void Process_Recv_From_Server(long socket_created, int num_recv);
void Process_SMS_From_Server(char *buffer, int len, char *num_from);
char* replaceWord(const char* s, const char* oldW,const char* newW);
static void DetectorBuff_Reset(void)
{    
	detector_board.cpltUartdata = 0;
	detector_board.rxIndex = 0;
	detector_board.dataLen = 0;
	memset(detector_board.rxTmp,'\0',2);
	memset(detector_board.rxBuffer,'\0',EB_RX_BUFFER_SIZE);
}

/* Task Create --------------------------------------------------------------*/
void task_Cellular(void *params)
{
	//Init state
	HAL_UART_Receive_IT(detector_board.huart,(uint8_t *)detector_board.rxTmp,1); // Enable uart receive interupt

	MODEM_RUNNING_STATE modemState = MODEM_REG_NETWORK;
//	Std_ReturnType ret = STD_NOT_OK;
	//Init variables
	sockaddr_t socketAddr;
	int socket_Connect = 0, send_Socket_Result = 0;
	int retry_socket = 0, retry_connection = 0, try_use_mdm = 0;
	
	uint8_t send_buffer[256];
	
	while(1)
	{
			
		switch(modemState)
		{
			//=========================================================================//
			case MODEM_REG_NETWORK:
				systemInfo.Cycle_MDM_Led = 100;
				if(xSemaphoreTake(sem_Modem_Bus,portMAX_DELAY))
				{
					thanhntMDM.reset();
					if(thanhntMDM.connect())
					{
						try_use_mdm = 0;
						modemState = MODEM_CREAT_SOCKET_HANDLER;
					}
					xSemaphoreGive(sem_Modem_Bus);
				}
			break;
			//=========================================================================//
			case MODEM_CREAT_SOCKET_HANDLER:
				//Create socket
				systemInfo.Cycle_MDM_Led = 500;
			
				//Check GPRS Enable?
				if(!serverCfg.Enable_GPRS)
				{
					vTaskDelay(30000); //Free time to system check sms if disable gprs
					break;
				}
				
				//Reset temp variables
				memset(&socketAddr, 0, sizeof(socketAddr));
				retry_socket = 0;
				retry_connection = 0;
				socket_Create = -1;
				socket_Connect = 0;
				send_Socket_Result = 0;
				
				if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
				{
					//Creat socket on Modem
					socket_Create = socket_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NIF_DEFAULT);
					xSemaphoreGive(sem_Modem_Bus);
					if(socket_Create < 0) //Check create socket OK?
					{
						retry_socket++;
						vTaskDelay(10000);
						if(retry_socket > 3) modemState = MODEM_REG_NETWORK; //Try -> Not OK => Reset
					}
					else modemState = MODEM_CREAT_CONNECTION;
				}
			break;
			//=========================================================================//
			case MODEM_CREAT_CONNECTION:
				systemInfo.Cycle_MDM_Led = 1000;
				if(!serverCfg.Enable_GPRS)
				{
					modemState = MODEM_CLOSE_SOCKET;
					break;
				}
				if(systemInfo.Flag_Change_Server)
				{
					systemInfo.Flag_Change_Server = 0;
					modemState = MODEM_CLOSE_SOCKET;
					break;
				}
				// the family is always AF_INET
				socketAddr.sa_family = AF_INET;
				// the destination port: 5530
				socketAddr.sa_data[0] = serverCfg.Port >> 8; 		//High nibble
				socketAddr.sa_data[1] = serverCfg.Port  & 0xff;	//Low nibble
				// the destination IP address: 
				socketAddr.sa_data[2] = serverCfg.IP_Server >> 24;
				socketAddr.sa_data[3] = serverCfg.IP_Server >> 16 & 0xff;
				socketAddr.sa_data[4] = serverCfg.IP_Server >>  8 & 0xff;
				socketAddr.sa_data[5] = serverCfg.IP_Server  &  0xff;
				//Connect socket to address
				if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
				{
					socket_Connect = socket_connect(socket_Create, &socketAddr, sizeof (socketAddr));
					xSemaphoreGive(sem_Modem_Bus);
					if(!socket_Connect) //Connect OK
					{
						modemState = MODEM_SEND_PACKAGE;
					}
					else 
					{
						retry_connection++;
						try_use_mdm++;
						vTaskDelay(20000); //Free time to system check sms if change server
						if(retry_connection > 3) modemState = MODEM_CLOSE_SOCKET;
					}
				}
			break;
			//=========================================================================//
			case MODEM_SEND_PACKAGE:
			  systemInfo.Cycle_MDM_Led = 5000;
				if(systemInfo.Flag_Change_Server)
				{
					systemInfo.Flag_Change_Server = 0;
					modemState = MODEM_CLOSE_SOCKET;
					break;
				}
				if(!serverCfg.Enable_GPRS)
				{
					modemState = MODEM_CLOSE_SOCKET;
					break;
				}
				if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
				{
					
//						if(detector_board.cpltUartdata == 1)
//						{
//							uint8_t Size = strlen((char *)detector_board.rxBuffer);
//							sprintf((char*)(sendBuffer),"%s,@PY_LTE,%d,02,6d,", serverCfg.IP_Gen,Size);
//							sprintf((char *)(sendBuffer+strlen((char*)sendBuffer)),"%d:%d:%d,",(int)(GPS.GPGGA.UTC_Hour), \
//				                                                                         (int)(GPS.GPGGA.UTC_Min),(int)(GPS.GPGGA.UTC_Sec));
//							memcpy(&sendBuffer[strlen((char*)sendBuffer)],detector_board.rxBuffer,detector_board.dataLen);
//							sprintf((char *)(sendBuffer+strlen((char*)sendBuffer)),",%f,%f",(float)(GPS.GPGGA.LatitudeDecimal), \
//				                                                                    (float)(GPS.GPGGA.LongitudeDecimal));
//							DEBUG_USER("%s\r\n",sendBuffer);
//						  send_Socket_Result = Send_DataPakage_Normal(0x03, socket_Create,(uint8_t *) sendBuffer);
//						}
//						if(LTE_flag == 1)
//						{
//							DEBUG_USER("Send data via cellu\r\n");
//							send_Socket_Result = Send_DataPakage_Normal(0x03, socket_Create,(uint8_t *) tx_msg.FRMPayload);
							if( xQueue1 != 0 )
							{
								if( xQueueReceive( xQueue1, ( sendBuffer ), ( portTickType ) 10 ) == pdTRUE )
								{
									DEBUG_USER("Lora fail, change to Cellular\r\n");
									char* result = NULL;
									result = replaceWord((char*)sendBuffer, "@HN_LORA", "@HN_LTE");
									send_Socket_Result = Send_DataPakage_Normal(0x03, socket_Create,(uint8_t *) result);
									vPortFree(result);
								}
							}
						//}
						xSemaphoreGive(sem_Modem_Bus);
						if(send_Socket_Result<0) 
						{ 
							/* Sending error */
							modemState = MODEM_CLOSE_SOCKET;
							vTaskDelay(10000);
							break;
						}
						else
						{
//							DEBUG_USER("GPRSsend\r\n");
								DetectorBuff_Reset();
						    memset(sendBuffer,0,sizeof(sendBuffer));
								LTE_flag = 0;
						}
						vTaskDelay(3000);
						break;
						vTaskDelay(100);
				}
			break;
			//=========================================================================//
			case MODEM_CLOSE_SOCKET:
				if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
				{
					socket_close(socket_Create);
					xSemaphoreGive(sem_Modem_Bus);
					if(try_use_mdm < 10)
					{
					modemState = MODEM_CREAT_SOCKET_HANDLER;
					}
					else modemState = MODEM_REG_NETWORK;
				}
			break;
		}
		vTaskDelay(100);
	}
}

void task_Command_From_Server(void *params)
{
	int socket_num_recv = 0;
	while(1)
	{
		if(systemInfo.Cycle_MDM_Led > 1000) //Connected
		{
			if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
			{
				// Check recv data
				socket_num_recv = thanhntMDM.socketReadable(socket_Create);
				if(socket_num_recv > 0)
				{
					Process_Recv_From_Server(socket_Create, socket_num_recv);
				}
				xSemaphoreGive(sem_Modem_Bus);
			}
		}
		vTaskDelay(1000);
	}
}

void task_SMS_From_Server(void *params)
{

	int numSMS = 0, ix[3];
	char buf[160];
	char fromNumber[32];
	while(1)
	{
		if(systemInfo.Cycle_MDM_Led > 100) //Registered to network
		{
			
				
			if(xSemaphoreTake(sem_Modem_Bus, portMAX_DELAY))
			{
				
				//Get Network Status
				thanhntMDM.getSignalStrength(netStatus_MDM);
				systemInfo.GSM_RSSI = netStatus_MDM.rssi;
				DEBUG_D("GSM_RSSI:%d\r\n",systemInfo.GSM_RSSI);
				//Check SMS and Process if exist
				numSMS = thanhntMDM.smsList("ALL",ix, 3);
				if(numSMS > 3) numSMS = 3;
				DEBUG_D("num: %d\r\n",numSMS);
				while(0 < numSMS--)
				{
					memset(&buf, 0, sizeof(buf));
					memset(&fromNumber, 0, sizeof(fromNumber));
					if(thanhntMDM.smsRead(ix[numSMS], fromNumber, (char *)&buf, sizeof(buf)))
					{
						//DEBUG_D("start SMS3");
						DEBUG_D("\nGot at [%d] from %s: %s\n", ix[numSMS], fromNumber, buf);
						thanhntMDM.smsDelete(ix[numSMS]);
						Process_SMS_From_Server((char *)&buf, strlen(buf), (char *)&fromNumber);
					}
				}
				xSemaphoreGive(sem_Modem_Bus);
				
			}
		}
		vTaskDelay(10000);
	}
}

/* Functions Create --------------------------------------------------------------*/
int Send_DataPakage_Normal(char command, long socket_created, uint8_t *data)
{
	int i, send_result, base_data_size = 0;
	
	char buff_header[6];
//	int en_base85_len = 0;
	
	//put IMEI 
	for(i = 0; i<15; i++) gprsDataPackage.IMEI[i] = serverCfg.ClientID[i];
	//put Command
	gprsDataPackage.Command = command;
	//put Time
	gprsDataPackage.Time = timeSystemCr;
  
	send_result = socket_send(socket_created, data, strlen((char *)data));
	return send_result;
}

void Process_Recv_From_Server(long socket_created, int num_recv)
{
	short day, mon, year, hour, min, sec;
	char ad_num[12];
	char protocol[3];
	char new_id[15];
//	char new_ip[16];
	//Clear buffer
	memset(socket_recv_buff, 0, MAXSIZE_SOCKET_RECV);
	socket_receive(socket_created, &socket_recv_buff, num_recv, 1000); //Recv with timeout
	DEBUG_D(socket_recv_buff);
	if(strncmp(socket_recv_buff,serverCfg.IP_Gen,strlen(serverCfg.IP_Gen)) != 0 && strncmp(socket_recv_buff,"Server is",9) != 0)
	{
//		flag_error_socket = 1;
	}
	if(strncmp(socket_recv_buff,serverCfg.IP_Gen,strlen(serverCfg.IP_Gen)) == 0)
	{
		strcpy(socket_recv_buff,socket_recv_buff+strlen(serverCfg.IP_Gen)+1);
		if(strncmp(socket_recv_buff,"RESET",5) == 0)
		{
			socket_send(socket_created,"RESET_OK",8);
			HAL_Core_System_Reset();
		}
		if(strncmp(socket_recv_buff,"PHONE",5) == 0)
		{
			sscanf(socket_recv_buff,"PHONE,%s",ad_num);
			if(strlen(ad_num)==12)
			{
				memset(&serverCfg.admin_num, 0, sizeof(serverCfg.admin_num));
				strcpy(serverCfg.admin_num,ad_num);
				DEBUG_D("\r\n%s",serverCfg.admin_num);
				socket_send(socket_created,"PHONE_OK",8);
			}
		}
		if(strncmp(socket_recv_buff,"PROTOCOL",8) == 0)
		{
			sscanf(socket_recv_buff,"PROTOCOL,%s",protocol);
			if(strncmp(protocol,"TCP",3) == 0)
			{
				serverCfg.Protocol = IPPROTO_TCP;
				systemInfo.Flag_Change_Server = 1;
			}
			if(strncmp(protocol,"UDP",3) == 0)
			{
				serverCfg.Protocol = IPPROTO_UDP;
				systemInfo.Flag_Change_Server = 1;
			}
			DEBUG_D("\r\n%d", serverCfg.Protocol);
			socket_send(socket_created,"PROTOCOL_OK",11);
		}
		if(strncmp(socket_recv_buff,"ID",2) == 0) // change ClientID
		{
			memset(new_id, 0, sizeof(new_id));
			sscanf(socket_recv_buff,"ID,%s",new_id);
			if(strlen(new_id) == 15)
			{
				memcpy(serverCfg.ClientID, new_id, 15);
				DEBUG_D("\r\n%s",(char*)serverCfg.ClientID);
				socket_send(socket_created,"ID_OK",5);
			}
		}
		if(strncmp(socket_recv_buff,"IP",2) == 0)
		{
			int a;
			if(sscanf(socket_recv_buff+3,"%d.%d.%d.%d",&a,&a,&a,&a) == 4)
			{
				if(strlen(socket_recv_buff)<19)
				{	
					memset(serverCfg.IP_Gen, 0,sizeof(serverCfg.IP_Gen));
					strcpy(serverCfg.IP_Gen,socket_recv_buff+3);
					DEBUG_D("\r\n%s",(char*)serverCfg.IP_Gen);
					socket_send(socket_created,"IP_OK",5);
				} 
			}
		}
		if(strncmp(socket_recv_buff,"LIMIT",5) == 0)
		{
			//DEBUG_D("socket_recv_buff:%s, length: %d",socket_recv_buff,strlen(socket_recv_buff));
			*(socket_recv_buff+strlen(socket_recv_buff)+1)=13;
			HAL_UART_Transmit(&huart5,(uint8_t*)&socket_recv_buff,strlen(socket_recv_buff)+1,100);
			DEBUG_D("\r\nTransmit %s done",socket_recv_buff);
			socket_send(socket_created,"LIMIT_OK",8);
		}
	}
}

//void Process_SMS_From_Server(char *buffer, int len, char *num_from)
//{
//	uint32_t password, password_new, timer_adc, timer_gprs, timer_log2sd;
//	int ip1, ip2, ip3, ip4, port,on_off_sd_gprs, ret_ussd = 0;
//	char new_id[15], buff_sms_send[159], buff_ussd[20], buff_ussd_ret[159];
//	
//	//Process command from server through SMS - Password
//	if(strncmp(buffer, "#RESET", 6) == 0)      //System Reset
//	{
//		sscanf(buffer, "#RESET,%u.*",&password);
//		if(serverCfg.Password == password)
//		{
//			thanhntMDM.smsSend(num_from, "[OK]");
//			HAL_Core_System_Reset();
//		}
//	}
//	else if(strncmp(buffer, "#SERVER", 7) == 0) //Reconfig server info
//	{
//		if(sscanf(buffer, "#SERVER,%u,%d.%d.%d.%d,%d.*", &password, &ip1, &ip2, &ip3, &ip4, &port) == 6)
//		{
//			if(serverCfg.Password == password)
//			{
//				serverCfg.IP_Server = (((uint32_t)(ip1))<<24) | (((uint32_t)(ip2))<<16) | (((uint32_t)(ip3))<< 8) | (((uint32_t)(ip4))<< 0);
//				serverCfg.Port = port;
//				systemInfo.Flag_Change_Server = 1;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//			}
//		}
//	}
//	else if(strncmp(buffer, "#PASSWORD", 9) == 0) //Change Password
//	{
//		if(sscanf(buffer, "#PASSWORD,%u,%u.*",&password, &password_new) == 2)
//		{
//			if(serverCfg.Password == password)
//			{
//				serverCfg.Password = password_new;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//			}
//		}
//	}
//	else if(strncmp(buffer, "#SETID", 6) == 0) //Set new ID
//	{
//		memset(new_id, 0, sizeof(new_id));
//		if(sscanf(buffer, "#SETID,%u,%[^.*].*",&password, new_id) == 2) //[^.*] => not include . and * (http://docs.roxen.com/(en)/pike/7.0/tutorial/strings/sscanf.xml)
//		{
//			if(serverCfg.Password == password)
//			{
//				if(strlen(new_id) == 15)
//				{
//					memcpy(serverCfg.ClientID, new_id, 15);
////					Write_Flash_New_Config();
//					thanhntMDM.smsSend(num_from, "[OK]");
//				}
//			}
//		}
//	}
//	else if(strncmp(buffer, "#SD_ON", 6) == 0) //Enable/Disable Log2SD
//	{
//		if(sscanf(buffer, "#SD_ON,%u,%d.*", &password, &on_off_sd_gprs) == 2)
//		{
//			if(serverCfg.Password == password)
//			{
//				serverCfg.Enable_Log2SD = on_off_sd_gprs ? 1 : 0;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//			}
//		}
//	}
//	else if(strncmp(buffer, "#GPRS_ON", 8) == 0) //Enable/Disable GPRS
//	{
//		if(sscanf(buffer, "#GPRS_ON,%u,%d.*", &password, &on_off_sd_gprs) == 2)
//		{
//			if(serverCfg.Password == password)
//			{
//				serverCfg.Enable_GPRS = on_off_sd_gprs ? 1 : 0;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//			}
//		}
//	}
//	else if(strncmp(buffer, "#TIMER", 6) == 0) //Set Timer
//	{
//		if(sscanf(buffer, "#TIMER,%u,%u,%u,%u.*", &password, &timer_gprs, &timer_log2sd, &timer_adc) == 4)
//		{
//			if(serverCfg.Password == password)
//			{
//				if((timer_gprs >= 30) && (timer_gprs < 3000000))			
//				timeDataCtl.Cycle_SendGPRS_Data = timer_gprs*1000;
//				if((timer_log2sd > 0) && (timer_log2sd < 3000000))			
//				timeDataCtl.Cycle_Log2SDCARD = timer_log2sd*1000;
//				if((timer_adc > 0) && (timer_adc < 3000000))			
//				timeDataCtl.Cycle_GetADC = timer_adc*1000;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//			}
//		}
//	}
//	else if(strncmp(buffer, "#RECOVERY_FACTORY", 17) == 0) //Recovery config to default
//	{
//		if(sscanf(buffer, "#RECOVERY_FACTORY,%u.*", &password) == 1)
//		{
//			if(serverCfg.Password == password)
//			{
//				Set_Default_System_Config();
//				serverCfg.Revision_Firmware -= 1;
////				Write_Flash_New_Config();
//				thanhntMDM.smsSend(num_from, "[OK]");
//				HAL_Core_System_Reset();
//			}
//		}
//	}
//	else if(strncmp(buffer, "#INFOR", 6) == 0) //Get system info
//	{
//		if(sscanf(buffer, "#INFOR,%u.*", &password) == 1)
//		{
//			if(serverCfg.Password == password)
//			{
//				memset(&buff_sms_send, 0, sizeof(buff_sms_send));
//				sprintf(buff_sms_send, "ID:%s\nUp:%u[s]\nTimer:%u/%u/%u\nServer:" IPSTR ":%d\nRSSI:%d[dBm]\nSD:%s - %s\nGPRS:%s",serverCfg.ClientID, systemInfo.SystemUptime, timeDataCtl.Cycle_SendGPRS_Data/1000, timeDataCtl.Cycle_Log2SDCARD/1000, timeDataCtl.Cycle_GetADC/1000, IPNUM(serverCfg.IP_Server), serverCfg.Port, systemInfo.GSM_RSSI, systemInfo.SDCard_State ? "YES" : "NO", serverCfg.Enable_Log2SD ? "EN" : "DIS", serverCfg.Enable_GPRS ? "EN" : "DIS");
//				thanhntMDM.smsSend(num_from, buff_sms_send);
//			}
//		}
//	}
//	else if(strncmp(buffer, "#TT_CAMBIEN", 11) == 0) //Get sensor info
//	{
//		if(sscanf(buffer, "#TT_CAMBIEN,%u.*", &password) == 1)
//		{
//			if(serverCfg.Password == password)
//			{
//				memset(&buff_sms_send, 0, sizeof(buff_sms_send));
//				sprintf(buff_sms_send, "This device doesn't support this command");
//				thanhntMDM.smsSend(num_from, buff_sms_send);
//			}
//		}
//	}
//	else if(strncmp(buffer, "#USSD", 5) == 0) //Run USSD command
//	{
//		memset(&buff_ussd, 0, sizeof(buff_ussd));
//		memset(&buff_sms_send, 0, sizeof(buff_sms_send));
//		memset(&buff_ussd_ret, 0, sizeof(buff_ussd_ret));
//		if(sscanf(buffer, "#USSD,%u,\"%[^\"]\".*", &password, buff_ussd) == 2)
//		{
//			if(serverCfg.Password == password)
//			{
//				ret_ussd = thanhntMDM.ussdCommand(buff_ussd, buff_ussd_ret);
//				if(ret_ussd > 0)
//				{
//					sprintf(buff_sms_send, "%*s", ret_ussd, buff_ussd_ret);
//					DEBUG_D("\nCommand: %s", buff_ussd);
//					DEBUG_D("\nUSSD: %s", buff_sms_send);
//					thanhntMDM.smsSend(num_from, buff_sms_send);
//				}
//			}
//		}
//	}
//}
void Process_SMS_From_Server(char *buffer, int len, char *num_from)
{
	uint32_t password, password_new;
	int ip1, ip2, ip3, ip4, port,on_off_sd_gprs, ret_ussd = 0;
	char new_id[15], buff_sms_send[20];
	char ad_num[12];
	uint32_t PIN;
	 if(strncmp(buffer,"begin",5) == 0) // begin run node
	{
		sscanf(buffer,"begin,%u#",&password);
		if(serverCfg.Password == password)
		{
			Set_Default_System_Config();
			serverCfg.Revision_Firmware -= 1;
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			 check = thanhntMDM.smsSend(num_from,"begin OK");
			}
			HAL_Core_System_Reset();
		}
	}
	else if(strncmp(buffer,"admin",5) == 0) // set admin number phone 
	{
		sscanf(buffer,"admin,%u,[%12s",&password,ad_num);
		if(serverCfg.Password == password)
		{
			memset(&serverCfg.admin_num, 0, sizeof(serverCfg.admin_num));
			strcpy(serverCfg.admin_num,ad_num);
			DEBUG_D(serverCfg.admin_num);
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,"admin OK");
			}
		}
	}
	else if(strncmp(buffer,"password",8) == 0) // change password via to admin number phone and PIN
	{
		DEBUG_D(num_from);
		DEBUG_D(serverCfg.admin_num);
		sscanf(buffer,"password,%u,%u,%u#",&password,&password_new,&PIN);
		if(serverCfg.Password == password && serverCfg.PIN == PIN && strncmp(num_from,serverCfg.admin_num,12)==0)
		{
			serverCfg.Password = password_new;
			DEBUG_D("%d",serverCfg.Password);
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,"new password ok");
			}
		}
	}
	else if(strncmp(buffer,"location",8) == 0) // get location
	{
\
		sscanf(buffer,"location,%u#",&password);
		if(serverCfg.Password == password)
		{

			memset(&buff_sms_send, 0, sizeof(buff_sms_send));
		
		  bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,buff_sms_send);
			}
		}
	}
	else if(strncmp(buffer,"inactivegps",11) == 0)
	{
		sscanf(buffer,"inactivegps,%u#",&password);
		if(serverCfg.Password == password)
		{
			
		}
	}
	else if(strncmp(buffer,"reset",5) == 0) // reset 
	{
		sscanf(buffer,"reset,%u#",&password);
		if(serverCfg.Password == password)
		{
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,"reset OK");
			}
			HAL_Core_System_Reset();
		}
	}
	else if(strncmp(buffer,"imei",4) == 0) // get IMEI of the SIM
	{
		sscanf(buffer,"imei,%u#",&password);
		if(serverCfg.Password == password)  
		{
			DevStatus dev;
			thanhntMDM.getDevStatus(&dev);
			memset(&buff_sms_send, 0, sizeof(buff_sms_send));
		  sprintf(buff_sms_send,"[%s]", dev.imei);
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,buff_sms_send);
			}
			DEBUG_D(buff_sms_send);
		}
	}
	else if(strncmp(buffer,"wserver",7)==0) // get server information (PORT, IP)
	{
		sscanf(buffer,"wserver,%u#",&password);
		if(serverCfg.Password == password)
		{
			memset(&buff_sms_send, 0, sizeof(buff_sms_send));
			sprintf(buff_sms_send,IPSTR ",%d",IPNUM(serverCfg.IP_Server),serverCfg.Port);
		  bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
				check = thanhntMDM.smsSend(num_from,buff_sms_send);
			}
			DEBUG_D(buff_sms_send);
		}
	}
	else if(strncmp(buffer,"wid",3) == 0) // get Client ID
	{
		sscanf(buffer,"wid,%u#",&password);
		if(serverCfg.Password == password)
		{
			memset(&buff_sms_send, 0, sizeof(buff_sms_send));
			sprintf(buff_sms_send,"[%s]",serverCfg.ClientID);
			bool check = false;
			int cnt = 0;
			while(check == false && cnt < 3)
			{
			  check = thanhntMDM.smsSend(num_from,buff_sms_send);
			}
			DEBUG_D(buff_sms_send);
		}
	}
	else if(strncmp(buffer,"server",6) == 0) // change server with 2 option domain name or IP
	{
		MDM_IP IP;
	  int choice;
		char domain[20];
		sscanf(buffer,"server,%d",&choice);
		if(choice == 1) // change server via domain
		{
			if(sscanf(buffer,"server,%d,%s ,%d,%d#",&choice,domain,&port,&password)==4)
			{
			 if(serverCfg.Password == password)
			 {
			   IP = thanhntMDM.gethostbyname(domain);
			   serverCfg.IP_Server=IP;
				 serverCfg.Port = port;
				 systemInfo.Flag_Change_Server = 1;
				 bool check = false;
			   int cnt = 0;
				 while(check == false && cnt < 3)
				 {
				   check = thanhntMDM.smsSend(num_from, "server ok!");
				 }
			 }
		 }
		}
		if(choice == 0) //change server via IP
		{
			if(sscanf(buffer, "server,%d,%d.%d.%d.%d,%d,%d#",&choice, &ip1, &ip2, &ip3, &ip4, &port, &password) == 7)
			{
				if(serverCfg.Password == password)
			  {
				  serverCfg.IP_Server = (((uint32_t)(ip1))<<24) | (((uint32_t)(ip2))<<16) | (((uint32_t)(ip3))<< 8) | (((uint32_t)(ip4))<< 0);
				  serverCfg.Port = port;
				  systemInfo.Flag_Change_Server = 1;
					bool check = false;
			    int cnt = 0;
					while(check == false && cnt < 3)
					{
					  check = thanhntMDM.smsSend(num_from, "server ok!");
					}
			  }
		  }
		}
		DEBUG_D(IPSTR ",%d",IPNUM(serverCfg.IP_Server),serverCfg.Port);
	}
	else if(strncmp(buffer,"id",2) == 0) // change ClientID
	{
		memset(new_id, 0, sizeof(new_id));
		sscanf(buffer,"id,%u,%[^#]",&password,new_id);
		if(serverCfg.Password == password)
			{
				if(strlen(new_id) == 15)
				{
					memcpy(serverCfg.ClientID, new_id, 15);
					bool check = false;
			    int cnt = 0;
					 while(check == false && cnt < 3)
					{
					  check = thanhntMDM.smsSend(num_from,"id ok!");
					}
					DEBUG_D((char*)serverCfg.ClientID);
				}
			}
	}
//	else if(strncmp(buffer,"airplane",8) == 0)
//	{
//		uint32_t time;
//		sscanf(buffer,"airplane,%u,%u#",&time,&password);
//		if(serverCfg.Password == password)
//		{
//			thanhntMDM.powerOff();
//			HAL_Delay(time);
//			thanhntMDM.connect();
//		}
//	}
	
}
char* replaceWord(const char* s, const char* oldW,
                  const char* newW)
{
    char* result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
  
    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;
  
            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
  
    // Making new string of enough length
    result = (char*) pvPortMalloc(i + cnt * (newWlen - oldWlen) + 1);
  
    i = 0;
    while (*s) {
        // compare the substring with the result
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
  
    result[i] = '\0';
    return result;
}