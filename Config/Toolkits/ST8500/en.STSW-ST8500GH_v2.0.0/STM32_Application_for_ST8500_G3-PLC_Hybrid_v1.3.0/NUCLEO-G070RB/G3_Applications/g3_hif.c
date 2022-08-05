/**
  ******************************************************************************
  * @file    g3_hif.c
  * @author  AMG/IPC Application Team
  * @brief   Implements the low-level msg handling
  @verbatim
  @endverbatim

  ******************************************************************************
  * @attention
  *
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2021 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include <g3_hif.h>
#include <cmsis_os2.h>
#include <assert.h>
#include <string.h>
#include <mem_pool.h>
#include <crc.h>
#include <stm32g070xx.h>
#include <stm32g0xx_hal.h>
#include <stm32g0xx_ll_usart.h> /* if not already, file has to be added in corresponding \Drivers\STM32Yyxx_HAL_Driver */
#include <usart.h>

/** @defgroup g3_hif_uart G3 HOST INTERFACE UART HANDLER
  * @{
  */

#define FIELD_COUNTER_LEN       4U
#define FIELD_MSGLEN_LEN        2U
#define FIELD_TAG_LEN           8U
#define FIELD_CRC_LEN           2U

typedef enum {
    UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST = 0,
    UART_RX_PARSER_ST_SEARCH_PREAMBLE_2ND,
    UART_RX_PARSER_ST_GET_CMD_ID,
    UART_RX_PARSER_ST_GET_MSG_LEN,
    UART_RX_PARSER_ST_GET_SEC_MODE,
    UART_RX_PARSER_ST_GET_COUNTER,
    UART_RX_PARSER_ST_GET_PAYLOAD,
    UART_RX_PARSER_ST_GET_TAG,
    UART_RX_PARSER_ST_GET_CRC,
    UART_RX_PARSER_ST_CNT
} uart_rx_parser_state_t;

#pragma pack(1)

typedef struct {
    uint16_t    sync;
    uint8_t     cmd_id;
    uint16_t    len;
    uint8_t     mode;
    uint32_t    cnt;
    uint8_t     data[];
} g3_hi_tx_msg_t;

typedef struct {
    uint16_t    sync;
    uint8_t     cmd_id;
    uint16_t    len;
    uint8_t     mode;
    uint32_t    cnt;
    uint8_t     ec;
    uint8_t     data[];
} g3_hi_rx_msg_t;

#pragma pack()

static struct {
    uint8_t *payload_buffer;
    uint16_t payload_len;
    uart_rx_parser_state_t state;
    uint16_t buff_idx;
    uint16_t multibyte_field_cnt;
    uint8_t cmd_id;
    uint16_t msg_len;
    uint8_t sec_mode;
    uint16_t crc;
    bool rx_ongoing;
} uart_rx_parser_data;

static struct {
    uint16_t msg_len;
    uint16_t buff_idx;
    bool tx_ongoing;
} uart_tx_handler_data;

static task_comm_info_t *g3_task_comm_info;

static uint8_t *uart_rx_parser_data_buffer;
static uint8_t uart_tx_handler_data_buffer[MEM_BUFFER_SIZE];

bool hif_uart_is_free(void)
{
    return !(uart_tx_handler_data.tx_ongoing || uart_rx_parser_data.rx_ongoing);
}

static void hif_uart_tx_handler_init(void)
{
    memset(&uart_tx_handler_data, 0, sizeof(uart_tx_handler_data));
}

static void hif_uart_tx_send_first_byte(void)
{
    uart_tx_handler_data.tx_ongoing = true;
    LL_USART_TransmitData8(USART1, uart_tx_handler_data_buffer[uart_tx_handler_data.buff_idx++]);
}

const void *hif_rx_get_msg_payload(const void *buff, uint16_t *len)
{
    const g3_hi_rx_msg_t *msg = buff;

    if (len)
        *len = msg->len - 1; // we subtract the ec field len to get the payload

    return msg->data;
}

bool hif_uart_send_message(uint8_t cmd_id, void *buff, uint16_t len)
{
    g3_hi_tx_msg_t *msg;
    uint16_t crc, complete_msg_len = len + sizeof(*msg);

    if (uart_tx_handler_data.tx_ongoing || uart_rx_parser_data.rx_ongoing)
        return false;

    uart_tx_handler_data.msg_len = complete_msg_len + sizeof(crc);

    msg = (g3_hi_tx_msg_t *)uart_tx_handler_data_buffer;

    msg->sync = 0x1616U;
    msg->cmd_id = cmd_id;
    msg->len = len;
    msg->mode = 0U;
    msg->cnt = 0U;

    if (len && buff)
        memcpy(msg->data, buff, len);

    crc = crc_calc(msg, complete_msg_len);

    uart_tx_handler_data_buffer[complete_msg_len] = (uint8_t)(crc & 0xFFU);
    uart_tx_handler_data_buffer[complete_msg_len + 1] = (uint8_t)((crc >> 8) & 0xFFU);

    hif_uart_tx_send_first_byte();

    return true;
}

bool hif_uart_tx_is_ongoing(void)
{
    return uart_tx_handler_data.tx_ongoing;
}

bool g3_hif_uart_tx_handler(uint8_t *tx_data)
{
    assert(uart_tx_handler_data.buff_idx <= uart_tx_handler_data.msg_len);

    if (uart_tx_handler_data.buff_idx == uart_tx_handler_data.msg_len) {
        hif_uart_tx_handler_init();
        return false;
    } else {
        *tx_data = uart_tx_handler_data_buffer[uart_tx_handler_data.buff_idx++];
        return true;
    }
}

static void hif_uart_rx_handler_init(void)
{
    memset(&uart_rx_parser_data, 0, sizeof(uart_rx_parser_data));
    uart_rx_parser_data_buffer = MEM_ALLOC(MEM_BUFFER_SIZE);

    assert(uart_rx_parser_data_buffer != NULL);
}

static uart_rx_parser_state_t uart_rx_parser_search_preamble(uint8_t rx_data,
                                                             uart_rx_parser_state_t curr_state)
{
    assert((UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST == curr_state) ||
           (UART_RX_PARSER_ST_SEARCH_PREAMBLE_2ND == curr_state));

    if (0x16U == rx_data) {
        uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;

        if (UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST == curr_state)
            return UART_RX_PARSER_ST_SEARCH_PREAMBLE_2ND;
        else {
            uart_rx_parser_data.rx_ongoing = true;
            return UART_RX_PARSER_ST_GET_CMD_ID;
        }
    } else
        return curr_state;
}

static uart_rx_parser_state_t uart_rx_parser_get_cmdid(uint8_t rx_data)
{
    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;
    uart_rx_parser_data.cmd_id = rx_data;

    return UART_RX_PARSER_ST_GET_MSG_LEN;
}

// msg_len is a 16bites field, so we count 2 bytes
static uart_rx_parser_state_t uart_rx_parser_get_msglen(uint8_t rx_data, uint16_t *byte_cnt)
{
    assert(*byte_cnt < FIELD_MSGLEN_LEN);

    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;

    *byte_cnt += 1;

    if (*byte_cnt < FIELD_MSGLEN_LEN) {
        uart_rx_parser_data.msg_len = rx_data;
        return uart_rx_parser_data.state;
    } else if (FIELD_MSGLEN_LEN == *byte_cnt) {
        uart_rx_parser_data.msg_len |= (uint16_t)rx_data << 8;
        uart_rx_parser_data.payload_len = uart_rx_parser_data.msg_len - 1; // we subtract the len of the error code
        *byte_cnt = 0;
        return UART_RX_PARSER_ST_GET_SEC_MODE;
    }

    // we should never get here
    return UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST;
}

static uart_rx_parser_state_t uart_rx_parser_get_secmode(uint8_t rx_data)
{
    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;
    uart_rx_parser_data.sec_mode = rx_data;

    return UART_RX_PARSER_ST_GET_COUNTER;
}

// counter is a 4 bytes field, so we count 4 bytes
static uart_rx_parser_state_t uart_rx_parser_get_counter(uint8_t rx_data, uint16_t *byte_cnt)
{
    assert(*byte_cnt < FIELD_COUNTER_LEN);

    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;

    *byte_cnt += 1;

    if (*byte_cnt < FIELD_COUNTER_LEN)
        return uart_rx_parser_data.state;
    else {
        *byte_cnt = 0;
        return UART_RX_PARSER_ST_GET_PAYLOAD;
    }
}

// for payload, we count a number of bytes equal to msg_len
static uart_rx_parser_state_t uart_rx_parser_get_payload(uint8_t rx_data, const uint16_t msg_len,
                                                         uint16_t *byte_cnt)
{
    assert(*byte_cnt < msg_len);

    if (1 == *byte_cnt) {
        uart_rx_parser_data.payload_buffer = &uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx];
    }

    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;

    *byte_cnt += 1;

    if (*byte_cnt < msg_len)
        return uart_rx_parser_data.state;
    else {
        *byte_cnt = 0;

        if (!uart_rx_parser_data.sec_mode)
            return UART_RX_PARSER_ST_GET_CRC;
        else
            return UART_RX_PARSER_ST_GET_TAG;
    }
}

// temporary partial implementation
static uart_rx_parser_state_t uart_rx_parser_get_tag(void)
{
    return uart_rx_parser_data.state;
}

static uart_rx_parser_state_t uart_rx_parser_get_crc(uint8_t rx_data, uint16_t *byte_cnt)
{
    assert(*byte_cnt < FIELD_CRC_LEN);

    uart_rx_parser_data_buffer[uart_rx_parser_data.buff_idx++] = rx_data;

    *byte_cnt += 1;

    if (*byte_cnt < FIELD_CRC_LEN) {
        uart_rx_parser_data.crc = rx_data;
        return uart_rx_parser_data.state;
    } else {
        *byte_cnt = 0;
        uart_rx_parser_data.crc |= (uint16_t)rx_data << 8;

        if (!task_comm_put_recv_buffer(g3_task_comm_info, G3_PROT, uart_rx_parser_data.cmd_id, uart_rx_parser_data_buffer, uart_rx_parser_data.buff_idx, NO_WAIT)) {
            // Warning: queue full
            assert(false);
        }

        hif_uart_rx_handler_init();

        return UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST;
    }
}

void g3_hif_uart_rx_handler(uint8_t rx_data)
{
    switch (uart_rx_parser_data.state) {
    case UART_RX_PARSER_ST_SEARCH_PREAMBLE_1ST:
        uart_rx_parser_data.state = uart_rx_parser_search_preamble(rx_data, uart_rx_parser_data.state);
        break;
    case UART_RX_PARSER_ST_SEARCH_PREAMBLE_2ND:
        uart_rx_parser_data.state = uart_rx_parser_search_preamble(rx_data, uart_rx_parser_data.state);
        break;
    case UART_RX_PARSER_ST_GET_CMD_ID:
        uart_rx_parser_data.state = uart_rx_parser_get_cmdid(rx_data);
        break;
    case UART_RX_PARSER_ST_GET_MSG_LEN:
        uart_rx_parser_data.state = uart_rx_parser_get_msglen(rx_data, &uart_rx_parser_data.multibyte_field_cnt);
        break;
    case UART_RX_PARSER_ST_GET_SEC_MODE:
        uart_rx_parser_data.state = uart_rx_parser_get_secmode(rx_data);
        break;
    case UART_RX_PARSER_ST_GET_COUNTER:
        uart_rx_parser_data.state = uart_rx_parser_get_counter(rx_data, &uart_rx_parser_data.multibyte_field_cnt);
        break;
    case UART_RX_PARSER_ST_GET_PAYLOAD:
        uart_rx_parser_data.state = uart_rx_parser_get_payload(rx_data, uart_rx_parser_data.msg_len,
                                                               &uart_rx_parser_data.multibyte_field_cnt);
        break;
    case UART_RX_PARSER_ST_GET_TAG:
        uart_rx_parser_data.state = uart_rx_parser_get_tag();
        break;
    case UART_RX_PARSER_ST_GET_CRC:
        uart_rx_parser_data.state = uart_rx_parser_get_crc(rx_data, &uart_rx_parser_data.multibyte_field_cnt);
        break;
    }
}

/**
* @brief  This functions initializes the input and output buffer
* @retval None
*/
void g3_hif_uart_init(task_comm_info_t *g3_task_comm_info_)
{
    g3_task_comm_info = g3_task_comm_info_;

    hif_uart_tx_handler_init();
    hif_uart_rx_handler_init();
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
