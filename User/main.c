#include "main.h"




uint8_t software_200ms_Flag = 0;
uint8_t software_500ms_Flag = 0;
uint8_t mode_operation_Flag = 0;
void software_system_time(void)
{
	static uint16_t ms_count = 0;
	if(++ms_count == 10000 )
		ms_count = 0;
	if(ms_count%200 == 0)
	{
		software_200ms_Flag = 1;
	}
	if(ms_count%500 == 0)
	{
		software_500ms_Flag = 1;

	}
	software_time_Frequency();
}

uint8_t Show_state = 0;		// ģʽ״̬
uint8_t Mode_state = 0;		// ״̬
uint8_t Mode_step = 0;		// ���в���
float Mode_data[8] = {0};	// ��������
float Mode_data_x[8] = {0}; // �������ݣ�ѵ�����ݣ�
uint8_t Mode1_flag = 0;

int main(void)
{

	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2); // ���ȼ�����
	systick_config();
	led_gpio_config(); // led��ʼ��
	key_gpio_config(); // key��ʼ��
	usart_gpio_config(115200);
	/* config the EXMC access mode */
	exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
	*(uint16_t *)SDRAM_DEVICE0_ADDR = 0xaaaa;
	
	WB_dwt_init();
	
#if USART_DMA
	dma_config();
#else
	Debug("Interrupt receive\r\n ");
#endif	
	Debug("\r\n\r\nHCLK = %d M\r\n", SystemCoreClock / 1000000);

	delay_1ms(5);
	
	delay_1ms(5);
	WS_SD_init();
	delay_1ms(5);
	spi4_init();
	delay_1ms(5);
	WS_W25Qxx_Init();
	delay_1ms(10);
	WS_W25Qxx_Font_Chenk_Write();
	delay_1ms(10); //  �ֿ�����д��
	basic_timer_config(100, 2000);
	/* RGB�� */
	lcd_disp_config(); // ��ʾ����ʼ������
	delay_1ms(10);
	WS_TFT_Load_Pic_Bmp(1, 0, 0, "dsbj.bmp");
	delay_1ms(1000);
	LCD_Clear(1, 0x7fff);

	bsp_dac_init();
	adc_config();
	
	adc_power_set(1);
	uint32_t adc_time_count = WB_getDWT_Count();
	while(!adc_state_Flag);
		timer_disable(TIMER1);
		adc_state_Flag = 0;
		Debug("ADC����ʱ��Ϊ��%.4f ms \r\n",(float)WB_getDWT_CountDt(adc_time_count,WB_getDWT_Count())/200000.f);
		Debug("ADC��%d \r\n",adc_value[4999]);
	
	
	while (1)
	{
		if(mode_operation_Flag)
		{
			switch (Mode_state)
			{
			case 0:
				Task_Mode1();
				break;
			case 1:
				Task_Mode2();
				break;

			case 2:
				Task_Mode3();
				break;

			case 3:
				Task_Mode4();
				break;

			case 4:
				Task_Mode5();
				break;

			case 5:
				Task_Mode6();
				break;
			}
		}
		else if(software_200ms_Flag)
		{
			software_200ms_Flag = 0;
			//ˢ����ʾ
			Task_Show();

		}
		if(software_500ms_Flag)
		{
			software_500ms_Flag = 0;
			Task_Led_Test();

		}
		
		if (usart_rx_flag)//���ڽ���
		{
			usart_rx_flag = 0;
		}
	}
}

//   �ź������ж�
void EXTI2_IRQHandler(void)
{

	 exti_interrupt_flag_clear(EXTI_2);	
	if(!adc_state_Flag)
		adc_power_set(1);//��ʼ����  
}



void show_waveform(uint16_t *buff)
{
	uint16_t i,j;
	uint16_t adc = 0;
	uint16_t start_x = 310, start_y = 380 / 2 + 90, end_y = 380 / 2 + 90;

	 /*         ���������ʾ����            */
	for(j=90;j<470-180;j++)
	{
		for (i = 310; i < 790 ; i ++)
		{
				LTDC_Draw_Point(1,i,j,0);
		}
	}
	 /*         ˢ�²�����ʾ            */
	adc = testOutputMag_f32[0 ]        ;   //  ȡ��һ����
	if(adc >= 4095)  adc = 4095;
	start_y = -(adc/30) + (280);
	for(i=1;i<480;i++)            //   �ӵڶ����㿪ʼ����
	{
		adc = testOutputMag_f32[i*3];
		if(adc >= 280/2)  adc = 280/2; 
		end_y = -(adc) + (280)  ;
		WS_TFT_Draw_Line(1,start_x,start_y,start_x+1,end_y,0x801f);
		
		start_x ++     ;
		start_y = end_y;
	}
}


void Task_Show(void)
{

	switch (Show_state)
	{
	case 0: /*�����ʼ��*/
		Mode_step = 0;
		LCD_Clear(1, 0x7fff);
		LCD_Clear(0, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 120, 30, "2023��ȫ����ѧ��������ƾ��� K �� ", 0x7fff, 0x801f);
		WS_TFT_Dis_16X32_String(1, 336, 200, "��������", 0x7fff, 0x801f);
		WS_TFT_Dis_16X32_String(1, 336, 320, "��չ����", 0x7fff, 0x801f);

		Show_state = 1;
		break;
	case 1:
		switch (read_key())
		{
		case 1:
			mode_operation_Flag = 1;
			break;
		case 2:
			if (Mode_state < 5)
				Mode_state++;
			break;
		case 3:
			if (Mode_state > 0)
				Mode_state--;
			break;
		}

		WS_TFT_Dis_16X32_String(1, 200, 240, "����һ", Mode_state == 0 ? YELLOW : WHITE, BROWN);
		WS_TFT_Dis_16X32_String(1, 352, 240, "���ܶ�", Mode_state == 1 ? YELLOW : WHITE, BROWN);
		WS_TFT_Dis_16X32_String(1, 504, 240, "������", Mode_state == 2 ? YELLOW : WHITE, BROWN);

		WS_TFT_Dis_16X32_String(1, 200, 360, "����һ", Mode_state == 3 ? YELLOW : WHITE, BROWN);
		WS_TFT_Dis_16X32_String(1, 352, 360, "���ܶ�", Mode_state == 4 ? YELLOW : WHITE, BROWN);
		WS_TFT_Dis_16X32_String(1, 504, 360, "������", Mode_state == 5 ? YELLOW : WHITE, BROWN);
		break;
	}
}

extern uint16_t Buzzer_count; 
extern uint8_t Buzzer_flag;	 

void Task_Mode1(void)
{
	switch (Mode_step)
	{
	case 0:
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 30, "��������һ", 0x7fff, 0x801f);
		WS_TFT_Draw_Fill_Rectangle(0, 200, 250, 600, 360, YELLOW);
		WS_TFT_Dis_16X32_String(1, 304, 290, "���ڲ�������", 0x7fff, BROWN); // ��ʾ
		Mode_step = 1;
		Mode1_flag = 1; // ��������
		Buzzer_count = 0;
		Buzzer_flag  = 0;
	
		
		break;
	case 1:
		if (!Mode1_flag)
		{
			Mode_step = 2;
			WS_TFT_Dis_16X32_String(1, 304, 290, "�����������", 0x7fff, BROWN); // ��ʾ
			WS_TFT_Draw_Fill_Rectangle(0, 200, 250, 600, 360, 0x7fff);
		}
		else if (read_key() == 1)
		{
			Show_state = 0;
			mode_operation_Flag = 0;
			Mode1_flag = 0;
		}
		break;
	case 2:
		if (read_key() == 1)
		{
			mode_operation_Flag = 0;
			Show_state = 0;
		}
		break;
	}
}
void Task_Mode2(void)
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	switch (Mode_step)
	{
	case 0:
		i = 0;
		j = 0;
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 30, "�������ܶ�", 0x7fff, 0x801f);
		WS_TFT_Load_Pic_Bmp(0, 310, 90, "bx_back1.bmp");			  //  480 * 380����
		WS_TFT_Dis_16X32_String(1, 5, 100, "Ƶ��:0", 0x7fff, 0x801f); // ��ʾ֡��
		WS_TFT_Dis_16X32_String(1, 5, 150, "ѧϰģʽ", 0x7fff, 0x801f);
		WS_TFT_Draw_Fill_Rectangle(0, 5, 250, 300, 475, WHITE);
		WS_TFT_Dis_16X32_String(1, 30, 300, "1�ű���ѧϰ", 0x7fff, RED); // ��ʾ
		WS_TFT_Dis_16X32_String(1, 88, 350, "��1��", 0x7fff, RED);		 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 313, 450, "8HZ", 0x7fff, RED);	 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 740, 450, "3750HZ", 0x7fff, RED);		 // ��ʾ
		Mode_step = 1;
		adc_state_Flag = 0;
		break;

	case 1:
		if (adc_state_Flag)
		{
			
			fft_arm_rfft_f32_app(adc_value);
			Mode_data_x[i] = fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			show_waveform(adc_value);
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)Mode_data_x[i]),0x7fff,0x801f);
			if (Mode_data_x[i] == 781.f)
				break;
			if (++i == 3)
			{
				Mode_data[j] = findMostFrequent(Mode_data_x, 3);
				i = 0;
				if (++j == 3)
				{
					Mode_step = 2;
					WS_TFT_Dis_16X32_String(1, 5, 150, "ʶ��ģʽ", 0x7fff, RED);
					WS_TFT_Dis_16X32_String(1, 30, 300, "           ", 0x7fff, RED); // ��ʾ
					WS_TFT_Dis_16X32_String(1, 88, 350, "           ", 0x7fff, RED); // ��ʾ
					break;
				}
			}
			WS_TFT_Dis_16X32_String(1, 30, 300, WB_Format("%d�ű���ѧϰ", j * 2 + 1), 0x7fff, RED); // ��ʾ
			WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("��%d��", i + 1), 0x7fff, RED);			// ��ʾ
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;

	case 2:
		if (adc_state_Flag)
		{

			fft_arm_rfft_f32_app(adc_value);
			float TTF_dat= fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			show_waveform(adc_value);
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)TTF_dat),0x7fff,0x801f);
			if (TTF_dat <= 781.f)
				break;
			for (i = 0; i < 3; i++)
			{
				if (SpecialContrast(Mode_data[i], TTF_dat))
				{
					WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("%d�ű�", i == 0 ? 1 : i == 1 ? 3
																								: 5),
											0x7fff, RED); // ��ʾ
					break;
				}
			}
			if (i == 3)
			{
				WS_TFT_Dis_16X32_String(1, 88, 350, "     ", 0x7fff, RED);
			}
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;
	}
}
void Task_Mode3(void)
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	switch (Mode_step)
	{
	case 0:
		i = 0;
		j = 0;
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 30, "����������", 0x7fff, 0x801f);
		WS_TFT_Load_Pic_Bmp(0, 310, 90, "bx_back1.bmp");			  //  480 * 380����
		WS_TFT_Dis_16X32_String(1, 5, 100, "Ƶ��:0", 0x7fff, 0x801f); // ��ʾ֡��
		WS_TFT_Dis_16X32_String(1, 5, 150, "ѧϰģʽ", 0x7fff, 0x801f);
		WS_TFT_Draw_Fill_Rectangle(0, 5, 250, 300, 475, WHITE);
		WS_TFT_Dis_16X32_String(1, 30, 300, "2�ű���ѧϰ", 0x7fff, RED); // ��ʾ
		WS_TFT_Dis_16X32_String(1, 88, 350, "��1��", 0x7fff, RED);		 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 313, 450, "8HZ", 0x7fff, RED);	 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 740, 450, "3750HZ", 0x7fff, RED);		 // ��ʾ
		Mode_step = 1;
		adc_state_Flag = 0;
		break;

	case 1:
		if (adc_state_Flag)
		{

			fft_arm_rfft_f32_app(adc_value);
			Mode_data_x[i] = fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)Mode_data_x[i]),0x7fff,0x801f);
			show_waveform(adc_value);
			if (Mode_data_x[i] == 781.f)
				break;
			if (++i == 3)
			{
				Mode_data[j] = findMostFrequent(Mode_data_x, 3);
				i = 0;
				if (++j == 3)
				{
					Mode_step = 2;
					WS_TFT_Dis_16X32_String(1, 5, 150, "ʶ��ģʽ", 0x7fff, RED);
					WS_TFT_Dis_16X32_String(1, 30, 300, "           ", 0x7fff, RED); // ��ʾ
					WS_TFT_Dis_16X32_String(1, 88, 350, "           ", 0x7fff, RED); // ��ʾ
					break;
				}
			}
			WS_TFT_Dis_16X32_String(1, 30, 300, WB_Format("%d�ű���ѧϰ", j == 0 ? 2 : j == 1 ? 4
																							  : 5),
									0x7fff, RED);										  // ��ʾ
			WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("��%d��", i + 1), 0x7fff, RED); // ��ʾ
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;

	case 2:
		if (adc_state_Flag)
		{

			fft_arm_rfft_f32_app(adc_value);
			float TTF_dat= fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)TTF_dat),0x7fff,0x801f);
			show_waveform(adc_value);
			if (TTF_dat <= 781.f)
				break;
			for (i = 0; i < 3; i++)
			{
				if (SpecialContrast(Mode_data[i], TTF_dat))
				{
					WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("%d�ű�", i == 0 ? 2 : i == 1 ? 4
																								: 5),
											0x7fff, RED); // ��ʾ
					open_Frequency(TTF_dat, 500);
					break;
				}
			}
			if (i == 3)
			{
				WS_TFT_Dis_16X32_String(1, 88, 350, "     ", 0x7fff, RED);
			}
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;
	}
}

void Task_Mode4(void)
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	uint16_t RhythmTabl_Mode4[] = {
		1568,
		1397,
		1318,
		1175,
		1046,
	};
	switch (Mode_step)
	{
	case 0:
		i = 0;
		j = 0;
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 30, "��չ����һ", 0x7fff, 0x801f);
		WS_TFT_Load_Pic_Bmp(0, 310, 90, "bx_back1.bmp");			  //  480 * 380����
		WS_TFT_Dis_16X32_String(1, 5, 100, "Ƶ��:0", 0x7fff, 0x801f); // ��ʾ֡��
		WS_TFT_Dis_16X32_String(1, 5, 150, "ѧϰģʽ", 0x7fff, 0x801f);
		WS_TFT_Draw_Fill_Rectangle(0, 5, 250, 300, 475, WHITE);
		WS_TFT_Dis_16X32_String(1, 30, 300, "1�ű���ѧϰ", 0x7fff, RED); // ��ʾ
		WS_TFT_Dis_16X32_String(1, 88, 350, "��1��", 0x7fff, RED);		 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 313, 450, "8HZ", 0x7fff, RED);	 // ��ʾ
		WS_TFT_Dis_8X16_String(1, 740, 450, "3750HZ", 0x7fff, RED);		 // ��ʾ
		Mode_step = 1;
		adc_state_Flag = 0;
		break;

	case 1:
		if (adc_state_Flag)
		{
	
			fft_arm_rfft_f32_app(adc_value);
			Mode_data_x[i] = fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			show_waveform(adc_value);
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)Mode_data_x[i]),0x7fff,0x801f);
			if (Mode_data_x[i] == 781.f)
				break;
			if (++i == 3)
			{
				Mode_data[j] = findMostFrequent(Mode_data_x, 3);
				i = 0;
				if (++j == 5)
				{
					Mode_step = 2;
					WS_TFT_Dis_16X32_String(1, 5, 150, "ʶ��ģʽ", 0x7fff, RED);
					WS_TFT_Dis_16X32_String(1, 30, 300, "           ", 0x7fff, RED); // ��ʾ
					WS_TFT_Dis_16X32_String(1, 88, 350, "           ", 0x7fff, RED); // ��ʾ
					Mode_Static_Data_Write(Mode_data, 5);
					break;
				}
			}
			WS_TFT_Dis_16X32_String(1, 30, 300, WB_Format("%d�ű���ѧϰ", j + 1), 0x7fff, RED); // ��ʾ
			WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("��%d��", i + 1), 0x7fff, RED);		// ��ʾ
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;

	case 2:
		if (adc_state_Flag)
		{
		
			fft_arm_rfft_f32_app(adc_value);
			float TTF_dat= fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			show_waveform(adc_value);
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)TTF_dat),0x7fff,0x801f);
			if (TTF_dat <= 781.f)
				break;
			for (i = 0; i < 5; i++)
			{
				if (SpecialContrast(Mode_data[i], TTF_dat))
				{
					WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("%d�ű�", i + 1), 0x7fff, RED); // ��ʾ
					open_Frequency(RhythmTabl_Mode4[i], 500);
					break;
				}
			}
			if (i == 5)
			{
				WS_TFT_Dis_16X32_String(1, 88, 350, "     ", 0x7fff, RED);
			}
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;
	}
}

void Task_Mode5(void)
{
	static uint8_t i = 0;
	switch (Mode_step)
	{
	case 0:
		i = 0;
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 30, "��չ���ܶ�", 0x7fff, 0x801f);
		WS_TFT_Load_Pic_Bmp(0, 310, 90, "bx_back1.bmp");			  //  480 * 380����
		WS_TFT_Dis_16X32_String(1, 5, 100, "Ƶ��:0", 0x7fff, 0x801f); // ��ʾ֡��
		WS_TFT_Dis_16X32_String(1, 5, 150, "ʶ��ģʽ", 0x7fff, 0x801f);
		WS_TFT_Draw_Fill_Rectangle(0, 5, 250, 300, 475, WHITE);
		WS_TFT_Dis_8X16_String(1, 313, 450, "8HZ", 0x7fff, RED); // ��ʾ
		WS_TFT_Dis_8X16_String(1, 740, 450, "3750HZ", 0x7fff, RED);	  // ��ʾ
		if (Mode_data[4] == 0)
		{
			Mode_Static_Data_Read(Mode_data, 5);
		}

		adc_state_Flag = 0;
		break;

	case 1:
		if (adc_state_Flag)
		{
			
			fft_arm_rfft_f32_app(adc_value);
			float TTF_dat = fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			show_waveform(adc_value);
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)TTF_dat),0x7fff,0x801f);
			if (TTF_dat <= 781.f)
				break;
			for (i = 0; i < 5; i++)
			{
				if (SpecialContrast(Mode_data[i], TTF_dat))
				{
					WS_TFT_Dis_16X32_String(1, 88, 350, WB_Format("%d�ű�", i + 1), 0x7fff, RED); // ��ʾ
					break;
				}
			}
			if (i == 5)
			{
				WS_TFT_Dis_16X32_String(1, 88, 350, "0�ű�", 0x7fff, RED); // ��ʾ
			}
			adc_state_Flag = 0;
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;
	}
}

void Task_Mode6(void)
{
	switch (Mode_step)
	{
	case 0:
		LCD_Clear(1, 0x7fff);
		WS_TFT_Dis_16X32_String(1, 320, 10, "��չ������", 0x7fff, 0x801f);
		WS_TFT_Load_Pic_Bmp(0, 310, 90, "bx_back1.bmp");				   //  480 * 380����
		WS_TFT_Dis_16X32_String(1, 5, 100, "Ƶ��:0", 0x7fff, 0x801f);	   // ��ʾ֡��
		WS_TFT_Dis_16X32_String(1, 55, 350, "����FFT������", 0x7fff, RED); // ��ʾ
		WS_TFT_Draw_Fill_Rectangle(0, 5, 250, 300, 475, WHITE);
		WS_TFT_Dis_8X16_String(1, 313, 450, "8HZ", 0x7fff, RED); // ��ʾ
		WS_TFT_Dis_8X16_String(1, 740, 450, "3750HZ", 0x7fff, RED);	  // ��ʾ
		Mode_step = 1;
		adc_state_Flag = 0 ;
		break;

	case 1:
		if (adc_state_Flag)
		{
			adc_state_Flag = 0;
			fft_arm_rfft_f32_app(adc_value);
			float TTF_dat = fft_index_get_frequnce(fft_get_max_amplitude_index(500));
			WS_TFT_Dis_16X32_String(1,5,100,WB_Format("Ƶ��:%d HZ  ",(int)TTF_dat),0x7fff,0x801f);
			show_waveform(adc_value);
		}
		else
		{
			if (read_key() == 1)
			{
				Show_state = 0;
				mode_operation_Flag = 0;
			}
		}
		break;
	}
}
/**
* @brief ��ϣ���ѯ���ִ���
* @param arr
* @param size
* @return
*/
float findMostFrequent(float arr[], int size)
{
	float mostFrequent = arr[0]; // ��ǰ�����ֵ
	int maxCount = 1;			 // ��ǰ���ֵ���ֵĴ���

	int current = 1;		   // ��ǰ���ĳ��ִ���
	float currentNum = arr[0]; // ��ǰ��

	// ��������
	for (int i = 1; i < size; i++)
	{
		// �����ǰ����֮ǰ�������
		if (arr[i] == currentNum)
		{
			current++;
		}
		// �����ǰ��������֮ǰ����
		else
		{
			// �����ǰ���ĳ��ִ����������ֵ���ֵĴ���
			if (current > maxCount)
			{
				maxCount = current;
				mostFrequent = currentNum;
			}

			// ���µ�ǰ���ͳ��ִ���
			currentNum = arr[i];
			current = 1;
		}
	}
	// �������һ���������
	if (current > maxCount)
	{
		mostFrequent = currentNum;
	}

	return mostFrequent;
}

// ����ģʽ��̬����д
void Mode_Static_Data_Write(float *data, uint32_t len)
{
	uint32_t configSector = 0;
	if (WS_W25Qxx_Chip_Type != W25Qxx_Not)
	{
		//  ������Ϣ�����ڵ����ڶ�������
		configSector = W25QxxInfo.chipSectorNbr - 2;
		WS_W25Qxx_Write((uint8_t *)data, configSector * WS_W25Qxx_Sector_Size, len * 4);
	}
}

// ����ģʽ��̬���ݶ�
void Mode_Static_Data_Read(float *data, uint32_t len)
{

	uint32_t configSector = 0;
	if (WS_W25Qxx_Chip_Type != W25Qxx_Not)
	{
		//  ������Ϣ�����ڵ����ڶ�������
		configSector = W25QxxInfo.chipSectorNbr - 2;
		WS_W25Qxx_Read((uint8_t *)data, configSector * WS_W25Qxx_Sector_Size, len * 4);
	}
}

/**
* @brief ����Աȷ�
* @param x1 ���Ա�
* @param x2 �Ա�
* @return �ɹ�����1��ʧ�ܷ���0
*/
int SpecialContrast(int x1, int x2)
{
	if ((x1) == (x2 + 3) || (x1) == (x2 - 3) ||
		(x1) == (x2 + 4) || (x1) == (x2 - 4) ||
		(x1) == (x2))
		return 1;

	return 0;
}
uint16_t Frequency_time = 0;
uint16_t Frequency_flag = 0;
void open_Frequency(uint16_t F, uint16_t time)
{
	Frequency_flag = F;
	Frequency_time = time;
}

void software_time_Frequency(void)
{
	if (Frequency_time > 0)
	{
		if (--Frequency_time == 0)
			Frequency_flag = 0;
	}
}

/***************************************��ʦ������**********************************************/
uint16_t RhythmTabl[] = {
	//
	// 1  	1#   	2   	 2#  	3    	4    	4#   	5    	5#   	6    	6#   	7
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,			   // ��Ƶ  12
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,			   // ��Ƶ  24
	1046, 1109, 1175, 1245, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1976 // ��Ƶ  36

};

// uint16_t two_tigers[36] = {
// 	24, 26, 28, 24, 24, 26, 28, 24, 28, 29, 31, 31,
// 	28, 29, 31, 31, 32, 34, 32, 30, 28, 24,
// 	32, 34, 32, 30, 28, 24, 26, 19, 24, 24, 26, 19, 24, 24};
uint16_t two_tigers[36] = { // 1231|1231|345|345|565431|565431|151|151
	24, 26, 28, 24, 24, 26, 28, 24, 28, 29, 31, 31,
	28, 29, 31, 31, 32 - 1, 34 - 1, 32 - 1, 30, 28, 24,
	32 - 1, 34 - 1, 32 - 1, 30, 28, 24, 26, 19, 24, 24, 26, 19, 24, 24};

uint16_t Buzzer_count = 0; // ������Ƶ�ʼ���
uint8_t Buzzer_flag = 0;   // ���������־λ

// ���÷�������Ƶ��
void WS_Borad_Buzzer_Set_Freq(uint16_t freq)
{
	TIMER_CAR(BSP_TIMER) = (uint32_t)1000000 / freq;
}
void BSP_TIMER_IRQHANDLER(void)
{
	if (timer_interrupt_flag_get(BSP_TIMER, TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(BSP_TIMER, TIMER_INT_FLAG_UP);

		if (Mode1_flag == 1)
		{
			gpio_bit_toggle(GPIOB, GPIO_PIN_3);

			if (Buzzer_count++ >= 650)
			{
				Buzzer_count = 0;
				WS_Borad_Buzzer_Set_Freq(RhythmTabl[two_tigers[Buzzer_flag]]);
				if (Buzzer_flag++ >= 35)
				{
					Buzzer_flag = 0;
					Mode1_flag = 0;
				}
			}
		}
		else if (Frequency_flag)
		{
			gpio_bit_toggle(GPIOB, GPIO_PIN_3);
			WS_Borad_Buzzer_Set_Freq(Frequency_flag * 10 / 15);
		}
	}
}
