#ifndef ECS_TYPES_H
#define ECS_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef int16_t Entity;

typedef struct EcsManager EcsManager;
typedef struct EcsPool EcsPool;
typedef struct ECSFilter EcsFilter;
typedef struct ECSMask EcsMask;
typedef struct EntityContainer EntityContainer;

typedef void (*ResetItemHandler)(void*, size_t);

typedef void (*CopyItemHandler)(void*, const void*, size_t);

typedef struct UpdateSystem UpdateSystem;
typedef struct DrawSystem DrawSystem;
typedef struct SystemHandler SystemHandler;

#endif
