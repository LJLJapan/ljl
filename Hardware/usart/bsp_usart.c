#include "bsp_usart.h"
#include "stdio.h"
#include "bsp_dma.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


void Debug_Loop_Transmit(void);
uint8_t usart_rx_buff[USART_RECEIVE_LENGTH];
uint16_t usart_rx_length = 0;
uint8_t usart_rx_flag = 0;

uint8_t *p_Usart1TxBuffer;
uint16_t usart1TxLength = 0;
uint16_t usart1TxCount = 0;

void usart_gpio_config(uint32_t band_rate)
{
	/* 开启时钟 */
	rcu_periph_clock_enable(BSP_USART_TX_RCU);
	rcu_periph_clock_enable(BSP_USART_RX_RCU);
	rcu_periph_clock_enable(BSP_USART_RCU);

	/* 配置GPIO复用功能 */
	gpio_af_set(BSP_USART_TX_PORT, BSP_USART_AF, BSP_USART_TX_PIN);
	gpio_af_set(BSP_USART_RX_PORT, BSP_USART_AF, BSP_USART_RX_PIN);

	/* 配置GPIO的模式 */
	/* 配置TX为复用模式 上拉模式 */
	gpio_mode_set(BSP_USART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BSP_USART_TX_PIN);
	/* 配置RX为复用模式 上拉模式 */
	gpio_mode_set(BSP_USART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BSP_USART_RX_PIN);

	/* 配置TX为推挽输出 50MHZ */
	gpio_output_options_set(BSP_USART_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_USART_TX_PIN);
	/* 配置RX为推挽输出 50MHZ */
	gpio_output_options_set(BSP_USART_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_USART_RX_PIN);

	/* 配置串口的参数 */
	usart_deinit(BSP_USART);
	usart_baudrate_set(BSP_USART, band_rate);
	usart_parity_config(BSP_USART, USART_PM_NONE);
	usart_word_length_set(BSP_USART, USART_WL_8BIT);
	usart_stop_bit_set(BSP_USART, USART_STB_1BIT);

	/* 使能串口 */
	usart_enable(BSP_USART);
	usart_transmit_config(BSP_USART, USART_TRANSMIT_ENABLE);
	usart_receive_config(BSP_USART, USART_RECEIVE_ENABLE);

	nvic_irq_enable(BSP_USART_IRQ, 2, 2);

#if !USART_DMA
	usart_interrupt_enable(BSP_USART, USART_INT_RBNE);
#endif

	usart_interrupt_enable(BSP_USART, USART_INT_IDLE);
	usart_interrupt_enable(BSP_USART, USART_INT_TC); //  启动发送完成中断
}

/* 发送函数 */
void usart_send_data(uint8_t *ucch, int size)
{
	while (size--)
	{
		usart_data_transmit(BSP_USART, (uint8_t)*ucch++);
		while (RESET == usart_flag_get(BSP_USART, USART_FLAG_TBE))
			;
	}
}

/* 串口发送字符串 */
void usart_send_string(char *ucstr)
{
	usart_send_data((uint8_t *)ucstr, strlen((char *)ucstr));
}

/* 串口发送数据 中断方式 */
void usart_send_date_IT(uint8_t *date, unsigned int size)
{
	if (date == NULL)
		return;
	p_Usart1TxBuffer = date;
	usart1TxLength = size;
	usart1TxCount = 0;
	usart_data_transmit(BSP_USART, *p_Usart1TxBuffer++);
}

/* 串口发送字符串 中断方式 */
void usart_send_string_IT(char *str)
{
	usart_send_date_IT((uint8_t *)str, strlen((char *)str));
}

void USART0_IRQHandler(void)
{
#if !USB_USART_DMA
	if (usart_interrupt_flag_get(BSP_USART, USART_INT_FLAG_RBNE) == SET)
	{
		usart_rx_buff[usart_rx_length++] = usart_data_receive(BSP_USART);
	}
#endif

	if (usart_interrupt_flag_get(BSP_USART, USART_INT_FLAG_IDLE) == SET)
	{
		usart_data_receive(BSP_USART);

#if USB_USART_DMA
		usart_rx_length = USART_RECEIVE_LENGTH - dma_transfer_number_get(BSP_DMA, BSP_DMA_CH);
		dma_channel_disable(BSP_DMA, BSP_DMA_CH);
		dma_config();
#endif

		usart_rx_buff[usart_rx_length] = '\0';
		usart_rx_flag = 1;
	}

	if (RESET != usart_interrupt_flag_get(BSP_USART, USART_INT_FLAG_TC))
	{
		usart_interrupt_flag_clear(BSP_USART, USART_INT_FLAG_TC);
		if (++usart1TxCount < usart1TxLength)
		{
			usart_data_transmit(BSP_USART, *p_Usart1TxBuffer++);
		}
		else
		{
			usart1TxCount = 0;
			usart1TxLength = 0;
			Debug_Loop_Transmit();
			//  发送寄存器空中断标志只能通过填发送数据清除，所以数据发送完毕以后需要失能中断
			//	usart_interrupt_disable(BSP_USART,USART_INT_TBE) ;
		}
	}
}

//   格式化可变变量字符串缓存
#define DEBUG_LOOP_LEN 10
char debugBuf[DEBUG_LOOP_LEN][128];
uint16_t loopCount = 0;
uint16_t loopTxCount = 0;
//   格式化输出的调试函数
void Debug(char *p, ...) // 与 printf  一摸一样的功能
{
	/*  格式化数据  */
	va_list ap;
	va_start(ap, p);
	vsprintf(debugBuf[loopCount], p, ap);
	va_end(ap);
	usart_send_string(debugBuf[loopCount]);
}
//   循环发送调试信息
void Debug_Loop_Transmit(void)
{
    if(loopTxCount != loopCount)
		{
		    usart_send_date_IT((uint8_t *)debugBuf[loopTxCount],strlen(debugBuf[loopTxCount]));
		    
			  if(++ loopTxCount == DEBUG_LOOP_LEN)
					loopTxCount = 0;
		}
}

