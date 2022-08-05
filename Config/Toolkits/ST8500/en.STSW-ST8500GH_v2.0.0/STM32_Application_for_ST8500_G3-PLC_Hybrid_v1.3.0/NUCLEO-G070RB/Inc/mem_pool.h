/**
  ******************************************************************************
  * @file           : mem_pool_internal.h
  * @brief          : Internal header for mem_pool.c
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

#ifndef MEM_POOL_H_
#define MEM_POOL_H_

// Define this to help debugging memory allocation/deallocation patterns
//#define DEBUG_MEMORY

#include <stdint.h>
#include <stddef.h>

#define MEM_BUFFER_SIZE        2048UL

#ifdef DEBUG_MEMORY
#define AT __func__
#else
#define AT NULL
#endif

#define check_ptrs_match(expr1, expr2)         \
        (sizeof((expr1) != (expr2)) - sizeof((expr1) != (expr2)))

#define container_off(containing_type, member)  \
        offsetof(containing_type, member)

#define container_of(member_ptr, containing_type, member)               \
         ((containing_type *)                                           \
          (((char *)(member_ptr)                                         \
           - container_off(containing_type, member))                    \
           + check_ptrs_match((member_ptr), &((containing_type *)0)->member)))

/**
  * @brief    This function is used to allocate a memory buffer
  *           of size at least equal to the parameter <size>.
  * @param    [in] size is the minimum required size of the buffer to allocate.
  * @param    [in] at is a string containing the name of the function where MAC_MemAlloc
  *           was called.
  * @return   None.
  */
void *MEM_Alloc(int32_t size, const char *at);

#define MEM_ALLOC(size) (mempool_allocs_inc(), MEM_Alloc((size), AT))

/**
  * @brief    This function is used to free a memory buffer
  *           which was allocated by MAC_MemAlloc.
  * @param    [in] mem is the pointer to the memory buffer to be freed
  * @param    [in] at is a string containing the name of the function where MAC_MemFree
  *           was called.
  * @return   None.
  */
void MEM_Free(void *mem, const char *at);


/**
  * @brief    This function is used to free all the memory buffers which have been
  *           allocated by MAC_MemAlloc.
  * @param    None.
  * @return   None.
  */
void MEM_FreeAll(void);

#define MEM_FREE(mem) do {                                         \
    const char *mem_free_at_ = AT;                                 \
    void **mem_free_ptr_ = (void **)(&(mem));                      \
    MEM_Free(*mem_free_ptr_, mem_free_at_);                        \
    *mem_free_ptr_ = NULL;                                         \
    mempool_allocs_dec();                                          \
} while (0)

void mempool_allocs_inc(void);
void mempool_allocs_dec(void);
uint16_t mempool_allocs_get_max(void);

#endif
