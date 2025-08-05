#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "gd32f4xx.h"
#include "systick.h"

#define adc_buff_max    5000
//采样速度设置 
#define adc_speed_2M        0
#define adc_speed_1M        1
#define adc_speed_500K      2
#define adc_speed_250K      3
#define adc_speed_125K      4
#define adc_speed_min      	9

void adc_config(void);
void adc_power_set(unsigned char onoff);
void adc_speed_set(unsigned char speed);

extern uint16_t adc_value[adc_buff_max];   //数据存储缓存
extern uint8_t adc_state_Flag;                 //更新标志

#endif

