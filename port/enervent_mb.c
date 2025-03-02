#include "enervent_mb.h"
#include "enervent_registers.h"

static eMBMasterReqErrCode eErrCode = MB_MRE_NO_ERR;
static BOOL xResultAvailable = false;

static uint8_t pucCoilValues[COIL_DEFINITION_COUNT] = {0};
static uint16_t pucRegisterValues[REGISTER_DEFINITION_COUNT] = {0};

eMBErrorCode eMBMasterRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                   USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int16_t iRegIndex, prevIndex;

    const register_def_t *pusRegHoldingBuf = get_register_def_array();
    uint16_t *pucRegisterValues = envent_get_register_value_array();

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress + usNRegs <= (LAST_REGISTER_ADDRESS + 1)))
    {
        /* write current register values with new values from the protocol stack. */
        prevIndex = 0;
        iRegIndex = find_register_index_binary(pusRegHoldingBuf, usAddress);
        while (usNRegs > 0)
        {
            // Write only if we are interested about the value aka the index is found from the array
            if (iRegIndex > -1)
            {
                pucRegisterValues[iRegIndex] = *pucRegBuffer++ << 8;
                pucRegisterValues[iRegIndex] |= *pucRegBuffer++;
                prevIndex = iRegIndex;
            }
            else
            {
                // If the index is not found, we will skip the value
                pucRegBuffer += 2;
                iRegIndex = prevIndex;
            }
            usAddress++;
            iRegIndex = find_register_index_seq(pusRegHoldingBuf, usAddress, iRegIndex + 1);
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

eMBErrorCode eMBMasterRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                                 USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex, iRegBitIndex;
    const coil_def_t *pucCoilBuf = get_coil_def_array();
    uint8_t *pucCoilValues = envnet_get_coil_value_array();

    /* it already plus one in modbus function method. */
    usAddress--;
    if ((usAddress + usNCoils <= (LAST_COIL_ADDRESS + 1)))
    {
        iRegIndex = (USHORT)(usNCoils) / 8;
        iRegBitIndex = (USHORT)(usNCoils) % 8;
        int16_t index = 0;
        int16_t prevIndex = 0;
        /* write current coil values with new values from the protocol stack. */

        if ((index = find_coil_index_binary(pucCoilBuf, usAddress)) > -1)
        {
            prevIndex = index;
        }
        while (iRegIndex > 0)
        {
            for (USHORT bitIdx = 0; bitIdx < 8; bitIdx++)
            {
                index = find_coil_index_seq(pucCoilBuf, usAddress, prevIndex);
                if (index > -1)
                {
                    pucCoilValues[index] = (*pucRegBuffer >> bitIdx & 0x01);
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
            index = find_coil_index_seq(pucCoilBuf, usAddress, prevIndex);
            if (iRegIndex > -1)
            {
                pucCoilValues[index] = (*pucRegBuffer >> bitIdx & 0x01);
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

bool envent_get_request_result(eMBMasterReqErrCode *eResult)
{
    bool xHasResult = FALSE;

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

uint8_t *envnet_get_coil_value_array()
{
    return pucCoilValues;
}

uint16_t *envent_get_register_value_array()
{
    return pucRegisterValues;
}
