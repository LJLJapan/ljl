#ifndef __BSP_SDCARD__
#define __BSP_SDCARD__

#include "main.h"

extern  sd_card_info_struct sd_cardinfo;                            /* information of SD card */
extern  sd_error_enum sd_status ;

sd_error_enum sd_io_init(void);
void card_info_printf(void)   ;

//   SD ø®∂¡–¥≤‚ ‘
void card_test(void);

//  ≥ı ºªØSDø®
sd_error_enum WS_SD_init(void);

char WS_SDcard_Font16_Read_Date(uint8_t * buffer  , uint8_t *string);
char WS_SDcard_Font24_Read_Date(uint8_t * buffer  , uint8_t *string);
char WS_SDcard_Font32_Read_Date(uint8_t * buffer  , uint8_t *string);



#endif
