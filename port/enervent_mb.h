/**
 * @file enervent_mb.h
 * @brief Header file for Enervent Modbus port.
 *
 * This file contains the necessary includes and definitions for the Enervent
 * Modbus port implementation.
 *
 *
 * @include "mb.h"
 * @include "mb_m.h"
 */
#ifndef ENERVENT_MB_H
#define ENERVENT_MB_H

#include "mb.h"
#include "mb_m.h"

/**
 * @brief Retrieves the result of the most recent Modbus master request.
 *
 * This function checks the result of the last Modbus master request and 
 * returns it through the provided pointer.
 *
 * @param[out] eResult Pointer to a variable where the result of the request 
 *                     will be stored. The result is of type eMBMasterReqErrCode.
 *
 * @return true if the request result was successfully retrieved, FALSE otherwise.
 */
bool envent_get_request_result(eMBMasterReqErrCode *eResult);

/**
 * @brief Retrieve the array of coil values.
 *
 * This function returns a pointer to an array containing the values of the coils.
 *
 * @return Pointer to an array of uint8_t representing the coil values.
 */

uint8_t* envent_get_coil_value_array();

/**
 * @brief Retrieve the array of register values.
 *
 * This function returns a pointer to an array containing the values of the registers.
 *
 * @return Pointer to an array of uint16_t representing the register values.
 */
uint16_t* envent_get_register_value_array();

#endif