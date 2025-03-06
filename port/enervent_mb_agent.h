/**
 * @file enervent_mb_agent.h
 * @brief Header file for the Enervent Modbus agent.
 *
 * This file contains the necessary includes, definitions, and declarations
 * for the Enervent Modbus agent implementation.
 *
 * @note This file is part of the FreeModbus project for the Raspberry Pi Pico.
 *
 * @def ENAGENT_SLAVE_ID
 * @brief Defines the Modbus slave ID for the Enervent agent.
 */
#ifndef ENERVENT_MB_AGENT_H
#define ENERVENT_MB_AGENT_H

#include "pico/util/queue.h"
#include "pico/critical_section.h"

#include "enervent_registers.h"

#define ENAGENT_SLAVE_ID 1

typedef struct enagent_task_controller_t enagent_task_controller_t;

/**
 * @brief Structure representing the IPC interface for Envent.
 * 
 * This structure contains the necessary elements for inter-process communication
 * between different components of the Envent system.
 * 
 * @typedef envent_ipc_interface_t
 * 
 * @field command_queue Queue for sending commands.
 * @field notify_queue Queue for sending notifications.
 * @field update_mutex Mutex for protecting updates to the interface.
 * @field coilValues Pointer to an array of coil values.
 * @field registerValues Pointer to an array of register values.
 * @field coilCount Number of coils.
 * @field registerCount Number of registers.
 */
typedef struct
{
    queue_t command_queue;
    queue_t notify_queue;

    critical_section_t update_mutex;
    uint8_t *coilValues;
    uint16_t *registerValues;
    uint16_t coilCount;
    uint16_t registerCount;
} envent_ipc_interface_t;

/**
 * @brief Structure representing an Envent command.
 * 
 * This structure is used to define a command that can be sent to an Envent device.
 * It contains the target address, the value to be written, and a function pointer
 * to the command function that will be executed.
 * Note! Create using the command creation functions.
 * 
 * @param address The register address to which the command will be sent.
 * @param value The value to be written to the specified register address.
 * @param command_function A function pointer to the command function that takes
 *        the register address and value as parameters and returns a boolean indicating
 *        the success or failure of the command execution.
 */
typedef struct
{
   // envent_command_target_t target;
    uint16_t address;
    uint16_t value;
    bool (*command_function)(enagent_task_controller_t* /*self*/, 
        uint16_t /*register_address*/, uint16_t /*value*/); 
} envent_command_t;

/**
 * @brief Creates a write coil command for the Envent Modbus agent.
 *
 * This function initializes an Envent command structure to write a value to a specified coil address.
 *
 * @param coil_address The address of the coil to write to.
 * @param value The value to write to the coil. A non-zero value will be converted to 0xFF, and zero will remain 0.
 * @param command Pointer to the Envent command structure to be initialized.
 */
void envent_create_write_coil_command(uint16_t coil_address, uint16_t value, envent_command_t* command);

/**
 * @brief Creates a write register command for the Envent Modbus agent.
 *
 * This function initializes an Envent command structure to write a value to a specified register address.
 *
 * @param register_address The address of the register to write to.
 * @param value The value to write to the register.
 * @param command Pointer to the Envent command structure to be initialized.
 */
void envent_create_write_register_command(uint16_t register_address, uint16_t value, envent_command_t* command);

/**
 * @brief Starts the command loop for the agent.
 * 
 * This function initiates the command loop for the agent using the provided
 * IPC interface.
 * 
 * @param interface Pointer to the IPC interface to be used by the agent.
 * @return true if the command loop started successfully, false otherwise.
 */
bool enagent_start_command_loop(envent_ipc_interface_t *interface);


#endif