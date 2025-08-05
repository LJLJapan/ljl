#include "wb_core.h"



//   格式化可变变量字符串缓存
char  formatBuf[512];
//   格式化可变变量字符串
char *WB_Format(char *p,...)
{
	va_list ap;
		va_start(ap,p);
		vsprintf(formatBuf,p,ap);
		va_end(ap);
	  return formatBuf;
}

/**
 * @brief 查找字符串
 * @param buff 被查找字符串
 * @param input 需要查找的字符串
 * @return buff字符串里面存在input则返回 0,否则返回1
 */
char WB_string_seek(char *buff, char *input)
{
	  if (strstr((const char *)buff, (const char *)input) != NULL)
		  return 0;
	  return 1;
}

/********************************************************************************
	*函数名称： long WB_string_int(char *buf,int MIN,int MAX)
	*功能：   	数字字符转换成长整型
	*参数：    	buf目标字符串
				MIN 开始   
				MAX	结束
				碰到非数字字符退出
	*						
	*返回：		数字字符转换成长整型的值
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
	*函数名称： WB_string_bytes(char *A_buf,char *B_buf,char STOP_CHAR,int index)
	*功能：   	在A数组中查找字符串存入B数组中
				如果STOP_CHAR是','，index是0
				那么字符串就是xxxxxx,之前的x
				
	*参数：    	A_buf 目标字符串
				B_buf 用于储存查找到的字符串  
				STOP_CHAR 开始标志位和停止标志位
				index 跳过几个标志位
	*						
	*返回：		查找成功返回1，失败返回0
*********************************************************************************/
char WB_string_bytes(char *A_buf,char *B_buf,char STOP_CHAR,int index)
{
	
	   int strnum = 0; //  记录查找字符串数
	
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

//初始化时间戳
void WB_dwt_init(void)
{
    /* 使能DWT外设 */
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    /* DWT CYCCNT寄存器计数清0 */
    DWT_CYCCNT = (uint32_t)0u;

    /* 使能Cortex-M DWT CYCCNT寄存器 */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}


//   计算计数差值
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


