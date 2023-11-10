message("Default MMIO-bus at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/bus)

aux_source_directory(src MMIO_BUS_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${MMIO_BUS_SRCS}
        )

add_library(mmio_bus SHARED ${SOURCE_FILES})
