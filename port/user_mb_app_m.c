/*
 * FreeModbus Libary: user callback functions and buffer define in master mode
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include "user_mb_app_m.h"
#include "registers.h"


static eMBMasterReqErrCode eErrCode = MB_MRE_NO_ERR;
static BOOL xResultAvailable = false;

/**
 * Modbus master holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBMasterRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                   USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, prevIndex;
    HoldingRegister *pusRegHoldingBuf;
   
    pusRegHoldingBuf = get_holding_buffer();

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress + usNRegs <= REG_HOLDING_NREGS))
    {
        /* write current register values with new values from the protocol stack. */
        prevIndex = 0;
        iRegIndex = find_index_binary((BaseData*) pusRegHoldingBuf, sizeof(HoldingRegister), DEFINED_HOLDING_NREGS, usAddress);
        while (usNRegs > 0)
        {
            // Write only if we are interested about the value aka the index is found from the array
            if (iRegIndex > -1)
            {
                pusRegHoldingBuf[iRegIndex].value = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex].value |= *pucRegBuffer++;
                prevIndex = iRegIndex;
            }
            else 
            {
                // If the index is not found, we will skip the value
                pucRegBuffer += 2;
                iRegIndex = prevIndex;
            }
            usAddress++;
            iRegIndex = find_index_seq((BaseData*) pusRegHoldingBuf, sizeof(HoldingRegister), DEFINED_HOLDING_NREGS, usAddress, iRegIndex + 1);
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    eMBMasterSetReqResult(eStatus);
    return eStatus;
}

/**
 * Modbus master coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBMasterRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                 USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, iRegBitIndex;
    Coil *pucCoilBuf;
    pucCoilBuf = get_coil_buffer();

    /* it already plus one in modbus function method. */
    usAddress--;
    if ((usAddress + usNCoils <= COIL_NCOILS))
    {
        iRegIndex = (USHORT)(usNCoils) / 8;
        iRegBitIndex = (USHORT)(usNCoils) % 8;
        USHORT index, prevIndex = 0;
        /* write current coil values with new values from the protocol stack. */
        
        if(find_index_binary((BaseData*) pucCoilBuf, sizeof(Coil), DEFINED_NCOILS, usAddress) > -1)
        {
            prevIndex = index;
        }
        while (iRegIndex > 0)
        {
            for (USHORT bitIdx = 0; bitIdx < 8; bitIdx++)
            {
                index = find_index_seq((BaseData*) pucCoilBuf, sizeof(Coil), DEFINED_NCOILS, usAddress, prevIndex);
                if (iRegIndex > -1)
                {
                    pucCoilBuf[index].value = (*pucRegBuffer >> bitIdx & 0x01);
                    prevIndex = index;
                }
                usAddress++;
            }
            iRegIndex--;
            pucRegBuffer++;
        }

        // Last remaining bits
        USHORT bitIdx = 0;
        while (iRegBitIndex > 0)
        {
            index = find_index_seq((BaseData*) pucCoilBuf, sizeof(Coil), DEFINED_NCOILS, usAddress, prevIndex);
            if (iRegIndex > -1)
            {
                pucCoilBuf[index].value = (*pucRegBuffer >> bitIdx & 0x01);
                prevIndex = index;
            }
            usAddress++;
            iRegBitIndex--;
            bitIdx++;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    eMBMasterSetReqResult(eStatus);
    return eStatus;
}

BOOL eMBMasterGetReqResult(eMBMasterReqErrCode *eResult)
{
    BOOL xHasResult = FALSE;

    if (xResultAvailable)
    {
        *eResult = eErrCode;
        xResultAvailable = FALSE;
        xHasResult = TRUE;
    }
    return xHasResult;
}

void eMBMasterSetReqResult(eMBMasterReqErrCode eResult)
{
    xResultAvailable = TRUE;
    eErrCode = eResult;
}

