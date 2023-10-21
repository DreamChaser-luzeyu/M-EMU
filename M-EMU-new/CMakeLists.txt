cmake_minimum_required(VERSION 3.25)
project(M_EMU)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(modules)

link_directories(${PROJECT_SOURCE_DIR}/lib)

aux_source_directory(src FRAMEWORK_SRCS)

set(SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/unit_test.cpp
        ${FRAMEWORK_SRCS}
   )

add_executable(memu ${SOURCE_FILES})

target_link_libraries(memu #[[default_platform default_bus rv64core_simple uart_lite rv_clint rv_plic]])