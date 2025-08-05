

#include "string.h"
#include "systick.h"
#include "ws_drive_w25qxx.h"



WS_W25QxxTypeDef WS_W25Qxx_Chip_Type   = W25Qxx_Not   ;	  //  Ĭ�Ͽ�

WS_W25Qxx_FlashInfoTypeDef W25QxxInfo                 ;   //  W25Qxx �����Ϣ

uint8_t  W25QxxWriteBuffer[4096]                     ;   //  ����дW25QxxоƬ�������ݻ���


/********************************************************************************
	*�������ƣ� uint8_t WS_W25Qxx_Read_Write_Byte(uint8_t data)
	*���ܣ�     SPI��дһ���ֽ�
	*������     date:д�������
	*���أ�			r_date:����������
*********************************************************************************/
uint8_t WS_W25Qxx_Write_Read_Byte(uint8_t date)
{    
	    uint8_t r_date  ;
	
	    while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_TBE));
	    spi_i2s_data_transmit(SPI4,date);
	
	    while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_RBNE));
	    r_date =   spi_i2s_data_receive(SPI4);
    	 
	    return r_date ;   
}

/********************************************************************************
	*�������ƣ� uint8_t WS_W25Qxx_Read_Byte(void)
	*���ܣ�     SPI��һ���ֽ�
	*������     ��
	*���أ�			r_date:����������
*********************************************************************************/
uint8_t WS_W25Qxx_Read_Byte(void)
{
	   return WS_W25Qxx_Write_Read_Byte(0xff);
}

/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write_Byte(uint8_t date)
	*���ܣ�     SPI��һ���ֽ�
	*������     date:д�������
	*���أ�			��
*********************************************************************************/
void WS_W25Qxx_Write_Byte(uint8_t date)
{
	    WS_W25Qxx_Write_Read_Byte(date);
}

/********************************************************************************
	*�������ƣ� void WS_SPI_Write_Buffer(uint8_t date)
	*���ܣ�     SPIд����ֽ�
	*������     date:д�������
	*���أ�			��
*********************************************************************************/
void WS_SPI_Write_Buffer(uint8_t *date,uint16_t  size)
{
	 if(size == 0)  return ;
	 while(size > 0)
	 {
	    WS_W25Qxx_Write_Read_Byte(*date++);
		  size --;
	 }
}

/********************************************************************************
	*�������ƣ� void WS_SPI_Write_Buffer(uint8_t date)
	*���ܣ�     SPIд����ֽ�
	*������     date:д�������
	*���أ�			��
*********************************************************************************/
void WS_SPI_Read_Buffer(uint8_t *date,uint16_t  size)
{
	 if(size == 0)  return ;
	 while(size > 0)
	 {
	    *date++ =  WS_W25Qxx_Write_Read_Byte(0x00);  
		  size --;
	 }
}


/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Init(void)
	*���ܣ�     ��ʼ��W25QXX����ȡоƬ�ͺż�������Ϣ���ã����������Ϣ
	*������     ��
	*���أ�			W25Qxx ����
*********************************************************************************/
WS_W25QxxTypeDef WS_W25Qxx_Init(void)
{
	   if(WS_W25Qxx_Chip_Type == W25Qxx_Not)
		 {
			 uint16_t id =  WS_W25Qxx_Read_ID() ;   
			 id =  WS_W25Qxx_Read_ID()          ;	
			 //  �����Ϣ�ṹ��
			 memset((char *)&W25QxxInfo,0,sizeof(WS_W25Qxx_FlashInfoTypeDef))	;
			 switch(id)
			 {
				 case W25Qxx_16 :  WS_W25Qxx_Chip_Type = W25Qxx_16 ; 
													 W25QxxInfo.chipSectorNbr = 512      ;			 
													 break;
				 
				 case W25Qxx_32 :  WS_W25Qxx_Chip_Type = W25Qxx_32 ;
													 W25QxxInfo.chipSectorNbr = 1024      ;
													 break;
				 
				 case W25Qxx_64 :  WS_W25Qxx_Chip_Type = W25Qxx_64 ;
													 W25QxxInfo.chipSectorNbr = 2048      ;
													 break;
				 
				 case W25Qxx_128 : WS_W25Qxx_Chip_Type = W25Qxx_128 ; 
													 W25QxxInfo.chipSectorNbr = 4096      ;
													 break; 
			 }
			 if(WS_W25Qxx_Chip_Type != W25Qxx_Not)
			 {
				 W25QxxInfo.chipType       = WS_W25Qxx_Chip_Type      ;
				 W25QxxInfo.chipSectorSize = WS_W25Qxx_Sector_Size    ;
				 W25QxxInfo.fontStartSector= (W25QxxInfo.chipSectorNbr>>1) + 2 ; //  
				 W25QxxInfo.fatfsSectorNbr = (W25QxxInfo.chipSectorNbr>>1)* (4096/WS_W25Qxx_Fatfs_Sector_Size) ; //  �ļ�ϵͳ��һ��ռ�
				 W25QxxInfo.fatfsSectorSize= WS_W25Qxx_Fatfs_Sector_Size ;
				 Debug("W25Qxx id : %X\r\n",id)  ;
			 }
			 else
			 {
				 Debug("Not W25Qxx %X\r\n",id)   ;    
			 }
		 
		 }
		 return WS_W25Qxx_Chip_Type ;
}


/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Init(void)
	*���ܣ�     ��ʼ��W25QXX����ȡоƬ�źţ����������Ϣ
	*������     ��
	*���أ�			оƬ�ͺţ� W25Qxx_16  =	0XEF14  
												 W25Qxx_32  =	0XEF15  
												 W25Qxx_64  =	0XEF16  
												 W25Qxx_128 =	0XEF17 	
*********************************************************************************/ 
uint16_t WS_W25Qxx_Read_ID(void)
{
	uint16_t id = 0;	  
	WS_W25Qxx_CS_RESET;
	WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_ID);      //���Ͷ�ȡID����	    
	WS_W25Qxx_Write_Byte(0x00); 	    
	WS_W25Qxx_Write_Byte(0x00); 	    
	WS_W25Qxx_Write_Byte(0x00); 	 			   
	id|=WS_W25Qxx_Read_Byte()<<8;  
	id|=WS_W25Qxx_Read_Byte();	 
	WS_W25Qxx_CS_SET;				    
	return id;
} 													 


/********************************************************************************
	*�������ƣ� uint8_t WS_W25Qxx_Read_SR(void) 
	*���ܣ�     ��ȡW25Qxx��״̬�Ĵ���
	*������     ��
	*���أ�			״̬�Ĵ���ֵ
	�Ĵ�������
	BIT7  6   5   4   3   2   1   0
	SPR   RV  TB BP2 BP1 BP0 WEL BUSY
	SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
	TB,BP2,BP1,BP0:FLASH����д��������
	WEL:дʹ������
	BUSY:æ���λ(1,æ;0,����)
	Ĭ��:0x00
*********************************************************************************/ 
uint8_t WS_W25Qxx_Read_SR(void)   
{  
	uint8_t byte=0;   
	WS_W25Qxx_CS_RESET;                            //ʹ������   
	WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_Read);      //���Ͷ�ȡ״̬�Ĵ�������    
	byte=WS_W25Qxx_Read_Byte();                    //��ȡһ���ֽ�  
	WS_W25Qxx_CS_SET;                              //ȡ��Ƭѡ     
	return byte;   
} 

/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write_SR(uint8_t date) 
	*���ܣ�     дW25QXX״̬�Ĵ���
	*������     ��
	*���أ�			��
  ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*********************************************************************************/ 
void WS_W25Qxx_Write_SR(uint8_t date)   
{   
	WS_W25Qxx_CS_RESET;                                //ʹ������   
	WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_Write);    //����дȡ״̬�Ĵ�������    
	WS_W25Qxx_Write_Byte(date);               	   //д��һ���ֽ�  
	WS_W25Qxx_CS_SET;                                  //ȡ��Ƭѡ     	      
} 


/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write_Enable(void)  
	*���ܣ�     W25QxxоƬдʹ��	�� ��������ǰ��Ҫдʹ��
	*������     ��
	*���أ�		  ��
*********************************************************************************/ 
 void WS_W25Qxx_Write_Enable(void)   
{
	WS_W25Qxx_CS_RESET;                          
  WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_WriteEnable); 	
	WS_W25Qxx_CS_SET;                           	    
} 


/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write_Disable(void)    
	*���ܣ�     W25QxxоƬдʧ��	�� ������������Ҫдʧ��
	*������     ��
	*���أ�		  ��
*********************************************************************************/ 
void WS_W25Qxx_Write_Disable(void)   
{  
	WS_W25Qxx_CS_RESET;                           
  WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_WriteDisable);  
	WS_W25Qxx_CS_SET;                                 
} 		
 
/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Read(uint8_t* buffer,uint32_t addr,uint16_t len)     
	*���ܣ�     ����ָ����ַ�����ֽ�Ϊ��λ����ȡоƬ���ݣ�
  *������     buffer:���ݻ���
              addr  :��ȡ��ַ��
              len   :��ȡ���ݳ���
	*���أ�		  ��
*********************************************************************************/ 
WS_StatusTypeDef WS_W25Qxx_Read(uint8_t* buffer,uint32_t addr,uint32_t len)   
{			
    //WS_StatusTypeDef  rtn ;	
		WS_W25Qxx_CS_RESET;                            
    WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_ReadData);   //���Ͷ�ȡ����   
    WS_W25Qxx_Write_Byte((uint8_t)((addr)>>16));  	//����24bit��ַ    
    WS_W25Qxx_Write_Byte((uint8_t)((addr)>>8)) ;   
    WS_W25Qxx_Write_Byte((uint8_t) addr);   
    WS_SPI_Read_Buffer(buffer,len);                //ѭ����ȡ  
	  WS_W25Qxx_CS_SET; 
    return WS_Status_OK;
} 

/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write_Page(uint8_t* buffer,uint32_t addr,uint16_t len)    
	*���ܣ�     ����ָ����ַ�����ֽ�Ϊ��λ��д���ݵ�оƬ�����256���� ÿ��������Ҫ�ֶ��д��
  *������     buffer:���ݻ���
              addr  :д��ַ��
              len   :д���ݳ���
	*���أ�		  ��
*********************************************************************************/ 
WS_StatusTypeDef WS_W25Qxx_Write_Page(uint8_t* buffer,uint32_t addr,uint16_t len)
{
 
  WS_W25Qxx_Write_Enable();                  	        
	WS_W25Qxx_CS_RESET  ;                           
	WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_PageProgram);      	//����дҳ����   
	WS_W25Qxx_Write_Byte((uint8_t)((addr)>>16)); 	//����24bit��ַ    
	WS_W25Qxx_Write_Byte((uint8_t)((addr)>>8)) ;   
	WS_W25Qxx_Write_Byte((uint8_t)  addr);   
	WS_SPI_Write_Buffer(buffer,len);//ѭ��д��
	WS_W25Qxx_CS_SET   ; 
  return  WS_W25Qxx_Wait_Idle() ;	
} 



/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Write(uint8_t* buffer,uint32_t addr,uint16_t len)     
	*���ܣ�     ����ָ����ַ�����ֽ�Ϊ��λ��д���ݵ�оƬ��
  *������     buffer:���ݻ���
              addr  :д���ַ��
              len   :д�����ݳ���
	*���أ�		  ��
*********************************************************************************/ 
WS_StatusTypeDef WS_W25Qxx_Write(uint8_t* buffer,uint32_t addr,uint32_t len)   
{								    
	uint32_t size        ; 
  uint32_t  chipSector , addrAdd , writeCount ,writeAddr ;
	uint16_t i   = 0  ,  l    ;

	//  ������֤�������
	if(len == 0 || buffer == NULL || addr > W25QxxInfo.chipSectorNbr*W25QxxInfo.chipSectorSize)   return  WS_Status_ERROR;
	
	//   ����д������
	while(len  >  0)
	{
		
		  l =   addr % WS_W25Qxx_Sector_Size ;
		  //  �����Ҫ����д����ֽڣ���������������д��
		  if(((WS_W25Qxx_Sector_Size) - l) <  len)
			{
			   writeCount = ((WS_W25Qxx_Sector_Size) - l);
				 len       -= ((WS_W25Qxx_Sector_Size) - l); 
			}
			else
			{
			   writeCount       = len;
				 len             -= len;
			}
			
			//   ����д���ַ���ڵ������������ַƫ��
			chipSector =addr / 4096; // оƬ������ַ  
			addrAdd    =addr % 4096; // �������ڵ�ƫ��
			
			//  ����д���ַ
			writeAddr = chipSector  *   4096 ; 
		

			//   ��������������
			WS_W25Qxx_Read   (W25QxxWriteBuffer, chipSector * (WS_W25Qxx_Sector_Size) 
												,WS_W25Qxx_Sector_Size) ;
			
			 
			 //   ����д������ݿ�����ָ��λ��
			 memcpy(W25QxxWriteBuffer + addrAdd , buffer , writeCount);

			 //  ������Ӧ����
			 WS_W25Qxx_Sector_Erase(chipSector);  
			 
			 //  ��16ҳд��
			 for(i=0; i <  16 ; i++)
				{
					 size  = (i * 256)                                               ; 
					 WS_W25Qxx_Write_Page(W25QxxWriteBuffer+size,writeAddr+size,256) ;
				}
			WS_W25Qxx_Read(W25QxxWriteBuffer,addr,writeCount)       ;
				
			if(memcmp(W25QxxWriteBuffer ,buffer,writeCount) != 0)
			{
				Debug("W25Qxx write error:%d~%d\r\n",writeAddr,writeAddr+writeCount);
        return  WS_Status_ERROR;				
			}
			else
			{
				//Debug(0,"W25Qxx write OK Sector:%d~%d\r\n",writeAddr,writeAddr+writeCount);
			}
			
			addr   += writeCount;   //  ��ӵ�ַƫ�ƣ���ɲ���д��
			buffer += writeCount;
			
	}
  return WS_Status_OK;	     		    	      
}



/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Sector_Erase(uint32_t sector)      
	*���ܣ�     ����ָ����������һ������( 4096 byte),����ʱ��:40ms����
  *������     sector:������       
	*���أ�		  ��
*********************************************************************************/ 
WS_StatusTypeDef WS_W25Qxx_Sector_Erase(uint32_t sector)   
{  
 	  uint32_t addr  =  WS_W25Qxx_Sector_Size * sector;
//	  uint32_t  i = 0         ;
    WS_W25Qxx_Write_Enable();                 
    WS_W25Qxx_Wait_Idle()   ;   
  	WS_W25Qxx_CS_RESET      ;                            
    WS_W25Qxx_Write_Byte(WS_W25Qxx_Reg_Erase_Sector);      	
    WS_W25Qxx_Write_Byte((uint8_t)((addr)>>16));  	
    WS_W25Qxx_Write_Byte((uint8_t)((addr)>>8));   
    WS_W25Qxx_Write_Byte((uint8_t)  addr);  
	  WS_W25Qxx_CS_SET  ; 
   	//�ȴ��������                    	      
    return WS_W25Qxx_Wait_Idle();   
} 

//   ʵ�����ҳʱ�� 36ms , д��ʱ��26ms
WS_StatusTypeDef  WS_W25Qxx_Sector_Write(uint8_t *buffer  ,uint32_t sector ,  uint32_t size)   
{
	 uint16_t i                    ;
		 uint32_t addr  , addr_inc  = 0  , len = size;
	   if(buffer ==NULL || size == 0)  return WS_Status_ERROR;
		 addr = sector * 4096           ;
		 WS_W25Qxx_Sector_Erase(sector) ;   
		 for(i=0; i <  16 ; i++)    //  ��16ҳд��
			{
				 addr_inc  = (i * 256)                                   ; 
				 if( size >= 256 )
				 {
				   WS_W25Qxx_Write_Page(buffer+addr_inc,addr+addr_inc,256) ;
					 size -= 256;
				 }
				 else
				 {
				   WS_W25Qxx_Write_Page(buffer+addr_inc,addr+addr_inc,size) ; 
					 size -= size;
				 }
				 if(size == 0) break;
			}
			
		  WS_W25Qxx_Read(W25QxxWriteBuffer,addr,len)       ;
				
			if(memcmp(W25QxxWriteBuffer ,buffer,len) != 0)
			{
				Debug("W25Qxx write error:%d~%d\r\n",addr,addr+addr_inc);
        return  WS_Status_ERROR;				
			}
			else
			{
				//Debug(0,"W25Qxx write OK Sector:%d~%d\r\n",writeAddr,writeAddr+writeCount);
			}	
	   return  WS_Status_OK;   
	//return  WS_W25Qxx_Write(buffer, sector * WS_W25Qxx_Sector_Size , size * WS_W25Qxx_Sector_Size) ;   
}

WS_StatusTypeDef  WS_W25Qxx_Sector_Read(uint8_t *buffer  ,uint32_t sector ,  uint32_t size) 
{
  return  WS_W25Qxx_Read (buffer,sector * WS_W25Qxx_Sector_Size , size * WS_W25Qxx_Sector_Size) ;
}

/********************************************************************************
	*�������ƣ� void WS_W25Qxx_Wait_Busy(void)      
	*���ܣ�     �ȴ�оƬ��ɲ���, ��Ҫ�����߳��ڶ�оƬ��ɲ���
  *������     ��       
	*���أ�		  ��
*********************************************************************************/ 
WS_StatusTypeDef WS_W25Qxx_Wait_Idle(void)   
{  
  int i = 0, n = 0 ; 	 
	while((WS_W25Qxx_Read_SR()&0x01)==0x01)  // �ȴ�BUSYλ���
	{
		 for (n = 0 ; n < 20000 ; n ++){}
		 if(++ i >= 1000)  break ; 
	}	
	if(i < 1000)  return WS_Status_OK;
  else          return WS_Status_TIMEOUT;		
} 





//д��ϵͳ������Ϣ�麯��
void  WS_Config_Write_Struct_Callback(uint8_t *  date  ,  uint32_t  len )
{
      uint32_t   configSector = 0;
      if(WS_W25Qxx_Chip_Type != W25Qxx_Not) 
			{
				 //  ������Ϣ���������һ������
				 configSector = W25QxxInfo.chipSectorNbr-1;
				 WS_W25Qxx_Write(date,configSector*WS_W25Qxx_Sector_Size,len);   
			} 				
}


//      ��ȡϵͳ������Ϣ�麯��
void  WS_Config_Read_Struct_Callback(uint8_t *  date  ,  uint32_t len)   
{
	    uint32_t   configSector = 0;
      if(WS_W25Qxx_Chip_Type != W25Qxx_Not) 
			{
			   //  ������Ϣ���������һ������
				  configSector = W25QxxInfo.chipSectorNbr-1; 
				  //Debug(1,"configSector:%d\r\n",configSector);
			    WS_W25Qxx_Read(date , configSector * WS_W25Qxx_Sector_Size , len);
			}		
}


#if Font_W25QXX_Read_EN == 1


//  ��ȡ16�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//   font: ���壬1->16�� ��2->24�㣬3->32��
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_W25Qxx_Font_Read_Date(uint8_t *buffer ,uint8_t *gbk, uint16_t size,uint16_t font)
{
	  uint32_t  readSize  = 0;
	  uint32_t addr;
    if(buffer == NULL || gbk == 0 || size == 0 )  return 0;
	  // ���������޸�ÿ�����ֵ���������
	  if(font == 1) 
		{
			 font = 32  ; 
       //  �ֿ������� + ƫ������		
			 addr = (WS_W25Qxx_FONT16_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
	  if(font == 2) 
		{
			 font = 72  ; 	
			 addr = (WS_W25Qxx_FONT24_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
		if(font == 3) 
		{
			 font = 128  ; 	
			 addr = (WS_W25Qxx_FONT32_Start_Sector+WS_W25Qxx_FONT_Start_Sector) * WS_W25Qxx_Sector_Size + 4;
		}
	  
	  while(*gbk > 0x80 && size > 0)  //  ������ȡ�����ֿ����ݣ������ַ�ֹͣ
		{
		  uint32_t 	n =  (94*(gbk[0]-0xa1)  +  (gbk[1] - 0xa1) ) * font ;
			WS_W25Qxx_Read(buffer, addr + n , font );
			//  ׼����ȡ�¸�����
			readSize += font  ;     
			buffer   += font  ;     //   �����ַƫ��
			gbk += 2      ;         //   �¸����� 
			size --       ;  			
		}
    return readSize ;		
}

//  ʵ���麯�� ��ȡ16�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font16_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	 if(W25QxxInfo.font16Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,1);
}


//  ʵ���麯�� ��ȡ24�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font24_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	 if(W25QxxInfo.font24Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,2);
}

//  ʵ���麯�� ��ȡ24�� �ֿ����� �� gbk�������ݻ���ָ���������ֶ�ȡ��Ͻ�������
//  ��SD��ʵ�� ��ȡ����
//  buffer:  ���ݻ����ַ
//  gbk   :  ������������
//  size  :  ��ȡ���ָ����������������������ݣ����ߴﵽ��ȡ����ʱ����������
//  ����ֵ�� ��ȡ�������ݸ���
uint32_t  WS_Font32_Read_Date(uint8_t *buffer , uint8_t * gbk , uint32_t size)
{
	 if(W25QxxInfo.font32Flag == 0) return 0;
   return   WS_W25Qxx_Font_Read_Date(buffer,gbk,size,3);
}

#endif



//    �ڴ濨�����ļ�����+�洢λ�÷���
const  WS_ConfigFileTypeDef   loadConfigFileInfo[] =
{
   "H_16.dzk",WS_W25Qxx_FONT16_Start_Sector,70,
	 "H_24.dzk",WS_W25Qxx_FONT24_Start_Sector,266,
	 "H_32.dzk",WS_W25Qxx_FONT32_Start_Sector,544
};



//   ����Կ��ļ��Ƿ����
//   �ֿ��ļ���Ÿ�ʽ��4byte(�ļ�����) + n byte(�ļ�) + 1 byte(1~n �������׸��ֽں�У��)
WS_StatusTypeDef WS_W25Qxx_Check_File(WS_ConfigFileTypeDef W25QxxFileInfo)
{
	  uint32_t size ;
	  uint32_t fontStartSector ;
	  uint32_t Sector , i;
	  uint8_t  check = 0 ,dat ;
	                    //  �ֿ������� + ƫ������
	  fontStartSector =  W25QxxFileInfo.startSector + WS_W25Qxx_FONT_Start_Sector ;
    WS_W25Qxx_Read((uint8_t *)&size , WS_W25Qxx_Sector_Size*fontStartSector , 4)  ;
	
	  //  �����ļ����ȳ���100M�� ����0 �� ���ش���
	  if(size > 100 * 1024*1024 || size == 0)  
		{	
			return WS_Status_ERROR;
		}

		//   ��ȥ�ļ����ȵ����ĸ��ֽڣ�����ļ����ȣ�size-4���պ���4096������
    Sector = (size-4-1) /  WS_W25Qxx_Sector_Size + 1;	
	  for(i=fontStartSector+1;i<Sector+fontStartSector;i++)
	  {
	    WS_W25Qxx_Read(&dat,WS_W25Qxx_Sector_Size*i,1);  
		  check += dat;
	  }
		WS_W25Qxx_Read(&dat,WS_W25Qxx_Sector_Size*fontStartSector + size+4  ,1);
		if(dat == check )  return WS_Status_OK;
	  else               return WS_Status_ERROR;
}

//    д���ļ�
WS_StatusTypeDef WS_W25Qxx_Font_Write(WS_ConfigFileTypeDef W25QxxFileInfo)
{
      //FIL file;
			FRESULT 	res ; 
	    uint32_t fontStartSector ;
			UINT br ;
		  static	uint8_t buffer[4097]     ;
	    char    filePath[24]     ;
		  uint32_t Sector = 0      ;
      uint8_t  check  = 0      ;         //  ���У��	
	    uint32_t  n      = 0     ;         //  ������
	    WS_StatusTypeDef   rtn   ;
	                                                  // ���ֿ�洢ƫ����
      fontStartSector =  W25QxxFileInfo.startSector + W25QxxInfo.fontStartSector;	
			
      sprintf(filePath,"0:/%s",W25QxxFileInfo.name);  
	
			res = f_open(&SDFile,filePath,FA_READ); 
			if(res!=FR_OK)
			{
				Debug("TF Card %s NOT OPEN\r\n",W25QxxFileInfo.name);
				//WS_TFT_Dis_Debug(0,48,RED,"TF Card %s NOT OPEN",W25QxxFileInfo.name);
				return WS_Status_ERROR         ;
			}else{
				Debug("%s Open size: %d\r\n",W25QxxFileInfo.name,SDFile.fsize); 
				
				Debug("Start write %s\r\n",W25QxxFileInfo.name);
				//WS_TFT_Dis_Debug(0,32,BLUE,"%s Open size: %d",W25QxxFileInfo.name,file.fsize);
				//WS_TFT_Dis_Debug(0,48,BLUE,"Start copy %s:",W25QxxFileInfo.name);
			}
			
      Sector = fontStartSector  ;	                  // ����16�������Sector 0 ��ʼ�洢
			*(uint32_t *)buffer = SDFile.fsize  ;
      res = f_read(&SDFile, buffer+4, 4092, &br);
      br  += 4;
 			  
			while(res == FR_OK)
			{
				 if(br > 0 )
				 {
					  // �ӵڶ�������ʼ��һ���ֽڽ��к�У��
					  if(Sector >  fontStartSector)
						{
						    check += buffer[0] ;
  					}

						if(br == 4096)
						{
				      //rtn = WS_W25Qxx_Sector_Write(Sector++ ,buffer ,br) ;
							rtn   =  WS_W25Qxx_Write(buffer,Sector++*WS_W25Qxx_Sector_Size , br) ;
						}
						else
						{
						  buffer[ br ++]  =  check ;
              //rtn =WS_W25Qxx_Sector_Write(Sector++ ,buffer ,br) ;
							rtn   = WS_W25Qxx_Write(buffer,Sector++*WS_W25Qxx_Sector_Size , br) ;
						}
						
						if(rtn != WS_Status_OK)         // д���쳣���˳�д��
						{
							f_close(&SDFile);
							return WS_Status_ERROR;
						}
					  if(++n % 20 != 0)
					     Debug(".")    ; 
						else
							 Debug("\r\n."); 
						
					 // float percentage = 	 (float)n / (file.fsize/4096+1) ;
					 //	WS_TFT_Dis_Debug(170,48,BLUE,"%3d %%",(int)(percentage*100));
				 }
				 else
				 {
					  //  �洢���ĸ��ֽ�Ϊ�ļ�����
					  if((SDFile.fsize-4) % 4096 == 0)
						{
						   buffer[ br ++]  =  check ;
               //rtn = WS_W25Qxx_Sector_Write(Sector++ ,buffer ,br) ;
							 rtn = WS_W25Qxx_Write(buffer,Sector++*WS_W25Qxx_Sector_Size , br) ;
							 if(rtn != WS_Status_OK)     // д���쳣���Ƴ�д��
							 {
								  f_close(&SDFile);
								  return WS_Status_ERROR;
							 }
						}			
						if(WS_W25Qxx_Check_File(W25QxxFileInfo) == WS_Status_OK)
						{
					    delay_1ms(1);
					    Debug("\r\nwrite %s finish\r\n",W25QxxFileInfo.name , SDFile.fsize);
						}
						f_close(&SDFile);
						return WS_Status_OK;
					}
				  res     = f_read (&SDFile, buffer, 4096, &br);
				}
		return WS_Status_ERROR ;
}



//   ���ڴ濨�����ֿ���Ϣ��W25Qxx
void WS_W25Qxx_Font_Chenk_Write(void)
{
	  uint16_t  i ;	
	  WS_StatusTypeDef   rtn ;
    for(i=0;i<sizeof(loadConfigFileInfo)/sizeof(WS_ConfigFileTypeDef);i++)
		  {
			   if(WS_W25Qxx_Check_File(loadConfigFileInfo[i]) == WS_Status_OK)
		     {
					  Debug("%s exist\r\n",loadConfigFileInfo[i].name);
					  if(i == 0)  W25QxxInfo.font16Flag = 1;
					  if(i == 1)  W25QxxInfo.font24Flag = 1;
					  if(i == 2)  W25QxxInfo.font32Flag = 1;
				 }
				 else
				 {	     
						#if (FS_FAT12 != 0 || FS_FAT16 != 0  || FS_FAT32 != 0 )
					      
					      if(sd_status != SD_OK)  return ;
					 
								FRESULT   retSD;
								retSD = f_mount(&SDFatFS, SDPath, 1);
								if(retSD)
								{
										Debug("TF card mount error\r\n",retSD);	
								}
								else
								{
									  rtn = 	WS_W25Qxx_Font_Write(loadConfigFileInfo[i]); 
                    if(rtn != WS_Status_OK)
										{
										   //WS_TFT_Clear(WHITE);
											 //WS_TFT_Dis_12X24_String(10,140,WS_Format("%s write error",loadConfigFileInfo[i].name),WHITE,RED);
										}	
                    else
										{
										   //WS_TFT_Clear(WHITE);
											 //WS_TFT_Dis_12X24_String(10,140,WS_Format("%s write ok",loadConfigFileInfo[i].name),WHITE,GREEN);
											 if(i == 0)  W25QxxInfo.font16Flag = 1;
											 if(i == 1)  W25QxxInfo.font24Flag = 1;
											 if(i == 2)  W25QxxInfo.font32Flag = 1;
										}	
                    delay_1ms(500);	
                    										
								}	  
						#endif
				 }
				 
			}  	
}













