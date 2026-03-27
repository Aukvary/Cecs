# Test sources
set(TEST_SOURCES
        CoreTest/main_tests.c
        CoreTest/Tests/TestRegisterAll.c
        CoreTest/Tests/TestCreateRemoveEntity.c
        CoreTest/Tests/TestParentChildrenRelations.c
        CoreTest/Tests/TestFilter.c
        CoreTest/Tests/TestPools.c
        CoreTest/Tests/TestComponentRegister.c
        CoreTest/Tests/TestSystemsRegister.c
        CoreTest/Tests/TestSceneParse.c
        CoreTest/Tests/TestModuleLoad.c
)

set(TEST_MODULE
        CoreTest/TestModule/TestSource.c
)

# Test executable
add_executable(DtEngineTest ${TEST_SOURCES})
set_target_properties(DtEngineTest PROPERTIES
        OUTPUT_NAME "DtEngineTest"
        RUNTIME_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
)

# Test module
add_library(DtEngineTestModule SHARED ${TEST_MODULE})
target_compile_options(DtEngineTestModule PRIVATE -fPIC)
set_target_properties(DtEngineTestModule PROPERTIES
        OUTPUT_NAME "ModuleTest"
        LIBRARY_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
)

# Include directories
target_include_directories(DtEngineTest PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Core"
)
target_include_directories(DtEngineTestModule PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Core"
)