#include "gyro.h"
#include <stdint.h>

volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;
volatile static uint32_t timeout;

static void nrf_drv_mpu_twi_event_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch(p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
        {
            switch(p_event->xfer_desc.type)
            {
                case NRF_DRV_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        }
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
    }
}

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

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, nrf_drv_mpu_twi_event_handler, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_drv_twi_enable(&m_twi);
}

uint8_t gyro_read_register(uint8_t reg)
{
    ret_code_t err_code;
    uint8_t data;
    err_code = nrf_drv_twi_tx(&m_twi, GYRO_ADDRESS, &reg, 1, true);
    APP_ERROR_CHECK(err_code);
    timeout = 100;
    while((!twi_tx_done) && timeout)
    {
        nrf_delay_ms(1);
        timeout--;
    }
    if(!twi_tx_done)
    {
        NRF_LOG_DEBUG("GYRO: READ TIMEOUT\n");
    }
    twi_tx_done = false;

    err_code = nrf_drv_twi_rx(&m_twi, GYRO_ADDRESS, &data, 1);
    APP_ERROR_CHECK(err_code);
    timeout = 100;
    while((!twi_rx_done) && timeout)
    {
        nrf_delay_ms(1);
        timeout--;
    }
    if(!twi_rx_done)
    {
        NRF_LOG_DEBUG("GYRO: READ ACK TIMEOUT\n");
    }
    twi_tx_done = false;
    return data;
}

void gyro_write_register(uint8_t reg, uint8_t data)
{
    ret_code_t err_code;
    uint8_t tx_data[2];
    tx_data[0] = reg;
    tx_data[1] = data;
    err_code = nrf_drv_twi_tx(&m_twi, GYRO_ADDRESS, tx_data, 2, false);
    APP_ERROR_CHECK(err_code);

    //Wait until twi_tx_done is true with 100ms timeout
    timeout = 100;
    while((!twi_tx_done) && timeout)
    {
        nrf_delay_ms(1);
        timeout--;
    }
    if(!twi_tx_done)
    {
        NRF_LOG_DEBUG("GYRO: WRITE TIMEOUT\n");
    }
    twi_tx_done = false;
}

bool gyro_get_bit(uint8_t const value, uint8_t const bit_number){ 
        return (value&(1<<bit_number)) >> bit_number;
}

uint8_t gyro_flip_bit(uint8_t const value, uint8_t const bit_number){ 
	return value ^ (1 << bit_number);
}

void set_standby(bool standby) {
    uint8_t old_value = gyro_read_register(GYRO_CTRL_REG1);
	uint8_t current_mode = gyro_get_bit(old_value, 7);

    if (current_mode == standby) {
        old_value = gyro_flip_bit(old_value, 7);
    }
    gyro_write_register(GYRO_CTRL_REG1, old_value);
}

void interrupt_handler(long unsigned int pin, nrf_gpiote_polarity_t polarity){
    uint8_t reg = gyro_read_register(GYRO_INT_REL);
    NRF_LOG_DEBUG("GYRO: interrupt release %X\n", reg);
}

void interrupt_init() {
  ret_code_t err_code;
  nrf_drv_gpiote_in_config_t in_config = {
      .sense = INT_PULL == NRF_GPIO_PIN_PULLUP ? NRF_GPIOTE_POLARITY_HITOLO
                                                  : NRF_GPIOTE_POLARITY_LOTOHI,
      .pull = INT_PULL,
      .hi_accuracy = false,
      .is_watcher = false};
  err_code = nrf_drv_gpiote_in_init(GYRO_INT, &in_config, interrupt_handler);
  APP_ERROR_CHECK(err_code);
  nrf_drv_gpiote_in_event_enable(GYRO_INT, true);
}

void gyro_init(void)
{
    twi_init();
    NRF_LOG_DEBUG("GYRO Init\n");   
    if(gyro_read_register(GYRO_WHO_AM_I) == 0x35)
    {
        NRF_LOG_DEBUG("GYRO found\n");
        interrupt_init();
        NRF_LOG_DEBUG("GYRO: interupt initialised\n");
        gyro_write_register(GYRO_CTRL_REG1, 0x00); // Set to standby, reset the register
        gyro_write_register(GYRO_CTRL_REG1, 0x12); // Standby, low-current, no int data, +-8g range, WUFE enabled
        // No need to set anything in CTRL_REG2 since the default 0x0 value is fine? Also, it has some reserved bits
        gyro_write_register(GYRO_INT_CTRL_REG1, 0x32); // Interrupt enabled, active HIGH, latched
        gyro_write_register(GYRO_INT_CTRL_REG1, 0x3F); // Latched, any direction interrupt
        gyro_write_register(GYRO_DATA_CTRL_REG, 0x08); // 0.781Hz, lowest possible ODR
        /*
        Minimal time beetwen interrupts
        Formula: WAKEUP_COUNTER = Desired seconds / ODR (0.781Hz)
        1s = 1.28; Minimal value is 1
        */
        gyro_write_register(GYRO_WAKEUP_COUNTER, (uint8_t) 1); 
        /* 
        Wakeup threshold, 12 bits stored in two separate registers
        Formula: WAKEUP_THRESHOLD (counts) = Desired Threshold (g) x 256 (counts/g)
        0x8, 0x0 is 0.5g acc
        Last lower 4 bits should be always 0
        */
        gyro_write_register(GYRO_WAKEUP_THRD_H, 0x2); 
        gyro_write_register(GYRO_WAKEUP_THRD_L, 0x0);
        gyro_write_register(GYRO_CTRL_REG1, 0x92); // Active, low-current, no int data, +-8g range, WUFE enabled
        NRF_LOG_DEBUG("GYRO: registers set\n");
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