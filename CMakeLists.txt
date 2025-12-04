cmake_minimum_required(VERSION 4.0)
project(Double_Trouble C)

set(CMAKE_C_STANDARD 11)

add_executable(Double_Trouble main.c
        Collections/Collections.h
        Collections/Collections.c
        Collections/Iterator.c
        Collections/Iterator.h
        Ecs/EcsTypes.h
        Ecs/EcsManager/EcsManager.c
        Ecs/EcsManager/EcsManager.h
        Ecs/Entity/Entity.c
        Ecs/Entity/Entity.h
        Ecs/Pools/EcsPool.c
        Ecs/Pools/EcsPool.h
        Ecs/Systems/Systems.h
        Ecs/Systems/Systems.c
        Ecs/Pools/TagPool.c
        Ecs/Pools/ComponentPool.c
)
