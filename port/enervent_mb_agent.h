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
 * @enum envent_command_target_t
 * @brief Enumeration for command target types.
 * 
 * This enumeration defines the possible targets for commands.
 * 
 * @var EN_HOLDING_REG
 * Command target is a holding register.
 * 
 * @var EN_COIL
 * Command target is a coil.
 */
typedef enum
{
    EN_HOLDING_REG,
    EN_COIL
} envent_command_target_t;

/**
 * @struct envent_command_t
 * @brief Structure for representing a command.
 * 
 * This structure holds the information for a command, including the target,
 * address, and value.
 * 
 * @var envent_command_t::target
 * The target of the command, specified by the envent_command_target_t enumeration.
 * 
 * @var envent_command_t::address
 * The address for the command.
 * 
 * @var envent_command_t::value
 * The value for the command.
 */
typedef struct
{
    envent_command_target_t target;
    uint16_t address;
    uint16_t value;
} envent_command_t;

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