#pragma once

#include "SystemCore/list.h"

#include <stdint.h>
#include <stddef.h>

struct mem_slab
{
    list_item_t li;       // make sure this is the first element in the struct

    uint32_t magic;

    size_t item_size;
    size_t max_count;
    size_t used_count;
    size_t first_free;

    uint8_t data[0];
};

typedef struct mem_slab mem_slab_t;

#ifdef __cplusplus
extern "C"
{
#endif

    bool mem_slab_contains(const mem_slab_t *slab, void *buffer);
    bool mem_slab_full(const mem_slab_t *slab);
    bool mem_slab_empty(const mem_slab_t *slab);
    
    mem_slab_t *mem_slab_create(size_t item_size);
    void mem_slab_destroy(mem_slab_t *slab);

    void *mem_slab_alloc(mem_slab_t *slab);
    void mem_slab_free(mem_slab_t *slab, void *buffer);

#ifdef __cplusplus
}
#endif
