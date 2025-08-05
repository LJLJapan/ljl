

#include "bsp_led.h"

void led_gpio_config(void)
{
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_LED1);
	rcu_periph_clock_enable(RCU_LED2);
	rcu_periph_clock_enable(RCU_LED3);
	rcu_periph_clock_enable(RCU_LED4);

	/* 配置GPIO的模式 */
	gpio_mode_set(PORT_LED1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED1);
	gpio_mode_set(PORT_LED2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED2);
	gpio_mode_set(PORT_LED3, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED3);
	gpio_mode_set(PORT_LED4, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED4);

	//	gpio_mode_set(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_3);

	/* 配置GPIO的输出 */
	gpio_output_options_set(PORT_LED1, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_LED1);
	gpio_output_options_set(PORT_LED2, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_LED2);
	gpio_output_options_set(PORT_LED3, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_LED3);
	gpio_output_options_set(PORT_LED4, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_LED4);

	//	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_2MHZ,GPIO_PIN_3);

	/* 全部出高 */
	gpio_bit_write(PORT_LED1, PIN_LED1, SET);
	gpio_bit_write(PORT_LED2, PIN_LED2, SET);
	gpio_bit_write(PORT_LED3, PIN_LED3, SET);
	gpio_bit_write(PORT_LED4, PIN_LED4, SET);

	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
	gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	gpio_bit_write(GPIOE, GPIO_PIN_5, SET);
	gpio_bit_write(GPIOE, GPIO_PIN_6, SET);
}


//  流水灯任务
void Task_Led_Test(void)
{

		static int step = 0;
		if (++step == 6)
			step = 0;
		gpio_bit_write(PORT_LED1, PIN_LED1, RESET);
		gpio_bit_write(PORT_LED2, PIN_LED2, RESET);
		gpio_bit_write(PORT_LED3, PIN_LED3, RESET);
		gpio_bit_write(PORT_LED4, PIN_LED4, RESET);
		switch (step)
		{
		case 0:
			gpio_bit_write(PORT_LED1, PIN_LED1, SET);
			break;
		case 1:
			gpio_bit_write(PORT_LED2, PIN_LED2, SET);
			break;
		case 2:
			gpio_bit_write(PORT_LED3, PIN_LED3, SET);
			break;
		case 3:
			gpio_bit_write(PORT_LED4, PIN_LED4, SET);
			break;
		case 4:
			gpio_bit_write(PORT_LED3, PIN_LED3, SET);
			break;
		case 5:
			gpio_bit_write(PORT_LED2, PIN_LED2, SET);
			break;
		}
	
}
