/*****************************
��д���Ĵ��룬��������������Ӳ��



*****************************/

#ifndef __CORE_H__
#define __CORE_H__

#include "gd32f4xx.h"
#include <stdarg.h>		  //  ��׼ͷ�ļ�
#include <stdio.h>
#include "string.h"


// ��ʽ���ɱ�����ַ���
char *WB_Format(char *p, ...);

// �ַ�������
long WB_string_int(char *buf, int MIN, int MAX);
char WB_string_bytes(char *A_buf, char *B_buf, char STOP_CHAR, int index);
char WB_string_seek(char *buff, char *input);
// ��ʼ��ʱ���
void WB_dwt_init(void);
// ���������ֵ
uint32_t WB_getDWT_CountDt(uint32_t startTick , uint32_t  stopTick );
// ��ȡ����
uint32_t WB_getDWT_Count(void);
#endif
