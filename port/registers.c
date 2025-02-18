#include "registers.h"

/* Defines for Coils we want to read with this Master. These are enervent modbus registers. */

static const Coil coilBuffer[] = {
    {{1, false}, 0},
    {{3, false}, 0},
    {{6, false}, 0},
    {{7, false}, 0},
    {{8, false}, 0},
    {{9, false}, 0},
    {{10, false}, 0},
    {{11, false}, 0},
    {{18, false}, 0},
    {{19, false}, 0},
    {{30, false}, 0},
    {{32, false}, 0},
    {{36, false}, 0},
    {{40, false}, 0},
    {{41, true}, 0},
    {{42, true}, 0},
    {{49, false}, 0}
};

/* Defines for Holding Regs we want to read with this Master. These are enervent modbus registers.*/

static const HoldingRegister holdingBuffer[] = {
    {{6, true}, INT16, 10, 0},
    {{7, true}, INT16, 10, 0},
    {{8, true}, INT16, 10, 0},
    {{9, true}, INT16, 10, 0},
    {{10, true}, INT16, 10, 0},
    {{12, true}, INT16, 10, 0},
    {{13, true}, UINT16, 1, 0},
    {{23, true}, UINT16, 1, 0},
    {{29, true}, UINT16, 1, 0},
    {{30, true}, UINT16, 1, 0},
    {{35, true}, UINT16, 1, 0},
    {{44, true}, BITFIELD, 1, 0},
    {{45, true}, ENUMERATION, 1, 0},
    {{48, true}, INT16, 10, 0},
    {{49, true}, INT16, 1, 0},
    {{50, true}, UINT16, 1, 0},
    {{51, false}, UINT16, 1, 0},
    {{52, false}, UINT16, 1, 0},
    {{54, false}, UINT16, 1, 0},
    {{55, false}, UINT16, 1, 0},
    {{56, false}, UINT16, 1, 0},
    {{57, false}, UINT16, 1, 0},
    {{66, false}, UINT16, 1, 0},
    {{67, false}, UINT16, 1, 0},
    {{68, false}, UINT16, 1, 0},
    {{69, false}, UINT16, 1, 0},
    {{70, false}, UINT16, 1, 0},
    {{72, false}, UINT16, 1, 0},
    {{74, false}, UINT16, 1, 0},
    {{76, false}, UINT16, 1, 0},
    {{100, false}, UINT16, 1, 0},
    {{101, false}, INT16, 10, 0},
    {{102, false}, UINT16, 1, 0},
    {{103, false}, INT16, 10, 0},
    {{104, false}, UINT16, 1, 0},
    {{110, false}, UINT16, 10, 0},
    {{114, false}, UINT16, 10, 0},
    {{116, false}, UINT16, 10, 0},
    {{120, false}, UINT16, 10, 0},
    {{122, false}, INT16, 1, 0},
    {{126, false}, INT16, 1, 0},
    {{128, false}, INT16, 1, 0},
    {{132, false}, INT16, 1, 0},
    {{134, true}, INT16, 10, 0},
    {{135, false}, INT16, 10, 0},
    {{137, false}, INT16, 10, 0},
    {{138, false}, INT16, 10, 0},
    {{139, false}, INT16, 10, 0},
    {{164, false}, INT16, 10, 0},
    {{172, false}, INT16, 10, 0},
    {{196, false}, INT16, 10, 0},
    {{343, false}, UINT16, 1, 0},
    {{348, false}, UINT16, 1, 0},
    {{354, false}, UINT16, 1, 0},
    {{538, false}, UINT16, 1, 0},
    {{581, false}, UINT16, 1, 0},
    {{599, false}, UINT16, 100, 0},
    {{649, false}, UINT16, 1, 0},
    {{650, false}, UINT16, 1, 0},
    {{676, false}, UINT16, 10, 0},
    {{677, false}, UINT16, 10, 0},
    {{678, false}, UINT16, 10, 0},
    {{703, false}, UINT16, 1, 0},
    {{710, false}, UINT16, 1, 0},
    {{726, false}, UINT16, 1, 0},
    {{733, false}, ENUMERATION, 1, 0},
    {{734, false}, ENUMERATION, 1, 0},
    {{774, true}, UINT16, 1, 0},
    {{780, true}, UINT16, 10, 0},
    {{781, true}, UINT16, 10, 0},
    {{783, true}, UINT16, 10, 0},
    {{784, true}, UINT16, 10, 0},
    {{785, true}, UINT16, 10, 0},
    {{788, true}, UINT16, 1, 0},
    {{789, true}, UINT16, 1, 0},
    {{798, true}, UINT16, 1, 0}
};

Coil* get_coil_buffer() 
{
    return (Coil*) coilBuffer;
}

HoldingRegister* get_holding_buffer() 
{
    return (HoldingRegister*) holdingBuffer;
}

int16_t find_index_binary(  const BaseData* arr, const uint8_t item_size, 
    const uint16_t arr_length, const uint16_t target)
{
    uint16_t left = 0;
    uint16_t right = arr_length - 1;
   
    while (left <= right) 
    {
        int mid = left + (right - left) / 2;

        if ((arr + ( mid*item_size ))->number == target) 
        {
            return mid;
        } else if ((arr + ( mid*item_size ))->number < target) 
        {
            left = mid + 1;
        } else 
        {
            right = mid - 1;
        }
    }

    return -1;
}

int16_t find_index_seq( const BaseData* arr, const uint8_t item_size, const uint16_t arr_length, 
    uint16_t const target, uint16_t const start)
{
    for (int i = start; i < arr_length; i++) 
    {
        if ((arr + ( i * item_size ))->number == target) 
        {
            return i;
        }
    }
    return -1;
}