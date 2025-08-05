/*!
    \file    main.h
    \brief   the header file of main

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
    \version 2020-09-30, V2.1.0, firmware for GD32F4xx
    \version 2022-03-09, V3.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __MAIN_H
#define __MAIN_H

#include "gd32f4xx.h"
#include "systick.h"


#include <stdio.h>
#include "stdlib.h"
#include "string.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_basic_timer.h"
#include "bsp_spi.h"
#include "bsp_dma.h"
#include "sdcard.h"
#include "bsp_sdcard.h"
#include "bsp_lcd.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "exmc_sdram.h"

#include "ws_drive_w25qxx.h"
#include "wb_core.h"
#include "fft.h"

/*文件管理系统*/
#include "fatfs.h"
#include "ffconf.h"
void software_system_time(void);
void software_time_Frequency(void);
void show_waveform(uint16_t *buff);
void Task_Show(void);
void Task_Mode1(void);
void Task_Mode2(void);
void Task_Mode3(void);
void Task_Mode4(void);
void Task_Mode5(void);
void Task_Mode6(void);
int SpecialContrast(int x1, int x2);
void open_Frequency(uint16_t F, uint16_t time);
float findMostFrequent(float arr[], int size);
void Mode_Static_Data_Write(float *date, uint32_t len);
void Mode_Static_Data_Read(float *data, uint32_t len);

#endif /* __MAIN_H */


