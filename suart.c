#include "suart.h"
#include "stm32f1xx_hal_def.h"
#include <string.h>

uint8_t read_rx_pin(SUART_HandleTypeDef *hsuart);
void write_tx_pin(SUART_HandleTypeDef *hsuart, uint8_t state);
void tx_process(SUART_HandleTypeDef *hsuart);
void rx_process(SUART_HandleTypeDef *hsuart, uint8_t state);

HAL_StatusTypeDef HAL_SUART_Init(SUART_HandleTypeDef *hsuart) {
    if (hsuart->Init.TX_ENABLED && (!hsuart->Init.TX_PORT || !hsuart->Init.TX_PIN)) { return HAL_ERROR; }
    if (hsuart->Init.RX_ENABLED && (!hsuart->Init.RX_PORT || !hsuart->Init.RX_PIN)) { return HAL_ERROR; }

    hsuart->Instance->timer = 0;

    hsuart->Instance->tx_index = 0;
    hsuart->Instance->tx_size = 0;
    hsuart->Instance->tx_bit_shift = 0;
    hsuart->Instance->tx_bit_counter = 0;
    hsuart->Instance->tx_completed = 0;

    hsuart->Instance->rx_index = 0;
    hsuart->Instance->rx_bit_shift = 0;
    hsuart->Instance->rx_bit_counter = 0;
    hsuart->Instance->rx_completed = 0;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SUART_Handler(SUART_HandleTypeDef *hsuart) {
    if (hsuart->Init.TX_ENABLED) {
        if (hsuart->Instance->timer == 0 && !hsuart->Instance->tx_completed) {
            tx_process(hsuart);
        }
    }

    if (hsuart->Init.RX_ENABLED) {
        if (hsuart->Instance->timer == 1) {
            rx_process(hsuart, read_rx_pin(hsuart));
        }
    }

    // need to sample rx in middle of bit time
    // handler must be called at twice the speed of the baudrate

    hsuart->Instance->timer += 1;
    if (hsuart->Instance->timer > 1) { hsuart->Instance->timer = 0; }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SUART_Write(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len) {
    if (!hsuart->Init.TX_ENABLED) { return HAL_ERROR; }
    if (len >= sizeof(hsuart->Instance->tx_buf)) { return HAL_ERROR; }

    memcpy(hsuart->Instance->tx_buf, buf, len);

    hsuart->Instance->timer = 0;
    hsuart->Instance->tx_index = 0;
    hsuart->Instance->tx_size = len;
    hsuart->Instance->tx_completed = 0;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SUART_Read(SUART_HandleTypeDef *hsuart, uint8_t *buf, uint8_t len) {
    if (!hsuart->Init.RX_ENABLED) { return HAL_ERROR; }
    if (len > hsuart->Instance->rx_index) { return HAL_ERROR; }

    memcpy(buf, hsuart->Instance->rx_buf, len);

    hsuart->Instance->rx_index -= len;
    memmove(hsuart->Instance->rx_buf, (hsuart->Instance->rx_buf + len), hsuart->Instance->rx_index);

    return HAL_OK;
}


uint8_t HAL_SUART_Tx_Available(SUART_HandleTypeDef *hsuart) {
    return hsuart->Instance->tx_completed;
}

uint8_t HAL_SUART_Rx_Available(SUART_HandleTypeDef *hsuart) {
    return hsuart->Instance->rx_index;
}

uint8_t read_rx_pin(SUART_HandleTypeDef *hsuart) {
    return HAL_GPIO_ReadPin(hsuart->Init.RX_PORT, hsuart->Init.RX_PIN);
}

void write_tx_pin(SUART_HandleTypeDef *hsuart, uint8_t state) {
    HAL_GPIO_WritePin(hsuart->Init.TX_PORT, hsuart->Init.TX_PIN, state);
}

void tx_process(SUART_HandleTypeDef *hsuart) {
    if (hsuart->Instance->tx_bit_counter == 0) {
        hsuart->Instance->tx_bit_shift = 0;
        write_tx_pin(hsuart, 0);
        hsuart->Instance->tx_bit_counter += 1;
    }

    else if (hsuart->Instance->tx_bit_counter < 9) {
        uint8_t byte_val = (hsuart->Instance->tx_buf[hsuart->Instance->tx_index]);
        uint8_t bit_val = (byte_val >> hsuart->Instance->tx_bit_shift) & 0x1;

        write_tx_pin(hsuart, bit_val);
        hsuart->Instance->tx_bit_counter += 1;
        hsuart->Instance->tx_bit_shift += 1;
    }

    else if (hsuart->Instance->tx_bit_counter == 9) {
        write_tx_pin(hsuart, 1);
        hsuart->Instance->tx_bit_counter += 1;
    }


    else if (hsuart->Instance->tx_bit_counter == 10) {
        hsuart->Instance->tx_bit_counter = 0;
        hsuart->Instance->tx_index += 1;

        if (hsuart->Instance->tx_index < hsuart->Instance->tx_size) {
            hsuart->Instance->tx_completed = 0;
        }

        else {
            hsuart->Instance->tx_completed = 1;
        }
    }
}

void rx_process(SUART_HandleTypeDef *hsuart, uint8_t state) {
    if (hsuart->Instance->rx_bit_counter == 0) {
        if (state != 0) { return; }  // Start bit is 0

        hsuart->Instance->rx_bit_shift = 0;
        hsuart->Instance->rx_bit_counter += 1;
        hsuart->Instance->rx_buf[hsuart->Instance->rx_index] = 0;
    }

    else if (hsuart->Instance->rx_bit_counter < 9) {
        uint8_t bit_val = (state & 0x1) << hsuart->Instance->rx_bit_shift;
        hsuart->Instance->rx_buf[hsuart->Instance->rx_index] |= (bit_val);
        hsuart->Instance->rx_bit_shift += 1;
        hsuart->Instance->rx_bit_counter += 1;
    }

    else if (hsuart->Instance->rx_bit_counter == 9) {
        hsuart->Instance->rx_bit_counter = 0;

        if (state) {
            if (hsuart->Instance->rx_index < sizeof(hsuart->Instance->rx_buf)) {
                hsuart->Instance->rx_index += 1;
            }
        }
    }
}

