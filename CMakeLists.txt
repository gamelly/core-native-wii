cmake_minimum_required(VERSION 3.18)

set(DEVKITPRO "$ENV{DEVKITPRO}")
set(DEVKITPPC "${DEVKITPRO}/devkitPPC")

set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_LINKER "${DEVKITPPC}/bin/powerpc-eabi-ld")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${DEVKITPRO}/cmake")
set(TOOLCHAIN_FILE "${DEVKITPRO}/cmake/Wii.cmake")

project(Wii_Game C)

link_directories("${DEVKITPRO}/libogc/lib/wii")
include("${DEVKITPRO}/cmake/Platform/NintendoWii.cmake")

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/vendor")
include_directories("${DEVKITPRO}/libogc/include")

set(CMAKE_C_FLAGS "-DGEKKO -mcpu=750 -meabi -mhard-float ${OGC_COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${OGC_STANDARD_LIBRARIES} -T${DEVKITPPC}/powerpc-eabi/lib/rvl.ld")

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ogc m)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${DEVKITPPC}/bin/powerpc-eabi-objcopy" -O binary ${PROJECT_NAME}.elf bin/${PROJECT_NAME}.dol
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
