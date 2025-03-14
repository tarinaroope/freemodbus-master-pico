

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"


#include <stdio.h>

#include "enervent_registers.h"
#include "enervent_mb_agent.h"
#include "enervent_mb.h"
#include "evlogging.h"

static envent_ipc_interface_t gInterface = {0};

void core1_main(void)
{
    EVLOG_INFO("Start listening for notifications in Core 1...");
    sleep_ms(5000);
/*
    uint16_t target_address = 139;
    uint16_t payload_integer = 230;
    envent_command_t cmd;
    envent_create_write_command(target_address,payload_integer, &cmd);
   
   
    printf("cmd %d %d", target_address, payload_integer);
    queue_try_add(&gInterface.command_queue, &cmd);
*/

    const register_def_t* registerArray = get_register_def_array();
    const coil_def_t* coilArray = get_coil_def_array();
    while (true)
    {
        int value = 0;
        if (queue_try_remove(&gInterface.notify_queue, &value))
        {
            critical_section_enter_blocking(&gInterface.update_mutex);
            for (int i = 0; i < gInterface.coilCount; i++)
            {
                EVLOG_INFO("Coil Address %d, Value %d", coilArray[i].base.address, gInterface.coilValues[i]);
                printf("Coil Address %d, Value %d\n", coilArray[i].base.address, gInterface.coilValues[i]);

            }
            for (int i = 0; i < gInterface.registerCount; i++)
            {
                EVLOG_INFO("Register Address %d, Value %d", registerArray[i].base.address, gInterface.registerValues[i]);
            }
            critical_section_exit(&gInterface.update_mutex);
            value = 0;
        }
    }
}

int main(void)
{
    timer_hw->dbgpause = 0; // hack!
    stdio_init_all();

    sleep_ms(1000);
    EVLOG_INFO("Init...");
 
    queue_init(&gInterface.command_queue, sizeof(envent_command_t), 10);
    queue_init(&gInterface.notify_queue, sizeof(uint8_t), 1);

    multicore_launch_core1(core1_main);
    // Enter loop
    if (!enagent_start_command_loop(&gInterface, 10))
    {
        EVLOG_INFO("Fatal error in starting command loop!");
    }
}