message("RV-PLIC Built-in at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/intc)

aux_source_directory(src RV_PLIC_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${RV_PLIC_SRCS}
   )

add_library(rv_plic SHARED ${SOURCE_FILES})
