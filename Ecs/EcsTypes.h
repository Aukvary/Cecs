#ifndef ECS_TYPES_H
#define ECS_TYPES_H

#include <stdlib.h>

typedef int16_t Entity;

/**
 * @brief main entity handler
 */
typedef struct EcsManager EcsManager;

/**
 * @brief contains entity component data
 */

typedef struct EcsPool EcsPool;

/**
 * @brief archetype container for entities with identical components
 */
typedef struct ECSFilter EcsFilter;

/**
 * @brief mask with data about ecs filter
 */
typedef struct ECSMask EcsMask;

/**
 * @brief data struct for contains entity
 *
 * @note use dense_items/dense_entities and count to iteration
 * @note implements an ID recycled system for reuse
 */
typedef struct EntityContainer EntityContainer;

/**
 * @brief func pointer to handle reset operation in EntityContainer
 *
 * @param data pointer to data
 * @param size size of data
 */
typedef void (*ResetItemHandler)(void* data, size_t size);

/**
 * @brief func pointer to handle copy operation in EntityContainer
 *
 * @param dst pointer to destination
 * @param src pointer to source
 * @param size size of data
 */
typedef void (*CopyItemHandler)(void* dst, const void* src, size_t size);

/**
 * @brief container for system funcs
 *
 * @warning if system hasn't any func it must be initialized by NULL
 */
typedef struct EcsSystem EcsSystem;

/**
 * @brief container where systems grouped by funcs
 */
typedef struct SystemHandler SystemHandler;

#endif
