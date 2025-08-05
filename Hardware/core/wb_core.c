#include "wb_core.h"



//   ��ʽ���ɱ�����ַ�������
char  formatBuf[512];
//   ��ʽ���ɱ�����ַ���
char *WB_Format(char *p,...)
{
	va_list ap;
		va_start(ap,p);
		vsprintf(formatBuf,p,ap);
		va_end(ap);
	  return formatBuf;
}

/**
 * @brief �����ַ���
 * @param buff �������ַ���
 * @param input ��Ҫ���ҵ��ַ���
 * @return buff�ַ����������input�򷵻� 0,���򷵻�1
 */
char WB_string_seek(char *buff, char *input)
{
	  if (strstr((const char *)buff, (const char *)input) != NULL)
		  return 0;
	  return 1;
}

/********************************************************************************
	*�������ƣ� long WB_string_int(char *buf,int MIN,int MAX)
	*���ܣ�   	�����ַ�ת���ɳ�����
	*������    	bufĿ���ַ���
				MIN ��ʼ   
				MAX	����
				�����������ַ��˳�
	*						
	*���أ�		�����ַ�ת���ɳ����͵�ֵ
*********************************************************************************/
long WB_string_int(char *buf,int MIN,int MAX)
{
	long size=0;
	int i=0;
	int cc=strlen(buf);
	for(i=MIN;i<MAX;i++)
	{
		if(buf[i]>='0'&&buf[i]<='9')
		{
			size=(size*10)+(buf[i]-'0');
		}
		else break;
	}
	return size;
}

/********************************************************************************
	*�������ƣ� WB_string_bytes(char *A_buf,char *B_buf,char STOP_CHAR,int index)
	*���ܣ�   	��A�����в����ַ�������B������
				���STOP_CHAR��','��index��0
				��ô�ַ�������xxxxxx,֮ǰ��x
				
	*������    	A_buf Ŀ���ַ���
				B_buf ���ڴ�����ҵ����ַ���  
				STOP_CHAR ��ʼ��־λ��ֹͣ��־λ
				index ����������־λ
	*						
	*���أ�		���ҳɹ�����1��ʧ�ܷ���0
*********************************************************************************/
char WB_string_bytes(char *A_buf,char *B_buf,char STOP_CHAR,int index)
{
	
	   int strnum = 0; //  ��¼�����ַ�����
	
			*B_buf=0;
		while( *A_buf!=0)
		{
			if(*A_buf  == STOP_CHAR)
			{
				strnum++;
				if(strnum==index)
				{
					 A_buf ++;
					 while(*A_buf != 0 && *A_buf != STOP_CHAR)
							 {
							   *B_buf = *A_buf;
								  A_buf ++ ;
								  B_buf ++;
							 }
							 *B_buf = 0;
							 return 1;
				}
						
			}
			
			A_buf++;
		}
		*B_buf = 0;
		return 0;

}


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

//��ʼ��ʱ���
void WB_dwt_init(void)
{
    /* ʹ��DWT���� */
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    /* DWT CYCCNT�Ĵ���������0 */
    DWT_CYCCNT = (uint32_t)0u;

    /* ʹ��Cortex-M DWT CYCCNT�Ĵ��� */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}


//   ���������ֵ
uint32_t  WB_getDWT_CountDt(uint32_t startTick , uint32_t  stopTick )
{
	   uint32_t tick;
     if(stopTick < startTick)
			{
				 tick = (0xffffffff - startTick) + stopTick  + 1 ;
			}
			else
			{
				 tick = stopTick - startTick;
			}
		 return tick;
}

uint32_t WB_getDWT_Count(void)
{
	return DWT_CYCCNT;
}


