//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#include "SystemCore/kmem.h"
#include "SystemCore/mem_cache.h"
#include "SystemCore/panic.h"

#include <stdio.h>
#include <assert.h>

static bool initialized = false;

// 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
static mem_cache_t caches[10];
static uint16_t cache_count = sizeof(caches) / sizeof(caches[0]);

void kmem_init(void)
{
    char name[32];
    size_t size = 4;
    for (size_t i = 0; i < cache_count; ++i)
    {
        snprintf(name, sizeof(name), "kmem-%lu", (uint32_t)size);
        mem_cache_init(&caches[i], name, size);
        size <<= 1;
    }
}

typedef struct allocation_info
{
    mem_cache_t *cache;
    uint8_t data[0];
} allocation_info_t;

void *kmem_alloc(size_t size)
{
    void *buffer = NULL;
    if (!initialized)
    {
        cl_core_panic("kmem_alloc failed", __FILE__, __LINE__);
    }

    if (size > 0)
    {
        const size_t allocation_size = size + sizeof(allocation_info_t);

        mem_cache_t *cache = NULL;
        for (size_t i = 0; i < cache_count; ++i)
        {
            if (mem_cache_item_size(&caches[i]) >= allocation_size)
            {
                cache = &caches[i];
                break;
            }
        }

        allocation_info_t *allocation = NULL;
        if (cache == NULL)
        {
            allocation = (allocation_info_t *)malloc(allocation_size);
        }
        else
        {
            allocation = mem_cache_alloc(cache);
        }

        if (allocation != NULL)
        {
            allocation->cache = cache;
            buffer = &allocation->data;
        } 
    }

    return buffer;
}

void kmem_free(void *buffer)
{
    if (buffer != NULL)
    {
        uint8_t *p = (uint8_t *)buffer;
        p -= sizeof(allocation_info_t);

        allocation_info_t *allocation = (allocation_info_t *)p;

        if (allocation->cache == NULL)
        {
            free(allocation);
        }
        else
        {
            mem_cache_free(allocation->cache, allocation);
        }
    }
}