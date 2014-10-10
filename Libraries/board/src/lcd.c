/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：lcd.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：LCD 驱动。 
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "sdram.h"
#include "lcd.h"
/*
 ******************************************************************************
 *函数：void LCD_GPIOInit(void)
 *输入：void
 *输出：void
 *描述：LCD GPIO初始化
 ******************************************************************************
 */
void LCD_GPIOInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /*
   ****************************************************************************
   *R3 -->  PH9 | G2 --> PH13 | B3 --> PG11 | VSYNC --> PI10
   *R4 --> PH10 | G3 --> PH14 | B4 --> PG12 | HSYNC -->  PI9
   *R5 --> PH11 | G4 --> PH15 | B5 -->  PI5 | DE    --> PF10
   *R6 -->  PB1 | G5 -->  PI0 | B6 -->  PI6 | CLK   -->  PG7
   *R7 -->  PG6 | G6 -->  PI1 | B7 -->  PI7 | BL_EN --> PG13 
   *            | G7 -->  PI2 |
   ****************************************************************************
   */
	/* 使能GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG 
                         | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  /* PB1设置为LCD_R6 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, 0x09);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, 0x09);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOH, GPIO_PinSource9, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource13, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource14, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource15, GPIO_AF_LTDC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13
                                | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource5, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource9, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5
                                | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
  /* BL_EN pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}
/*
 ******************************************************************************
 *函数：void LCD_GPIOInit(void)
 *输入：void
 *输出：void
 *描述：LCD初始化
 ******************************************************************************
 */
void LCD_Init(void)
{
  LTDC_InitTypeDef       LTDC_InitStruct;
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;
  LTDC_Layer_TypeDef     LTDC_Layerx;
  
	
  /* IO口初始化 */
  LCD_GPIOInit();
	
  LCD_DisplayOff();
  /* 使能LCD时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
  /* 使能DMA失踪*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
  /* 水平同步信号---低电平有效 */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* 垂直同步信号---低电平有效 */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* 数据使能信号---低电平有效 */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* 像素时钟配置--- */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_DEPolarity_AL;
	/* LCD背光设置 */
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;  	
  /*
   ****************************************************************************
   *PLLSAI_VCO = HSE*PLLSAI_N / PLL_M = 8 * 192 / 8 = 192MHz
   *PLLLCDCLK = PLLSAI_VCO / PLLSAI_R = 192 / 3 = 64 Mhz
   *LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64 / 8 = 8 Mhz
   ****************************************************************************
   */
  RCC_PLLSAIConfig(192, 7, 3);
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);	
  /* 使能PLLSAI时钟 */
  RCC_PLLSAICmd(ENABLE);
  /* 等待PLLSAI时钟 */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET){}
  /*  */
  LTDC_InitStruct.LTDC_HorizontalSync = 40;
  /*  */
  LTDC_InitStruct.LTDC_VerticalSync = 9;
  /*  */
  LTDC_InitStruct.LTDC_AccumulatedHBP = 42; 
  /*  */
  LTDC_InitStruct.LTDC_AccumulatedVBP = 11;  
  /*  */
  LTDC_InitStruct.LTDC_AccumulatedActiveW = 522;
  /*  */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = 283;
  /*  */
  LTDC_InitStruct.LTDC_TotalWidth = 524; 
  /*  */
  LTDC_InitStruct.LTDC_TotalHeigh = 285;
            
  LTDC_Init(&LTDC_InitStruct);
		
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 43;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (480 + 43 - 1); 
  LTDC_Layer_InitStruct.LTDC_VarticalStart = 12;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (272 + 12 - 1);	

  /* Pixel Format configuration*/            
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  /* Configure blending factors */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3 
     Active high width         = LCD_PIXEL_WIDTH 
     number of bytes per pixel = 2    (pixel_format : RGB565) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((480 * 2) + 3);
  /*  the pitch is the increment from the start of one line of pixels to the 
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     
  */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (480 * 2);
  /* configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 272;

  /* Input Address configuration */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;
   
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* Configure Layer2 */
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
	
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground & background Layers */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
//  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
  LTDC_ReloadConfig(LTDC_IMReload);
	
  LCD_DisplayOn();
}
/*
 ******************************************************************************
 *函数：void LCD_DisplayOn(void)
 *输入：void
 *输出：void
 *描述：LCD显示开启，使能背光，使能电源
 ******************************************************************************
 */
void LCD_DisplayOn(void)
{
  /* Enable LCD Backlight */ 
  GPIO_SetBits(GPIOG, GPIO_Pin_13);
  /* Display On */
  LTDC_Cmd(ENABLE); /* display ON */
}

/*
 ******************************************************************************
 *函数：void LCD_DisplayOff(void)
 *输入：void
 *输出：void
 *描述：LCD显示关闭，失能背光，失能电源
 ******************************************************************************
 */
void LCD_DisplayOff(void)
{
  /* Enable LCD Backlight */ 
  GPIO_ResetBits(GPIOG, GPIO_Pin_13);
  /* Display Off */
  LTDC_Cmd(DISABLE); 
}

