#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "gd32f4xx.h"
#include "systick.h"

#define BSP_USART_TX_RCU  RCU_GPIOA
#define BSP_USART_RX_RCU  RCU_GPIOA
#define BSP_USART_RCU     RCU_USART0

#define BSP_USART_TX_PORT GPIOA
#define BSP_USART_RX_PORT GPIOA
#define BSP_USART_AF 			GPIO_AF_7
#define BSP_USART_TX_PIN  GPIO_PIN_9
#define BSP_USART_RX_PIN  GPIO_PIN_10

#define BSP_USART 				USART0
#define BSP_USART_IRQ     USART0_IRQn

#define USART_RECEIVE_LENGTH  4096


/* 0 : 中断接收  1 ：DMA接收 */
#define USART_DMA 0

extern uint8_t usart_rx_buff[USART_RECEIVE_LENGTH];
extern uint16_t usart_rx_length;
extern uint8_t usart_rx_flag; 


void usart_gpio_config(uint32_t band_rate);
void usart_send_data(uint8_t *ucch,int size);
void usart_send_string(char *ucstr)    ;

/* 串口发送字符串 中断方式 */
void usart_send_string_IT( char  *str);

void Debug(char *p,...)  ; // 与 printf  一摸一样的功能 

#endif




