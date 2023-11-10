message("RV-CLInt Built-in at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/intc)

aux_source_directory(src RV_CLINT_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${RV_CLINT_SRCS}
   )

add_library(rv_clint SHARED ${SOURCE_FILES})
