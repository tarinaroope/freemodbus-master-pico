#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>
#include "pico/stdlib.h"

#define COIL_NCOILS                 49  // Last coil number we want to read
#define REG_HOLDING_NREGS           798 // Last holding register number we want to read

#define DEFINED_NCOILS              17  // Number of coils we want to read
#define DEFINED_HOLDING_NREGS       76  // Number of holding registers we want to read

typedef enum 
{
    UINT16,
    INT16,
    BITFIELD,
    ENUMERATION
} Type;

typedef struct 
{
    uint16_t number;
    bool readonly;
} BaseData;

typedef struct 
{
    BaseData base;
    uint16_t value;
    Type type;
    int8_t multiplier;
} HoldingRegister;

typedef struct 
{
    BaseData base;
    uint8_t value;
} Coil;

Coil* get_coil_buffer();
HoldingRegister* get_holding_buffer();

int16_t find_index_binary(  const BaseData* arr, const uint8_t item_size, 
                            const uint16_t arr_length, const uint16_t target);

int16_t find_index_seq( const BaseData* arr, const uint8_t item_size, const uint16_t arr_length, 
                        uint16_t const target, uint16_t const start);



#endif // REGSISTERS_H

