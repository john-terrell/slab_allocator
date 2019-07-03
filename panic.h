//
// Copyright 2015 by John W. Terrell - All Rights Reserved.
//
// This is not to be distributed or disclosed without the expressed written consent of the copyright holder
//
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (*cl_core_panic_handler_t)(const char *message, const char *file, uint32_t line);

    void cl_core_panic_set_handler(cl_core_panic_handler_t panic_handler);
    void cl_core_panic(const char *message, const char *file, uint32_t line);

#ifdef __cplusplus
}
#endif
