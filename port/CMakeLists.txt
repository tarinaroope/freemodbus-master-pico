
add_library(freemodbus-pico STATIC
    ${CMAKE_CURRENT_LIST_DIR}/RP2040/port.c
    ${CMAKE_CURRENT_LIST_DIR}/RP2040/portevent_m.c
    ${CMAKE_CURRENT_LIST_DIR}/RP2040/portserial_m.c
    #${CMAKE_CURRENT_LIST_DIR}/RP2040/portserial.c
    ${CMAKE_CURRENT_LIST_DIR}/RP2040/porttimer_m.c


    user_mb_app_m.c
    registers.c

    ../modbus/mb_m.c
    
    ../modbus/functions/mbfunccoils_m.c
    ../modbus/functions/mbfuncdisc_m.c
    ../modbus/functions/mbfuncholding_m.c
    ../modbus/functions/mbfuncinput_m.c
    ../modbus/functions/mbutils.c
    

    ../modbus/rtu/mbcrc.c
    ../modbus/rtu/mbrtu_m.c
)

target_link_libraries(freemodbus-pico
    pico_time
    hardware_timer
    hardware_uart
    hardware_irq
    pico_stdlib 
)

target_include_directories(freemodbus-pico PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/RP2040/
    ${CMAKE_CURRENT_LIST_DIR}/./
    ${CMAKE_CURRENT_LIST_DIR}/../modbus/rtu/
    ${CMAKE_CURRENT_LIST_DIR}/../modbus/include/
    
)