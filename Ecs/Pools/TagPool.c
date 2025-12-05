#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../EcsManager/EcsManager.h"
#include "EcsPool.h"

typedef uint16_t TagBucket;
#define BUCKET_SIZE (sizeof(TagBucket) * 8)

static void tag_pool_add(void*, Entity, const void*);
static int tag_pool_has(const void* pool, Entity entity);
static void tag_pool_remove(void*, Entity);
static void tag_pool_resize(void*, size_t);
static void tag_pool_free(void*);

typedef struct TagPool {
    EcsPool pool;
    TagBucket* buckets;
    size_t size;
    size_t max_entities;
} TagPool;

EcsPool* tag_pool_new(const EcsManager* manager, const char* name) {
    TagPool* pool = malloc(sizeof(TagPool));
    if (!pool)
        return NULL;

    size_t num_buckets = (manager->sparse_size + BUCKET_SIZE - 1) / BUCKET_SIZE;

    *pool = (TagPool) {
        .pool =
            (EcsPool) {
                .manager = manager,
                .count = 0,
                .info =
                    (PoolInfo) {
                        .name = name,
                        .hash = ecs_pool_get_hash(name),
                    },
                .data = pool,

                .add = tag_pool_add,
                .get = NULL,
                .has = tag_pool_has,
                .remove = tag_pool_remove,
                .resize = tag_pool_resize,
                .free = tag_pool_free,
            },
        .buckets = calloc(num_buckets, sizeof(TagBucket)),
        .size = num_buckets,
        .max_entities = num_buckets * BUCKET_SIZE,
    };

    return &pool->pool;
}

static void tag_pool_add(void* pool, Entity entity, const void* data) {
    TagPool* tag_pool = pool;

    size_t bucket_idx = entity / BUCKET_SIZE;
    size_t bit_offset = entity % BUCKET_SIZE;

    tag_pool->buckets[bucket_idx] |= (1 << bit_offset);
    tag_pool->pool.count++;
}

static int tag_pool_has(const void* pool, Entity entity) {
    const TagPool* tag_pool = pool;

    if (entity >= tag_pool->max_entities) {
        return 0;
    }

    size_t bucket_idx = entity / BUCKET_SIZE;
    size_t bit_offset = entity % BUCKET_SIZE;

    return (tag_pool->buckets[bucket_idx] & 1 << bit_offset) != 0;
}

static void tag_pool_remove(void* pool, Entity entity) {
    TagPool* tag_pool = pool;

    if (entity >= tag_pool->max_entities) {
        return;
    }

    size_t bucket_idx = entity / BUCKET_SIZE;
    size_t bit_offset = entity % BUCKET_SIZE;

    tag_pool->buckets[bucket_idx] &= ~(1 << bit_offset);
}

static void tag_pool_resize(void* pool, size_t new_max_entities) {
    TagPool* tag_pool = pool;

    size_t new_capacity = (new_max_entities + BUCKET_SIZE - 1) / BUCKET_SIZE;
    size_t new_size = new_capacity * sizeof(TagBucket);

    TagBucket* new_buckets = realloc(tag_pool->buckets, new_size);
    if (!new_buckets) {
        printf("[ERROR] Failed to resize tag pool to %zu entities\n", new_max_entities);
        return;
    }

    if (new_capacity > tag_pool->size) {
        size_t old_size = tag_pool->size * sizeof(TagBucket);
        memset(new_buckets + tag_pool->size, 0, new_size - old_size);
    }

    tag_pool->buckets = new_buckets;
    tag_pool->size = new_capacity;
    tag_pool->max_entities = new_capacity * BUCKET_SIZE;
}

void tag_pool_free(void* pool) {
    free(((TagPool*) pool)->buckets);
    free(pool);
}
