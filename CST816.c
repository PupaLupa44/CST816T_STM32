#include "CST816.h"
#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"


CST816_Info CST816_Instance;

touchpad_mode tp_mode;

uint8_t State=0;//ідентифікатор жесту


GPIO_InitTypeDef GPIO_InitStruct = {0};

__STATIC_INLINE void Delay_us (uint32_t __IO us) //Функція затримки в микросекундах
{
us *=(SystemCoreClock/1000000)/5;
while(us--);
}



void cst816_Start(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	SDA_OUT();
	
	SDA_SET();
	SCL_SET();
	Delay_us(5);
	SDA_CLR();
	Delay_us(5);
	SCL_CLR();
	
}


void cst816_Stop(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	SDA_OUT();
	
	SCL_CLR();
	SDA_CLR();
	Delay_us(5);
	SCL_SET();
	SDA_SET();
	Delay_us(5);
}


void cst816_Ack(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	SDA_OUT();
	
	
	SCL_CLR();
	SDA_CLR();
	Delay_us(5);
	SCL_SET();
	Delay_us(5);
	SCL_CLR();	
}

void cst816_NAck(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	SDA_OUT();
	
	
	SCL_CLR();
	SDA_SET();
	Delay_us(5);
	SCL_SET();
	Delay_us(5);
	SCL_CLR();
	
}



unsigned char cst816_WaitAck(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char t = 0;
	
	SDA_IN();
	SDA_SET();
	Delay_us(5);
	SCL_SET();
	Delay_us(5);
	
	while(SDA_GET())
	{
		t++;
		if(t>250)
		{
			cst816_Stop();
			return 1;
		}
	}
	SCL_CLR();
	return 0;
}



void cst816_SendByte(unsigned char byte)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char BitCnt;
	
	SDA_OUT();
	SCL_CLR();
	for(BitCnt = 0; BitCnt < 8; BitCnt++)
	{
		if(byte & 0x80) SDA_SET();
		else SDA_CLR();
		byte <<= 1;
		Delay_us(5);
		SCL_SET();
		Delay_us(5);
		SCL_CLR();
		Delay_us(5);
	}
	
}


unsigned char cst816_ReceiveByte(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char BitCnt;
	unsigned char retc;
	
	retc = 0;
	
	SDA_IN();
	for(BitCnt = 0; BitCnt < 8; BitCnt++)
	{
		SCL_CLR();
		Delay_us(5);
		SCL_SET();
		retc = retc<<1;
		if(SDA_GET()) retc++;
		Delay_us(5);
	}
	return retc;
}



void receive_byte(uint8_t Addr,uint8_t* Data)
{
	cst816_Start();
	cst816_SendByte(0x2a);
	cst816_WaitAck();
	cst816_SendByte(Addr);
	cst816_WaitAck();
	cst816_Start();
	cst816_SendByte(0x2b);
	cst816_WaitAck();
	*Data = cst816_ReceiveByte();
	//cst816_WaitAck();
	cst816_Stop();
}



void cst816_Write_Reg(unsigned char reg,unsigned char date)
{
	cst816_Start();
	cst816_SendByte(0x2a);
	cst816_WaitAck();
	cst816_SendByte(reg);
	cst816_WaitAck();
	cst816_SendByte(date);
	cst816_WaitAck();
	cst816_Stop();
	HAL_Delay(10);
}

void Write_Update(uint8_t dev_addr,uint8_t Data,uint8_t len)
{
	cst816_Start();
	cst816_SendByte(0x2a);
	cst816_WaitAck();
	cst816_SendByte(dev_addr);
	cst816_WaitAck();
	cst816_SendByte(Data);
	cst816_WaitAck();
	cst816_SendByte(len);
	cst816_Stop();
	HAL_Delay(10);

}


unsigned char cst816_ReadReg(unsigned char reg)
{
	unsigned char date;
	cst816_Start();
	cst816_SendByte(0x2a);
	cst816_WaitAck();
	cst816_SendByte(reg);
	cst816_WaitAck();
	cst816_Start();
	//Delay_us(5);
	cst816_SendByte(0x2b);
	cst816_WaitAck();
	date = cst816_ReceiveByte();
	//cst816_Ack();
	cst816_Stop();
	//Delay_us(5);
	return date;
}



uint8_t Get_Chip_ID(void)//отримання ID мікросхеми
{
	uint8_t id;
	
	receive_byte(ChipID,&id);
	CST816_Instance.chipID = id;
	if(id !=0)return id;
	else return 0;
	
}


void cst816_Reset(void)
{
	CST816_RESET_L();
	HAL_Delay(50);
	CST816_RESET_H();
	HAL_Delay(100);
}



void cst816_Disable_Auto_Sleep(bool state)//вмикає або вимикає  автоматичне засинання
{
	if(state == true)cst816_Write_Reg(DisAutoSleep,0xFF);
	if(state == false)cst816_Write_Reg(DisAutoSleep,0x00);
}




bool cst816_Init(touchpad_mode tp_mode)//ініциалізація тачскрина
{
	
	cst816_Reset();//скидання чіпа
	cst816_Disable_Auto_Sleep(false);//вимикаємо автоматичний перехід у режим сну
cst816_Reset();//скидання чіпа
	
	
	
	if(Get_Chip_ID() == 0)
	{
		return 0;
	}
/*	
	EnTest Переривання перевірки контакту, періодично автоматично надсилає низький імпульс після ввімкнення.
	EnTouch виявляє дотик, він періодично надсилає низький імпульс.
	EnChange Надсилає низький імпульс, коли виявляється зміна стану дотику.
	EnMotion Надсилає низький пульс, коли розпізнається жест.
	OnceWLP Жест тривалого натискання видає лише один низький імпульс.
	*/

	uint8_t irq_en = 0;
	uint8_t motion_mask = 0;
	
	switch (tp_mode)//вибір режиму переривання
  {
  	case mode_touch:
			irq_en = IRQ_EN_TOUCH;
  		break;
  	case mode_change:
      irq_en = IRQ_EN_CHANGE;
      break;
    case mode_fast:
      irq_en = IRQ_EN_MOTION;
      break;
    case mode_motion:
      irq_en = IRQ_EN_MOTION | IRQ_EN_LONGPRESS;
      motion_mask = MOTION_MASK_DOUBLE_CLICK;
      break;
  	default:
  		break;
		
  }
	HAL_Delay(100);
	cst816_Write_Reg(IrqCrl,irq_en);
	HAL_Delay(100);
	cst816_Write_Reg(MotionMask,motion_mask);
	HAL_Delay(100);
	
}




void cst816_Set_Sleep_Time(uint8_t sec)//задає час засинання сенсору(5sec. max)
{
	
	if(sec == 1)cst816_Write_Reg(AutoSleepTime,0x01);
	if(sec == 2)cst816_Write_Reg(AutoSleepTime,0x02);
	if(sec == 3)cst816_Write_Reg(AutoSleepTime,0x03);
	if(sec == 4)cst816_Write_Reg(AutoSleepTime,0x04);
	if(sec == 5)cst816_Write_Reg(AutoSleepTime,0x05);
	
	
}


unsigned char cst816_Get_Status_Gesture(void)//отримання ID жесту	
{
	uint8_t status;
	cst816_Write_Reg(AutoReset,2);//автоматичне скидання значення у регістрі GestureID,якщо є торкання але немає жесту протягом 2сек.
	receive_byte(GestureID,&status);

	if(status != 255 & status != 0)return status;
	else return 0;
}



touch_coordinates cst816_Get_XY(void)//отримати координати натискання(x/y)
{
	
	uint8_t temp[6];
	uint16_t x,y;
	static touch_coordinates touch;
	

	receive_byte(0x01,temp);
	receive_byte(0x03,temp+1);
	receive_byte(0x04,temp+2);
	receive_byte(0x05,temp+3);
	receive_byte(0x06,temp+4);

	State = temp[0];
	CST816_Instance.Gesture_ID = State;
	
	x = (uint16_t)((temp[1] & 0x0f)<<8)|temp[2];//(temp[0] & 0x0f)<<4)
	y = (uint16_t)((temp[3] & 0x0f)<<8)|temp[4];//(temp[2] & 0x0f)<<4)

	
	if(x < 240 && y < 280)
	{
		touch.X_Pos = x;
		touch.Y_Pos = y;
		
		
		
	}
	
	return touch;
}


void cst816_Clear_pos_and_ges(void)//скидання координат та жестів
{
	State=0;
	static touch_coordinates touch;
	
	touch.X_Pos=0;
	touch.Y_Pos=0;
}







