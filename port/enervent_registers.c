#include "enervent_registers.h"

// switch, sensor, setting, status

/* Defines for Coils we want to read with this Master. These are enervent modbus coils. */
static const coil_def_t coil_defs[] = {
    {{1, false, REGTYPE_SWITCH}},
    {{3, false, REGTYPE_SWITCH}},
    {{6, false, REGTYPE_SWITCH}},
    {{7, false, REGTYPE_SWITCH}},
    {{8, false, REGTYPE_SWITCH}},
    {{9, false, REGTYPE_SWITCH}},
    {{10, false, REGTYPE_SWITCH}},
    {{11, false, REGTYPE_SWITCH}},
    {{18, false, REGTYPE_SWITCH}},
    {{19, false, REGTYPE_SWITCH}},
    {{30, false, REGTYPE_SWITCH}},
    {{32, false, REGTYPE_SWITCH}},
    {{36, false, REGTYPE_SWITCH}},
    {{40, false, REGTYPE_SWITCH}},
    {{41, true, REGTYPE_STATUS}},
    {{42, true, REGTYPE_STATUS}},
    {{49, false, REGTYPE_SWITCH}}};

/* Defines for Holding Regs we want to read with this Master. These are enervent modbus holding registers.*/
static const register_def_t holding_defs[] = {
    {{6, true, REGTYPE_SENSOR}, INT16, 10},
    {{7, true, REGTYPE_SENSOR}, INT16, 10},
    {{8, true, REGTYPE_SENSOR}, INT16, 10},
    {{9, true, REGTYPE_SENSOR}, INT16, 10},
    {{10, true, REGTYPE_SENSOR}, INT16, 10},
    {{12, true, REGTYPE_SENSOR}, INT16, 10},
    {{13, true, REGTYPE_SENSOR}, UINT16, 1},
    {{23, true, REGTYPE_STATUS}, UINT16, 1},
    {{29, true, REGTYPE_STATUS}, UINT16, 1},
    {{30, true, REGTYPE_STATUS}, UINT16, 1},
    {{35, true, REGTYPE_SENSOR}, UINT16, 1},
    {{44, true, REGTYPE_STATUS}, BITFIELD, 1},
    {{45, true, REGTYPE_STATUS}, ENUMERATION, 1},
    {{48, true, REGTYPE_STATUS}, INT16, 10},
    {{49, true, REGTYPE_STATUS}, INT16, 1},
    {{50, false, REGTYPE_SETTING}, UINT16, 1},
    {{51, false, REGTYPE_SETTING}, UINT16, 1},
    {{52, false, REGTYPE_SETTING}, UINT16, 1},
    {{54, false, REGTYPE_SETTING}, UINT16, 1},
    {{55, false, REGTYPE_SETTING}, UINT16, 1},
    {{56, false, REGTYPE_SETTING}, UINT16, 1},
    {{57, false, REGTYPE_SETTING}, UINT16, 1},
    {{66, false, REGTYPE_SETTING}, UINT16, 1},
    {{67, false, REGTYPE_SETTING}, UINT16, 1},
    {{68, false, REGTYPE_SETTING}, UINT16, 1},
    {{69, false, REGTYPE_SETTING}, UINT16, 1},
    {{70, false, REGTYPE_SETTING}, UINT16, 1},
    {{72, false, REGTYPE_SETTING}, UINT16, 1},
    {{74, false, REGTYPE_SETTING}, UINT16, 1},
    {{76, false, REGTYPE_SETTING}, UINT16, 1},
    {{100, false, REGTYPE_SETTING}, UINT16, 1},
    {{101, true, REGTYPE_SETTING}, INT16, 10},
    {{102, false, REGTYPE_SETTING}, UINT16, 1},
    {{103, true, REGTYPE_SETTING}, INT16, 10},
    {{134, true, REGTYPE_SENSOR}, INT16, 10},
    {{135, false, REGTYPE_SETTING}, INT16, 10},
    {{137, true, REGTYPE_SETTING}, INT16, 10},
    {{138, true, REGTYPE_SETTING}, INT16, 10},
    {{139, true, REGTYPE_SETTING}, INT16, 10},
    {{164, true, REGTYPE_SETTING}, INT16, 10},
    {{172, true, REGTYPE_SETTING}, INT16, 10},
    {{196, true, REGTYPE_SETTING}, INT16, 10},
    {{538, true, REGTYPE_STATUS}, UINT16, 1},
    {{581, true, REGTYPE_STATUS}, UINT16, 1},
    {{676, true, REGTYPE_SETTING}, UINT16, 10},
    {{677, true, REGTYPE_SETTING}, UINT16, 10},
    {{678, true, REGTYPE_SETTING}, UINT16, 10}};
   
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