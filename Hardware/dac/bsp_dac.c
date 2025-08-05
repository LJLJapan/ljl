

#include "bsp_dac.h"

/*!
    \brief      configure the RCU of peripherals
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_rcu_config(void)
{
    /* enable the clock of peripherals */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
}

/*!
    \brief      configure the related GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_gpio_config(void)
{
    /* once enabled the DAC, the corresponding GPIO pin is connected to the DAC converter automatically */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);
}

/*!
    \brief      configure the DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_dac_init(void)
{
	  dac_rcu_config();
	  dac_gpio_config();
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_disable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);
    
    /* configure the DAC1 */
    dac_trigger_disable(DAC1);
    dac_wave_mode_config(DAC1, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC1);
    
    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    set_dac_vol(0.2f,0.8f);
}

//   
void set_dac_vol(float v0 , float v1)
{
     dac_concurrent_data_set(DAC_ALIGN_12B_R, v0 * 4095u / 3.328f, v1 * 4095u / 3.328f); 
}
