#include "GameComponents.h"

void game_collider_2D_reset(void* data) {
    GameCollider2D* collider = data;
}

DT_REGISTER_COMPONENT(GameCollider2D, GAME_COLLIDER_2D)