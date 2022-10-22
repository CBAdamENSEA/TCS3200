/*
 * TCS3200.h
 *
 *  Created on: Oct 21, 2022
 *      Author: cheik
 */

#include <stdio.h>

#ifndef INC_TCS3200_H_
#define INC_TCS3200_H_

////Private Defines

// State
#define TCS3200_ENABLE 1
#define TCS3200_DISABLE 0
// Filters
#define TCS3200_RED 0
#define TCS3200_BLUE 1
#define TCS3200_CLEAR 2
#define TCS3200_GREEN 3
// f_o scales
#define TCS3200_POWER_DOWN 0
#define TCS3200_fo2 1
#define TCS3200_fo20 2
#define TCS3200_fo100 3
// Calibration
#define MIN_RED 11730.0
#define MAX_RED 130000.0
#define MIN_GREEN 10668.0
#define MAX_GREEN 160000.0
#define MIN_BLUE 14492.0
#define MAX_BLUE 160000.0

//// Typedef
typedef struct TCS3200{
  uint8_t state;
  uint8_t fo_scale;
  uint8_t filter;
  uint8_t led_state;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  volatile uint32_t freq;
  volatile uint8_t check_freq;
} TCS3200;


//// Functions

uint8_t TCS3200_Enable(uint8_t state);
uint8_t TCS3200_LED_Enable(uint8_t led_state);
uint8_t TCS3200_Set_Scale(uint8_t fo_scale);
uint8_t TCS3200_Set_Filter(uint8_t filter);
void TCS3200_Init(TCS3200 *tcs);
uint8_t TCS3200_Calibration(uint8_t color,uint32_t freq);
void TCS3200_Display_Frequency(uint8_t color,uint32_t freq);
void TCS3200_Display_Color(uint8_t color,uint8_t Output_Color);
void TCS3200_Display_Colors(TCS3200 tcs);
uint8_t TCS3200_Read_Color(TCS3200 *tcs,uint8_t color);


#endif /* INC_TCS3200_H_ */
