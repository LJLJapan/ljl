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
#define LCD_SDA_PIN    		    GPIO_PIN_6//GPIO_PIN_5  //v0.0.1新屏幕6，老屏幕5
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




//画笔颜色
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
#define BROWN        0XBC40 //棕色
#define BRRED        0XFC07 //棕红色
#define GRAY         0X8430 //灰色
//GUI颜色
#define DARKBLUE         0X01CF	//深蓝色
#define LIGHTBLUE        0X7D7C	//浅蓝色  
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色 
#define LIGHTGREEN       0X841F //浅绿色
#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)




//LCD的画笔颜色和背景色	   
extern uint16_t  POINT_COLOR;//默认红色    
extern uint16_t  BACK_COLOR; //背景颜色.默认为白色





////////////////
void LTDC_Draw_Point(uint8_t layer, uint16_t x, uint16_t y, uint16_t color);   //  画点
void LCD_Clear(uint8_t layer,uint16_t  color);                                 //  清屏函数   

//   显示8X16字符串
void WS_TFT_Dis_8X16_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   刷16*16点区域,根据点阵信息显示字符
void WS_TFT_Dis_16X16_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);

//   显示12X24字符串
void WS_TFT_Dis_12X24_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   刷24*24点区域,根据点阵信息显示字符
void WS_TFT_Dis_24X24_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);

//   显示16X32字符串
void WS_TFT_Dis_16X32_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor);

//   刷32*32点区域,根据点阵信息显示字符
void WS_TFT_Dis_32X32_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor);


char WS_TFT_Load_Pic_Bmp(uint8_t layer,uint16_t x,uint16_t y, char  *path );


void WS_TFT_Draw_Fill_Rectangle(uint8_t layer, uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd,uint16_t color);
void WS_TFT_Draw_Line(uint8_t layer,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);



#endif  




