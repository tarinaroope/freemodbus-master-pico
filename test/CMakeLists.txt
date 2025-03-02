# Initialize the Pico SDK
pico_sdk_init() 

add_executable(freemodbus-pico-test
                main.c
                )

#target_include_directories(mqtt-multicore-test   
#        PRIVATE ${CMAKE_CURRENT_LIST_DIR}
#        PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
#        )

# pull in common dependencies
target_link_libraries(freemodbus-pico-test 
        freemodbus-pico
        pico_stdlib                        # for core pico functionality
        pico_multicore
        )

# Disable both for stdio
pico_enable_stdio_usb(freemodbus-pico-test 0) 
pico_enable_stdio_uart(freemodbus-pico-test 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(freemodbus-pico-test)