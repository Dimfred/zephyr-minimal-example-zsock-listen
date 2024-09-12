cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tflite_micro_example)

set(NO_THREADSAFE_STATICS $<TARGET_PROPERTY:compiler-cpp,no_threadsafe_statics>)
zephyr_compile_options($<$<COMPILE_LANGUAGE:CXX>:${NO_THREADSAFE_STATICS}>)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

if("$ENV{DEBUG}" STREQUAL "1")
    add_compile_definitions(DEBUG=1)
endif()

file(GLOB APP_SOURCES_C src/*.c)
file(GLOB APP_SOURCES_CPP src/*.cpp)
file(GLOB TASK_SOURCES_CPP src/*/*.cpp)
set(APP_SOURCES "${APP_SOURCES_C};${APP_SOURCES_CPP};${TASK_SOURCES_CPP}")
message("-- APP_SOURCES=${APP_SOURCES}")

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE "${APP_SOURCES}")
target_link_libraries(app PRIVATE zephyr zephyr_interface)


################################################################################
# HTTP SERVER
target_link_libraries(app PRIVATE zephyr_interface zephyr)
zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME http_resource_desc_httpserver_service
    KVMA RAM_REGION GROUP RODATA_REGION SUBALIGN Z_LINK_ITERABLE_SUBALIGN)
