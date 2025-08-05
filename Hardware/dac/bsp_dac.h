#ifndef _BSP_DAC_H
#define _BSP_DAC_H


#include "gd32f4xx.h"
#include "systick.h"

void dac_rcu_config(void);
void dac_gpio_config(void);
void bsp_dac_init(void);
void set_dac_vol(float v0 , float v1);



#endif
