#ifndef GAME_SCRIPTS_H
#define GAME_SCRIPTS_H

#include "Ecs/RegisterHandler.h"

#define GAME_TYPE(X, name)\
    X(int, field1, name)\
    X(char*, field2, name)
DT_DEFINE_COMPONENT(GameType, GAME_TYPE)

#endif /*GAME_SCRIPTS_H*/
