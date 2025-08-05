/*****************************
编写核心代码，尽量做到不依赖硬件



*****************************/

#ifndef __CORE_H__
#define __CORE_H__

#include "gd32f4xx.h"
#include <stdarg.h>		  //  标准头文件
#include <stdio.h>
#include "string.h"


// 格式化可变变量字符串
char *WB_Format(char *p, ...);

// 字符串处理
long WB_string_int(char *buf, int MIN, int MAX);
char WB_string_bytes(char *A_buf, char *B_buf, char STOP_CHAR, int index);
char WB_string_seek(char *buff, char *input);
// 初始化时间戳
void WB_dwt_init(void);
// 计算计数差值
uint32_t WB_getDWT_CountDt(uint32_t startTick , uint32_t  stopTick );
// 获取计数
uint32_t WB_getDWT_Count(void);
#endif
