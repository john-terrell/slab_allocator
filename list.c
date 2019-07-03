//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#include "list.h"
#include <assert.h>

void list_init(list_t *l)
{
    l->head.prev = NULL;
    l->head.next = &l->tail;
    
    l->tail.prev = &l->head;
    l->tail.next = NULL;
    l->count = 0;
}

size_t list_count(const list_t *l)
{
    return l->count;
}

void list_append(list_t *l, list_item_t *i)
{
    i->prev = l->tail.prev;
    i->next = &l->tail;
    
    i->prev->next = i;
    i->next->prev = i;
    
    ++l->count;
}

list_item_t *list_remove_head(list_t *l)
{
    assert(list_count(l) != 0);
    list_item_t *item = NULL;
    
    item = l->head.next;
    
    item->prev->next = item->next;
    item->next->prev = item->prev;
    
    item->next = item->prev = NULL;
    --l->count;
    
    return item;
}
