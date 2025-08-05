
#include "bsp_spi.h"

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi4_rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SPI4);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi4_gpio_config(void)
{
    /* configure SPI4 GPIO */
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_bit_write(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, SET);

    /* set SPI4_NSS as GPIO*/
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_bit_write(GPIOF, GPIO_PIN_6, SET);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi4_config(void)
{
    spi_parameter_struct spi_init_struct;

    /* configure SPI1 parameter */
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_16;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI4, &spi_init_struct);

    spi_enable(SPI4);
}

/*!
    \brief      init the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi4_init(void)
{
    spi4_rcu_config();
    spi4_gpio_config();
    spi4_config();
}
