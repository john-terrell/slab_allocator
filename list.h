//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct list_item
{
    struct list_item *prev;
    struct list_item *next;
} list_item_t;

typedef struct list
{
    list_item_t head;
    list_item_t tail;
    size_t count;
} list_t;

#ifdef __cplusplus
extern "C"
{
#endif

    void list_item_remove(list_item_t *li);

    void list_init(list_t *l);
    bool list_empty(const list_t *l);
    size_t list_count(const list_t *l);
    list_item_t *list_tail(list_t *l);

    void list_append(list_t *l, list_item_t *i);
    list_item_t *list_remove_head(list_t *l);

#ifdef __cplusplus
}
#endif
