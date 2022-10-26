/*
 * TCS3200.c
 *
 *  Created on: Oct 21, 2022
 *      Author: cheik
 */


#include "TCS3200.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim10;
extern UART_HandleTypeDef huart1;
char display_msg[100];
extern uint32_t freq;
extern uint8_t check_freq;

uint8_t TCS3200_Enable(uint8_t state)
{
	if (state)
	{
		HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET); // To enable the sensor
	}
	else
	{
		HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_SET); // To disable the sensor
	}
	return state;
}

uint8_t TCS3200_LED_Enable(uint8_t led_state)
{
	if (led_state)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // To enable the led
	}
	else
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // To disable the led
	}
	return led_state;
}

uint8_t TCS3200_Set_Scale(uint8_t fo_scale)
{
	switch(fo_scale)
	{
	case TCS3200_fo2:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_SET);
		break;
	case TCS3200_fo20:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
		break;
	case TCS3200_fo100:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_SET);
		break;
	default:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
		break;
	}
	return fo_scale;
}

uint8_t TCS3200_Set_Filter(uint8_t filter)
{
	switch(filter)
	{
	case TCS3200_RED:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
		break;
	case TCS3200_GREEN:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_SET);
		break;
	case TCS3200_BLUE:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_SET);
		break;
	default:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
		break;
	}
	return filter;
}

void TCS3200_Init(TCS3200 *tcs)
{
	tcs->state=TCS3200_Enable(TCS3200_ENABLE); // To enable the sensor
	HAL_Delay(100); // Not necessary
	tcs->filter=TCS3200_Set_Filter(TCS3200_CLEAR);
	tcs->fo_scale=TCS3200_Set_Scale(TCS3200_fo100);
	tcs->led_state=TCS3200_LED_Enable(TCS3200_DISABLE);
	tcs->red=0;
	tcs->green=0;
	tcs->blue=0;
	tcs->freq=0;
	tcs->check_freq=0;
}

uint8_t TCS3200_Calibration(uint8_t color,uint32_t freq)
{
	uint8_t Output_Color;
	switch (color){
	case TCS3200_RED:
		Output_Color = (255.0/(MAX_RED-MIN_RED))*(freq-MIN_RED);
		break;
	case TCS3200_GREEN:
		Output_Color = (255.0/(MAX_GREEN-MIN_GREEN))*(freq-MIN_GREEN);
		break;
	case TCS3200_BLUE:
		Output_Color = (255.0/(MAX_RED-MIN_BLUE))*(freq-MIN_BLUE);
		break;
	}
	if (Output_Color > 255) Output_Color = 255;
	if (Output_Color < 0) Output_Color = 0;
	return Output_Color;
}

void TCS3200_Display_Frequency(uint8_t color,uint32_t freq)
{
	switch (color){
	case TCS3200_RED:
		sprintf(display_msg, "RED = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case TCS3200_GREEN:
		sprintf(display_msg, "GREEN = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case TCS3200_BLUE:
		sprintf(display_msg, "BLUE = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	}
}

void TCS3200_Display_Color(uint8_t color,uint8_t Output_Color)
{
	switch (color){
	case TCS3200_RED:
		sprintf(display_msg, "RED = %u \r\n",Output_Color);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case TCS3200_GREEN:
		sprintf(display_msg, "GREEN = %u \r\n",Output_Color);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case TCS3200_BLUE:
		sprintf(display_msg, "BLUE = %u \r\n",Output_Color);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	}
}

void TCS3200_Display_Colors(TCS3200 tcs)
{
	sprintf(display_msg, "(%d,%d,%d)\r\n",tcs.red,tcs.green,tcs.blue);
	HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
}

void TCS3200_Detected_Color(TCS3200 tcs)
{
	if ((tcs.red>tcs.green)&(tcs.red>tcs.blue))
	{
		sprintf(display_msg, "RED\r\n",tcs.red,tcs.green,tcs.blue);
	}
	else if ((tcs.green>tcs.red)&(tcs.green>tcs.blue))
	{
		sprintf(display_msg, "GREEN\r\n",tcs.red,tcs.green,tcs.blue);
	}
	else if ((tcs.blue>tcs.green)&(tcs.blue>tcs.red))
	{
		sprintf(display_msg, "BLUE\r\n",tcs.red,tcs.green,tcs.blue);
	}
	else
	{
		sprintf(display_msg, "No color\r\n",tcs.red,tcs.green,tcs.blue);
	}
	HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
}

uint8_t TCS3200_Read_Color(TCS3200 *tcs,uint8_t color)
{
	uint8_t Output_Color;
	uint8_t clear;
	HAL_TIM_Base_Start_IT(&htim10);
	HAL_TIM_IC_Start_IT(&htim10, TIM_CHANNEL_1);
	tcs->led_state=TCS3200_LED_Enable(TCS3200_ENABLE);
	tcs->filter=TCS3200_Set_Filter(color);
	HAL_Delay(100);
	tcs->check_freq=1; //So that the input capture timer can start calculating the frequency
	while(tcs->check_freq); // Wait until the frequency is calculated
	tcs->led_state=TCS3200_LED_Enable(TCS3200_DISABLE);
	clear=TCS3200_Set_Filter(TCS3200_CLEAR);
	HAL_TIM_IC_Stop_IT(&htim10, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim10);
	Output_Color=TCS3200_Calibration(tcs->filter,tcs->freq);
	//TCS3200_Display_Frequency(tcs->filter,tcs->freq);
	return Output_Color;
}
