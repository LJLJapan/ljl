/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
FRESULT  fatfs_mount_fresult = FR_OK; 

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

#define font_16_file_path  "H_16.dzk" 
#define font_24_file_path  "H_24.dzk" 
#define font_32_file_path  "H_32.dzk" 

#define Font_SDCard_Read_EN 0  // 0: ʧ�ܺ���ʵ�� �� 1��ʹ�ܺ���ʵ��


#if Font_SDCard_Read_EN == 1

//  ��ȡ16�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//   font: ���壬1->16�� ��2->24�㣬3->32��
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font_Read_Date(uint8_t *buffer , uint8_t * path ,uint8_t *gbk, uint16_t size,uint16_t font)
{
	  uint32_t readSize  = 0;
    if(buffer == NULL ||path == NULL || gbk == 0 || size == 0)  return 0;
	  if(font == 1) font = 32 ;  // ���������޸�ÿ�����ֵ���������
	  if(font == 2) font = 72 ;
	  if(font == 3) font = 128 ;
    if(fatfs_mount_fresult == FR_OK)
		 {
			  FRESULT fresult;
			  fresult = f_open(&SDFile,(char *)path,FA_READ);
        if(fresult == FR_OK)
				{
					while(*gbk > 0x80 && size > 0)  //  ������ȡ�����ֿ����ݣ������ַ�ֹͣ
					{
						uint32_t addr =  (94*(gbk[0]-0xa1)  +  (gbk[1] - 0xa1) ) * font ;
						uint32_t res , br;
					  f_lseek(&SDFile , addr);
					  res = f_read(&SDFile, (void *)buffer, font, &br); 
						if(res == FR_OK && br == font)
						{
							//  ׼����ȡ�¸�����
						  readSize += font;     
							buffer += font  ;     //   �����ַƫ��
							gbk += 2      ;       //   �¸����� 
							size --       ;     
						}
						else
						{
						   break;
						}
					} 
				  f_close(&SDFile);
				}
        else
				{
				   if(font == 1)  WS_Debug("open file 0:/H_16.dzk ERR\r\n");
					 if(font == 2)  WS_Debug("open file 0:/H_24.dzk ERR\r\n");
					 if(font == 3)  WS_Debug("open file 0:/H_32.dzk ERR\r\n");
				}					
		 }
		 return readSize ;
}



//  ��ȡ16�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font16_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	  
     return WS_Font_Read_Date(buffer,(uint8_t *)font_16_file_path,gbk,size,1) ;
}


//  ��ȡ24�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font24_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	   
     return WS_Font_Read_Date(buffer,(uint8_t *)font_24_file_path,gbk,size,2) ; 
}


//  ��ȡ32�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font32_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
    return WS_Font_Read_Date(buffer,(uint8_t *)font_32_file_path,gbk,size,3)  ;
}

#endif

     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
