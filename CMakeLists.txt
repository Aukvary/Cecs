cmake_minimum_required(VERSION 3.16)
project(Double_Trouble C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif ()

set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RAYLIB_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(RAYLIB_LIB_NAME "raylib")

if (WIN32)
    set(RAYLIB_LIB_FILE "${RAYLIB_LIB_DIR}/windows/libraylib.a")
endif ()

if (LINUX)
    set(RAYLIB_LIB_FILE "${RAYLIB_LIB_DIR}/linux/libraylib.a")
endif ()

add_executable(Double_Trouble
        main.c
        Collections/Collections.c
        Collections/Iterator.c
        Ecs/DtEcsManager.c
        Ecs/Entity.c
        Ecs/EcsPool.c
        Ecs/Systems.c
        Ecs/TagPool.c
        Ecs/ComponentPool.c
        Tests/TestCreateRemoveEntity.c
        Tests/TestParentChildrenRelations.c
        Ecs/ComponentRegister.c
        DtNumericalTypes.h
        Ecs/ComponentHandler.c
        DtAllocators.h
        Tests/TestComponentRegister.c
)

# ============ Подключаем заголовки ============
target_include_directories(Double_Trouble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${RAYLIB_INCLUDE_DIR}
)

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
            opengl32 # OpenGL
            gdi32 # GDI для рисования
            winmm # Windows Multimedia (таймеры, звук)
            m # Math library (может понадобиться)
    )
    # target_link_options(Double_Trouble PRIVATE -mwindows)
endif ()

if (LINUX)
    target_link_libraries(Double_Trouble
            raylib
            GL X11 Xrandr Xi Xxf86vm Xcursor Xinerama
            asound m pthread dl rt
    )
endif ()
