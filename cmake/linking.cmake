foreach (TARGET DtEngine DtEngineTestModule DtEngineTest GameLibStatic GameLibShared EditorLib Editor Game)
    if (TARGET ${TARGET})
        target_include_directories(${TARGET} PRIVATE
                ${CJSON_INCLUDE_DIRS}
                ${RAYLIB_INCLUDE_DIRS}
        )

        target_link_libraries(${TARGET} PRIVATE
                ${CJSON_LINK_LIBRARIES}
                ${RAYLIB_LINK_LIBRARIES}
        )

        if (WIN32)
            target_link_libraries(${TARGET} PRIVATE
                    winmm
                    gdi32
            )
        else ()
            target_link_libraries(${TARGET} PRIVATE
                    m
            )
        endif ()
    endif ()
endforeach ()

foreach (TARGET DtEngineTestModule DtEngineTest GameLibStatic GameLibShared Editor EditorLib)
    target_link_libraries(${TARGET} PRIVATE $<TARGET_OBJECTS:DtEngine_Objects>)
endforeach ()

target_link_libraries(GameLibShared PRIVATE EditorLib)
target_link_libraries(Game PUBLIC GameLibStatic)

set(MY_TARGETS
        DtEngine_Objects
        DtEngineTest
        GameLibStatic
        GameLibShared
        Editor
        EditorLib
)

foreach (TGT ${MY_TARGETS})
    if (TARGET ${TGT})
        target_compile_definitions(${TGT} PRIVATE
                $<$<OR:$<BOOL:${DEBUG_ENABLE}>,$<CONFIG:Debug>>:DEBUG>
        )
    endif ()
endforeach ()