//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#include "SystemCore/mem_slab.h"
#include "SystemCore/panic.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

const size_t slab_size_in_bytes = 8192;

const uint32_t magic = 0x8876AABB;

bool mem_slab_contains(const mem_slab_t *slab, void *buffer)
{
    bool contains = false;
    assert(slab != NULL);
    assert(slab->magic == magic);
    assert(buffer != NULL);
    if (slab != NULL && slab->magic == magic)
    {
        const uint8_t *block_addr = (const uint8_t *)buffer;
        if (block_addr >= (const uint8_t *)slab->data &&
            block_addr <= (const uint8_t *)slab->data + (slab->max_count * slab->item_size))
        {
            contains = true;
        }
    }
    else
    {
        cl_core_panic("invalid slab", __FILE__, __LINE__);
    }

    return contains;
}

mem_slab_t *mem_slab_create(size_t item_size)
{
    mem_slab_t *new_slab = (mem_slab_t *)malloc(slab_size_in_bytes);
    if (new_slab)
    {
        new_slab->magic = magic;
        new_slab->item_size = item_size;
        new_slab->max_count = (slab_size_in_bytes - sizeof(mem_slab_t)) / item_size;
        new_slab->used_count = 0;
        new_slab->first_free = (size_t)-1;
    }

    return new_slab;
}

void mem_slab_destroy(mem_slab_t *slab)
{
    assert(slab != NULL);
    assert(slab->magic == magic);
    if (slab != NULL && slab->magic == magic)
    {
        memset(slab, 0, sizeof(slab_size_in_bytes));
        free(slab);
    }
    else
    {
        cl_core_panic("invalid slab", __FILE__, __LINE__);
    }
}

void *mem_slab_alloc(mem_slab_t *slab)
{
    void *item = NULL;

    assert(slab != NULL);
    assert(slab->magic == magic);
    if (slab != NULL && slab->magic == magic)
    {
        if (slab->first_free != (size_t)-1)
        {
            const size_t item_offset = slab->first_free * slab->item_size;

            assert(item_offset <= (slab->max_count * slab->item_size));
            uint8_t *item_address = &slab->data[item_offset];
            
            slab->first_free = *(size_t *)item_address;

            item = item_address;
        }
        else
        {
            if (slab->used_count < slab->max_count)
            {
                const size_t item_offset = slab->used_count * slab->item_size;

                ++slab->used_count;  // this needs to happen AFTER the above offset calculation
                uint8_t *item_address = &slab->data[item_offset];
            
                item = item_address;
            }
        }
    }
    else
    {
        cl_core_panic("invalid slab", __FILE__, __LINE__);
    }

    return item;
}


void mem_slab_free(mem_slab_t *slab, void *buffer)
{
    assert(slab != NULL);
    assert(slab->magic == magic);
    if (slab != NULL && slab->magic == magic)
    {
        if (mem_slab_contains(slab, buffer))
        {
            const size_t block_offset = ((size_t)buffer - (size_t)slab->data) / slab->item_size;

            if (block_offset == (slab->used_count - 1))
            {
                --slab->used_count;
            }
            else
            {
                *(size_t *)buffer = slab->first_free;
                slab->first_free = block_offset;
            }
        }
        else
        {
            assert(false);
            cl_core_panic("attempt to free item not in slab", __FILE__, __LINE__);
        }
    }
    else
    {
        cl_core_panic("invalid slab", __FILE__, __LINE__);
    }
}
