#include "gyro.h"

volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;
volatile static uint32_t timeout;

static void nrf_drv_mpu_twi_event_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch(p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
        {
            NRF_LOG_DEBUG("GYRO: GOT EV\n");
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
      NRF_LOG_DEBUG("GYRO: READ START %X\n", reg);

    ret_code_t err_code;
    uint8_t data;
    err_code = nrf_drv_twi_tx(&m_twi, GYRO_ADDRESS, &reg, 1, true);
    APP_ERROR_CHECK(err_code);
    timeout = 500;
    while((!twi_tx_done) && timeout)
    {
        nrf_delay_ms(1);
        timeout--;
    }
    if(!twi_tx_done)
    {
        NRF_LOG_DEBUG("GYRO: READ ACK TIMEOUT\n");
    }
    twi_tx_done = false;

    err_code = nrf_drv_twi_rx(&m_twi, GYRO_ADDRESS, &data, 1);
    APP_ERROR_CHECK(err_code);
    timeout = 500;
    while((!twi_tx_done) && timeout)
    {
        nrf_delay_ms(1);
        timeout--;
    }
    if(!twi_tx_done)
    {
        NRF_LOG_DEBUG("GYRO: READ ACK TIMEOUT\n");
    }
    twi_tx_done = false;
    return data;
}

void gyro_write_register(uint8_t reg, uint8_t data)
{
    NRF_LOG_DEBUG("GYRO: WRITE START %X\n", reg);
    ret_code_t err_code;
    uint8_t tx_data[2];
    tx_data[0] = reg;
    tx_data[1] = data;
    err_code = nrf_drv_twi_tx(&m_twi, GYRO_ADDRESS, tx_data, 2, false);
    APP_ERROR_CHECK(err_code);

    //Wait until twi_tx_done is true with 100ms timeout
    timeout = 500;
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
    NRF_LOG_DEBUG("GYRO: interrupt\n");
}

void interrupt_init() {
  NRF_LOG_DEBUG("GYRO: setting interrupt\n");
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
        // gyro_write_register(GYRO_NA_COUNTER, 0x08); // 0.781Hz
        gyro_write_register(GYRO_CTRL_REG, 0x08); // 0.781Hz
        gyro_write_register(GYRO_CTRL_REG1, 0x80 | (0x00 << 2)); // Papieskie

        set_standby(true);
        NRF_LOG_DEBUG("GYRO set pooling p\n");
        gyro_write_register(GYRO_INT_CTRL_REG1, 0x22); // Interrupt enabled, active LOW, non-latched
        uint8_t reg = gyro_read_register(GYRO_CTRL_REG1);
        NRF_LOG_DEBUG("GYRO set int\n");
        gyro_write_register(GYRO_CTRL_REG1, reg | (0x01 << 1)); // Wake-Up (motion detect)
        NRF_LOG_DEBUG("GYRO set motion detect\n");
        gyro_write_register(GYRO_CTRL_REG2, 0xBF); // enable interrupt on all axis any direction - Unlatched
        NRF_LOG_DEBUG("GYRO int on all axis\n");
        gyro_write_register(GYRO_WAKEUP_THRD_H, (uint8_t) (WAKEUP_THRESHOLD >> 4)); 
        gyro_write_register(GYRO_WAKEUP_THRD_L, (uint8_t) (WAKEUP_THRESHOLD << 4));
        NRF_LOG_DEBUG("GYRO set int wakeup threshold\n");

        gyro_write_register(GYRO_WAKEUP_COUNTER, (uint8_t) 1); 
        gyro_write_register(GYRO_NA_COUNTER, (uint8_t) 10);
        NRF_LOG_DEBUG("GYRO set counters\n");

        set_standby(false);
        NRF_LOG_DEBUG("GYRO disable gyro standby\n");

        interrupt_init();
        NRF_LOG_DEBUG("GYRO set nrf interrupt\n");
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