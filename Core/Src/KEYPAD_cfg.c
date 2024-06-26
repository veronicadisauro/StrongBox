/**
  ******************************************************************************
  * @file    		KEYPAD_cfg.c
  * @brief   		This file contains all the function declared on
  *          		the KEYPAD_cfg.h file
  * @driver name 	[[ 4x4 KEYPAD ]]
  ******************************************************************************
  */

#include "KEYPAD.h"

const KEYPAD_CfgType KEYPAD_CfgParam[KEYPAD_UNITS] =
{
	// KeyPAD Unit 1 Configurations
    {
    	/* ROWs Pins Info */
	    {GPIOB, GPIOB, GPIOB, GPIOB},
		{GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_4},
		/* COLs Pins */
		{GPIOB, GPIOB, GPIOB, GPIOB},
		{GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8}
	}
};
