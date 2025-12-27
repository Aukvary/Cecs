#ifndef GAME_H
#define GAME_H
#include "Ecs/Ecs.h"

void init_game();

void pre_update();
void update();
void post_update();

void pre_draw();
void draw();
void post_draw();

void destroy_game();

#endif
