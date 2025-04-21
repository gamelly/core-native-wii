cmake_minimum_required(VERSION 3.18)
include(FetchContent)

set(DEVKITPRO "$ENV{DEVKITPRO}")
set(DEVKITPPC "${DEVKITPRO}/devkitPPC")

find_program(ELF2DOL_EXE NAMES elf2dol HINTS "${DEVKITPRO}/tools/bin" REQUIRED NO_DEFAULT_PATH)

set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-g++")
set(CMAKE_LINKER "${DEVKITPPC}/bin/powerpc-eabi-ld")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${DEVKITPRO}/cmake")
set(TOOLCHAIN_FILE "${DEVKITPRO}/cmake/Wii.cmake")

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

project(game C)

link_directories("${DEVKITPRO}/libogc/lib/wii")
link_directories("${DEVKITPRO}/portlibs/ppc/lib")
link_directories("${DEVKITPRO}/portlibs/wii/lib")
set(ENV{PKG_CONFIG_LIBDIR} "${DEVKITPRO}/portlibs/ppc/lib/pkgconfig")
include("${DEVKITPRO}/cmake/Platform/NintendoWii.cmake")

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/vendor")
include_directories("${DEVKITPRO}/libogc/include")

add_compile_definitions(HW_RVL __wii__)
set(CMAKE_C_FLAGS "-DGEKKO -mcpu=750 -meabi -mhard-float ${OGC_COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${OGC_STANDARD_LIBRARIES} -T${DEVKITPPC}/powerpc-eabi/lib/rvl.ld")

# GRRLIB
set(GRRLIB_DIR "${CMAKE_SOURCE_DIR}/vendor/GRRLIB")
set(GRRLIB_VERSION "4.6.0")
set(GRRLIB_GIT "https://github.com/GRRLIB/GRRLIB.git")
FetchContent_Declare(dep_grrlib GIT_REPOSITORY "${GRRLIB_GIT}" GIT_TAG "v${GRRLIB_VERSION}" SOURCE_DIR ${GRRLIB_DIR})
FetchContent_MakeAvailable(dep_grrlib)

# Zig
set(ZIG_VERSION "0.13.0")
set(ZIG_DIR "${CMAKE_SOURCE_DIR}/vendor/zig")
set(ZIG_DOWNLOAD "http://ziglang.org/download")
string(TOLOWER ${CMAKE_HOST_SYSTEM_NAME} CMAKE_HOST_SYSTEM_NAME_LOWER)
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-macos-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.tar.xz")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.tar.xz")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-windows-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.zip")
endif()
if(NOT EXISTS ${ZIG_DIR})
    FetchContent_Populate(dep_zig URL ${ZIG_DOWNLOAD} SOURCE_DIR ${ZIG_DIR})
endif()
find_program(ZIGC zig PATHS ${ZIG_DIR} REQUIRED NO_DEFAULT_PATH)

# Lua
set(LUA_VERSION "5.4.7")
set(LUA_DIR "${CMAKE_SOURCE_DIR}/vendor/lua")
set(LUA_DOWNLOAD "https://github.com/lua/lua/archive/refs/tags/v${LUA_VERSION}.tar.gz")
set(LUA_OPTMIZE FALSE)
if(NOT EXISTS "${LUA_DIR}/lua")
    set(LUA_OPTMIZE TRUE)
    FetchContent_Populate(dep_lua URL "${LUA_DOWNLOAD}" SOURCE_DIR ${LUA_DIR})
    execute_process(
        WORKING_DIRECTORY ${LUA_DIR}
        COMMAND ${ZIGC} cc -DMAKE_LUA onelua.c -o lua
            -target ${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME_LOWER}
    )
endif()
file(GLOB lua_files "${LUA_DIR}/*.c")
list(REMOVE_ITEM lua_files "${LUA_DIR}/lua.c")
list(REMOVE_ITEM lua_files "${LUA_DIR}/onelua.c")
add_library(lua-static STATIC ${lua_files})
find_program(LUAC lua PATHS ${LUA_DIR} REQUIRED NO_DEFAULT_PATH)

# Engine
set(ENGINE_VERSION "0.0.20")
set(ENGINE_DIR "${CMAKE_SOURCE_DIR}/vendor/engine")
set(ENGINE_CLI "${ENGINE_DIR}/src/cli/main.lua")
set(ENGINE_DOWNLOAD "https://github.com/gamelly/gly-engine/archive/refs/tags/${ENGINE_VERSION}.tar.gz")
set(ENGINE_HEADER "${ENGINE_DIR}/bytecode.h")
set(ENGINE_LOVE_VERSION "0.0.17")
set(ENGINE_LOVE_DIR1 "${ENGINE_DIR}/love")
set(ENGINE_LOVE_DIR2 "${CMAKE_SOURCE_DIR}/vendor/love")
set(ENGINE_LOVE_DOWNLOAD "https://github.com/gamelly/love-engine/archive/refs/tags/${ENGINE_LOVE_VERSION}.tar.gz")
if(NOT EXISTS ${ENGINE_HEADER})
    FetchContent_Populate(dep_engine URL ${ENGINE_DOWNLOAD} SOURCE_DIR ${ENGINE_DIR})
    file(MAKE_DIRECTORY "${ENGINE_DIR}/lite")
    file(MAKE_DIRECTORY "${ENGINE_DIR}/love")
    file(MAKE_DIRECTORY "${ENGINE_DIR}/micro")
    file(MAKE_DIRECTORY "${ENGINE_DIR}/native")
    file(MAKE_DIRECTORY "${ENGINE_DIR}/samples/template")
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} build --core micro --bundler --dist ${ENGINE_DIR}/micro @helloworld
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} build --core lite --bundler --dist ${ENGINE_DIR}/lite @helloworld
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} build --core native --bundler --dist ${ENGINE_DIR}/native @helloworld
        COMMAND_ERROR_IS_FATAL ANY
    )
    if(NOT DEFINED TEMPLATE_SIZE)
        set(TEMPLATE_SIZE 100000)
    endif()
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} tool-template-fill ${ENGINE_DIR}/samples/template/game.lua ${TEMPLATE_SIZE}
    )
    if (NOT DEFINED ENGINE)
        set(ENGINE "@lite")
    endif()
endif()
if(DEFINED ENGINE)
    if (ENGINE STREQUAL "@love" AND NOT EXISTS "${ENGINE_LOVE_DIR1}/main.lua")
        message(WARNING "currently LOVE2D is experimental.")
        FetchContent_Populate(dep_love URL ${ENGINE_LOVE_DOWNLOAD} SOURCE_DIR ${ENGINE_LOVE_DIR2})
        file(COPY "${ENGINE_LOVE_DIR2}/src/main.lua" DESTINATION "${ENGINE_LOVE_DIR1}")
    endif()
    if (ENGINE MATCHES "^\\@")
        string(SUBSTRING "${ENGINE}" 1 -1 ENGINE)
        set(ENGINE "${ENGINE_DIR}/${ENGINE}/main.lua")
    endif()
    if(NOT EXISTS ${ENGINE})
        message(FATAL_ERROR "engine not found: ${ENGINE}")
    endif()
    get_filename_component(ENGINE "${ENGINE}" ABSOLUTE)
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} fs-xxd-i ${ENGINE} ${ENGINE_HEADER} --const --name engine_bytecode_lua
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# Game
string(LENGTH "${GAME}" GAME_LENGTH)
set(GAME_HEADER "${CMAKE_SOURCE_DIR}/vendor/game/bytecode.h")
if (DEFINED GAME AND GAME_LENGTH AND GAME MATCHES "^\\@love:")
    string(SUBSTRING "${GAME}" 6 -1 GAME)
    set(GAME "${ENGINE_LOVE_DIR2}/samples/${GAME}/main.lua")
endif()
if (DEFINED GAME AND GAME_LENGTH AND GAME MATCHES "^\\@")
    string(SUBSTRING "${GAME}" 1 -1 GAME)
    set(GAME "${ENGINE_DIR}/samples/${GAME}/game.lua")
endif()
if ((NOT DEFINED GAME AND NOT EXISTS ${GAME_HEADER}) OR (DEFINED GAME AND NOT GAME_LENGTH))
    set(GAME "${ENGINE_DIR}/samples/template/game.lua")
    set(GAME_LENGTH 1)
endif()
if(DEFINED GAME AND GAME_LENGTH)
    if(NOT EXISTS ${GAME})
        message(FATAL_ERROR "game not found: ${GAME}")
    endif()
    get_filename_component(GAME "${GAME}" ABSOLUTE)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/game")
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} fs-xxd-i ${GAME} ${GAME_HEADER} --const --name game_bytecode_lua
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# Font
set(FONT_DIR "${CMAKE_SOURCE_DIR}/vendor/font")
set(FONT_DOWNLOAD "https://github.com/gamelly/fonts/archive/refs/heads/main.tar.gz")
if(NOT EXISTS "${FONT_DIR}")
    FetchContent_Populate(dep_font URL ${FONT_DOWNLOAD} SOURCE_DIR ${FONT_DIR})
    set(FONT_NOTO "${FONT_DIR}/Noto_Sans")
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} fs-xxd-i --const --name notosans_regular_ttf
            ${FONT_NOTO}/NotoSans-Regular.ttf
            ${FONT_NOTO}/notosans_regular.h 
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# Lua perfomance
if(LUA_OPTMIZE)
    execute_process(
        WORKING_DIRECTORY ${ENGINE_DIR}
        COMMAND ${LUAC} ${ENGINE_CLI} fs-luaconf ${LUA_DIR}/luaconf.h --32bits
    )
endif()

# Entrypoint
set(ENTRYPOINT "${CMAKE_CURRENT_BINARY_DIR}/crt0.c")
if(NOT EXISTS "${ENTRYPOINT}")
    #/opt/devkitpro/devkitPPC/powerpc-eabi/lib/crtmain.o
    file(WRITE "${ENTRYPOINT}" "int main();char dvmInitDefault(){return 1;}int __crtmain(){return main();}")
endif()

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(${PROJECT_NAME} "${SOURCES};${ENTRYPOINT}")
target_link_libraries(${PROJECT_NAME} grrlib freetype png z bz2)
target_link_libraries(${PROJECT_NAME} lua-static)
ogc_create_dol(${PROJECT_NAME})