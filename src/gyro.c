#include "gyro.h"

/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    NRF_LOG_DEBUG("TWI init\n");
    const nrf_drv_twi_config_t twi_config = {
       .scl                = GYRO_SCL,
       .sda                = GYRO_SDA,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };
    NRF_LOG_DEBUG("SCL: %d\n", GYRO_SCL);
    NRF_LOG_DEBUG("SDA: %d\n", GYRO_SDA);
    NRF_LOG_DEBUG("Freq: %d\n", NRF_TWI_FREQ_100K);
    NRF_LOG_DEBUG("IRQ: %d\n", APP_IRQ_PRIORITY_HIGH);
    NRF_LOG_DEBUG("Clear: %d\n", false);

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_drv_twi_enable(&m_twi);
}

uint8_t read_register(uint8_t reg)
{
    ret_code_t err_code;
    uint8_t data;
    err_code = nrf_drv_twi_tx(&m_twi, GYRO_ADDRESS, &reg, 1, true);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_twi_rx(&m_twi, GYRO_ADDRESS, &data, 1);
    APP_ERROR_CHECK(err_code);
    return data;
}

void gyro_init(void)
{
    twi_init();
    NRF_LOG_DEBUG("GYRO Init\n");   
    if(read_register(GYRO_WHO_AM_I) == 0x35)
    {
        NRF_LOG_DEBUG("GYRO found\n");
    }
    else
    {
        NRF_LOG_DEBUG("GYRO not found\n");
    }
}

void gyro_read(void)
{
    return;
}