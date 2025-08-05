#include "bsp_key.h"
#include "sys.h"
#include "bsp_led.h"
#include "stdio.h"

void key_gpio_config(void)
{
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_SYSCFG);  
	
	/* 配置GPIO的模式   PA2 中断输入 */
	gpio_mode_set(GPIOA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_2);
	gpio_bit_write(GPIOA,GPIO_PIN_2,SET);

	nvic_irq_enable(EXTI2_IRQn,0U,0U);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA,EXTI_SOURCE_PIN2);
	exti_init(EXTI_2,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
	exti_interrupt_enable(EXTI_2);
	exti_interrupt_flag_clear(EXTI_2);

	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_1);
	gpio_mode_set(GPIOC,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_5);
	gpio_mode_set(GPIOA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_7);
}


int read_key(void)
{
	int ret = 0;
	uint16_t count = 10000;
	if(!gpio_input_bit_get(GPIOA,GPIO_PIN_7) || !gpio_input_bit_get(GPIOC,GPIO_PIN_5)|| !gpio_input_bit_get(GPIOB,GPIO_PIN_1))
	{
		delay_1ms(10);
		if(gpio_input_bit_get(GPIOA,GPIO_PIN_7) == 0)
		{
			ret =1;
		}
		else if(gpio_input_bit_get(GPIOC,GPIO_PIN_5) == 0)
		{
			ret =2;
		}
		else if(gpio_input_bit_get(GPIOB,GPIO_PIN_1) == 0)
		{
			ret =3;
		}
	    while (!(gpio_input_bit_get(GPIOA,GPIO_PIN_7) || gpio_input_bit_get(GPIOC,GPIO_PIN_5)||gpio_input_bit_get(GPIOB,GPIO_PIN_1))&&count--);		

	}
	return ret;
}

