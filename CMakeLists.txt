cmake_minimum_required(VERSION 3.16)
project(Double_Trouble C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RAYLIB_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(RAYLIB_LIB_NAME "raylib")
set(RAYLIB_LIB_FILE "${RAYLIB_LIB_DIR}/libraylib.a")

add_executable(Double_Trouble
        main.c
        Collections/Collections.c
        Collections/Iterator.c
        Ecs/EcsManager/EcsManager.c
        Ecs/Entity/Entity.c
        Ecs/Pools/EcsPool.c
        Ecs/Systems/Systems.c
        Ecs/Pools/TagPool.c
        Ecs/Pools/ComponentPool.c
        Game/Systems/GameSystems.c
        Game/Game.c
        Game/Components/Components.c
        Tests/EcsManagerTests.c
        serializing/pref.c
        serializing/pref.h
        serializing/SizeTypes.h
        serializing/SerializedComponent.h
)

# ============ Подключаем заголовки ============
target_include_directories(Double_Trouble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${RAYLIB_INCLUDE_DIR}
)

# ============ Линковка с Raylib ============
if (EXISTS ${RAYLIB_LIB_FILE})
    message(STATUS "Linking with: ${RAYLIB_LIB_FILE}")
    target_link_libraries(Double_Trouble ${RAYLIB_LIB_FILE})
else ()
    message(WARNING "Raylib library not found: ${RAYLIB_LIB_FILE}")
    # Пробуем найти библиотеку по имени
    find_library(RAYLIB_LIB ${RAYLIB_LIB_NAME} PATHS ${RAYLIB_LIB_DIR})
    if (RAYLIB_LIB)
        message(STATUS "Found library: ${RAYLIB_LIB}")
        target_link_libraries(Double_Trouble ${RAYLIB_LIB})
    else ()
        message(FATAL_ERROR "Cannot find Raylib library!")
    endif ()
endif ()

# ============ Системные библиотеки для Windows ============
if (WIN32)
    target_link_libraries(Double_Trouble
            opengl32   # OpenGL
            gdi32      # GDI для рисования
            winmm      # Windows Multimedia (таймеры, звук)
            m          # Math library (может понадобиться)
    )
    # target_link_options(Double_Trouble PRIVATE -mwindows)
endif ()