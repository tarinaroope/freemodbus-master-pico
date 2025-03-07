/**
 * @file enervent_registers.h
 * @brief Header file for Enervent register and coil definitions and related functions.
 *
 * This file contains definitions for coils and holding registers used in the Enervent system.
 * It also provides function declarations for accessing and searching these definitions.
 */

#ifndef ENERVENT_REGISTERS_H
#define ENERVENT_REGISTERS_H

#include "pico/stdlib.h"

#define LAST_COIL_ADDRESS                   49  ///< Last coil number to read
#define LAST_REGISTER_ADDRESS               734 ///< Last holding register number to read

#define COIL_DEFINITION_COUNT               17  ///< Number of coils to read
#define REGISTER_DEFINITION_COUNT           69  ///< Number of holding registers to read

/**
 * @enum data_type_def_t
 * @brief Enumeration for data types of registers.
 */
typedef enum 
{
    UINT16,      ///< Unsigned 16-bit integer
    INT16,       ///< Signed 16-bit integer
    BITFIELD,    ///< Bitfield
    ENUMERATION  ///< Enumeration
} data_type_def_t;

typedef enum
{
    REGTYPE_SWITCH,
    REGTYPE_SENSOR,
    REGTYPE_SETTING,
    REGTYPE_STATUS
} base_type_def_t;

/**
 * @struct base_def_t
 * @brief Base structure for coil and register definitions.
 */
typedef struct 
{
    uint16_t        address;    ///< Address of the coil or register
    bool            readonly;   ///< Read-only flag
    base_type_def_t base_type;

} base_def_t;

/**
 * @struct register_def_t
 * @brief Structure for holding register definitions.
 */
typedef struct 
{
    base_def_t      base;       ///< Base definition
    data_type_def_t type;       ///< Data type of the register
    int8_t          multiplier; ///< Multiplier for the register value
} register_def_t;

/**
 * @struct coil_def_t
 * @brief Structure for coil definitions.
 */
typedef struct 
{
    base_def_t base; ///< Base definition
} coil_def_t;

/**
 * @brief Get the array of coil definitions.
 * @return Pointer to the array of coil definitions.
 */
const coil_def_t* get_coil_def_array();

/**
 * @brief Get the array of register definitions.
 * @return Pointer to the array of register definitions.
 */
const register_def_t* get_register_def_array();

/**
 * @brief Find the index of a coil using binary search.
 * @param arr Array of coil definitions.
 * @param target_address Address of the target coil.
 * @return Index of the target coil, or -1 if not found.
 */
int16_t find_coil_index_binary( const coil_def_t* arr, const uint16_t target_address);

/**
 * @brief Find the index of a register using binary search.
 * @param arr Array of register definitions.
 * @param target_address Address of the target register.
 * @return Index of the target register, or -1 if not found.
 */
int16_t find_register_index_binary( const register_def_t* arr, const uint16_t target_address);

/**
 * @brief Find the index of a coil using sequential search.
 * @param arr Array of coil definitions.
 * @param target_address Address of the target coil.
 * @param start_index Index to start the search from.
 * @return Index of the target coil, or -1 if not found.
 */
int16_t find_coil_index_seq(const coil_def_t* arr, const uint16_t target_address, const uint16_t start_index);

/**
 * @brief Find the index of a register using sequential search.
 * @param arr Array of register definitions.
 * @param target_address Address of the target register.
 * @param start_index Index to start the search from.
 * @return Index of the target register, or -1 if not found.
 */
int16_t find_register_index_seq(const register_def_t* arr, const uint16_t target_address, const uint16_t start_index);

#endif // REGSISTERS_H