//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void kmem_init(void);
    void *kmem_alloc(size_t size);
    void kmem_free(void *buffer);

#ifdef __cplusplus
}
#endif