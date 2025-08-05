#include "bsp_lcd.h"
#include "stdio.h"
#include "string.h" 
#include "drive_dzcode.h"

//LCD�Ļ�����ɫ�ͱ���ɫ	   
uint16_t  POINT_COLOR = 0x0000;	//������ɫ
uint16_t  BACK_COLOR  = 0xFFFF;  //����ɫ 






typedef struct
{
  uint32_t n;
  uint32_t p;
  uint32_t r;
  uint32_t v;
} TLI_CLK_t;

TLI_CLK_t tli_clk;


#define HORIZONTAL_SYNCHRONOUS_PULSE 10
#define HORIZONTAL_BACK_PORCH 150
#define ACTIVE_WIDTH 800
#define HORIZONTAL_FRONT_PORCH 15

#define VERTICAL_SYNCHRONOUS_PULSE 10
#define VERTICAL_BACK_PORCH 140
#define ACTIVE_HEIGHT 480
#define VERTICAL_FRONT_PORCH 40

// LCD��Ӧ�ⲿSRAM��ַ
#define LCD_FRAME_BUF_ADDR 0XC0000000
uint16_t *ltdc_framebuf[2];                                                                    //
uint16_t ltdc_lcd_framebuf0[480][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));                 //ͼ��0
uint16_t ltdc_lcd_framebuf1[480][800] __attribute__((at(LCD_FRAME_BUF_ADDR + 800 * 480 * 2))); //ͼ��1



static void tli_gpio_config(void);
static void tli_config(void);
static void ipa_config(void);
static void lcd_disp_en_config(void);
static void lcd_disp_off(void);
static void lcd_disp_on(void);

void lcd_disp_config(void)
{
	
	/* config the EXMC access mode */

	
	
	  ltdc_framebuf[0] = &ltdc_lcd_framebuf0[0][0] ;
	  ltdc_framebuf[1] = &ltdc_lcd_framebuf1[0][0] ;
	  LCD_Clear(0,0x001f);
	  LCD_Clear(1,0x7fff) ;
	
    lcd_disp_en_config();
    lcd_disp_off();

    /* configure the GPIO of TLI */
    tli_gpio_config();
    lcd_disp_on();

    tli_config();
  
    ipa_config();
}

static void tli_config(void)
{
    tli_parameter_struct tli_init_struct;
    tli_layer_parameter_struct tli_layer_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();
    /* configure PLLSAI to generate TLI clock */
    // if(ERROR == rcu_pllsai_config(216, 2, 3)){
    if (ERROR == rcu_pllsai_config(192, 2, 3))
    {
        while (1)
            ;
    }
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);  //  25M  tli

    rcu_osci_on(RCU_PLLSAI_CK);

    if (ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK))
    {
        while (1)
            ;
    }

    /* configure TLI parameter struct */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* configure LCD background R,G,B values */
    tli_init_struct.backcolor_red =   0XD0        ;
    tli_init_struct.backcolor_green = 0XD0      ;
    tli_init_struct.backcolor_blue =  0XD0       ;
    tli_init(&tli_init_struct);


    /* TLI layer0 configuration */
    /* TLI window size configuration */
    tli_layer_init_struct.layer_window_leftpos = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    /* TLI window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_ARGB1555;
    /* TLI window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255;
    /* TLI layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue  = 0x00;
    tli_layer_init_struct.layer_default_green = 0x00;
    tli_layer_init_struct.layer_default_red   = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0xff;
    /* TLI window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    /* TLI layer frame buffer base address configuration */
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)ltdc_lcd_framebuf0;
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;
    tli_layer_init(LAYER0, &tli_layer_init_struct);
		
		/* TLI layer1 configuration */
    /* TLI window size configuration */
    tli_layer_init_struct.layer_window_leftpos = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    /* TLI window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_ARGB1555;
    /* TLI window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255;
    /* TLI layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue  = 0x00;
    tli_layer_init_struct.layer_default_green = 0x00;
    tli_layer_init_struct.layer_default_red   = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0xff;
    /* TLI window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;  //  
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    /* TLI layer frame buffer base address configuration */
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)ltdc_lcd_framebuf1;
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;
    tli_layer_init(LAYER1, &tli_layer_init_struct);
		
	
		tli_dither_config(TLI_DITHER_ENABLE);	
		tli_layer_enable(LAYER0);
	  tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
		
 //   

}

/*!
    \brief      IPA initialize and configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void ipa_config(void)
{
    rcu_periph_clock_enable(RCU_IPA);
    nvic_irq_enable(IPA_IRQn, 0, 2);
}

static void tli_gpio_config(void)
{
    /* enable the periphral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure HSYNC(PC6), VSYNC(PA4), PCLK(PG7), DE(PF10) */
    /* configure LCD_R7(PG6), LCD_R6(PA8), LCD_R5(PA12), LCD_R4(PA11), LCD_R3(PB0),
                 LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PB11), LCD_G4(PB10), LCD_G3(PG10), LCD_G2(PA6),
                 LCD_B7(PB9), LCD_B6(PB8), LCD_B5(PA3), LCD_B4(PG12), LCD_B3(PG11) */
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_3);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_4);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_12);

    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_0);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_9);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_10);
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_11);

    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_7);

    gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_3);

    gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);

    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_7);
    gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_10);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_12);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
}

/*!
    \brief      configure DISP ON/OFF GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_disp_en_config(void)
{
    /* enable the periphral clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
	 
}

/*!
    \brief      DISP GPIO OFF
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_disp_off(void)
{
    gpio_bit_reset(GPIOD, GPIO_PIN_13);
}

/*!
    \brief      DISP GPIO ON
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_disp_on(void)
{
    gpio_bit_set(GPIOD, GPIO_PIN_13);
}






// ����
void LTDC_Draw_Point(uint8_t layer, uint16_t x, uint16_t y, uint16_t color)
{
   // *(uint16_t *)((uint32_t)ltdc_framebuf[layer] + 2 * (ACTIVE_WIDTH * (y) + x)) = color;
	 if(layer == 0)
	 {
	   ltdc_lcd_framebuf0[y][x] = color;
	 }
	 else
	 {
	   ltdc_lcd_framebuf1[y][x] = color;
	 }
}


/**
 * @brief		����ɫ��ʵ�ľ���
 * @param   layer �㣨0 ��1�� 
 * @param   xStar,yStar	�������
 * @param   xEnd,yEnd	�յ�����
 * @param   color ��ɫ
 * @return  none
 */
//   
void WS_TFT_Draw_Fill_Rectangle(uint8_t layer, uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd,uint16_t color)
{
	uint16_t       i = 0;
	//   д������
	for(;yStar <= yEnd ; yStar ++   )
	{
	  for(i = xStar; i <= xEnd; i ++ )
	  {  
		    LTDC_Draw_Point(layer,i,yStar,color);
	  }
	}
}


/**
 * @brief		����ɫ���ߺ���(ֱ�ߡ�б��)
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 * @return  none
 */
void WS_TFT_Draw_Line(uint8_t layer,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
//  uint16_t	i = 0;
	int16_t		delta_x = 0, delta_y = 0;
	int8_t		incx = 0, incy = 0;
	uint16_t	distance = 0;
	uint16_t  t = 0;
	uint16_t	x = 0, y = 0;
	uint16_t 	x_temp = 0, y_temp = 0;
	
	if(y1 == y2)
	{
			/* ���ٻ�ˮƽ�� */
			WS_TFT_Draw_Fill_Rectangle(layer, x1,y1-1,x2,y2+1,color) ;
			return;
	}
	else if(x1 == x2)
	{
	   /* ���ٻ�??�� */
			WS_TFT_Draw_Fill_Rectangle(layer, x1-1,y1,x2+1,y2,color) ;
			return;
	}
	else
	{
		/* ��б�ߣ�Bresenham�㷨�� */
		/* ��������֮����x��y����ļ�࣬�õ�������x��y����Ĳ���ֵ */
		delta_x = x2 - x1;
		delta_y = y2 - y1;
		if(delta_x > 0)
		{
			//б��(������)
			incx = 1;
		}
		else if(delta_x == 0)
		{
			//��ֱб��(����)
			incx = 0;
		}
		else
		{
			//б��(���ҵ���)
			incx = -1;
			delta_x = -delta_x;
		}
		if(delta_y > 0)
		{
			//б��(������)
			incy = 1;
		}
		else if(delta_y == 0)
		{
			//ˮƽб��(ˮƽ��)
			incy = 0;
		}
		else
		{
			//б��(���ҵ���)
			incy = -1;
			delta_y = -delta_y;
		}			
		
		/* ���㻭�ʴ�����(ȡ��������е����ֵ) */
		if(delta_x > delta_y)
		{
			distance = delta_x;
		}
		else
		{
			distance = delta_y;
		}
		
		/* ��ʼ��� */
		x = x1;
		y = y1;
		//��һ������Ч������t�Ĵ�����һ
		for(t = 0; t <= distance + 1;t++)
		{ 
			LTDC_Draw_Point(layer , x, y, color)  ;
//			LTDC_Draw_Point(layer , x, y-1, color);
//		  LTDC_Draw_Point(layer , x, y+1, color);			
			
//			LTDC_Draw_Point(layer , x-1, y, color);
//			LTDC_Draw_Point(layer , x+1, y, color);

//			 WS_TFT_Draw_Fill_Rectangle(layer, x1-1,y1-1,x2+1,y2+1,color) ;
			
			/* �ж���ʵ��ֵ��������ص� */
			x_temp += delta_x;	
			if(x_temp > distance)
			{
				//x����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
				x_temp -= distance;		
				//��x����������
				x += incx;
					
			}
			y_temp += delta_y;
			if(y_temp > distance)
			{
				//y����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
				y_temp -= distance;
				//��y����������
				y += incy;
			}  
		}
	}
}



//��������   
void LCD_Clear(uint8_t layer,uint16_t  color)
{
    uint32_t index=0;
    uint32_t totalpoint=ACTIVE_WIDTH;
    totalpoint*=ACTIVE_HEIGHT; //�õ��ܵ���
    for(index=0;index<totalpoint;index++)
    {
        ltdc_framebuf[layer][index] = color;
    }
} 




//  ��ȡ16�� �ֿ�����
//  �麯���� ��SD����spi flas hʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ���
// ����ֵ:  ��ȡ�����ݸ���
__weak uint32_t  WS_Font16_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	      buffer = buffer ; gbk = gbk ; size = size ;
        return 0 ;
}

//  ��ȡ24�� �ֿ�����

//  �麯���� ��SD����spi flas hʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ���
// ����ֵ:  ��ȡ�����ݸ���
__weak uint32_t  WS_Font24_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	      buffer = buffer ; gbk = gbk ; size = size ;
        return 0 ;
}

//  ��ȡ32�� �ֿ�����
//  �麯���� ��SD����spi flas hʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ���
// ����ֵ:  ��ȡ�����ݸ���
__weak uint32_t  WS_Font32_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	      buffer = buffer ; gbk = gbk ; size = size ;
        return 0 ;
}



//   ˢ16*16������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_16X16_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor)
{
	     int i , j;
	     uint16_t   dat = 0;
	     while(n -- > 0)
			 {	
				 for(j=0;j<16;j++)
				 {
						dat   = buffer[j*2] <<8 ;
						dat  |= buffer[j*2+1] ;
						for(i=0;i<15;i++)
						{
								if((dat & 1<<(15-i))  != 0 )
								{
										LTDC_Draw_Point(layer,x+i,y+j,forColor);  				
								}
								else
								{
										LTDC_Draw_Point(layer,x+i,y+j,backColor); 
								} 
						}	
				 } 
				 x += 16;
				 buffer += 32;
         if(x  >  ACTIVE_WIDTH - 16) return; 		 
			 }
}

uint8_t font_buffer[1280];  //  ���洢10��32���ֿ�����


//   ��ʾ8X16�ַ���
void WS_TFT_Dis_8X16_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor)
{ 
	uint16_t i , j ;
	uint8_t len=strlen((char *)string);
	uint8_t idex=0 , *p_dat , dat  ;
	for(idex=0;idex<len;)
	{
		if((uint8_t)string[idex]<0x80)  // �ж� ASCII ������
		{
       p_dat = (uint8_t *)(string8X16Code + (string[idex] - 0x20)*16);	
			 for(j=0;j<16;j++)
			 {
					dat = p_dat[j]  ;
					for(i=0;i<8;i++)
					{
							if((dat & 1<<(7-i))  != 0 )
							{
							    LTDC_Draw_Point(layer,x+i,y+j,forColor);          						
							}
							else
							{
							    LTDC_Draw_Point(layer,x+i,y+j,backColor); 
							} 							
					}
			 }
			 x +=  8;
       idex ++;			 
		}
	  else  // 
		{
          uint32_t size = WS_Font16_Read_Date(font_buffer,(uint8_t *)&string[idex],20);
					if( size > 0)
					{
					    WS_TFT_Dis_16X16_Date(layer,x,y,font_buffer,size/32,backColor,forColor);
						  x +=   size/2;
					}	
			if(size != 0)   idex += size/16;
      else            idex += 2;					
		 				
		}
		if(x  >  ACTIVE_WIDTH - 8) return;      
	}
}



//   ˢ24*24������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_24X24_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor)
{
	     int        i , j;
	     uint32_t   dat = 0;
	     while(n -- > 0)
			 {	
				 for(j=0;j<24;j++)
				 {
						dat   = (uint32_t)buffer[j*3]    <<16 ;
						dat  |= (uint32_t)buffer[j*3+1]  <<8  ;
					  dat  |= (uint32_t)buffer[j*3+2]       ;
						for(i=0;i<24;i++)
						{
								if((dat & 1<<(23-i))  != 0 )
								{
										LTDC_Draw_Point(layer,x+i,y+j,forColor);  				
								}
								else
								{
										LTDC_Draw_Point(layer,x+i,y+j,backColor); 
								} 
						}	
				 } 
				 x += 24;
				 buffer += 72;
         if(x  >  ACTIVE_WIDTH - 24) return; 		 
			 }
}
//   ��ʾ12X24�ַ���
void WS_TFT_Dis_12X24_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor)
{ 
	uint16_t i , j ,dat;
	uint8_t len=strlen((char *)string);
	uint8_t idex=0 , *p_dat  ;
	for(idex=0;idex<len;)
	{
		if((uint8_t)string[idex]<0x80)  // �ж� ASCII ������
		{
       p_dat = (uint8_t *)(ascii12X24Code + (string[idex] - 0x20)*48);	
			 for(j=0;j<24;j++)
			 {
					dat  = p_dat[j*2]<<8  ;
				  dat |= p_dat[j*2+1]   ;
					for(i=0;i<12;i++)
					{
							if((dat & 1<<(15-i))  != 0 )
							{
							    LTDC_Draw_Point(layer,x+i,y+j,forColor);          						
							}
							else
							{
							    LTDC_Draw_Point(layer,x+i,y+j,backColor); 
							} 							
					}
			 }
			 x +=  12;
       idex ++;			 
		}
	  else  // 
		{
          uint32_t size = WS_Font24_Read_Date(font_buffer,(uint8_t *)&string[idex],20);
					if( size > 0)
					{
					    WS_TFT_Dis_24X24_Date(layer,x,y,font_buffer,size/72,backColor,forColor);
						  x +=   size / 3; //  24 * size/72
					}						
		      if(size != 0)     idex += size/36;  //  2 * size/72		
          else              idex += 2;	 				
		  					
		}
		if(x  >  ACTIVE_WIDTH - 12) return;      
	}
}


//   ˢ32*32������,���ݵ�����Ϣ��ʾ�ַ�
void WS_TFT_Dis_32X32_Date (uint8_t layer,int x, int y, uint8_t * buffer , uint16_t n, uint16_t backColor  ,uint16_t forColor)
{
	     int        i , j;
	     uint32_t   dat = 0;
	     while(n -- > 0)
			 {	
				 for(j=0;j<32;j++)
				 {
						dat   = (uint32_t)buffer[j*4]    <<24 ;
						dat  |= (uint32_t)buffer[j*4+1]  <<16 ;
					  dat  |= (uint32_t)buffer[j*4+2]  <<8  ;
					  dat  |= (uint32_t)buffer[j*4+3]       ;
						for(i=0;i<32;i++)
						{
								if((dat & 1<<(31-i))  != 0 )
								{
										LTDC_Draw_Point(layer,x+i,y+j,forColor);  				
								}
								else
								{
										LTDC_Draw_Point(layer,x+i,y+j,backColor); 
								} 
						}	
				 } 
				 x += 32;
				 buffer += 128;
         if(x  >  ACTIVE_WIDTH - 32) return; 		 
			 }
}


//   ��ʾ16X32�ַ���
void WS_TFT_Dis_16X32_String (uint8_t layer,int x,int y, char  *string , uint16_t backColor  ,uint16_t forColor)
{ 
	uint16_t i , j ,dat;
	uint8_t len=strlen((char *)string);
	uint8_t idex=0 , *p_dat  ;
	for(idex=0;idex<len;)
	{
		if((uint8_t)string[idex]<0x80)  // �ж� ASCII ������
		{
       p_dat = (uint8_t *)(ascii16X32Code + (string[idex] - 0x20)*64);	
			 for(j=0;j<32;j++)
			 {
					dat  = p_dat[j*2]<<8  ;
				  dat |= p_dat[j*2+1]   ;
					for(i=0;i<16;i++)
					{
							if((dat & 1<<(15-i))  != 0 )
							{
							    LTDC_Draw_Point(layer,x+i,y+j,forColor);          						
							}
							else
							{
							    LTDC_Draw_Point(layer,x+i,y+j,backColor); 
							} 							
					}
			 }
			 x +=  16;
       idex ++;			 
		}
	  else  // 
		{
          uint32_t size = WS_Font32_Read_Date(font_buffer,(uint8_t *)&string[idex],20);
					if( size > 0)
					{
					    WS_TFT_Dis_32X32_Date(layer,x,y,font_buffer,size/128,backColor,forColor);
						  x +=   size / 4; //  32 * size/128
					}						
		      if(size != 0)  idex += size/64;     //   2 * size/128	
          else           idex += 2;						
		 				
		}
		if(x  >  ACTIVE_WIDTH - 16) return;      
	}
}


//  ע�⣺ �ṹ��������ֽڶ��� �� ���������������ֽڣ� �¸��������¸����ֽڿ�ʼ
struct bmp_file             //BMP�ļ�ͷ�ṹ , ���ṹ����ռ���ֽ���,��14���ֽ�
{
	uint8_t  type[4];         //λͼ�ļ������ͣ�����ΪBM�����������Ͳ��ԣ�������ʾ����
	uint32_t size;            //λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	uint16_t rd1;             // λͼ�ļ������֣�����Ϊ0
	uint16_t rd2;             // λͼ�ļ������֣�����Ϊ0
	uint32_t offset;          // λͼ���ݵ���ʼλ�ã��������λͼ
};

struct bmp_info            //ͼ����Ϣ��
{
	uint32_t bsize;          //���ṹ����ռ���ֽ���,��40���ֽ�
	uint32_t width;          // λͼ�Ŀ�ȣ�������Ϊ��λ������������4�ֽڶ����
	uint32_t height;         // λͼ�ĸ߶ȣ�������Ϊ��λ
	uint16_t planes;         // Ŀ���豸�ļ��𣬱���Ϊ1
	uint16_t count;          // ÿ�����������λ����������1(˫ɫ��// 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
	uint32_t compression;    // λͼѹ�����ͣ������� 0(��ѹ��),// 1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
	uint32_t sizeimage;      // λͼ�Ĵ�С�����ֽ�Ϊ��λ
	uint32_t xmeter;         // λͼˮƽ�ֱ��ʣ�ÿ��������
	uint32_t ymeter;         // λͼ��ֱ�ֱ��ʣ�ÿ��������
	uint32_t cused;          // λͼʵ��ʹ�õ���ɫ���е���ɫ��
	uint32_t cimportant;     // λͼ��ʾ��������Ҫ����ɫ��
};

 

struct bmp_head 
{
  struct bmp_file file;
  struct bmp_info info;
};



struct bmp_head bmp ;


uint16_t RGB888ToRGB555(uint8_t r , uint8_t g, uint8_t b)
{
     uint16_t color = 0;
	
	   color |=  b>>3;
     color |=  (uint16_t)((g>>3)<<5) ;	
	   color |=  (uint16_t)((r>>3)<<10);
	
     if(color != 0x7fff) 
			 color |= 0x8000;	
	   return color;
}


char WS_TFT_Load_Pic_Bmp(uint8_t layer,uint16_t x,uint16_t y, char  *path )
{
     Debug("\r\n\r\n");
	   if(fatfs_mount_fresult == FR_OK)
		 {
			  FRESULT fresult;
			  uint32_t br    ;
			  fresult = f_open(&SDFile,(char *)path,FA_READ);
        if(fresult == FR_OK)
				{
					//��ȡbmp���ļ�ͷ	
          if(  f_read(&SDFile, (char *)&bmp + 2 , sizeof(struct bmp_head ) - 2, &br) == FR_OK)
					{
						//Debug("br =  %d\r\n", br );
						Debug("%s file open size %d\r\n",path,bmp.file.size)  ;
						Debug("offset  %d\r\n",bmp.file.offset)  ;
						Debug("pic width %d\r\n",bmp.info.width) ;
						Debug("pic width %d\r\n",bmp.info.height);
						Debug("pic count %d\r\n",bmp.info.count) ;
						
						//  �߽���
						if((x + bmp.info.width) > LCD_WIDTH  || (y + bmp.info.height) > LCD_HEIGHT)  
						{
							  f_close(&SDFile);
						    return WS_Status_ERROR;
						}
					  static	uint8_t  buff[300];
						uint16_t width = 0 ,  i;
						y += bmp.info.height -1            ; 
						f_lseek(&SDFile,bmp.file.offset)   ;
						if(bmp.info.count == 24)
						{
							 while(1)
							 {	 
									if(  f_read(&SDFile,buff , 300, &br) == FR_OK)
									{
										 if(br >= 3)
										 {
												 for (i=0;i<br;i+=3)
											   {
												    LTDC_Draw_Point(layer,x + width,y,RGB888ToRGB555(buff[i],buff[i+1],buff[i+2]));
													  width ++;
													  if(width >= bmp.info.width)
														{
													   	width = 0;
															y --     ;
														}
												 }
										 }
										 else
										 {
											 break;
										 }
									}
							 }
					  }	
					}
					f_close(&SDFile);
				}
        else
				{
					 Debug("%s open fail\r\n",path);
				}
				   
		 }
	   return WS_Status_OK;
}












