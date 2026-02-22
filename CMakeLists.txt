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

set(SOURCES
        Collections/Collections.c
        Collections/Iterator.c
        Ecs/DtEcsManager.c
        Ecs/Entity.c
        Ecs/EcsPool.c
        Ecs/Systems.c
        Ecs/TagPool.c
        Ecs/ComponentPool.c
        Ecs/ComponentRegister.c
        Ecs/ComponentHandler.c
        Ecs/UpdateHandler.c
        Ecs/UpdateHandler.h
        Ecs/UpdateRegister.c
        Ecs/DrawHandler.c
)

set(TEST_SOURCES
        Tests/TestComponentRegister.c
        Tests/TestCreateRemoveEntity.c
        Tests/TestParentChildrenRelations.c
        Tests/TestFilter.c
        Tests/TestPools.c
)

set(MAIN_TESTS mains/main_tests.c)
set(MAIN mains/main.c)


add_executable(Double_Trouble
        ${MAIN} ${SOURCES}
)

add_executable(Double_Trouble_Tests
        ${MAIN_TESTS} ${SOURCES} ${TEST_SOURCES}
)

# ============ Подключаем заголовки ============
target_include_directories(Double_Trouble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${RAYLIB_INCLUDE_DIR}
)

target_include_directories(Double_Trouble_Tests PRIVATE
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

if (EXISTS ${RAYLIB_LIB_FILE})
    message(STATUS "Linking with: ${RAYLIB_LIB_FILE}")
    target_link_libraries(Double_Trouble_Tests ${RAYLIB_LIB_FILE})
else ()
    message(WARNING "Raylib library not found: ${RAYLIB_LIB_FILE}")
    # Пробуем найти библиотеку по имени
    find_library(RAYLIB_LIB ${RAYLIB_LIB_NAME} PATHS ${RAYLIB_LIB_DIR})
    if (RAYLIB_LIB)
        message(STATUS "Found library: ${RAYLIB_LIB}")
        target_link_libraries(Double_Trouble_Tests ${RAYLIB_LIB})
    else ()
        message(FATAL_ERROR "Cannot find Raylib library!")
    endif ()
endif ()

# ============ Системные библиотеки для Windows ============
if (WIN32)
    target_link_libraries(Double_Trouble_Tests
            opengl32 # OpenGL
            gdi32 # GDI для рисования
            winmm # Windows Multimedia (таймеры, звук)
            m # Math library (может понадобиться)
    )
    # target_link_options(Double_Trouble PRIVATE -mwindows)
endif ()

if (LINUX)
    target_link_libraries(Double_Trouble_Tests
            raylib
            GL X11 Xrandr Xi Xxf86vm Xcursor Xinerama
            asound m pthread dl rt
    )
endif ()

if (WIN32)
    target_link_libraries(Double_Trouble_Tests
            opengl32 # OpenGL
            gdi32 # GDI для рисования
            winmm # Windows Multimedia (таймеры, звук)
            m # Math library (может понадобиться)
    )
    # target_link_options(Double_Trouble PRIVATE -mwindows)
endif ()

if (LINUX)
    target_link_libraries(Double_Trouble_Tests
            raylib
            GL X11 Xrandr Xi Xxf86vm Xcursor Xinerama
            asound m pthread dl rt
    )
endif ()
