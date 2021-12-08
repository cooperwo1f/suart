#ifndef _SUART_H_
#define _SUART_H_

#include "main.h"
#include "stm32f1xx_hal_def.h"

#define SUART_TX_BUF_LEN 128
#define SUART_RX_BUF_LEN 128

typedef struct {
    uint8_t tx_enabled;
    uint8_t tx_port;
    uint8_t tx_pin;

    uint8_t rx_enabled;
    uint8_t rx_port;
    uint8_t rx_pin;

} SUART_InitTypeDef;

typedef struct {
    uint8_t timer;

    uint8_t tx_buf[SUART_TX_BUF_LEN];
    uint8_t tx_index;
    uint8_t tx_size;
    uint8_t tx_bit_shift;
    uint8_t tx_bit_counter;
    uint8_t tx_complete;

    uint8_t rx_buf[SUART_RX_BUF_LEN];
    uint8_t rx_index;
    uint8_t rx_bit_shift;
    uint8_t rx_bit_counter;
    uint8_t rx_complete;

} SUART_TypeDef; 

typedef struct {
    SUART_TypeDef *Instance;
    SUART_InitTypeDef Init;

} SUART_HandleTypeDef;

HAL_StatusTypeDef SUART_init(SUART_HandleTypeDef *hsuart);
HAL_StatusTypeDef SUART_handler(SUART_HandleTypeDef *hsuart);
HAL_StatusTypeDef SUART_write(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len);
HAL_StatusTypeDef SUART_read(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len);

uint8_t SUART_tx_available(SUART_HandleTypeDef *hsuart);
uint8_t SUART_rx_available(SUART_HandleTypeDef *hsuart);

#endif /* _SUART_H_ */
