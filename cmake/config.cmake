if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED libcjson)
pkg_check_modules(RAYLIB REQUIRED raylib)

