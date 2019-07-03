cmake_minimum_required(VERSION 3.3)
project(SystemCore)

add_library(SystemCore STATIC
    kmem.c
    kmem.h
    mem_cache.c
    mem_cache.h
    mem_slab.c
    mem_slab.h
    panic.c
    panic.h
    )	

target_include_directories(SystemCore PUBLIC ./)
