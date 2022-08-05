/**
  ******************************************************************************
  * @file    g3_app_attrib_tbl.c
  * @author  AMG/IPC Application Team
  * @brief   Attribute table implementation
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

#include <g3_app_attrib_tbl.h>
#include <g3_app_boot_tbl.h>
#include <g3_app_config.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if !defined(DEVICE_TYPE_COORD) && !defined(DEVICE_TYPE_DEVICE)
#error "either DEVICE_TYPE_DEVICE or DEVICE_TYPE_COORD must be defined"
#endif

#if !defined(RF_IS_868) && !defined(RF_IS_915)
#error "either RF_IS_868 or RF_IS_915 must be defined"
#endif

static struct {
    uint16_t tbl_len;
    uint16_t tbl_idx;
    g3_attrib_tbl_elem_reader *elem_reader;
} g3_app_attrib_data;

static G3_LIB_PIB_t g3_app_attrib_tbl[] = {
#ifdef DEVICE_TYPE_COORD
    {.Attribute = {.ID = MAC_PANID_ID, .Index = 0}, .Len = 2, .Value = {0x57U, 0x8CU}},
    {.Attribute = {.ID = MAC_SHORTADDRESS_ID, .Index = 0}, .Len = 2, .Value = {0x00U, 0x00U}}
#else
    {.Attribute = {.ID = ADP_EAPPSKKEY_ID, .Index = 0}, .Len = 16, .Value = DEFAULT_PSK}
#endif
};

#define ATTRIB_TBL_LEN  (sizeof(g3_app_attrib_tbl) / sizeof(g3_app_attrib_tbl[0]))

static bool g3_attrib_reader(G3_LIB_PIB_t *attr_data)
{
    if (g3_app_attrib_data.tbl_idx >= g3_app_attrib_data.tbl_len)
        return false;

    *attr_data = g3_app_attrib_tbl[g3_app_attrib_data.tbl_idx++];

    return true;
}

void g3_app_attrib_tbl_init(g3_attrib_tbl_elem_reader *elem_reader,
                            g3_attrib_tbl_init *init)
{
    g3_app_attrib_data.elem_reader = elem_reader;

    if (init)
        init();
}

bool g3_app_attrib_tbl_get_next_data(G3_LIB_PIB_t *attr_data)
{
    return g3_app_attrib_data.elem_reader(attr_data);
}

static const G3_LIB_PIB_t *attrib_tbl_find_by_id(uint32_t id)
{
    const uint16_t attrib_tbl_len = ATTRIB_TBL_LEN;

    for (uint16_t i = 0; i < attrib_tbl_len; i++) {
        if (id == g3_app_attrib_tbl[i].Attribute.ID)
            return &g3_app_attrib_tbl[i];
    }

    return NULL;
}

uint16_t g3_app_attrib_tbl_get_coord_short_addr(void)
{
    uint16_t short_addr;
    const G3_LIB_PIB_t *pib = attrib_tbl_find_by_id(MAC_SHORTADDRESS_ID);

    assert(pib != NULL);

    memcpy(&short_addr, pib->Value, sizeof(short_addr));

    return short_addr;
}

uint16_t g3_app_attrib_tbl_get_coord_pan_id(void)
{
    uint16_t pan_id;
    const G3_LIB_PIB_t *pib = attrib_tbl_find_by_id(MAC_PANID_ID);

    assert(pib != NULL);

    memcpy(&pan_id, pib->Value, sizeof(pan_id));

    return pan_id;
}

void g3_app_attrib_tbl_init_default(void)
{
    g3_app_attrib_data.tbl_len = ATTRIB_TBL_LEN;
    g3_app_attrib_data.tbl_idx = 0;

    g3_app_attrib_tbl_init(g3_attrib_reader, NULL);
}
