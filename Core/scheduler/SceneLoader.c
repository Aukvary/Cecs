#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Collections/Collections.h"
#include "DtAllocators.h"
#include "scheduler/RuntimeScheduler.h"


#define SCENE_EXTENSION ".dt.scene"

static DtScene* dt_scene_parse(const char* path, DtEnvironment* env);
static DtScene* dt_scene_parse_from_json(const char* scene_info, DtEnvironment* env);
static void dt_scene_parse_ecs_manager(cJSON* json_cfg, DtScene* scene);
static void dt_scene_parse_update_systems(cJSON* systems, DtScene* scene);
static void dt_scene_parse_draw_systems(cJSON* systems, DtScene* scene);
static void dt_scene_parse_entities(cJSON* entities, DtScene* scene);

static u64 get_scene_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

static int is_scene(const char* filename) {
    const size_t len_filename = strlen(filename);
    const size_t len_ext = strlen(SCENE_EXTENSION);

    if (len_filename < len_ext)
        return 0;
    return strcmp(filename + len_filename - len_ext, SCENE_EXTENSION) == 0;
}

const DtScene* dt_add_scene(const char* path) {
    DtEnvironment* env = dt_environment_instance();
    if (!is_scene(path)) {
        fprintf(stderr, "[DEBUG] File is not a scene or doesn't exist: %s\n", path);
        return NULL;
    }

    char* file_name = strrchr(path, '/') + 1;
    if (file_name == NULL)
        file_name = path;
#if defined(_WIN32) || defined(_WIN64)
    char* win_file_name = strrchr(path, '\\');
    if (win_file_name != NULL)
        file_name = win_file_name;
#endif

    size_t len = strlen(file_name) - strlen(SCENE_EXTENSION);

    DtScene* scene = dt_scene_parse(path, env);
    if (!scene) {
        fprintf(stderr, "[DEBUG] Failed to parse scene: %s\n", path);
        return NULL;
    }

    char* name = DT_MALLOC(len + 1);
    name[len] = '\0';
    strncpy(name, file_name, len);

    scene->name = name;

    dt_rb_tree_add(&env->scenes, scene, get_scene_hash(name));

    return scene;
}

const DtScene* dt_add_scene_from_json(const char* json, const char* name) {
    DtEnvironment* env = dt_environment_instance();
    DtScene* scene = dt_scene_parse_from_json(json, env);
    if (!scene) {
        fprintf(stderr, "[DEBUG] Failed to parse scene: %s\n", "from source");
        return NULL;
    }

    char* name_copy = strdup(name);
    scene->name = name_copy;
    dt_rb_tree_add(&env->scenes, scene, get_scene_hash(name));

    return scene;
}

static DtScene* dt_scene_parse(const char* path, DtEnvironment* env) {
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "[ERROR] Could not open file %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    i32 size = (i32) ftell(file);
    fseek(file, 0, SEEK_SET);

    char* scene_info = DT_MALLOC(size + 1);
    fread(scene_info, 1, size, file);

    cJSON* root = cJSON_Parse(scene_info);
    if (root == NULL) {
        fprintf(stderr, "[ERROR] file hasn't scene data%s\n", path);
        return NULL;
    }

    cJSON* manager_config = cJSON_GetObjectItem(root, "manager_config");
    cJSON* update_systems = cJSON_GetObjectItem(root, "update_systems");
    cJSON* draw_systems = cJSON_GetObjectItem(root, "draw_systems");
    cJSON* entities = cJSON_GetObjectItem(root, "entities");

#ifdef DEBUG
    if (!manager_config) {
        fprintf(stderr, "[WARNING] file hasn't manager_config data%s\n", path);
    }

    if (!update_systems) {
        fprintf(stderr, "[WARNING] file hasn't update_systems data%s\n", path);
    }

    if (!draw_systems) {
        fprintf(stderr, "[WARNING] file hasn't draw_systems data%s\n", path);
    }

    if (!entities) {
        fprintf(stderr, "[WARNING] file hasn't entities data%s\n", path);
    }
#endif

    DtScene* scene = DT_MALLOC(sizeof(DtScene));

    if (scene == NULL) {
        fprintf(stderr, "[error] didn't manage to allocate memory for scene%s\n", path);
        return NULL;
    }

    *scene = (DtScene) {
        .environment = env,
    };

    dt_scene_parse_ecs_manager(manager_config, scene);
    dt_scene_parse_update_systems(update_systems, scene);
    dt_scene_parse_draw_systems(draw_systems, scene);
    dt_scene_parse_entities(entities, scene);


    fclose(file);
    free(scene_info);
    return scene;
}

static DtScene* dt_scene_parse_from_json(const char* scene_info, DtEnvironment* env) {
    cJSON* root = cJSON_Parse(scene_info);
    if (root == NULL) {
        fprintf(stderr, "[ERROR] file hasn't scene data%s\n", "from source");
        return NULL;
    }

    cJSON* manager_config = cJSON_GetObjectItem(root, "manager_config");
    cJSON* update_systems = cJSON_GetObjectItem(root, "update_systems");
    cJSON* draw_systems = cJSON_GetObjectItem(root, "draw_systems");
    cJSON* entities = cJSON_GetObjectItem(root, "entities");

#ifdef DEBUG
    if (!manager_config) {
        fprintf(stderr, "[WARNING] file hasn't manager_config data%s\n", "from source");
    }

    if (!update_systems) {
        fprintf(stderr, "[WARNING] file hasn't update_systems data%s\n", "from source");
    }

    if (!draw_systems) {
        fprintf(stderr, "[WARNING] file hasn't draw_systems data%s\n", "from source");
    }

    if (!entities) {
        fprintf(stderr, "[WARNING] file hasn't entities data%s\n", "from source");
    }
#endif

    DtScene* scene = DT_MALLOC(sizeof(DtScene));

    if (scene == NULL) {
        fprintf(stderr, "[error] didn't manage to allocate memory for scene%s\n", "from source");
        return NULL;
    }

    *scene = (DtScene) {
        .environment = env,
    };

    dt_scene_parse_ecs_manager(manager_config, scene);
    dt_scene_parse_update_systems(update_systems, scene);
    dt_scene_parse_draw_systems(draw_systems, scene);
    dt_scene_parse_entities(entities, scene);

    return scene;
}

static void dt_scene_parse_ecs_manager(cJSON* json_cfg, DtScene* scene) {
    if (!json_cfg) {
        scene->manager = dt_ecs_manager_new((DtEcsManagerConfig){0});
        return;
    }

    DtEcsManagerConfig cfg;

#define GET_JSON_INT(json, field)                                                                  \
    ({                                                                                             \
        cJSON* item = cJSON_GetObjectItem(json, #field);                                           \
        if (item) {                                                                                \
            cfg.field = (int) cJSON_GetNumberValue(item);                                          \
        } else {                                                                                   \
            fprintf(stderr, "[WARNING] Scene hasn't \"" #field "\" data\n");                        \
            cfg.field = 0;                                                                         \
        }                                                                                          \
    })

    GET_JSON_INT(json_cfg, dense_size);
    GET_JSON_INT(json_cfg, sparse_size);
    GET_JSON_INT(json_cfg, recycle_size);
    GET_JSON_INT(json_cfg, children_size);
    GET_JSON_INT(json_cfg, components_count);
    GET_JSON_INT(json_cfg, pools_size);
    GET_JSON_INT(json_cfg, include_mask_count);
    GET_JSON_INT(json_cfg, exclude_mask_count);
    GET_JSON_INT(json_cfg, filters_size);
    GET_JSON_INT(json_cfg, masks_size);

#undef GET_JSON_INT

    scene->manager = dt_ecs_manager_new(cfg);
}

static void dt_scene_parse_update_systems(cJSON* systems, DtScene* scene) {
    if (!systems) {
        scene->update_handler = dt_update_handler_new(scene->manager, 0);
        return;
    }

    u16 count = cJSON_GetArraySize(systems);

    scene->update_handler = dt_update_handler_new(scene->manager, count);
    cJSON* system = NULL;
    cJSON_ArrayForEach(system, systems) {
        const DtUpdateData* data = NULL;
        const char* name = cJSON_GetStringValue(system);
        if ((data = dt_update_get_data_by_name(name))) {
            UpdateSystem* s = data->new();
            dt_update_handler_add(scene->update_handler, s);
        }
    }
}

static void dt_scene_parse_draw_systems(cJSON* systems, DtScene* scene) {
    if (!systems) {
        scene->draw_handler = dt_draw_handler_new(scene->manager, 0);
        return;
    }
    u16 count = cJSON_GetArraySize(systems);

    scene->draw_handler = dt_draw_handler_new(scene->manager, count);
    cJSON* system = NULL;
    cJSON_ArrayForEach(system, systems) {
        const DtDrawData* data = NULL;
        const char* name = cJSON_GetStringValue(system);
        if ((data = dt_draw_get_data_by_name(name))) {
            dt_draw_handler_add(scene->draw_handler, data->new());
        }
    }
}

static void dt_scene_parse_entities(cJSON* entities, DtScene* scene) {
    if (!entities)
        return;
    const cJSON* json_entity = NULL;
    cJSON_ArrayForEach(json_entity, entities) {
        const cJSON* components = cJSON_GetObjectItem(json_entity, "components");

        const DtEntity entity = dt_ecs_manager_new_entity(scene->manager);
        const cJSON* component = NULL;
        cJSON_ArrayForEach(component, components) {
            if (cJSON_IsString(component)) {
                dt_ecs_manager_entity_add_component(scene->manager, entity,
                                                    cJSON_GetStringValue(component), NULL);
            } else {
                const char* name = cJSON_GetStringValue(cJSON_GetObjectItem(component, "name"));
                const cJSON* values = cJSON_GetObjectItem(component, "values");
                const DtComponentData* data = dt_component_get_data_by_name(name);
                void* instance = DT_STACK_ALLOC(data->component_size);

                cJSON* value = NULL;
                cJSON_ArrayForEach(value, values) {
                    const char* field_name = value->string;
                    const i32 i = dt_component_get_field_index(data, field_name);
                    if (i == -1)
                        continue;

                    u16 offset = data->field_offsets[i];
                    const char* type = data->field_types[i];

                    dt_parse_json_to_type(type, value, (u8*) instance + offset);
                }
                dt_ecs_manager_entity_add_component(scene->manager, entity, name, instance);
            }
        }
    }

    json_entity = NULL;
    cJSON_ArrayForEach(json_entity, entities) {
        const cJSON* parent = cJSON_GetObjectItem(json_entity, "parent");
        if (!parent)
            continue;
        dt_ecs_manager_set_parent(scene->manager, (u16) atoi(json_entity->string),
                                  (u16) cJSON_GetNumberValue(parent));
    }
}

void dt_scene_unload_by(const DtScene* scene) {
    if (!scene)
        return;

    dt_rb_tree_remove(&dt_environment_instance()->scenes, get_scene_hash(scene->name));

    DT_FREE(scene->name);
    dt_ecs_manager_free(scene->manager);

    dt_update_handler_destroy(scene->update_handler);
    dt_update_handler_free(scene->update_handler);

    dt_draw_handler_destroy(scene->draw_handler);
    dt_draw_handler_free(scene->draw_handler);
}
