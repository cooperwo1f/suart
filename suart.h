#ifndef _SUART_H_
#define _SUART_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"
#include "stm32f1xx_hal_def.h"

#define SUART_TX_BUF_LEN 128
#define SUART_RX_BUF_LEN 128

typedef struct {
    uint8_t TX_ENABLED;
    GPIO_TypeDef *TX_PORT;
    uint16_t TX_PIN;

    uint8_t RX_ENABLED;
    GPIO_TypeDef *RX_PORT;
    uint16_t RX_PIN;

} SUART_InitTypeDef;

typedef struct {
    uint8_t timer;

    uint8_t tx_buf[SUART_TX_BUF_LEN];
    uint8_t tx_index;
    uint8_t tx_size;
    uint8_t tx_bit_shift;
    uint8_t tx_bit_counter;
    uint8_t tx_completed;

    uint8_t rx_buf[SUART_RX_BUF_LEN];
    uint8_t rx_index;
    uint8_t rx_bit_shift;
    uint8_t rx_bit_counter;
    uint8_t rx_completed;

} SUART_TypeDef; 

typedef struct {
    SUART_TypeDef *Instance;
    SUART_InitTypeDef Init;

} SUART_HandleTypeDef;

HAL_StatusTypeDef HAL_SUART_Init(SUART_HandleTypeDef *hsuart);
HAL_StatusTypeDef HAL_SUART_Handler(SUART_HandleTypeDef *hsuart);
HAL_StatusTypeDef HAL_SUART_Write(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len);
HAL_StatusTypeDef HAL_SUART_Read(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len);

uint8_t HAL_SUART_Tx_Available(SUART_HandleTypeDef *hsuart);
uint8_t HAL_SUART_Rx_Available(SUART_HandleTypeDef *hsuart);

#ifdef __cplusplus
}
#endif

#endif /* _SUART_H_ */
