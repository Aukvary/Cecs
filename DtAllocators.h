#ifndef DT_ALLOCATORS_H
#define DT_ALLOCATORS_H

#ifndef DT_STACK_ALLOC
#if defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#define DT_STACK_ALLOC(size) alloca((size))
#endif

#ifndef DT_MALLOC
#include <malloc.h>
#define DT_MALLOC(size) malloc((size))
#endif

#ifndef DT_REALLOC
#include <malloc.h>
#define DT_REALLOC(ptr, size) realloc((ptr), (size))
#endif

#ifndef DT_REALLOC_ARRAY
#include <malloc.h>
#define DT_REALLOC_ARRAY(ptr, size) reallocarray((ptr), (size))
#endif

#ifndef DT_CALLOC
#include <malloc.h>
#define DT_CALLOC(ptr, size) calloc((ptr), (size))
#endif

#endif
