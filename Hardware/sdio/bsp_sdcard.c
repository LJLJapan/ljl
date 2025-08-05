#include "bsp_sdcard.h"

sd_card_info_struct        sd_cardinfo           ;        /* information of SD card */
sd_error_enum              sd_status  = SD_ERROR       ;        /* status of SD card */

#define font_16_file_path  "H_16.dzk" 
#define font_24_file_path  "H_24.dzk" 
#define font_32_file_path  "H_32.dzk" 

/*!
    \brief      initialize the card, get the card information, set the bus mode and transfer mode
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_io_init(void)
{
    sd_error_enum status = SD_OK;
    uint32_t cardstate = 0      ;
	
	  if(sd_status  != SD_OK )
		{
			status = sd_init();
			if(SD_OK == status) {
					status = sd_card_information_get(&sd_cardinfo);
			}
			if(SD_OK == status) {
					status = sd_card_select_deselect(sd_cardinfo.card_rca);
			}
			status = sd_cardstatus_get(&cardstate);
			if(cardstate & 0x02000000) {
					Debug("\r\n the card is locked!");
					return SD_ERROR;
			}
			if((SD_OK == status) && (!(cardstate & 0x02000000))) {
					/* set bus mode */
					status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
	//        status = sd_bus_mode_config(SDIO_BUSMODE_1BIT);
			}
			if(SD_OK == status) {
					/* set data transfer mode */
	//        status = sd_transfer_mode_config(SD_DMA_MODE);
					status = sd_transfer_mode_config(SD_POLLING_MODE);
			}
			sd_status  = status ;
	  }
    return status;
}

/*!
    \brief      get the card information and print it out by USRAT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void card_info_printf(void)
{
    uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
//    uint32_t block_count, block_size;
    uint16_t temp_ccc;
    Debug("\r\n Card information:");
    sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
    sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
    sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
    if(2 == sd_spec) {
        if(1 == sd_spec3) {
            if(1 == sd_spec4) {
                Debug("\r\n## Card version 4.xx ##");
            } else {
                Debug("\r\n## Card version 3.0x ##");
            }
        } else {
            Debug("\r\n## Card version 2.00 ##");
        }
    } else if(1 == sd_spec) {
        Debug("\r\n## Card version 1.10 ##");
    } else if(0 == sd_spec) {
        Debug("\r\n## Card version 1.0x ##");
    }

    sd_security = (sd_scr[1] & 0x00700000) >> 20;
    if(2 == sd_security) {
        Debug("\r\n## SDSC card ##");
    } else if(3 == sd_security) {
        Debug("\r\n## SDHC card ##");
    } else if(4 == sd_security) {
        Debug("\r\n## SDXC card ##");
    }

//    block_count = (sd_cardinfo.card_csd.c_size + 1) * 1024;
//   block_size = 512;
    //Debug("\r\n## Device size is %d MB ##", sd_card_capacity_get()/1024);
		Debug("\r\n## Device size is %d Nbr , %d MB ##", sd_cardinfo.card_capacity/512,sd_cardinfo.card_capacity/1024/1024);
    Debug("\r\n## Block size is % d B ##", sd_cardinfo.card_blocksize);
    Debug("\r\n## Block count is % d ##",  sd_cardinfo.card_capacity/sd_cardinfo.card_blocksize);

    if(sd_cardinfo.card_csd.read_bl_partial) {
        Debug("\r\n## Partial blocks for read allowed ##");
    }
    if(sd_cardinfo.card_csd.write_bl_partial) {
        Debug("\r\n## Partial blocks for write allowed ##");
    }
    temp_ccc = sd_cardinfo.card_csd.ccc;
    Debug("\r\n## CardCommandClasses is: %x ##", temp_ccc);
    if((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc)) {
        Debug("\r\n## Block operation supported ##");
    }
    if(SD_CCC_ERASE & temp_ccc) {
        Debug("\r\n## Erase supported ##");
    }
    if(SD_CCC_WRITE_PROTECTION & temp_ccc) {
        Debug("\r\n## Write protection supported ##");
    }
    if(SD_CCC_LOCK_CARD & temp_ccc) {
        Debug("\r\n## Lock unlock supported ##");
    }
    if(SD_CCC_APPLICATION_SPECIFIC & temp_ccc) {
        Debug("\r\n## Application specific supported ##");
    }
    if(SD_CCC_IO_MODE & temp_ccc) {
        Debug("\r\n## I/O mode supported ##");
    }
    if(SD_CCC_SWITCH & temp_ccc) {
        Debug("\r\n## Switch function supported ##");
    }
}

//   SD 卡读写测试
void card_test(void)
{
	  sd_error_enum sd_error;
  	uint8_t buf_write[512];                                    /* store the data written to the card */
    uint8_t buf_read [512];                                     /* store the data read from the card */

	  int i ;
      /* init the write buffer */
    for(i = 0; i < 512; i++) {
        buf_write[i] = i;
    }

    Debug("\r\n\r\n Card test:");

    /* single block operation test */
    sd_error = sd_block_write((uint32_t *)buf_write, 100 * 512, 512);
    if(SD_OK != sd_error) {
        Debug("\r\n Block write fail!");
        
    } else {
        Debug("\r\n Block write success!");
    }
		
    sd_error = sd_block_read((uint32_t *)buf_read, 0 * 512, 512);
    if(SD_OK != sd_error) {
        Debug("\r\n Block read fail!");
       
    } else {
        Debug("\r\n Block read success!");
    }

    /* lock and unlock operation test */
    if(SD_CCC_LOCK_CARD & sd_cardinfo.card_csd.ccc) {
        /* lock the card */
        sd_error = sd_lock_unlock(SD_LOCK);
        if(SD_OK != sd_error) {
            Debug("\r\n Lock failed!");
        } else {
            Debug("\r\n The card is locked!");
        }
        sd_error = sd_erase(100 * 512, 101 * 512);
        if(SD_OK != sd_error) {
            Debug("\r\n Erase failed!");
        } else {
            Debug("\r\n Erase success!");
        }

        /* unlock the card */
        sd_error = sd_lock_unlock(SD_UNLOCK);
        if(SD_OK != sd_error) {
            Debug("\r\n Unlock failed!");
           
        } else {
            Debug("\r\n The card is unlocked!");
        }
				
				
        sd_error = sd_erase(100 * 512, 101 * 512);
        if(SD_OK != sd_error) {
            Debug("\r\n Erase failed!");
        } else {
            Debug("\r\n Erase success!");
        }

        sd_error = sd_block_read((uint32_t *)buf_read, 100 * 512, 512);
        if(SD_OK != sd_error) {
            Debug("\r\n Block read fail!");
        } else {
            Debug("\r\n Block read success!");
        }
    }

    /* multiple blocks operation test */
    sd_error = sd_multiblocks_write((uint32_t *)buf_write, 200 * 512, 512, 1);
    if(SD_OK != sd_error) {
        Debug("\r\n Multiple block write fail!");
       
    } else {
        Debug("\r\n Multiple block write success!");
    }
		
    sd_error = sd_multiblocks_read((uint32_t *)buf_read, 0 * 512, 512, 1);
    if(SD_OK != sd_error) {
        Debug("\r\n Multiple block read fail!");
    } else {
        Debug("\r\n Multiple block read success!");  
    }
}



//  初始化SD卡
sd_error_enum WS_SD_init(void)
{
    uint16_t i = 5        ;
    sd_error_enum sd_rtn;

    /* initialize the card */
    do 
	  {
      sd_rtn = sd_io_init();
    } while((SD_OK != sd_rtn) && (--i)); //  最多五次初始化

    if(SD_OK == sd_rtn) 
		{
        Debug("\r\nCard init success!\r\n");
    } 
		else 
		{
        Debug("\r\nCard init failed!\r\n");
        return sd_rtn ;
    }
    sd_card_information_get(&sd_cardinfo)          ;
		float fCardSize = 1.0*sd_cardinfo.card_capacity/1024/1024;
		Debug("TF card size:%.1f M\r\n", fCardSize);
		
#if (FS_FAT12 != 0 || FS_FAT16 != 0  || FS_FAT32 != 0 ) 
		
		MX_FATFS_Init() ;	
		FRESULT   retSD ;
		retSD = f_mount(&SDFatFS, SDPath, 1);
		if(retSD == FR_OK)
	  {
		 FRESULT  fre ;
		 fre = f_open(&SDFile , "0:/test.txt",FA_READ | FA_WRITE );
		 if( fre != FR_OK)
		 {
			  fre = f_open(&SDFile , "0:/test.txt",FA_READ | FA_WRITE | FA_CREATE_NEW);
			  if(fre == FR_OK)
				{
				    Debug("test.txt file create ok %d\r\n");  delay_1ms(5);
					  uint32_t bw;
            if(f_write(&SDFile,"SD card write test",18,&bw) == FR_OK)
						{
						   Debug("test.txt file write ok %d\r\n\r\n");  delay_1ms(5);
						}
            f_close(&SDFile);						
				}
		  }
			else
			{
					uint32_t fileSize = SDFile.fsize;
					Debug("test.txt file size %d\r\n",fileSize);  delay_1ms(5); 
					if(fileSize < 128)
					{
						char  buff[129];
						uint32_t br;
						if( f_read(&SDFile,buff,SDFile.fsize,&br) == FR_OK)
						{
							 buff[fileSize] = 0;
							 Debug("test.txt: %s\r\n\r\n",buff);       delay_1ms(10); 
						}
					}
					f_close(&SDFile);
				 //Debug("test.txt no open or creat fail \r\n\r\n");  HAL_Delay(5); 
			}		
	}
	else
	{
	   Debug("\r\nSD card mount ERR \r\n\r\n");        delay_1ms(5); 
	}
		
								
#endif								
   return SD_OK;
}









