

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"


#include <stdio.h>

#include "enervent_registers.h"
#include "enervent_mb_agent.h"
#include "enervent_mb.h"

static envent_ipc_interface_t gInterface = {0};

void core1_main(void)
{
    printf("Start listening for notifications in Core 1...\n");
    sleep_ms(5000);
    //const coil_def_t* coilArray = get_coil_def_array();
    envent_command_t cmd;
    cmd.address = 1;
    cmd.target = EN_COIL;
    cmd.value = 0;
    queue_try_add(&gInterface.command_queue,&cmd);
    sleep_ms(5000);
    const register_def_t* registerArray = get_register_def_array();
    while (true)
    {
        int value = 0;
        if (queue_try_remove(&gInterface.notify_queue, &value))
        {
            critical_section_enter_blocking(&gInterface.update_mutex);
            for (int i = 0; i < gInterface.registerCount; i++)
            {
                printf("Register Address %d, Value %d\n", registerArray[i].base.address, gInterface.registerValues[i]);
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

    printf("\n\nSensorHub init...\n");

    queue_init(&gInterface.command_queue, sizeof(envent_command_t), 10);
    queue_init(&gInterface.notify_queue, sizeof(uint8_t), 1);

   // multicore_reset_core1();
    multicore_launch_core1(core1_main);
    // Enter loop
    if (!enagent_start_command_loop(&gInterface))
    {
        printf("Fatal error in starting command loop!\n");
    }
}