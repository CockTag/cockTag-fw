#ifndef GYRO_H
#define GYRO_H

#include "boards.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#define GYRO_ADDRESS 0x0E

// Read only registers
#define GYRO_WHO_AM_I 0x0F // Should return 0x35
#define GYRO_INT_REL 0x1A // Interrupt release? 

// Read/Write registers
#define GYRO_CTRL_REG1 0x1B
#define GYRO_CTRL_REG2 0x1D
#define GYRO_INT_CTRL_REG1 0x1E
#define GYRO_INT_CTRL_REG2 0x1F
#define GYRO_DATA_CTRL_REG 0x21
#define GYRO_WAKEUP_COUNTER 0x29
#define GYRO_NA_COUNTER 0x2A
#define GYRO_WAKEUP_THRD_H 0x6A
#define GYRO_WAKEUP_THRD_L 0x6B

// Write only registers
#define GYRO_RESET 0x3A 

// Gyro configuration
#define INT_PULL NRF_GPIO_PIN_PULLDOWN

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Number of possible TWI addresses. */
#define TWI_ADDRESSES      127

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void gyro_init (void);
void gyro_read (void);

#endif