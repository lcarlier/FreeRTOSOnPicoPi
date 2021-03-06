cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(TestFlash)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(include)

add_executable(main
        main.c
        list.c
        queue.c
        tasks.c
        memory.c
        port.c
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(main pico_stdlib hardware_flash)
add_compile_options(-Wall -Wextra -pedantic -Werror)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)


