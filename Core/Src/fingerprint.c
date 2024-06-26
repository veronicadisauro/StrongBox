/**
  ******************************************************************************
  * @file    fingerprint.c
  * @brief   This file contains all the function declared on
  *          the fingerprint.h file
  ******************************************************************************
  */

#include "fingerprint.h"


uint8_t FPHeader[6]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF};
uint8_t FPGetImage[6]={0x01,0x00,0x03,0x01,0x00,0x05};
uint8_t FPCreateCharFile1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08};
uint8_t FPCreateCharFile2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09};
uint8_t FPCreateTemplate[6]={0x01,0x00,0x03,0x05,0x00,0x09};
uint8_t FPDeleteAllFinger[6]={0x01,0x00,0x03,0x0D,0x00,0x11};
uint8_t FPSearchFinger[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x40,0x00,0x4E};
uint8_t FPGetNumberOfFinger[6]={0x01,0x00,0x03,0x1D,0x00,0x21};

extern uint8_t IDFromFinger;
extern uint8_t CurrentNumberFinger;

void SendFPHeader()
{
	HAL_UART_Transmit(&huart5,FPHeader,6,1000);
}

void SendFPGetImage()
{
	 HAL_UART_Transmit(&huart5,FPGetImage,6,1000);
}

void SendFPCreateCharFile1()
{
	HAL_UART_Transmit(&huart5,FPCreateCharFile1,7,1000);
}

void SendFPCreateCharFile2()
{
	HAL_UART_Transmit(&huart5,FPCreateCharFile2,7,1000);
}

void SendFPCreateTemplate()
{
	HAL_UART_Transmit(&huart5,FPCreateTemplate,6,1000);
}

void SendFPDeleteAllFinger()
{
	HAL_UART_Transmit(&huart5,FPDeleteAllFinger,6,1000);
}

void SendFPDSearchFinger()
{
	HAL_UART_Transmit(&huart5,FPSearchFinger,11,1000);
}

void SendFGetNumberOfFinger()
{
	HAL_UART_Transmit(&huart5,FPGetNumberOfFinger,6,1000);
}

//funzione che memorizza un'impronta alla posizione IDstore
void SendStoreFinger(uint16_t IDStore)
{
	uint16_t Sum=0;
	uint8_t DataSend[9]={0};

	DataSend[0]=0x01;
	Sum=Sum+DataSend[0];
	DataSend[1]=0x00;
	Sum=Sum+DataSend[1];
	DataSend[2]=0x06;
	Sum=Sum+DataSend[2];
	DataSend[3]=0x06;
	Sum=Sum+DataSend[3];
	DataSend[4]=0x01;
	Sum=Sum+DataSend[4];
	DataSend[5]= (uint8_t)(IDStore>> 8);
	Sum=Sum+DataSend[5];
	DataSend[6]=(uint8_t) (IDStore&0xFF);
	Sum=Sum+DataSend[6];
  DataSend[7]=(uint8_t)(Sum>> 8);
	DataSend[8]=(uint8_t)(Sum&0xFF);
	HAL_UART_Transmit(&huart5,DataSend,9,1000);
}

//funzione che elimina l'impronta alla posizione IDStore
void SendDeleteFinger(uint16_t IDDelete)
{
	uint16_t Sum=0;
	uint8_t DataSend[10]={0};

	DataSend[0]=0x01;
	Sum=Sum+DataSend[0];
	DataSend[1]=0x00;
	Sum=Sum+DataSend[1];
	DataSend[2]=0x07;
	Sum=Sum+DataSend[2];
	DataSend[3]=0x0C;
	Sum=Sum+DataSend[3];
	DataSend[4]=(uint8_t)(IDDelete>> 8);
	Sum=Sum+DataSend[4];
	DataSend[5]= (uint8_t) (IDDelete&0xFF);
	Sum=Sum+DataSend[5];
	DataSend[6]=0x00;
	Sum=Sum+DataSend[6];
	DataSend[7]=0x001;
	Sum=Sum+DataSend[7];
  DataSend[8]=(uint8_t)(Sum>> 8);
	DataSend[9]=(uint8_t)(Sum&0xFF);
	HAL_UART_Transmit(&huart5,DataSend,10,1000);
}



//Legge la risposta dalla UART e la analizza
uint8_t CheckFPRespsone(uint8_t MaxRead)
{
	uint8_t ByteCount=0;
	uint8_t FPRXData[20]={0xFF};
	uint8_t UARTData[1]={0};
	uint32_t TimeOut = HAL_GetTick();
	uint8_t Result;
	IDFromFinger=0xFF;
	while((HAL_GetTick() - TimeOut < 1000) && ByteCount<MaxRead) // time out is 1000 ms
	{
	    if(HAL_UART_Receive(&huart5, (uint8_t *)UARTData, 1, 1000) == HAL_OK)
			{
				FPRXData[ByteCount] = UARTData[0];
				ByteCount++;
			}
	}

	if(ByteCount==0)
	{
		//FPRXData[0]=0xEE;
		//FPRXData[1]=0xEE;
		//HAL_UART_Transmit(&huart2,FPRXData,2,1000);
		Result=FP_ERROR;
		return Result;
	}
	else if(ByteCount<MaxRead)
	{
		Result=FP_ERROR;
		return Result;
	}
	  else // vail data return
	{

		 Result=FPRXData[9];
		 IDFromFinger=FPRXData[11];
	   //HAL_UART_Transmit(&huart2,FPRXData,MaxRead,1000);
		 return Result;

	}
}
//ritorna il numero di impronte memorizzate
uint8_t GetNumberOfFinger()
{
	uint8_t Result;
	SendFPHeader();
	SendFGetNumberOfFinger();
	Result=CheckFPRespsone(14);
	if(Result!=FP_OK) return 0xFF;

	return IDFromFinger;
}




//registra una nuova impronta e la salva nella locazione indicata da ID
uint8_t RegistryNewFinger(uint16_t LocationID)
{

	uint8_t Result=FP_NOFINGER;
	uint32_t TimeOut = HAL_GetTick();

	while(Result==FP_NOFINGER&&(HAL_GetTick() - TimeOut < 5000)) // time out is 5000 ms
	{

		SendFPHeader();
		SendFPGetImage();
		Result=CheckFPRespsone(12);
	}
	if(Result!=FP_OK) return FP_ERROR;
	// continue if detect finger;
	SendFPHeader();
	SendFPCreateCharFile1();
	Result=CheckFPRespsone(12);
	if(Result!=FP_OK) return FP_ERROR;

	HAL_Delay(2000);
	Result=FP_NOFINGER;
	TimeOut = HAL_GetTick();

	while(Result==FP_NOFINGER&&(HAL_GetTick() - TimeOut < 5000)) // time out is 5000 ms
	{

		SendFPHeader();
		SendFPGetImage();
		Result=CheckFPRespsone(12);
	}
	if(Result!=FP_OK) return FP_ERROR;

	// continue if detect finger;
	SendFPHeader();
	SendFPCreateCharFile2();
	Result=CheckFPRespsone(12);
	if(Result!=FP_OK) return FP_ERROR;

	// Compare finger, create template
	SendFPHeader();
	SendFPCreateTemplate();
	Result=CheckFPRespsone(12);
	if(Result==FP_FINGER_NOTMATCH)
	{

		return FP_FINGER_NOTMATCH;
	}
	else if(Result!=FP_OK) return FP_ERROR;

	// save finger
	SendFPHeader();
	SendStoreFinger(LocationID);
	Result = CheckFPRespsone(12);
	if(Result != FP_OK) return FP_ERROR;
	else
	{
		return FP_OK;
	}

}

//verifica se un'impronta è presente o meno nel database
uint8_t CheckFinger()
{
	uint8_t Result=FP_NOFINGER;
	uint32_t TimeOut = HAL_GetTick();

	while(Result==FP_NOFINGER&&(HAL_GetTick() - TimeOut <500)) // time out is 500 ms and no button press
	{

		SendFPHeader();
		SendFPGetImage();
		Result=CheckFPRespsone(12);
	}
	if(Result==FP_NOFINGER)
	{

		return FP_NOFINGER;
	}
	if(Result!=FP_OK) return FP_ERROR;
	// continue if detect finger;
	SendFPHeader();
	SendFPCreateCharFile1();
	Result=CheckFPRespsone(12);
	if(Result!=FP_OK) return FP_ERROR;

	// Search Finger
	SendFPHeader();
	SendFPDSearchFinger();
	Result=CheckFPRespsone(16);
	return Result;


}

//mostra il risultato ottenuto dalla registrazione dell'impronta su LCD
uint8_t ProcessRegistryNewFinger()
{
	uint8_t FingerResult;

	HAL_Delay(100);

	FingerResult=RegistryNewFinger(CurrentNumberFinger+1);

	if(FingerResult==FP_OK)
	{

		//impronta salvata LUCE BLU
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);

		CurrentNumberFinger++;
		if(CurrentNumberFinger==100) CurrentNumberFinger=1;
		HAL_Delay(150);
	}
	else if(FingerResult==FP_FINGER_NOTMATCH)
	{
		//FINGER NOT MATCH LUCE ROSSO
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);


	}
	else
	{
		//ERRORE NEL SALVATAGGIO LUCE ARANCIONE
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 1);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 0);


	}
	return FingerResult;

}

//elimina tutte le impronte memorizzate
void DeleteAllFinger()
{

	uint8_t FingerResult;
	SendFPHeader();
	SendFPDeleteAllFinger();
	FingerResult=CheckFPRespsone(12);
	if(FingerResult==FP_OK)
	{
		//TUTTE LE IMPRONTE SONO STATE CANCELLATE
		CurrentNumberFinger=0;
	}
	else
	{
		HAL_Delay(1500);
	}

}

//genera un ritardo
void Delay_us(uint32_t TimeDelay)
{
	uint32_t TimeCount;
	for(TimeCount=0;TimeCount<TimeDelay;TimeCount++)
	{
		__ASM volatile ("NOP");
		__ASM volatile ("NOP");
		__ASM volatile ("NOP");
	}
}



