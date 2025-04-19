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
FetchContent_Declare(
    dep_grrlib
    GIT_REPOSITORY https://github.com/GRRLIB/GRRLIB.git
    GIT_TAG v4.6.0
    SOURCE_DIR ${GRRLIB_DIR}
)
FetchContent_MakeAvailable(dep_grrlib)

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} grrlib freetype png z bz2)
ogc_create_dol(${PROJECT_NAME})