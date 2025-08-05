#ifndef _BSP_LED_H
#define _BSP_LED_H


#include "gd32f4xx.h"
#include "systick.h"


#define RCU_LED1  RCU_GPIOE
#define PORT_LED1 GPIOE
#define PIN_LED1 GPIO_PIN_3

#define RCU_LED2  RCU_GPIOD
#define PORT_LED2 GPIOD
#define PIN_LED2 GPIO_PIN_7

#define RCU_LED3  RCU_GPIOG
#define PORT_LED3 GPIOG
#define PIN_LED3 GPIO_PIN_3

#define RCU_LED4  RCU_GPIOA
#define PORT_LED4 GPIOA
#define PIN_LED4 GPIO_PIN_5

void led_gpio_config(void);

//  流水灯任务
//int WS_OS_Task_Led_Test(void * date);
void Task_Led_Test(void);

#endif



