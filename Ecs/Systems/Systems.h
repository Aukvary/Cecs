#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "../EcsManager/EcsManager.h"
#include "../EcsTypes.h"

typedef void (*Action)(void*);
typedef void (*Init)(EcsManager*, void*);

/**
 * @brief pair of system data and init func
 */
typedef struct {
    void* data;
    Init init;
} InitDataPair;

/**
 * @brief pair of system data and some action func
 */
typedef struct {
    void* data;
    Action action;
} ActionDataPair;

/**
 * @brief pair of system data and some update func
 */
typedef struct {
    size_t init_size;

    size_t pre_update_size;
    size_t update_size;
    size_t post_update_size;

    size_t tag_remove_size;

    size_t destroy_size;
} SystemHandlerConfig;


/**
 * @brief container for system funcs
 *
 * @warning if system hasn't any func it must be initialized by NULL
 */
struct EcsSystem {
    void* data;

    Init init;

    Action pre_update;
    Action update;
    Action post_update;

    Action tag_remove;

    Action destroy;
};

/**
 * @brief return new system
 *
 * @param init called on system handler initialize
 * @param pre_update called before update func
 * @param update called on main update cycle
 * @param post_update called after update func
 * @param tag_remove use for remove tags after update funcs
 * @param destroy called on system handler destroy
 *
 * @warning if system hasn't any func it must be initialized by NULL
 */
EcsSystem* ecs_system_new(Init init, Action pre_update, Action update, Action post_update,
                          Action tag_remove, Action destroy);

/**
 * @brief container where systems grouped by funcs
 */
struct SystemHandler {
    EcsManager* manager;

    VEC(InitDataPair) inits;

    VEC(ActionDataPair) pre_updates;
    VEC(ActionDataPair) updates;
    VEC(ActionDataPair) post_updates;

    VEC(ActionDataPair) tag_removes;

    VEC(ActionDataPair) destroys;
};

/**
 * @brief return new system handler
 *
 * @param manager ecs manager that handle pools
 * @param cfg config for initialize
 *
 * @return return pointer to new system handler
 */
SystemHandler* system_handler_new(EcsManager* manager, const SystemHandlerConfig* cfg);

/**
 * @brief add system in pools if system has needed func
 *
 * @param handler handler where we add system
 * @param system added system
 *
 * @return return pointer to new system handler
 */
void system_handler_add(SystemHandler* handler, const EcsSystem* system);

/**
 * @brief call all init systems funcs
 */
void system_handler_init(const SystemHandler* handler);

/**
 * @brief call all pre_update systems funcs
 */
void system_handler_pre_update(const SystemHandler* handler);

/**
 * @brief call all update systems funcs
 */
void system_handler_update(const SystemHandler* handler);

/**
 * @brief call all post_update systems funcs
 */
void system_handler_post_update(const SystemHandler* handler);

/**
 * @brief call all remove_tag systems funcs
 */
void system_handler_remove_tag(const SystemHandler* handler);

/**
 * @brief call all destroy systems funcs
 */
void system_handler_destroy(const SystemHandler* handler);

/**
 * @brief free all systems and handler
 */
void system_handler_free(SystemHandler* handler);

#endif
