message("UART-lite Built-in at ${PROJECT_SOURCE_DIR}")

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/dev)

aux_source_directory(src UART_LITE_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${UART_LITE_SRCS}
   )

add_library(uart_lite SHARED ${SOURCE_FILES})
