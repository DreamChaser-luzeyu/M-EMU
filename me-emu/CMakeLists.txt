cmake_minimum_required(VERSION 3.25)
project(me_emu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-lstdc++fs")

include_directories(${PROJECT_SOURCE_DIR}/include)

#aux_source_directory(${PROJECT_SOURCE_DIR}/src/cpu/m6502 ME_EMU_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/cpu/riscv ME_EMU_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/mem ME_EMU_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/test ME_EMU_SRCS)

set(
        SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${ME_EMU_SRCS}
)



add_executable(me_emu ${SOURCE_FILES})
