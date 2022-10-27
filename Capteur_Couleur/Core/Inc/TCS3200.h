/*
 * TCS3200.h
 *
 *  Created on: Oct 21, 2022
 *      Author: cheik
 */
#include "main.h"
#include <stdio.h>

#ifndef INC_TCS3200_H_
#define INC_TCS3200_H_

////Private Defines


// Filters
typedef enum TCS3200_filter_enum
{
	FILTER_RED       = 0,
	FILTER_BLUE      = 1,
	FILTER_CLEAR  	= 2,
	FILTER_GREEN 	= 3
} TCS3200_filter_t;

// f_o scales
typedef enum TCS3200_fo_enum
{
	SCALE_FO_0     = 0,
	SCALE_FO_2      = 1,
	SCALE_FO_20  	= 2,
	SCALE_FO_100 	= 3
} TCS3200_fo_t;

// Calibration
#define MIN_RED 11971.0
#define MAX_RED 36137.0
#define MIN_GREEN 11693.0
#define MAX_GREEN 38882.0
#define MIN_BLUE 16297.0
#define MAX_BLUE 54935.0

//// Typedef
typedef struct TCS3200{
	GPIO_PinState state;
	GPIO_PinState led_state;
	TCS3200_fo_t fo_scale;
	TCS3200_filter_t filter;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	volatile uint32_t freq;
	volatile uint8_t freq_available;
} TCS3200;


//// Functions

uint8_t TCS3200_Enable(GPIO_PinState state);
uint8_t TCS3200_LED_Enable(GPIO_PinState led_state);
uint8_t TCS3200_Set_Scale(TCS3200_fo_t fo_scale);
uint8_t TCS3200_Set_Filter(TCS3200_filter_t filter);
void TCS3200_Init(TCS3200 *tcs);
uint8_t TCS3200_Calibration(TCS3200_filter_t filter,uint32_t freq);
void TCS3200_Display_Frequency(TCS3200_filter_t filter,uint32_t freq);
void TCS3200_Display_Color(TCS3200_filter_t filter,uint8_t Output_Color);
void TCS3200_Display_Colors(TCS3200 tcs);
void TCS3200_Detected_Color(TCS3200 tcs);
uint8_t TCS3200_Read_Color(TCS3200 *tcs,TCS3200_filter_t filter);


#endif /* INC_TCS3200_H_ */
