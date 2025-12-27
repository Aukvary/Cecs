cmake_minimum_required(VERSION 3.16)
project(Double_Trouble C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============ Настройки Raylib ============
# Пути (на основе вашей структуры)
set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RAYLIB_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Проверяем, какой компилятор используется
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")  # MinGW или Clang
    message(STATUS "Using GCC/Clang compiler")
    set(RAYLIB_LIB_NAME "raylib")
    set(RAYLIB_LIB_FILE "${RAYLIB_LIB_DIR}/libraylib.a")  # Для MinGW нужен .a файл
elseif (MSVC)  # Visual Studio
    message(STATUS "Using MSVC compiler")
    set(RAYLIB_LIB_NAME "raylib")
    set(RAYLIB_LIB_FILE "${RAYLIB_LIB_DIR}/raylib.lib")
endif ()

# ============ Исполняемый файл ============
# Только .c файлы, .h файлы НЕ нужно добавлять здесь
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
        Ecs/EcsTests.h
        Game/Components/Components.h
        Game/Systems/GameSystems.c
        Game/Systems/GameSystems.h
        Game/Game.c
        Game/Game.h
        Game/Components/Components.c
        Ecs/Pools/BaseComponents.h
)

# Копируем папку source в build директорию
add_custom_target(update_resources
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/source"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_BINARY_DIR}/source"
        COMMENT "Копирование ресурсов..."
        VERBATIM
)

# Делаем так, чтобы основная цель зависела от этой
add_dependencies(Double_Trouble update_resources)


# ============ Подключаем заголовки ============
target_include_directories(Double_Trouble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}          # Для ваших заголовков
        ${RAYLIB_INCLUDE_DIR}                # Для raylib.h
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
    if (MINGW)  # CLion с MinGW
        target_link_libraries(Double_Trouble
                opengl32   # OpenGL
                gdi32      # GDI для рисования
                winmm      # Windows Multimedia (таймеры, звук)
                m          # Math library (может понадобиться)
        )

        # Если хотите окно без консоли, раскомментируйте:
        # target_link_options(Double_Trouble PRIVATE -mwindows)

    elseif (MSVC)  # Visual Studio
        target_link_libraries(Double_Trouble
                opengl32.lib
                gdi32.lib
                winmm.lib
                user32.lib
        )

        # Для оконного приложения без консоли:
        # set_target_properties(Double_Trouble PROPERTIES
        #     LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        # )
    endif ()
endif ()