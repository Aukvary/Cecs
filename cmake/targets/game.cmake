set(GAME_SOURCES
        GameScripts/DeleteMe.c
)

# Game libraries
add_library(GameLibStatic STATIC ${GAME_SOURCES})
add_library(GameLibShared SHARED ${GAME_SOURCES})

# Platform-specific properties for GameLib
if (WIN32)
    set_target_properties(GameLibStatic PROPERTIES
            OUTPUT_NAME "GameLib"
            ARCHIVE_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
    )
    set_target_properties(GameLibShared PROPERTIES
            OUTPUT_NAME "GameLib"
            RUNTIME_OUTPUT_DIRECTORY "${EDITOR_OUTPUT_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${EDITOR_OUTPUT_DIR}"
    )
    target_compile_definitions(GameLibShared PRIVATE -DGAMELIB_EXPORTS)
else ()
    set_target_properties(GameLibStatic PROPERTIES
            OUTPUT_NAME "GameLib"
            ARCHIVE_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
    )
    set_target_properties(GameLibShared PROPERTIES
            OUTPUT_NAME "GameLib"
            LIBRARY_OUTPUT_DIRECTORY "${EDITOR_OUTPUT_DIR}"
            RUNTIME_OUTPUT_DIRECTORY "${EDITOR_OUTPUT_DIR}"
    )
endif ()

target_compile_definitions(GameLibShared PRIVATE -DEDITOR)

# Game executable
add_executable(Game Game/main_game.c ${GAME_SOURCES})
set_target_properties(Game PROPERTIES
        OUTPUT_NAME "Game"
        RUNTIME_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
)

# Include directories
target_include_directories(GameLibStatic PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Core"
)
target_include_directories(GameLibShared PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Core"
)
target_include_directories(Game PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Core"
        "${CMAKE_CURRENT_SOURCE_DIR}/Game"
)