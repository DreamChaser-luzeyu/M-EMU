message("RV64Core-Simple Built-in at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/core)

aux_source_directory(src RV64_SIMPLE_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${RV64_SIMPLE_SRCS}
   )

add_library(rv64core_simple SHARED ${SOURCE_FILES})
