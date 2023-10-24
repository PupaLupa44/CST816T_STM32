#ifndef _CST816T_H_
#define _CST816T_H_

#include "main.h"
#include <stdint.h>
#include "stdbool.h"
#include "stm32f1xx_hal.h"

/*
*Идентифікатор мікросхеми
*/
#define CHIPID_CST716 0x20
#define CHIPID_CST816S 0xB4
#define CHIPID_CST816T 0xB5
#define CHIPID_CST816D 0xB6


/*
*
*Основні Регістри
*/
#define GestureID	0x01
#define FingerNum	0x02
#define XposH	0x03
#define XposL	0x04
#define YposH	0x05
#define YposL	0x06
#define ChipID	0xA7
#define FwVersion	0xA9
#define MotionMask	0xEC
#define AutoSleepTime	0xF9
#define IrqCrl	0xFA
#define AutoReset	0xFB
#define LongPressTime	0xFC
#define DisAutoSleep	0xFE
#define IrqPulseWidth	0xED



#define GESTURE_NONE 0x00
#define GESTURE_SWIPE_UP 0x01
#define GESTURE_SWIPE_DOWN 0x02
#define GESTURE_SWIPE_LEFT 0x03
#define GESTURE_SWIPE_RIGHT 0x04
#define GESTURE_SINGLE_CLICK 0x05
#define GESTURE_DOUBLE_CLICK 0x0B
#define GESTURE_LONG_PRESS 0x0C



#define IRQ_EN_TOUCH 0x40
#define IRQ_EN_CHANGE 0x20
#define IRQ_EN_MOTION 0x10
#define IRQ_EN_LONGPRESS 0x01

#define MOTION_MASK_CONTINUOUS_LEFT_RIGHT 0x04
#define MOTION_MASK_CONTINUOUS_UP_DOWN 0x02
#define MOTION_MASK_DOUBLE_CLICK 0x01





typedef struct
{
	unsigned char chipID;
	unsigned char Sta;
	uint8_t Gesture_ID;
	uint8_t Finger_NUM;
}CST816_Info;

typedef struct{
	
	uint16_t X_Pos;
	uint16_t Y_Pos;
	
}touch_coordinates;



typedef enum 
{
	mode_touch,
  mode_change,
  mode_fast,
  mode_motion 
}touchpad_mode;



#define CST816_RESET_H() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET)
#define CST816_RESET_L() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET)

#define SCL_CLR() HAL_GPIO_WritePin(SOFT_SCL_GPIO_Port, SOFT_SCL_Pin, GPIO_PIN_RESET)
#define SCL_SET() HAL_GPIO_WritePin(SOFT_SCL_GPIO_Port, SOFT_SCL_Pin, GPIO_PIN_SET)

#define SDA_IN() GPIO_InitStruct.Pin = SOFT_SDA_Pin; GPIO_InitStruct.Mode = GPIO_MODE_INPUT; GPIO_InitStruct.Pull = GPIO_PULLUP; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(SOFT_SDA_GPIO_Port,&GPIO_InitStruct)
#define SDA_GET() HAL_GPIO_ReadPin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin)

#define SDA_OUT() GPIO_InitStruct.Pin = SOFT_SDA_Pin; GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_PULLUP; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(SOFT_SDA_GPIO_Port,&GPIO_InitStruct)
#define SDA_CLR() HAL_GPIO_WritePin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin, GPIO_PIN_RESET)
#define SDA_SET() HAL_GPIO_WritePin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin, GPIO_PIN_SET)


void cst816_Write_Reg(unsigned char reg,unsigned char date);
unsigned char cst816_ReadReg(unsigned char reg);
void Pulse_Widht(uint8_t time);

void cst816_Reset(void);
void cst816_Disable_Auto_Sleep(bool state);
uint8_t Get_Chip_ID(void);
bool cst816_Init(touchpad_mode tp_mode);//ініциалізація тачскрина
void cst816_Disable_Auto_Sleep(bool state);//відключає автоматичне засинання
void cst816_Set_Sleep_Time(uint8_t sec);//задає час засинання сенсору(5sec. max)
touch_coordinates cst816_Get_XY(void);//отримати координати натискання(x/y)
void cst816_Clear_pos_and_ges(void);//скидання координат та жестів
unsigned char cst816_Get_Status_Gesture(void);//отримання ID жесту	


#endif