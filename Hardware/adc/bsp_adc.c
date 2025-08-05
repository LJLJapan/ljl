#include "bsp_adc.h"

void  set_hc4051_rank(uint32_t  n);

uint16_t adc_value[adc_buff_max];   //数据存储缓存
uint8_t adc_state_Flag;               //更新标志

void adc_config(void)
{

    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    
    
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
    
    /* TIMER1 configuration */ 
    timer_initpara.prescaler         = 133;	/*	每秒采样(200MHZ / 121/ 100 = 16528...HZ)
												ADC每次采样数量是5000次，所以ADC每次只采样前
												0.33s（16528/5000=0.33...）的值,因为杯子的震
												荡声音是0s - 0.25s之间，同时又因为每个杯子
												的震荡时间不同，可能出现与我们实验的不同，声音
												更长，所以设置为采样的时间为0.33s
											*/
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;   
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);
    /* CH2 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,49);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
    timer_enable(TIMER1);
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);
    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = adc_buff_max*2;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);
    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
    dma_interrupt_enable(DMA1, DMA_CH0, DMA_CHXCTL_FTFIE|DMA_CHXCTL_HTFIE);//
    nvic_irq_enable(DMA1_Channel0_IRQn, 0, 0);
    
    adc_clock_config(ADC_ADCCK_HCLK_DIV5);//ADC_ADCCK_HCLK_DIV5
    //规则
    adc_channel_length_config(ADC0,ADC_REGULAR_CHANNEL,1);
    adc_regular_channel_config(ADC0,0,ADC_CHANNEL_1,ADC_SAMPLETIME_3);
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL,EXTERNAL_TRIGGER_FALLING);
    adc_external_trigger_source_config(ADC0,ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_T1_CH2);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    adc_resolution_config(ADC0,ADC_RESOLUTION_12B);
    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);
    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    delay_1ms(10);
	set_hc4051_rank(2);
}

//采样速度设置
void adc_speed_set(unsigned char speed)
{
    TIMER_PSC(TIMER1) = (uint32_t)((0x01<<speed) - 1);
}

//采样开始停止设置
void adc_power_set(unsigned char cmd)
{
    if(cmd) 
	{
		timer_enable(TIMER1);
	
	}
    else
	{
		timer_disable(TIMER1);
	}
}


//ADC采集
void DMA1_Channel0_IRQHandler(void)
{
	adc_state_Flag=1;
	adc_power_set(0);//关闭ADC采集
	dma_interrupt_flag_clear(DMA1, DMA_CH0, DMA_INT_FLAG_FTF|DMA_INT_FLAG_HTF);
}

//  切换幅值挡位
void  set_hc4051_rank(uint32_t  n)
{
	    
      if(n == 4)  //  缩小5倍
			{
			   gpio_bit_write(GPIOE,GPIO_PIN_6,SET); gpio_bit_write(GPIOE,GPIO_PIN_5,SET);
			}
		  if(n == 3)  //  直通
			{
			   gpio_bit_write(GPIOE,GPIO_PIN_6,SET); gpio_bit_write(GPIOE,GPIO_PIN_5,RESET);
			}
			if(n == 2)  //  直通
			{
			   gpio_bit_write(GPIOE,GPIO_PIN_6,RESET); gpio_bit_write(GPIOE,GPIO_PIN_5,SET);
			}
			if(n == 1)  //  放大10倍
			{
			   gpio_bit_write(GPIOE,GPIO_PIN_6,RESET); gpio_bit_write(GPIOE,GPIO_PIN_5,RESET);
			}		
}
