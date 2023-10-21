message("Default platform at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/core)

aux_source_directory(src DEFAULT_PLATFORM_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${DEFAULT_PLATFORM_SRCS}
   )

add_library(default_platform SHARED ${SOURCE_FILES})
