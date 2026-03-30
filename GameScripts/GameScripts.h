#ifndef GAME_SCRIPTS_H
#define GAME_SCRIPTS_H

#include <raylib.h>


#include "Ecs/RegisterHandler.h"

#define GAME_TYPE(X, name)\
    X(int, field1, name, (DtAttributeData){.attribute_name = DTE_INSPECTOR_HIDE})\
    X(char*, num, name)
DT_DEFINE_COMPONENT(GameType, GAME_TYPE)

#endif /*GAME_SCRIPTS_H*/
