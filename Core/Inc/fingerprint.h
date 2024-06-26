/**
  ******************************************************************************
  * @file    fingerprint.h
  * @brief   This file contains all the function prototypes for
  *          the fingerprint.c file
  ******************************************************************************
  */

#ifndef INC_FINGERPRINT_H_
#define INC_FINGERPRINT_H_

#include "main.h"

extern UART_HandleTypeDef huart5;

#define FP_OK 0x00
#define FP_ERROR 0xFE
#define FP_NOFINGER 0x02
#define FP_FINGER_NOTMATCH 0x0A
#define FP_FINGER_NOTMATCH 0x0A
#define FP_FINGER_NOTFOUND 0x09

void SendFPHeader();

void SendFPGetImage();

void SendFPCreateCharFile1();

void SendFPCreateCharFile2();

void SendFPCreateTemplate();

void SendFPDeleteAllFinger();

void SendFPDSearchFinger();

void SendFGetNumberOfFinger();

void SendStoreFinger(uint16_t IDStore);

void SendDeleteFinger(uint16_t IDDelete);

uint8_t CheckFPRespsone(uint8_t MaxRead);

uint8_t GetNumberOfFinger();

uint8_t RegistryNewFinger(uint16_t LocationID);

uint8_t CheckFinger();

uint8_t ProcessRegistryNewFinger();

void DeleteAllFinger();

void Delay_us(uint32_t TimeDelay);

#endif 	/* INC_FINGERPRINT_H_ */
