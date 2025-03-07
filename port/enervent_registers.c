#include "enervent_registers.h"

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

    // switch, sensor, setting, status

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
    {{101, false, REGTYPE_SETTING}, INT16, 10},
    {{102, false, REGTYPE_SETTING}, UINT16, 1},
    {{103, false, REGTYPE_SETTING}, INT16, 10},
    {{104, false, REGTYPE_SETTING}, UINT16, 1},
    {{110, false, REGTYPE_SETTING}, UINT16, 10},
    {{114, false, REGTYPE_SETTING}, UINT16, 10},
    {{116, false, REGTYPE_SETTING}, UINT16, 10},
    {{120, false, REGTYPE_SETTING}, UINT16, 10},
    {{122, false, REGTYPE_SETTING}, INT16, 1},
    {{126, false, REGTYPE_SETTING}, INT16, 1},
    {{128, false, REGTYPE_SETTING}, INT16, 1},
    {{132, false, REGTYPE_SETTING}, INT16, 1},
    {{134, true}, INT16, 10},
    {{135, false, REGTYPE_SETTING}, INT16, 10},
    {{137, false, REGTYPE_SETTING}, INT16, 10},
    {{138, false, REGTYPE_SETTING}, INT16, 10},
    {{139, false, REGTYPE_SETTING}, INT16, 10},
    {{164, false, REGTYPE_SETTING}, INT16, 10},
    {{172, false, REGTYPE_SETTING}, INT16, 10},
    {{196, false, REGTYPE_SETTING}, INT16, 10},
    {{343, false, REGTYPE_SETTING}, UINT16, 1},
    {{348, true, REGTYPE_STATUS}, UINT16, 1},
    {{354, true, REGTYPE_STATUS}, UINT16, 1},

    {{387, false, REGTYPE_STATUS}, UINT16, 1},
    {{388, false, REGTYPE_STATUS}, UINT16, 1},
    {{389, false, REGTYPE_STATUS}, UINT16, 1},
    {{390, false, REGTYPE_STATUS}, UINT16, 1},
    {{391, false, REGTYPE_STATUS}, UINT16, 1},
   

    {{538, true, REGTYPE_STATUS}, UINT16, 1},
    {{581, true, REGTYPE_STATUS}, UINT16, 1},
    {{599, true, REGTYPE_STATUS}, UINT16, 100},
    {{676, false, REGTYPE_SETTING}, UINT16, 10},
    {{677, false, REGTYPE_SETTING}, UINT16, 10},
    {{678, false, REGTYPE_SETTING}, UINT16, 10},
   // {{703, false}, UINT16, 1},
    {{710, true, REGTYPE_STATUS}, UINT16, 1},
    {{726, true, REGTYPE_SETTING}, UINT16, 1},
    {{733, true, REGTYPE_SETTING}, ENUMERATION, 1},
    {{734, true, REGTYPE_SETTING}, ENUMERATION, 1}};
  //  {{774, true}, UINT16, 1},
  //  {{780, true}, UINT16, 10},
  //  {{781, true}, UINT16, 10},
  //  {{783, true}, UINT16, 10},
  //  {{784, true}, UINT16, 10},
  //  {{785, true}, UINT16, 10},
  //  {{788, true}, UINT16, 1},
  //  {{789, true}, UINT16, 1},
  //  {{798, true}, UINT16, 1}};

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