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

uint8_t TCS3200_Enable(GPIO_PinState state)
{
	HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, state); // To enable the sensor
	return state;
}

uint8_t TCS3200_LED_Enable(GPIO_PinState led_state)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_state); // To enable the led
	return led_state;
}

TCS3200_fo_t TCS3200_Set_Scale(TCS3200_fo_t fo_scale)
{
	switch(fo_scale)
	{
	case SCALE_FO_2:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_SET);
		break;
	case SCALE_FO_20:
		HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
		break;
	case SCALE_FO_100:
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

TCS3200_filter_t TCS3200_Set_Filter(TCS3200_filter_t filter)
{
	switch(filter)
	{
	case FILTER_RED:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
		break;
	case FILTER_GREEN:
		HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_SET);
		break;
	case FILTER_BLUE:
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
	tcs->state=TCS3200_Enable(GPIO_PIN_RESET); // To enable the sensor (low active)
	HAL_Delay(100); // Not necessary
	tcs->filter=TCS3200_Set_Filter(FILTER_CLEAR);
	tcs->fo_scale=TCS3200_Set_Scale(SCALE_FO_100);
	tcs->led_state=TCS3200_LED_Enable(GPIO_PIN_RESET);
	tcs->red=0;
	tcs->green=0;
	tcs->blue=0;
	tcs->freq=0;
	tcs->freq_available=0;

}

uint8_t TCS3200_Calibration(TCS3200_filter_t filter,uint32_t freq)
{
	uint8_t filter_output;
	switch (filter){
	case FILTER_RED:
		filter_output = (255.0/(MAX_RED-MIN_RED))*(freq-MIN_RED);
		break;
	case FILTER_GREEN:
		filter_output = (255.0/(MAX_GREEN-MIN_GREEN))*(freq-MIN_GREEN);
		break;
	case FILTER_BLUE:
		filter_output = (255.0/(MAX_RED-MIN_BLUE))*(freq-MIN_BLUE);
		break;
	}
	if (filter_output > 255) filter_output = 255;
	if (filter_output < 0) filter_output = 0;
	return filter_output;
}

void TCS3200_Display_Frequency(TCS3200_filter_t filter,uint32_t freq)
{
	switch (filter){
	case FILTER_RED:
		sprintf(display_msg, "RED = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case FILTER_GREEN:
		sprintf(display_msg, "GREEN = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case FILTER_BLUE:
		sprintf(display_msg, "BLUE = %lu Hz\r\n",freq);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	}
}

void TCS3200_Display_Color(TCS3200_filter_t filter,uint8_t filter_output)
{
	switch (filter){
	case FILTER_RED:
		sprintf(display_msg, "RED = %u \r\n",filter_output);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case FILTER_GREEN:
		sprintf(display_msg, "GREEN = %u \r\n",filter_output);
		HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
		break;
	case FILTER_BLUE:
		sprintf(display_msg, "BLUE = %u \r\n",filter_output);
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
		sprintf(display_msg, "RED\r\n");
	}
	else if ((tcs.green>tcs.red)&(tcs.green>tcs.blue))
	{
		sprintf(display_msg, "GREEN\r\n");
	}
	else if ((tcs.blue>tcs.green)&(tcs.blue>tcs.red))
	{
		sprintf(display_msg, "BLUE\r\n");
	}
	else
	{
		sprintf(display_msg, "No color\r\n");
	}
	HAL_UART_Transmit(&huart1,display_msg,strlen(display_msg), HAL_MAX_DELAY);
}

uint8_t TCS3200_Read_Color(TCS3200 *tcs,TCS3200_filter_t filter)
{
	uint8_t filter_output;
	TCS3200_filter_t clear;

	HAL_TIM_Base_Start_IT(&htim10);
	HAL_TIM_IC_Start_IT(&htim10, TIM_CHANNEL_1);


	tcs->led_state=TCS3200_LED_Enable(GPIO_PIN_SET);
	tcs->filter=TCS3200_Set_Filter(filter);
	HAL_Delay(100);
	tcs->freq_available=1; //So that the input capture timer can start calculating the frequency
	while(tcs->freq_available); // Wait until the frequency is calculated
	tcs->led_state=TCS3200_LED_Enable(GPIO_PIN_RESET);
	clear=TCS3200_Set_Filter(FILTER_CLEAR);

	HAL_TIM_IC_Stop_IT(&htim10, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim10);
	filter_output=TCS3200_Calibration(tcs->filter,tcs->freq);
	//TCS3200_Display_Frequency(tcs->filter,tcs->freq);
	return filter_output;
}
