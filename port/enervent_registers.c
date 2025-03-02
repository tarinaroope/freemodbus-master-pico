#include "enervent_registers.h"

/* Defines for Coils we want to read with this Master. These are enervent modbus registers. */
static const coil_def_t coil_defs[] = {
    {{1, false}},
    {{3, false}},
    {{6, false}},
    {{7, false}},
    {{8, false}},
    {{9, false}},
    {{10, false}},
    {{11, false}},
    {{18, false}},
    {{19, false}},
    {{30, false}},
    {{32, false}},
    {{36, false}},
    {{40, false}},
    {{41, true}},
    {{42, true}},
    {{49, false}}};

/* Defines for Holding Regs we want to read with this Master. These are enervent modbus registers.*/
static const register_def_t holding_defs[] = {
    {{6, true}, INT16, 10},
    {{7, true}, INT16, 10},
    {{8, true}, INT16, 10},
    {{9, true}, INT16, 10},
    {{10, true}, INT16, 10},
    {{12, true}, INT16, 10},
    {{13, true}, UINT16, 1},
    {{23, true}, UINT16, 1},
    {{29, true}, UINT16, 1},
    {{30, true}, UINT16, 1},
    {{35, true}, UINT16, 1},
    {{44, true}, BITFIELD, 1},
    {{45, true}, ENUMERATION, 1},
    {{48, true}, INT16, 10},
    {{49, true}, INT16, 1},
    {{50, true}, UINT16, 1},
    {{51, false}, UINT16, 1},
    {{52, false}, UINT16, 1},
    {{54, false}, UINT16, 1},
    {{55, false}, UINT16, 1},
    {{56, false}, UINT16, 1},
    {{57, false}, UINT16, 1},
    {{66, false}, UINT16, 1},
    {{67, false}, UINT16, 1},
    {{68, false}, UINT16, 1},
    {{69, false}, UINT16, 1},
    {{70, false}, UINT16, 1},
    {{72, false}, UINT16, 1},
    {{74, false}, UINT16, 1},
    {{76, false}, UINT16, 1},
    {{100, false}, UINT16, 1},
    {{101, false}, INT16, 10},
    {{102, false}, UINT16, 1},
    {{103, false}, INT16, 10},
    {{104, false}, UINT16, 1},
    {{110, false}, UINT16, 10},
    {{114, false}, UINT16, 10},
    {{116, false}, UINT16, 10},
    {{120, false}, UINT16, 10},
    {{122, false}, INT16, 1},
    {{126, false}, INT16, 1},
    {{128, false}, INT16, 1},
    {{132, false}, INT16, 1},
    {{134, true}, INT16, 10},
    {{135, false}, INT16, 10},
    {{137, false}, INT16, 10},
    {{138, false}, INT16, 10},
    {{139, false}, INT16, 10},
    {{164, false}, INT16, 10},
    {{172, false}, INT16, 10},
    {{196, false}, INT16, 10},
    {{343, false}, UINT16, 1},
    {{348, false}, UINT16, 1},
    {{354, false}, UINT16, 1},
    {{538, false}, UINT16, 1},
    {{581, false}, UINT16, 1},
    {{599, false}, UINT16, 100},
    {{649, false}, UINT16, 1},
    {{650, false}, UINT16, 1},
    {{676, false}, UINT16, 10},
    {{677, false}, UINT16, 10},
    {{678, false}, UINT16, 10},
    {{703, false}, UINT16, 1},
    {{710, false}, UINT16, 1},
    {{726, false}, UINT16, 1},
    {{733, false}, ENUMERATION, 1},
    {{734, false}, ENUMERATION, 1},
    {{774, true}, UINT16, 1},
    {{780, true}, UINT16, 10},
    {{781, true}, UINT16, 10},
    {{783, true}, UINT16, 10},
    {{784, true}, UINT16, 10},
    {{785, true}, UINT16, 10},
    {{788, true}, UINT16, 1},
    {{789, true}, UINT16, 1},
    {{798, true}, UINT16, 1}};

const coil_def_t *get_coil_def_array()
{
    return (coil_def_t *)coil_defs;
}

const register_def_t *get_register_def_array()
{
    return (register_def_t *)holding_defs;
}

int16_t find_coil_index_binary(const coil_def_t *arr, const uint16_t target_address)
{
    uint16_t left = 0;
    uint16_t right = COIL_DEFINITION_COUNT - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (arr[mid].base.address == target_address)
        {
            return mid;
        }
        else if (arr[mid].base.address < target_address)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}

int16_t find_register_index_binary(const register_def_t *arr, const uint16_t target_address)
{
    uint16_t left = 0;
    uint16_t right = REGISTER_DEFINITION_COUNT - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (arr[mid].base.address == target_address)
        {
            return mid;
        }
        else if (arr[mid].base.address < target_address)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return -1;
}

int16_t find_coil_index_seq(const coil_def_t *arr, const uint16_t target_address, const uint16_t start_index)
{
    for (int i = start_index; i < COIL_DEFINITION_COUNT; i++)
    {
        if (arr[i].base.address == target_address)
        {
            return i;
        }
    }
    return -1;
}

int16_t find_register_index_seq(const register_def_t *arr, const uint16_t target_address, const uint16_t start_index)
{
    for (int i = start_index; i < REGISTER_DEFINITION_COUNT; i++)
    {
        if (arr[i].base.address == target_address)
        {
            return i;
        }
    }
    return -1;
}