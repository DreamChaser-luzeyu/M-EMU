message("RAM Built-in at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/dev)

aux_source_directory(src SIMPLE_RAM_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${SIMPLE_RAM_SRCS}
   )

add_library(simple_ram SHARED ${SOURCE_FILES})
