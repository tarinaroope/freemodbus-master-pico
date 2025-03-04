#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "enervent_mb_agent.h"
#include "enervent_mb.h"

#include "pico/stdlib.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define ENAGENT_IDLE_SLEEP_TIMEOUT 200    // ms
#define ENAGENT_DATA_REFRESH_PERIOD 10000 // ms
#define ENAGENT_RETRY_TIMOUT 1000         // ms
#define ENAGENT_COIL_ADDRESS_OFFSET 1000

typedef enum
{
    ENA_STATE_NONE,
    ENA_STATE_REFRESH_VALUES,
    ENA_STATE_REFRESHING_COILS,
    ENA_STATE_REFRESHING_REGS,
    ENA_STATE_UPDATING_VALUE
} ENAGENT_STATE;

static volatile bool isIdle = false;
//static alarm_id_t alarm_id = -1;
static critical_section_t mutex;
static volatile ENAGENT_STATE gState;
static uint8_t currentBatch = 0;

#define BATCH_COUNT 6
static const uint16_t holdingRegBatches[BATCH_COUNT][2] =
{
    {6,100},
    {105,100},
    {340,20},
    {538,65},
    {650,50},
    {700,99}
};

static int64_t __isr refresh_timer_expired(alarm_id_t id, void *user_data)
{
    // If we are idling, we can update
    if (isIdle)
    {
        isIdle = false;
        gState = ENA_STATE_REFRESH_VALUES;
        return (ENAGENT_DATA_REFRESH_PERIOD * 1000UL);
    }
    else
    {
        // We are busy, set retry timer
        return (ENAGENT_RETRY_TIMOUT * 1000UL);
    }
}

static bool enagent_write_coil(uint16_t coil_address, uint16_t value)
{
    if (coil_address > LAST_COIL_ADDRESS)
    {
        return false;
    }

    const coil_def_t *coil_defs = get_coil_def_array();
    int16_t index = find_coil_index_binary(coil_defs, coil_address);
    if (index >= 0)
    {
        if (!coil_defs[index].base.readonly)
        {
            eMBMasterReqWriteCoil(ENAGENT_SLAVE_ID, coil_address, value, -1);
            return true;
        }
    }
    return false;
}

static bool enagent_write_holding_register(uint16_t register_address, uint16_t value)
{
    if (register_address > LAST_REGISTER_ADDRESS)
    {
        return false;
    }

    const register_def_t *register_defs = get_register_def_array();
    int16_t index = find_register_index_binary(register_defs, register_address);
    if (index >= 0)
    {
        if (!register_defs[index].base.readonly)
        {
            eMBMasterReqWriteHoldingRegister(ENAGENT_SLAVE_ID, register_address, value, -1);
            return true;
        }
    }
    return false;
}

void envent_create_write_coil_command(uint16_t coil_address, uint16_t value, envent_command_t* command)
{
    command->address = coil_address;
    value = (value > 0) ? 0xff : 0;
    command->command_function = enagent_write_coil;
}

void envent_create_write_register_command(uint16_t register_address, uint16_t value, envent_command_t* command)
{
    command->address = register_address;
    command->value = value;
    command->command_function = enagent_write_holding_register;
}

static void enagent_refresh_coils()
{
    const coil_def_t *coil_defs = get_coil_def_array();
    eMBMasterReqReadCoils(ENAGENT_SLAVE_ID, coil_defs[0].base.address, LAST_COIL_ADDRESS - coil_defs[0].base.address + 1, -1);
}

static void enagent_refresh_registers(uint8_t batch)
{
    eMBMasterReqReadHoldingRegister(ENAGENT_SLAVE_ID, holdingRegBatches[batch][0], holdingRegBatches[batch][1], -1);
}

static void enagent_refresh_coil_data(envent_ipc_interface_t* interface)
{
    uint8_t* coils = envent_get_coil_value_array();
    critical_section_enter_blocking(&interface->update_mutex);
    memcpy(interface->coilValues, coils, COIL_DEFINITION_COUNT);
    critical_section_exit(&interface->update_mutex);
}

static void enagent_refresh_holding_data(envent_ipc_interface_t* interface)
{
    uint16_t* registers = envent_get_register_value_array();
    critical_section_enter_blocking(&interface->update_mutex);
    memcpy(interface->registerValues, registers, sizeof(registers) * REGISTER_DEFINITION_COUNT);
    critical_section_exit(&interface->update_mutex);
}

bool enagent_start_command_loop(envent_ipc_interface_t* interface)
{
    critical_section_init(&mutex);

    if (eMBMasterInit(MB_RTU, ENAGENT_SLAVE_ID, 19200, MB_PAR_NONE) != MB_ENOERR)
    {
        return false;
    }

    if (eMBMasterEnable() != MB_ENOERR)
    {
        return false;
    }

    interface->coilValues = calloc(COIL_DEFINITION_COUNT, sizeof(uint8_t));
    interface->registerValues = calloc(REGISTER_DEFINITION_COUNT, sizeof(uint16_t));
    
    if (!interface->coilValues || !interface->registerValues)
    {
        return false;
    }    
    interface->coilCount = COIL_DEFINITION_COUNT;
    interface->registerCount = REGISTER_DEFINITION_COUNT;

    critical_section_init(&interface->update_mutex);

    // Not sure if it's needed, but let's do one poll before starting the loop
    (void)eMBMasterPoll();

    isIdle = true;

    // Setup data refresh timer
    add_alarm_in_ms(ENAGENT_DATA_REFRESH_PERIOD, refresh_timer_expired, NULL, true);

    while (true)
    {
        critical_section_enter_blocking(&mutex);
        if (isIdle)
        {
            envent_command_t cmd;
            if (queue_try_remove(&interface->command_queue, &cmd))
            {
                isIdle = false;
                critical_section_exit(&mutex);
                if (cmd.command_function(cmd.address, cmd.value))
                {
                    gState = ENA_STATE_UPDATING_VALUE;
                }
                else
                {
                    // Failure
                    gState = ENA_STATE_NONE;
                    isIdle = true;
                }
            }
            else
            {
                critical_section_exit(&mutex);
            }
        }
        else
        {
            critical_section_exit(&mutex);
            eMBMasterReqErrCode err;

            switch (gState)
            {
            case ENA_STATE_REFRESH_VALUES:
                enagent_refresh_coils();
                gState = ENA_STATE_REFRESHING_COILS;
                break;

            case ENA_STATE_REFRESHING_COILS:
                if (envent_get_request_result(&err))
                {
                    enagent_refresh_coil_data(interface);
                    enagent_refresh_registers(0);
                    currentBatch = 1;
                    gState = ENA_STATE_REFRESHING_REGS;
                }
                break;

            case ENA_STATE_REFRESHING_REGS:
                if (envent_get_request_result(&err))
                {
                    if (currentBatch < 6)
                    {
                        enagent_refresh_registers(currentBatch);
                        currentBatch += 1;
                    }
                    else
                    {
                        enagent_refresh_holding_data(interface);         
                        gState = ENA_STATE_NONE;
                        isIdle = true;
    
                        // Notify about the re-freshed data
                        uint8_t value = 1;
                        queue_try_add(&interface->notify_queue, &value);
                    }
                }
                break;

            case ENA_STATE_UPDATING_VALUE:
                if (envent_get_request_result(&err))
                {
                    gState = ENA_STATE_NONE;
                    isIdle = true;
                }
                break;

            default:
                // Todo error
                break;
            }
        }
        (void)eMBMasterPoll();
    }
    return true;
}



