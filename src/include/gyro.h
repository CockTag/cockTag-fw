#ifndef GYRO_H
#define GYRO_H

#include "boards.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define GYRO_ADDRESS 0x0E
#define GYRO_WHO_AM_I 0x0F 

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Number of possible TWI addresses. */
#define TWI_ADDRESSES      127

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void gyro_init (void);
void gyro_read (void);

#endif