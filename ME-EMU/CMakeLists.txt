cmake_minimum_required(VERSION 3.25)
project(ME_EMU_new)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(src/core        ME_EMU_SRCS)
aux_source_directory(src/dev         ME_EMU_SRCS)
aux_source_directory(src/custom/dev  ME_EMU_SRCS)

set(SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/test.cpp
        ${ME_EMU_SRCS}
   )

add_executable(ME_EMU_new ${SOURCE_FILES})
