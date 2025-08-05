#ifndef __LCD_H
#define __LCD_H


#include "main.h"
#include "systick.h"


#define LCD_WIDTH  800
#define LCD_HEIGHT 480
#define LCD_FB_BYTE_PER_PIXEL 1


void lcd_disp_config(void);



#define LCD_SDA_RCU  			RCU_GPIOE   // SDA
#define LCD_SDA_PORT  		    GPIOE
#define LCD_SDA_PIN    		    GPIO_PIN_6//GPIO_PIN_5  //v0.0.1����Ļ6������Ļ5
#define LCD_SDA_WRITE(x)	    gpio_bit_write(LCD_SDA_PORT,LCD_SDA_PIN,(bit_status)x)
#define LCD_SCL_RCU  			RCU_GPIOE   // SCL
#define LCD_SCL_PORT  		    GPIOE
#define LCD_SCL_PIN    		    GPIO_PIN_2
#define LCD_SCL_WRITE(x)	    gpio_bit_write(LCD_SCL_PORT,LCD_SCL_PIN,(bit_status)x)
#define LCD_CS_RCU  			RCU_GPIOE   // CS
#define LCD_CS_PORT  		    GPIOE
#define LCD_CS_PIN    		    GPIO_PIN_4
#define LCD_CS_WRITE(x)	        gpio_bit_write(LCD_CS_PORT,LCD_CS_PIN,(bit_status)x)
#define LCD_RST_RCU  			RCU_GPIOD   // RST
#define LCD_RST_PORT  		    GPIOD
#define LCD_RST_PIN    		    GPIO_PIN_12
#define LCD_RST_WRITE(x)	    gpio_bit_write(LCD_RST_PORT,LCD_RST_PIN,(bit_status)x)
#define LCD_BLK_ON              gpio_bit_set(GPIOD,GPIO_PIN_13)	// PD13
#define LCD_BLK_OFF             gpio_bit_reset(GPIOD,GPIO_PIN_13)	// PD13
#define LCD_CS_Clr()            gpio_bit_reset(GPIOE,GPIO_PIN_4)	// PD13
#define LCD_CS_Set()            gpio_bit_set(GPIOE,GPIO_PIN_4)	// PD13
#define LCD_SDI_RCU  			RCU_GPIOC   // SDI
#define LCD_SDI_PORT  		    GPIOC
#define LCD_SDI_PIN    		    GPIO_PIN_1
#define LCD_SDI_WRITE(x)	    gpio_bit_write(LCD_SDI_PORT,LCD_SDI_PIN,(bit_status)x)







extern uint16_t *ltdc_framebuf[2];              
extern uint16_t ltdc_lcd_framebuf0[480][800];              
extern uint16_t ltdc_lcd_framebuf1[480][800]; 




//������ɫ
#define WHITE        0xFFFF
#define BLACK        0x0000	  
#define BLUE         0x001F  
#define BRED         0XF81F
#define GRED         0XFFE0
#define GBLUE        0X07FF
#define RED          0xF800
#define MAGENTA      0xF81F
#define GREEN        0x07E0
#define CYAN         0x7FFF
#define YELLOW       0xFFE0
#define BROWN        0XBC40 //��ɫ
#define BRRED        0XFC07 //�غ�ɫ
#define GRAY         0X8430 //��ɫ
//GUI��ɫ
#define DARKBLUE         0X01CF	//����ɫ
#define LIGHTBLUE        0X7D7C	//ǳ��ɫ  
#define GRAYBLUE         0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
#define LIGHTGREEN       0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY            0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)




//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern uint16_t  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern uint16_t  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ





////////////////
void LTDC_Draw_Point(uint8_t layer, uint16_t x, uint16_t y, uint16_t color);   //  ����
void LCD_Clear(uint8_t layer,uint16_t  color);                                 //  ��������   

//   ��ʾ8X16�ַ���
void WS_TFT_Dis_8X16_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   ˢ16*16������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_16X16_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);

//   ��ʾ12X24�ַ���
void WS_TFT_Dis_12X24_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   ˢ24*24������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_24X24_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);

//   ��ʾ16X32�ַ���
void WS_TFT_Dis_16X32_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   ˢ32*32������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_32X32_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);


char WS_TFT_Load_Pic_Bmp(uint8_t layer,uint16_t x,uint16_t y, char  *path );


void WS_TFT_Draw_Fill_Rectangle(uint8_t layer, uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd,uint16_t color);
void WS_TFT_Draw_Line(uint8_t layer,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);



#endif  




