//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#include "panic.h"
#include <stdlib.h>

static cl_core_panic_handler_t _panic_handler = NULL;

void cl_core_panic_set_handler(cl_core_panic_handler_t panic_handler)
{
    _panic_handler = panic_handler;
}

void cl_core_panic(const char *message, const char *file, uint32_t line)
{
    if (_panic_handler != NULL)
    {
        (*_panic_handler)(message, file, line);
    }

    abort();
}
