/**
  ******************************************************************************
  * @file    g3_app_boot_tbl.c
  * @author  AMG/IPC Application Team
  * @brief   Boot table implementation
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

#include <g3_app_boot_tbl.h>
#include <g3_app_keep_alive.h>
#include <stdlib.h>
#include <string.h>

static g3_app_boot_data_t g3_app_boot_data_tbl[] = {
    {
        .short_addr = 0x0001U,
        .ext_addr = {0x00U, 0x80U, 0xE1U, 0xFFU, 0xFEU, 0x4CU, 0xA6U, 0x6AU},
        .psk = DEFAULT_PSK
    },
    {
        .short_addr = 0x0002U,
        .ext_addr = {0x00U, 0x80U, 0xE1U, 0xFFU, 0xFEU, 0x4CU, 0x6DU, 0xC6U},
        .psk = DEFAULT_PSK
    }
};

#define G3_APP_BOOT_TBL_LEN (sizeof(g3_app_boot_data_tbl) / sizeof(g3_app_boot_data_tbl[0]))

static g3_app_boot_data_t g3_app_boot_data_tmp_tbl[G3_APP_MAX_DEVICES_IN_PAN - G3_APP_BOOT_TBL_LEN];

static struct {
    uint16_t short_addr_first_free;
    uint16_t tbl_first_free_idx;
} g3_app_boot_tbl_data;

static int boot_data_tbl_compare_elem(const void *a_, const void *b_)
{
    const g3_app_boot_data_t *a = a_;
    const g3_app_boot_data_t *b = b_;

    for (uint16_t i = MAC_ADDR64_SIZE - 1, cnt = 0; cnt < MAC_ADDR64_SIZE; i--, cnt++) {
        if (a->ext_addr[i] == b->ext_addr[i])
            continue;
        else if (a->ext_addr[i] > b->ext_addr[i])
            return 1;
        else
            return -1;
    }

    return 0;
}

const g3_app_boot_data_t *g3_app_boot_tbl_find_elem(const uint8_t *ext_addr)
{
    g3_app_boot_data_t key;

    memset(&key, 0, sizeof(key));
    memcpy(key.ext_addr, ext_addr, sizeof(key.ext_addr));

    void *ret = bsearch(&key, g3_app_boot_data_tbl, G3_APP_BOOT_TBL_LEN,
                        sizeof(g3_app_boot_data_tbl[0]), boot_data_tbl_compare_elem);

    return ret;
}

const g3_app_boot_data_t *g3_app_boot_temp_tbl_find_elem(const uint8_t *ext_addr)
{
    g3_app_boot_data_t key;

    memset(&key, 0, sizeof(key));
    memcpy(key.ext_addr, ext_addr, sizeof(key.ext_addr));

    void *ret = bsearch(&key, g3_app_boot_data_tmp_tbl, g3_app_boot_tbl_data.tbl_first_free_idx,
                        sizeof(g3_app_boot_data_tmp_tbl[0]), boot_data_tbl_compare_elem);

    return ret;
}

uint16_t g3_app_boot_get_next_short_addr(void)
{
    return g3_app_boot_tbl_data.short_addr_first_free++;
}

bool g3_app_boot_data_tbl_store_elem(uint16_t short_addr, const uint8_t *ext_addr, const uint8_t *psk)
{
    g3_app_boot_data_t *data_entry;

    if (g3_app_boot_tbl_data.tbl_first_free_idx >= G3_APP_MAX_DEVICES_IN_PAN)
        return false;

    data_entry = &g3_app_boot_data_tmp_tbl[g3_app_boot_tbl_data.tbl_first_free_idx++];

    data_entry->short_addr = short_addr;
    memcpy(data_entry->ext_addr, ext_addr, MAC_ADDR64_SIZE);
    memcpy(data_entry->psk, psk, ADP_EAP_PSK_KEY_LEN);

    qsort(g3_app_boot_data_tmp_tbl, g3_app_boot_tbl_data.tbl_first_free_idx, sizeof(g3_app_boot_data_tmp_tbl[0]),
          boot_data_tbl_compare_elem);

    return true;
}

void g3_app_boot_tbl_init(void)
{
    g3_app_boot_tbl_data.short_addr_first_free = G3_APP_BOOT_TBL_LEN + 1;
    g3_app_boot_tbl_data.tbl_first_free_idx = 0;

    qsort(g3_app_boot_data_tbl, G3_APP_BOOT_TBL_LEN, sizeof(g3_app_boot_data_tbl[0]),
          boot_data_tbl_compare_elem);
}
