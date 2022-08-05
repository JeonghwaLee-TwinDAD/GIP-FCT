/**
  ******************************************************************************
  * @file           : mem_pool.c
  * @brief          : Buffer Pool implementation
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include <mem_pool.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Undefine this to remove MemPool profile functionality
// #define MEMPOOL_DEBUG

typedef union
{
    char ch;
    short sh;
    int i;
    long l;
    long long lg;
    float f;
    double d;
    long double ld;
    void *p;
    void (*fp)();
} MemMaxAlign;

#define MAX_FILENAME_LEN 50

typedef struct
{
    uint32_t size;
    uint32_t pool_info;
#ifdef DEBUG_MEMORY
    char at[MAX_FILENAME_LEN];
#endif
    uint8_t mem[];
} MemInfo;

#define MEM_META_INFO_SIZE  sizeof(MemInfo)

#define MEM_POOL_ID         ((uint16_t)1)

#define MEMPoolDim          ((uint32_t)2048)

#define MAXPoolDim          MEMPoolDim

#define MEMPOOL_ISVALID_SIZE(size)  ((size) <= MAXPoolDim)

typedef struct
{
    bool used;
    union
    {
        uint8_t buf[MEM_META_INFO_SIZE + MEMPoolDim];
        MemMaxAlign force_alignment;
    };
} MEMPool;

#define MEMPOOL_SIZE      ((uint16_t)5)

static MEMPool MemPool[MEMPOOL_SIZE];

/** @addtogroup UTILS
  * @{
  */

/** @addtogroup UTILS_Functions
  * @{
  */

static uint16_t MEM_GetPoolIndex(uint32_t var)
{
    return (uint16_t)var;
}

static void MEM_SetPoolIndex(uint32_t *out_var, uint16_t index)
{
    *out_var &= ((uint32_t)0xFFFF) << 16;
    *out_var |= (uint32_t)index;
}

#ifdef DEBUG_MEMORY
static bool MEM_SetAtString(char *out_str, const char *in_str)
{
    const uint16_t str_len = strlen(in_str) + 1;

    memset(out_str, 0, 1);

    if (str_len > MAX_FILENAME_LEN)
        return false;

    memcpy(out_str, in_str, str_len);

    return true;
}
#endif

static bool MEM_FindFreeBlock(int32_t size, uint16_t *index)
{
    for (uint16_t i = 0; i < MEMPOOL_SIZE; i++)
    {
        if (!MemPool[i].used) {
            *index = i;
            return true;
        }
    }

    return false;
}

static void *MEM_GetBlock(int32_t size, bool do_zero_mem, const char *at)
{
    MemInfo *mem_info;
    uint16_t pool_index;
    uint8_t pool_found;
    uint16_t block_len;

    pool_found = MEM_FindFreeBlock(size, &pool_index);

    if (!pool_found)
    {
        return NULL;
    }

    mem_info = (MemInfo *)MemPool[pool_index].buf;
    mem_info->size = size;
    MEM_SetPoolIndex(&mem_info->pool_info, pool_index);
    MemPool[pool_index].used = true;
    block_len = MEMPoolDim;

    if (do_zero_mem) {
        memset(mem_info->mem, 0, block_len);
    }

#ifdef DEBUG_MEMORY
    MEM_SetAtString(mem_info->at, at);
#endif

    return mem_info->mem;
}

static void MEM_ReleaseBlock(MemInfo *mem_info)
{
    uint32_t pool_info = mem_info->pool_info;

    MemPool[MEM_GetPoolIndex(pool_info)].used = false;
}

/**
  * @brief    This function is used to allocate a memory buffer
  *           of size at least equal to the parameter <size>.
  * @param    [in] size is the minimum required size of the buffer to allocate.
  * @param    [in] at is a string containing the name of the function where MAC_MemAlloc
  *           was called.
  * @return   None.
  */
void *MEM_Alloc(int32_t size, const char *at)
{
    void *mem_block = NULL;

    if (size <= 0)
    {
        return NULL;
    }
    else if (!MEMPOOL_ISVALID_SIZE(size))
    {
        return NULL;
    }

    mem_block = MEM_GetBlock(size, false, at);

    if (!mem_block)
    {
    }

    return mem_block;
}

/**
  * @brief    This function is used to free a memory buffer
  *           which was allocated by MAC_MemAlloc.
  * @param    [in] mem is the pointer to the memory buffer to be freed
  * @param    [in] at is a string containing the name of the function where MAC_MemFree
  *           was called.
  * @return   None.
  */
void MEM_Free(void *mem, const char *at)
{
    MemInfo *mem_info;

    if (mem)
    {
        mem_info = container_of((uint8_t (*)[])mem, MemInfo, mem);
        MEM_ReleaseBlock(mem_info);
    }
    else
    {
    }
}

/**
  * @brief    This function is used to free all the memory buffers which have been
  *           allocated by MAC_MemAlloc.
  * @param    None.
  * @return   None.
  */
void MEM_FreeAll(void)
{
    for (uint16_t i = 0; i < MEMPOOL_SIZE; i++)
    {
        MemPool[i].used = false;
    }
}

#ifdef MEMPOOL_DEBUG

static volatile uint16_t mempool_allocs_cnt;
static volatile uint16_t mempool_allocs_max;

#pragma required=mempool_allocs_cnt
#pragma required=mempool_allocs_max

void mempool_allocs_inc(void)
{
    uint16_t max = mempool_allocs_max;

    mempool_allocs_cnt++;

    if (mempool_allocs_cnt > max)
        mempool_allocs_max = mempool_allocs_cnt;
}

void mempool_allocs_dec(void)
{
    mempool_allocs_cnt--;
}

uint16_t mempool_allocs_get_max(void)
{
    return mempool_allocs_max;
}

#else

void mempool_allocs_inc(void)
{
}

void mempool_allocs_dec(void)
{
}

uint16_t mempool_allocs_get_max(void)
{
    return 0;
}

#endif
