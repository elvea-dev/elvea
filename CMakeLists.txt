######################################
##  THE ELVEA PROGRAMMING LANGUAGE  ##
######################################

cmake_minimum_required(VERSION 3.9)
project(elvea C)

set(CMAKE_C_STANDARD 99)


#####################
##  BUILD OPTIONS  ##
#####################

set(BUILD_UNIT_TEST ON)


#add_definitions(-DELVEA_USE_WXWIDGETS=1)

###########################
##  VERSION INFORMATION  ##
###########################

# Major release: incremented when major changes are introduced
add_definitions(-DELVEA_VERSION_MAJOR=0)
# Minor release: incremented when new features and/or bug fixes are introduced
add_definitions(-DELVEA_VERSION_MINOR=0)
# Revision release: incremented when bugs are fixed are small features are added
add_definitions(-DELVEA_VERSION_MICRO=1)
# Build: this is only used internally for development (set to 0 on public release)
add_definitions(-DELVEA_VERSION_BUILD=0)

# Release date
add_definitions(-DELVEA_RELEASE_DATE_YEAR=2018)
add_definitions(-DELVEA_RELEASE_DATE_MONTH=2)
add_definitions(-DELVEA_RELEASE_DATE_DAY=1)


#####################
##  BUILD RUNTIME  ##
#####################

if(CMAKE_COMPILER_IS_GNUCC)
    message("Compiling with GCC")
    set(CMAKE_C_FLAGS "-Werror")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    message("Compiling with G++")
    set(CMAKE_CXX_FLAGS "-Werror")
endif()

# Operating system
if(WIN32)
    add_definitions(-DELVEA_WINDOWS -DUNICODE -D_UNICODE)
    if (MINGW)
        add_definitions(-DELVEA_POSIX)
        add_definitions(-DELVEA_MINGW)
    endif()
else()
    add_definitions(-DELVEA_POSIX)
    add_definitions(-DELVEA_UNIX)

    if(APPLE)
        add_definitions(-DELVEA_MACOS)
    else()
        # Assume linux
        add_definitions(-DELVEA_LINUX)
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/runtime)

## Use UTF-8 in PCRE2.
add_definitions(-DPCRE2_CODE_UNIT_WIDTH=8 -DPCRE2_STATIC=1)
#add_definitions(-DELVEA_WITH_REGEX=1)


if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

file(GLOB VM_FILES
        runtime/elvea/*.c runtime/elvea/*.h
        runtime/elvea/utils/*.c runtime/elvea/utils/*.h
        runtime/elvea/third_party/utf8proc/utf8proc.c
        runtime/elvea/third_party/utf8proc/utf8proc.h
        runtime/elvea/third_party/utf8.h
        runtime/elvea/third_party/tinycthread/tinycthread.h
        runtime/elvea/third_party/tinycthread/tinycthread.c
)


add_library(elvea-vm ${VM_FILES})

#find_package(wxWidgets COMPONENTS aui webview adv stc core base REQUIRED)
#include("${wxWidgets_USE_FILE}")

if(WIN32)
    target_link_libraries(elvea-vm pthread pcre2-8 shlwapi)
else()
    target_link_libraries(elvea-vm m) # pcre2-8)
endif()


if(BUILD_UNIT_TEST)
    file(GLOB_RECURSE TEST_FILES ./unit_test/*.c)
    add_executable(test_elvea ${TEST_FILES})
    target_link_libraries(test_elvea elvea-vm)
endif(BUILD_UNIT_TEST)

set(SRC_FILES runtime/elvea.c)
add_executable(elvea ${SRC_FILES})
target_link_libraries(elvea pthread elvea-vm)