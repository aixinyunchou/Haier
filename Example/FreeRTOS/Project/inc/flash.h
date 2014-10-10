
#ifndef _FLASH_H_
#define _FLASH_H_

#include "includes.h"

typedef union
{
	struct
	{
		uint8_t bit0 	:1;
		uint8_t bit1 	:1;	
		uint8_t bit2 	:1;
		uint8_t bit3 	:1;
		uint8_t bit4 	:1;
		uint8_t bit5 	:1;
		uint8_t bit6	:1;
		uint8_t bit7 	:1;			
	}BitValue;
	uint8_t ByteValue;
}Status_union;

extern Status_union FlashWorkFlag_union;
extern uint8_t InvalidBlockTable[];
#define FlashInitFlag FlashWorkFlag_union.BitValue.bit0 //flash 初始化标志位


#define FLASH_BLOCK_MAX_NUM 4096//Flash中最大block数
#define FLASH_PAGE_MAX_NUM_IN_A_BLOCK  64//一个block中最大page数
#define FLASH_BYTE_MAX_NUM_IN_A_PAGE   2112//一个page中最大byte数


#define FLASH_OPERATE_TRY_TIME 6//every flash operate's max try times


//*********************** 管脚定义 **************************************
///*
#define FLASH_IO0_GPIO          GPIOC
#define FLASH_IO0_PIN           GPIO_Pin_2
#define FLASH_IO0_PINS          GPIO_PinSource2
#define FLASH_IO0_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO1_GPIO          GPIOC
#define FLASH_IO1_PIN           GPIO_Pin_3
#define FLASH_IO1_PINS          GPIO_PinSource3
#define FLASH_IO1_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO2_GPIO          GPIOC
#define FLASH_IO2_PIN           GPIO_Pin_4
#define FLASH_IO2_PINS          GPIO_PinSource4
#define FLASH_IO2_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO3_GPIO          GPIOC
#define FLASH_IO3_PIN           GPIO_Pin_5
#define FLASH_IO3_PINS          GPIO_PinSource5
#define FLASH_IO3_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO4_GPIO          GPIOC
#define FLASH_IO4_PIN           GPIO_Pin_8
#define FLASH_IO4_PINS          GPIO_PinSource8
#define FLASH_IO4_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO5_GPIO          GPIOC
#define FLASH_IO5_PIN           GPIO_Pin_9
#define FLASH_IO5_PINS          GPIO_PinSource9
#define FLASH_IO5_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO6_GPIO          GPIOC
#define FLASH_IO6_PIN           GPIO_Pin_11
#define FLASH_IO6_PINS          GPIO_PinSource11
#define FLASH_IO6_CLK           RCC_AHB1Periph_GPIOC

#define FLASH_IO7_GPIO          GPIOC
#define FLASH_IO7_PIN           GPIO_Pin_13
#define FLASH_IO7_PINS          GPIO_PinSource13
#define FLASH_IO7_CLK           RCC_AHB1Periph_GPIOC



#define FLASH_WE_GPIO           GPIOD
#define FLASH_WE_PIN            GPIO_Pin_5
#define FLASH_WE_PINS           GPIO_PinSource5
#define FLASH_WE_CLK            RCC_AHB1Periph_GPIOD

#define FLASH_RE_GPIO           GPIOD
#define FLASH_RE_PIN            GPIO_Pin_4
#define FLASH_RE_PINS           GPIO_PinSource4
#define FLASH_RE_CLK            RCC_AHB1Periph_GPIOD

#define FLASH_ALE_GPIO          GPIOD
#define FLASH_ALE_PIN           GPIO_Pin_12
#define FLASH_ALE_PINS          GPIO_PinSource12
#define FLASH_ALE_CLK           RCC_AHB1Periph_GPIOD

#define FLASH_CLE_GPIO          GPIOD
#define FLASH_CLE_PIN           GPIO_Pin_11
#define FLASH_CLE_PINS          GPIO_PinSource11
#define FLASH_CLE_CLK           RCC_AHB1Periph_GPIOD

#define FLASH_RB_GPIO           GPIOD
#define FLASH_RB_PIN            GPIO_Pin_6
#define FLASH_RB_PINS           GPIO_PinSource6
#define FLASH_RB_CLK            RCC_AHB1Periph_GPIOD

#define FLASH_CE_GPIO           GPIOD
#define FLASH_CE_PIN            GPIO_Pin_7
#define FLASH_CE_PINS           GPIO_PinSource7
#define FLASH_CE_CLK            RCC_AHB1Periph_GPIOD
//*/

//输出电平
#define FLASH_IO0_OUTPUT_0(void)		FLASH_IO0_GPIO->BSRRH = FLASH_IO0_PIN
#define FLASH_IO0_OUTPUT_1(void)		FLASH_IO0_GPIO->BSRRL = FLASH_IO0_PIN

#define FLASH_IO1_OUTPUT_0(void)		FLASH_IO1_GPIO->BSRRH = FLASH_IO1_PIN
#define FLASH_IO1_OUTPUT_1(void)		FLASH_IO1_GPIO->BSRRL = FLASH_IO1_PIN

#define FLASH_IO2_OUTPUT_0(void)		FLASH_IO2_GPIO->BSRRH = FLASH_IO2_PIN
#define FLASH_IO2_OUTPUT_1(void)		FLASH_IO2_GPIO->BSRRL = FLASH_IO2_PIN

#define FLASH_IO3_OUTPUT_0(void)		FLASH_IO3_GPIO->BSRRH = FLASH_IO3_PIN
#define FLASH_IO3_OUTPUT_1(void)		FLASH_IO3_GPIO->BSRRL = FLASH_IO3_PIN

#define FLASH_IO4_OUTPUT_0(void)		FLASH_IO4_GPIO->BSRRH = FLASH_IO4_PIN
#define FLASH_IO4_OUTPUT_1(void)		FLASH_IO4_GPIO->BSRRL = FLASH_IO4_PIN

#define FLASH_IO5_OUTPUT_0(void)		FLASH_IO5_GPIO->BSRRH = FLASH_IO5_PIN
#define FLASH_IO5_OUTPUT_1(void)		FLASH_IO5_GPIO->BSRRL = FLASH_IO5_PIN

#define FLASH_IO6_OUTPUT_0(void)		FLASH_IO6_GPIO->BSRRH = FLASH_IO6_PIN
#define FLASH_IO6_OUTPUT_1(void)		FLASH_IO6_GPIO->BSRRL = FLASH_IO6_PIN

#define FLASH_IO7_OUTPUT_0(void)		FLASH_IO7_GPIO->BSRRH = FLASH_IO7_PIN
#define FLASH_IO7_OUTPUT_1(void)		FLASH_IO7_GPIO->BSRRL = FLASH_IO7_PIN

//控制脚
#define FLASH_WE_OUTPUT_0(void)			FLASH_WE_GPIO->BSRRH = FLASH_WE_PIN
#define FLASH_WE_OUTPUT_1(void)			FLASH_WE_GPIO->BSRRL = FLASH_WE_PIN

#define FLASH_RE_OUTPUT_0(void)			FLASH_RE_GPIO->BSRRH = FLASH_RE_PIN
#define FLASH_RE_OUTPUT_1(void)			FLASH_RE_GPIO->BSRRL = FLASH_RE_PIN

#define FLASH_ALE_OUTPUT_0(void)		FLASH_ALE_GPIO->BSRRH = FLASH_ALE_PIN
#define FLASH_ALE_OUTPUT_1(void)		FLASH_ALE_GPIO->BSRRL = FLASH_ALE_PIN

#define FLASH_CLE_OUTPUT_0(void)		FLASH_CLE_GPIO->BSRRH = FLASH_CLE_PIN
#define FLASH_CLE_OUTPUT_1(void)		FLASH_CLE_GPIO->BSRRL = FLASH_CLE_PIN

#define FLASH_RB_OUTPUT_0(void)			FLASH_RB_GPIO->BSRRH = FLASH_RB_PIN
#define FLASH_RB_OUTPUT_1(void)			FLASH_RB_GPIO->BSRRL = FLASH_RB_PIN

#define FLASH_CE_OUTPUT_0(void)			FLASH_CE_GPIO->BSRRH = FLASH_CE_PIN
#define FLASH_CE_OUTPUT_1(void)			FLASH_CE_GPIO->BSRRL = FLASH_CE_PIN


//输入
#define FLASH_IO0_INPUT_VALUE(void)  (FLASH_IO0_GPIO->IDR & FLASH_IO0_PIN) >> FLASH_IO0_PINS	//Flash IO0口输入电平 1-高 0-低
#define FLASH_IO1_INPUT_VALUE(void)  (FLASH_IO1_GPIO->IDR & FLASH_IO1_PIN) >> FLASH_IO1_PINS 	//Flash IO1口输入电平 1-高 0-低
#define FLASH_IO2_INPUT_VALUE(void)  (FLASH_IO2_GPIO->IDR & FLASH_IO2_PIN) >> FLASH_IO2_PINS 	//Flash IO2口输入电平 1-高 0-低
#define FLASH_IO3_INPUT_VALUE(void)  (FLASH_IO3_GPIO->IDR & FLASH_IO3_PIN) >> FLASH_IO3_PINS 	//Flash IO3口输入电平 1-高 0-低
#define FLASH_IO4_INPUT_VALUE(void)  (FLASH_IO4_GPIO->IDR & FLASH_IO4_PIN) >> FLASH_IO4_PINS	//Flash IO4口输入电平 1-高 0-低
#define FLASH_IO5_INPUT_VALUE(void)  (FLASH_IO5_GPIO->IDR & FLASH_IO5_PIN) >> FLASH_IO5_PINS	//Flash IO5口输入电平 1-高 0-低
#define FLASH_IO6_INPUT_VALUE(void)  (FLASH_IO6_GPIO->IDR & FLASH_IO6_PIN) >> FLASH_IO6_PINS 	//Flash IO6口输入电平 1-高 0-低
#define FLASH_IO7_INPUT_VALUE(void)  (FLASH_IO7_GPIO->IDR & FLASH_IO7_PIN) >> FLASH_IO7_PINS 	//Flash IO7口输入电平 1-高 0-低
#define FLASH_WE_INPUT_VALUE(void)   FLASH_WE_GPIO->IDR & FLASH_WE_PIN 	//Flash WE口输入电平  1-高 0-低
#define FLASH_RE_INPUT_VALUE(void)   FLASH_RE_GPIO->IDR & FLASH_RE_PIN 	//Flash RE口输入电平  1-高 0-低
#define FLASH_ALE_INPUT_VALUE(void)  FLASH_ALE_GPIO->IDR & FLASH_ALE_PIN 	//Flash ALE口输入电平 1-高 0-低
#define FLASH_CLE_INPUT_VALUE(void)  FLASH_CLE_GPIO->IDR & FLASH_CLE_PIN 	//Flash CLE口输入电平 1-高 0-低
#define FLASH_RB_INPUT_VALUE(void)   (FLASH_RB_GPIO->IDR & FLASH_RB_PIN) >> FLASH_RB_PINS 	//Flash RB口输入电平  1-高 0-低
#define FLASH_CE_INPUT_VALUE(void)   FLASH_CE_GPIO->IDR & FLASH_CE_PIN 	//Flash CE口输入电平  1-高 0-低


/********************************************************************
**Flash管脚状态
********************************************************************/
#define FLASH_CE_NULL   0XFF//flash ce status null
#define FLASH_CE_DISABLE 0//flash ce status disable
#define FLASH_CE_ENABLE  1//flash ce status enable

#define FLASH_IO_STATUS_NULL   0xFF
#define FLASH_IO_STATUS_INPUT  0
#define FLASH_IO_STATUS_OUTPUT 1


/********************************************************************
**Flash相关命令宏定义
********************************************************************/
#define FLASH_READ_1ST_COMMAND 0X00//Read 1st CYCLE
#define FLASH_READ_2ND_COMMAND 0X30//Read 2nd CYCLE 
#define FLASH_READ_FOR_COPY_BACK_1ST_COMMAND 0X00//Read for copy back 1st CYCLE
#define FLASH_READ_FOR_COPY_BACK_2ND_COMMAND 0X35//Read for copy back 2nd CYCLE
#define FLASH_READ_ID_COMMAND 0X90//Read ID
#define FLASH_RESET_COMMAND 0XFF//Reset 
#define FLASH_PAGE_PROGRAM_1ST_COMMAND 0X80//Page Program 1st CYCLE
#define FLASH_PAGE_PROGRAM_2ND_COMMAND 0X10//Page Program 2nd CYCLE
#define FLASH_COPYBACK_PROGRAM_1ST_COMMAND 0X85//Copy-Back Program 1st CYCLE
#define FLASH_COPYBACK_PROGRAM_2ND_COMMAND 0X10//Copy-Back Program 2nd CYCLE
#define FLASH_BLOCK_ERASE_1ST_COMMAND 0X60//Block Erase 1st CYCLE
#define FLASH_BLOCK_ERASE_2ND_COMMAND 0Xd0//Block Erase 2nd CYCLE
#define FLASH_RANDOM_DATA_INPUT_COMMAND 0X85//Random data input
#define FLASH_RANDOM_DATA_OUTPUT_1ST_COMMAND 0X05//Random data output 1st CYCLE
#define FLASH_RANDOM_DATA_OUTPUT_2ND_COMMAND 0XE0//Random data output 2nd CYCLE
#define FLASH_READ_STATUS_COMMAND 0X70//Read status
#define FLASH_READ_EDC_STATUS_COMMAND 0X7B//Read EDC status
#define FLASH_TWO_PLANE_PAGE_PROGRAM_1ST_LOW 0X80//Two-Plane Page Program 1st low
#define FLASH_TWO_PLANE_PAGE_PROGRAM_1ST_HIGH 0X11//Two-Plane Page Program 1st high
#define FLASH_TWO_PLANE_PAGE_PROGRAM_2ND_LOW 0X81//Two-Plane Page Program 2nd low
#define FLASH_TWO_PLANE_PAGE_PROGRAM_2ND_HIGH 0X10//Two-Plane Page Program 2nd high
#define FLASH_TWO_PLANE_BLOCK_ERASE_1ST_LOW 0X60//Two-Plane Block Erase 1st low
#define FLASH_TWO_PLANE_BLOCK_ERASE_1ST_HIGH 0X60//Two-Plane Block Erase 1st high
#define FLASH_TWO_PLANE_BLOCK_ERASE_2ND 0Xd0//Two-Plane Block Erase 2nd

#define FLASH_BAD_BLOCK_SIGN_ADDRESS  2048//flash坏块标志的页地址 每个block的第一个和第二个page的2048字节地址
#define FLASH_BAD_BLOCK_SIGN 0//坏块标志 0
#define FLASH_GOOD_BLOCK_SIGN 0XFF//好块标志 0xff
#define FLASH_RB_DELAY_TIME 60000//RB管脚状态判断时间延时最长时间 unit 1us

#define FLASH_OPERATE_OK 0//Flash操作正确完成
#define FLASH_OPERATE_ERROR 1//Flash操作没有正确完成
#define FLASH_OPERATE_BREAK 0XFF//Flash操作进行中退出
#define FLASH_EDC_INVALID 	2//EDC无效
#define FLASH_BAD_BLOCK 		3//坏块
#define FLASH_GOOD_BLOCK 		4//好块



typedef union 
{
	struct 
	{
		unsigned IO0:1; 
  	unsigned IO1:1;
  	unsigned IO2:1;
  	unsigned IO3:1;
  	unsigned IO4:1;
  	unsigned IO5:1;
  	unsigned IO6:1;
  	unsigned IO7:1;
	}OneBit; 
	uint8_t AllBit;
}FLASH_8BIT_DATA_Union;

extern FLASH_8BIT_DATA_Union FlashIoOutputValue_Union,FlashIoInputValue_Union;

extern uint8_t InterruptOccurWhenFlashOperatorFlag;//interrupt occurd when flash is operated flag
extern struct air_condition air_condition_record;
/*----------------------------------Function declare-------------------------------*/
//void Flash_Set_Io_Input(void);
//void Flash_Set_Io_Output(void);
void Flash_Ce_Enable(void);
void Flash_Ce_Disable(void);
void Flash_Init(void);
void Flash_Write_8Bit_Data(uint8_t data);
uint8_t Flash_Read_8Bit_Data(void);
uint8_t Flash_Read_Io_Status(uint8_t bitnum);
void Flash_Write_Command(uint8_t com);
void Flash_Write_Row_Column_Address(uint16_t BLockNUm,uint8_t PAgeNUm,uint16_t BYteNUm); 
void Flash_Write_Row_Address(uint16_t BLockNUm,uint8_t PAgeNUm); 
void Flash_Write_Column_Address(uint16_t BYteNUm);
void Flash_Write_1Byte_Address(uint8_t address); 
uint8_t Flash_RB_Value(void);
uint8_t Flash_Write_Data(uint16_t BLockNUM,uint8_t PAgeNUm,uint16_t BYteNUn,uint16_t NUm,uint8_t *pWRiteDAta);
uint8_t Flash_Read_Data(uint16_t BLockNUM,uint8_t PAgeNUm,uint16_t BYteNUn,uint16_t NUm,uint8_t *pREadDAta);
uint8_t Flash_Reset(void);
uint8_t Flash_Sign_Bad_Block(uint16_t BLockNUm);
uint8_t Flash_Check_Bad_Block(uint16_t BLockNUm);
uint8_t Flash_Erase_Block(uint16_t BLockNUm);
uint8_t Flash_Read_ID(void);
uint8_t Flash_Read_Random_Data(uint16_t BLockNUm,uint8_t PAgeNUm,uint16_t FIrstBYteNUm,uint16_t FIrstNUm,uint16_t SEcondBYteNUm,uint16_t SEcondNUm,uint8_t *pRAndomDAta);
uint8_t Flash_Write_Random_Data(uint16_t BLockNUm,uint8_t PAgeNUm,uint16_t FIrstBYteNUm,uint16_t FIrstNUm,uint8_t *pFIrstDAta,uint16_t SEcondBYteNUm,uint8_t SEcondNUm,uint8_t *pSCondDAta);
void Flash_Erase_The_Device(void);
void Flash_Multi_Gpio_Init(void);
uint8_t Test_Two_Mermory_Same_Or_Not(uint8_t *pTArgetDAta, uint8_t *pSOurceDAta, uint16_t DAtaLEngth);

void Block_Copy_Block(uint16_t block_old,uint16_t new);
void List_Array(struct air_condition);
void Flash_Write_UserData(struct air_condition air_condition_user);
void Flash_Test(void);
uint8_t BMP_Page_Write(uint32_t page);
uint8_t BMP_Page_Read(uint32_t page, uint8_t *pdata);
/*--------------------------------------------------------------------------------*/

#endif 
