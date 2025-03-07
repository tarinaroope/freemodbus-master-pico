#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "enervent_mb_agent.h"
#include "enervent_mb.h"
#include "evlogging.h"

#include "pico/stdlib.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define ENAGENT_DATA_REFRESH_PERIOD 30000 // ms
#define ENAGENT_RETRY_TIMOUT 1000         // ms
#define ENAGENT_COIL_ADDRESS_OFFSET 1000

typedef enum
{
    ENA_STATE_IDLE,
    ENA_STATE_START_REFRESH,
    ENA_STATE_REFRESH_COILS,
    ENA_STATE_REFRESH_REGISTERS,
    ENA_STATE_WRITE_COIL,
    ENA_STATE_WRITE_REGISTER,
    ENA_STATE_REFRESH_SINGLE_COIL,
    ENA_STATE_REFRESH_SINGLE_REGISTER
} enagent_task_state_t;

struct enagent_task_controller_t
{
    volatile enagent_task_state_t state;
    void (*state_function)(enagent_task_controller_t *self);

    envent_ipc_interface_t *ipc_interface;

    uint16_t current_task_address;
    critical_section_t mutex;
};

#define BATCH_COUNT 6
static const uint16_t holdingRegBatches[BATCH_COUNT][2] =
    {
        {6, 71},    // 6-76
        {100, 97},  // 100-196
        {343, 52},  // 340-391
        {538, 62},  // 538-599
        {676, 3},   // 676-678
        {710, 25}}; // 710-734

static void enagent_set_task_state(enagent_task_controller_t *self,
                                   enagent_task_state_t state, bool safe);

static void enagent_state_function_idle(enagent_task_controller_t *self)
{
    // empty
}

static void enagent_state_function_start_refresh(enagent_task_controller_t *self)
{
    const coil_def_t *coil_defs = get_coil_def_array();
    eMBMasterReqReadCoils(ENAGENT_SLAVE_ID,
                          coil_defs[0].base.address, LAST_COIL_ADDRESS - coil_defs[0].base.address + 1, -1);

    enagent_set_task_state(self, ENA_STATE_REFRESH_COILS, true);
}

static void enagent_state_function_refresh_coils(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err;
    if (envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }
        uint8_t *coils = envent_get_coil_value_array();

        critical_section_enter_blocking(&(self->ipc_interface->update_mutex));
        memcpy(self->ipc_interface->coilValues, coils, COIL_DEFINITION_COUNT);
        critical_section_exit(&(self->ipc_interface->update_mutex));

        enagent_set_task_state(self, ENA_STATE_REFRESH_REGISTERS, true);
    }
}

static void enagent_state_function_refresh_registers(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err = MB_MRE_NO_ERR;
    static uint8_t current_batch;

    /* We will trigger a new batch request only if this is the first call or if we have results
     *  ready from previous batch request
     */
    if (!current_batch || envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }
        if (current_batch == BATCH_COUNT)
        {
            EVLOG_DEBUG("Last batch done.", current_batch);

            // Last batch done. Update results
            uint16_t *registers = envent_get_register_value_array();

            critical_section_enter_blocking(&(self->ipc_interface->update_mutex));
            memcpy(self->ipc_interface->registerValues, registers,
                   sizeof(registers) * REGISTER_DEFINITION_COUNT);
            critical_section_exit(&(self->ipc_interface->update_mutex));

            current_batch = 0;
            enagent_set_task_state(self, ENA_STATE_IDLE, true);

            // Notify about the re-freshed data
            uint8_t value = 1;
            queue_try_add(&(self->ipc_interface->notify_queue), &value);
        }
        else
        {
            EVLOG_DEBUG("Refreshing batch %d", current_batch);
            eMBMasterReqReadHoldingRegister(ENAGENT_SLAVE_ID,
                                            holdingRegBatches[current_batch][0], holdingRegBatches[current_batch][1], -1);
            current_batch += 1;
        }
    }
}

static void enagent_state_function_write_coil(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err;

    if (envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }

        enagent_set_task_state(self, ENA_STATE_REFRESH_SINGLE_COIL, true);
        eMBMasterReqReadCoils(ENAGENT_SLAVE_ID,
                              self->current_task_address, 1, -1);
    }
}

static void enagent_state_function_write_register(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err;

    if (envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }

        enagent_set_task_state(self, ENA_STATE_REFRESH_SINGLE_REGISTER, true);
        eMBMasterReqReadHoldingRegister(ENAGENT_SLAVE_ID,
                                        self->current_task_address, 1, -1);
    }
}

static void enagent_state_function_refresh_single_coil(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err;

    if (envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }

        uint8_t *coils = envent_get_coil_value_array();

        critical_section_enter_blocking(&(self->ipc_interface->update_mutex));
        memcpy(self->ipc_interface->coilValues, coils, COIL_DEFINITION_COUNT);
        critical_section_exit(&(self->ipc_interface->update_mutex));

        enagent_set_task_state(self, ENA_STATE_IDLE, true);
    }
}

static void enagent_state_function_refresh_single_register(enagent_task_controller_t *self)
{
    eMBMasterReqErrCode err;

    if (envent_get_request_result(&err))
    {
        if (err != MB_MRE_NO_ERR)
        {
            EVLOG_ERROR("Request failed with error %d", err);
            enagent_set_task_state(self, ENA_STATE_IDLE, true);
            return;
        }

        uint16_t *registers = envent_get_register_value_array();

        critical_section_enter_blocking(&(self->ipc_interface->update_mutex));
        memcpy(self->ipc_interface->registerValues, registers,
               sizeof(registers) * REGISTER_DEFINITION_COUNT);
        critical_section_exit(&(self->ipc_interface->update_mutex));

        enagent_set_task_state(self, ENA_STATE_IDLE, true);
    }
}

static void enagent_set_task_state(enagent_task_controller_t *self,
                                   enagent_task_state_t state, bool safe)
{
    if (safe)
    {
        critical_section_enter_blocking(&(self->mutex));
        self->state = state;
        critical_section_exit(&(self->mutex));
    }
    else
    {
        self->state = state;
    }
    EVLOG_DEBUG("Setting state to %d", state);
    switch (state)
    {
    case ENA_STATE_IDLE:
        self->state_function = enagent_state_function_idle;
        break;
    case ENA_STATE_START_REFRESH:
        self->state_function = enagent_state_function_start_refresh;
        break;
    case ENA_STATE_REFRESH_COILS:
        self->state_function = enagent_state_function_refresh_coils;
        break;
    case ENA_STATE_REFRESH_REGISTERS:
        self->state_function = enagent_state_function_refresh_registers;
        break;
    case ENA_STATE_WRITE_COIL:
        self->state_function = enagent_state_function_write_coil;
        break;
    case ENA_STATE_WRITE_REGISTER:
        self->state_function = enagent_state_function_write_register;
        break;
    case ENA_STATE_REFRESH_SINGLE_COIL:
        self->state_function = enagent_state_function_refresh_single_coil;
        break;
    case ENA_STATE_REFRESH_SINGLE_REGISTER:
        self->state_function = enagent_state_function_refresh_single_register;
        break;
    default:
        EVLOG_ERROR("Trying to set invalid state %d", state);
        break;
    }
}

static bool enagent_write_coil(enagent_task_controller_t *self, uint16_t coil_address, uint16_t value)
{
    if (coil_address > LAST_COIL_ADDRESS)
    {
        EVLOG_ERROR("Invalid adress %d", coil_address);
        return false;
    }

    const coil_def_t *coil_defs = get_coil_def_array();
    int16_t index = find_coil_index_binary(coil_defs, coil_address);
    if (index >= 0)
    {
        if (!coil_defs[index].base.readonly)
        {
            EVLOG_DEBUG("Requesting coil write address %d, value %d", coil_address, value);
            eMBMasterReqWriteCoil(ENAGENT_SLAVE_ID, coil_address, value, -1);
            self->current_task_address = coil_address;
            enagent_set_task_state(self, ENA_STATE_WRITE_COIL, false);
            return true;
        }
    }
    EVLOG_ERROR("Coil address not found %d", coil_address);

    return false;
}

static bool enagent_write_holding_register(enagent_task_controller_t *self,
                                           uint16_t register_address, uint16_t value)
{
    if (register_address > LAST_REGISTER_ADDRESS)
    {
        EVLOG_ERROR("Invalid adress %d", register_address);
        return false;
    }

    const register_def_t *register_defs = get_register_def_array();
    int16_t index = find_register_index_binary(register_defs, register_address);
    if (index >= 0)
    {
        if (!register_defs[index].base.readonly)
        {
            EVLOG_DEBUG("Requesting holding register write address %d, value %d", register_address, value);
            eMBMasterReqWriteHoldingRegister(ENAGENT_SLAVE_ID, register_address, value, -1);
            self->current_task_address = register_address;
            enagent_set_task_state(self, ENA_STATE_WRITE_REGISTER, false);
            return true;
        }
    }
    EVLOG_ERROR("Holding register address not found %d", register_address);
    return false;
}

static int64_t __isr enagent_refresh_timer_expired(alarm_id_t id, void *user_data)
{
    enagent_task_controller_t *controller = (enagent_task_controller_t *)user_data;
    // If we are idling, we can update
    if (controller->state == ENA_STATE_IDLE)
    {
        enagent_set_task_state(controller, ENA_STATE_START_REFRESH, false);
        return (ENAGENT_DATA_REFRESH_PERIOD * 1000UL);
    }
    else
    {
        // We are busy, set retry timer
        return (ENAGENT_RETRY_TIMOUT * 1000UL);
    }
}

bool enagent_start_command_loop(envent_ipc_interface_t *interface)
{
    EVLOG_DEBUG("Initializing command loop");

    // Initialize freemodbus
    if (eMBMasterInit(MB_RTU, ENAGENT_SLAVE_ID, 19200, MB_PAR_NONE) != MB_ENOERR)
    {
        return false;
    }

    if (eMBMasterEnable() != MB_ENOERR)
    {
        return false;
    }

    // Initialize ipc interface
    interface->coilValues = calloc(COIL_DEFINITION_COUNT, sizeof(uint8_t));
    interface->registerValues = calloc(REGISTER_DEFINITION_COUNT, sizeof(uint16_t));

    if (!interface->coilValues || !interface->registerValues)
    {
        return false;
    }
    interface->coilCount = COIL_DEFINITION_COUNT;
    interface->registerCount = REGISTER_DEFINITION_COUNT;

    critical_section_init(&interface->update_mutex);

    // Initialize task controller
    enagent_task_controller_t task_controller = {0};
    task_controller.ipc_interface = interface;
    critical_section_init(&task_controller.mutex);
    enagent_set_task_state(&task_controller, ENA_STATE_IDLE, false);

    // Not sure if it's needed, but let's do one poll before starting the loop
    (void)eMBMasterPoll();

    // Setup data refresh timer
    add_alarm_in_ms(ENAGENT_DATA_REFRESH_PERIOD, enagent_refresh_timer_expired,
                    (void *)&task_controller, true);

    while (true)
    {
        // Need to be careful that we exit critical section...
        critical_section_enter_blocking(&task_controller.mutex);

        // Check possible commands only if we are idling
        if (task_controller.state == ENA_STATE_IDLE)
        {
            envent_command_t cmd;
            if (queue_try_remove(&interface->command_queue, &cmd))
            {
                EVLOG_DEBUG("Launching command address %d, value %d", cmd.address, cmd.value);

                // Command function will change the task controller state only if success
                if (!cmd.command_function(&task_controller, cmd.address, cmd.value))
                {
                    EVLOG_ERROR("Error launching command.");
                }

                critical_section_exit(&task_controller.mutex);
            }
            else
            {
                critical_section_exit(&task_controller.mutex);
                sleep_ms(250);
            }
            (void)eMBMasterPoll();
        }
        else
        {
            critical_section_exit(&task_controller.mutex);

            task_controller.state_function(&task_controller);

            (void)eMBMasterPoll();
        }
    }
    return true;
}

void envent_create_write_coil_command(uint16_t coil_address, uint16_t value, envent_command_t *command)
{
    command->address = coil_address;
    value = (value > 0) ? 0xff : 0;
    command->command_function = enagent_write_coil;
}

void envent_create_write_register_command(uint16_t register_address, uint16_t value, envent_command_t *command)
{
    command->address = register_address;
    command->value = value;
    command->command_function = enagent_write_holding_register;
}
