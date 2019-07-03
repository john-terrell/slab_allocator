#pragma once

#include "SystemCore/list.h"

#include <stdint.h>
#include <stddef.h>

struct mem_cache
{
    uint32_t magic;

    list_t full_slabs;
    list_t partial_slabs;
    list_t empty_slabs;

    size_t item_size;
    char name[32];
};

typedef struct mem_cache mem_cache_t;

#ifdef __cplusplus
extern "C"
{
#endif
    void mem_cache_init(mem_cache_t *cache, const char *name, size_t item_size);
    void mem_cache_clear(mem_cache_t *cache);

    void *mem_cache_alloc(mem_cache_t *cache);
    void mem_cache_free(mem_cache_t *cache, void *buffer);

    void mem_cache_lock(mem_cache_t *cache);
    void mem_cache_unlock(mem_cache_t *cache);

    size_t mem_cache_item_size(const mem_cache_t *cache);
#ifdef __cplusplus
}
#endif
