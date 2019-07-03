//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#include "SystemCore/mem_cache.h"
#include "SystemCore/mem_slab.h"
#include "SystemCore/panic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const uint32_t magic = 0x1234A5C5;

void mem_cache_init(mem_cache_t *cache, const char *name, size_t item_size)
{
    memset(cache, 0, sizeof(mem_cache_t));

    assert(item_size >= sizeof(size_t));
    if (item_size >= sizeof(size_t))
    {
        cache->magic = magic;

        list_init(&cache->full_slabs);
        list_init(&cache->partial_slabs);
        list_init(&cache->empty_slabs);

        snprintf(cache->name, sizeof(cache->name), name);

        cache->item_size = item_size;
    }
    else
    {
        cl_core_panic("Attempted to create a mem_cache with too small size", __FILE__, __LINE__);
    }
}

void mem_cache_clear(mem_cache_t *cache)
{
    assert(cache != NULL);
    assert(cache->magic == magic);
    if (cache != NULL && cache->magic == magic)
    {
        memset(cache, 0, sizeof(mem_cache_t));
    }
    else
    {
        cl_core_panic("invalid cache", __FILE__, __LINE__);
    }
}

void *mem_cache_alloc(mem_cache_t *cache)
{
    void *buffer = NULL;

    assert(cache != NULL);
    assert(cache->magic == magic);
    if (cache && cache->magic == magic)
    {
        mem_cache_lock(cache);

        // Find a slab to allocate from by looking in partial and empty slab lists
        mem_slab_t *slab_to_use = NULL;
        if (!list_empty(&cache->partial_slabs))
        {
            slab_to_use = (mem_slab_t *)list_tail(&cache->partial_slabs);
        }
        else
        if (!list_empty(&cache->empty_slabs))
        {
            slab_to_use = (mem_slab_t *)list_tail(&cache->empty_slabs);
        }
        else
        {
            mem_slab_t *new_slab = mem_slab_create(cache->item_size);
            if (new_slab)
            {
                list_append(&cache->empty_slabs, &new_slab->li);

                slab_to_use = new_slab;
            }
        }

        if (slab_to_use)
        {
            // allocate from the slab
            buffer = mem_slab_alloc(slab_to_use);
            if (buffer)
            {
                // remove the slab for a sec (it'll be put back in the correct list next)
                list_item_remove(&slab_to_use->li);

                // if the slab is now full, move it to the "full" list
                if (mem_slab_full(slab_to_use))
                {
                    list_append(&cache->full_slabs, &slab_to_use->li);
                }
                else
                {
                    // slab is not full so put it in the "partial" list
                    list_append(&cache->partial_slabs, &slab_to_use->li);
                }
            }
        }

        mem_cache_unlock(cache);
    }
    else
    {
        cl_core_panic("invalid cache", __FILE__, __LINE__);
    }

    return buffer;
}

static mem_slab_t *mem_cache_try_free_from_list(list_t *list, void *buffer)
{
    mem_slab_t *found_slab = NULL;

    // Each item in the list is a mem_slab_t *
    list_item_t *li = list->head.next;
    while (li != &list->tail)
    {
        mem_slab_t *slab = (mem_slab_t *)li;
        if (mem_slab_contains(slab, buffer))
        {
            mem_slab_free(slab, buffer);
            found_slab = slab;
            break;
        }

        li = li->next;
    }

    return found_slab;
}

void mem_cache_free(mem_cache_t *cache, void *buffer)
{
    assert(cache != NULL);
    assert(cache->magic == magic);
    if (cache != NULL && cache->magic == magic)
    {
        mem_cache_lock(cache);

        // The block has to be in either the full or partial lists, start looking there
        mem_slab_t *found_slab = mem_cache_try_free_from_list(&cache->partial_slabs, buffer);
        if (found_slab == NULL)
        {
            found_slab = mem_cache_try_free_from_list(&cache->full_slabs, buffer);
            if (found_slab == NULL)
            {
                // This is a bug!  Why did we try to free from a non-existent slab (or did a slab go away)
                cl_core_panic("Attempted to free from a non-existent slab", __FILE__, __LINE__);
            }
            else
            {
                // The deallocation was from the full slab...move it to the
                // partially full slab list
                // BUGBUG: what happens if the allocation went from full to empty?
                list_item_remove(&found_slab->li);

                list_append(&cache->partial_slabs, &found_slab->li);
            }
        }
        else
        {
            // The deallocation was from a partially full slab...if the
            // slab is now empty, move it to the "empty" large_list
            if (mem_slab_empty(found_slab))
            {
                list_item_remove(&found_slab->li);

                list_append(&cache->partial_slabs, &found_slab->li);
            }
        }

        mem_cache_unlock(cache);
    }
    else
    {
        cl_core_panic("invalid cache", __FILE__, __LINE__);
    }
}
