/*
 * FreeModbus Libary: RT-Thread Port
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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "pico/stdlib.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "mbconfig.h"

/* ----------------------- Variables ----------------------------------------*/

static uint32_t alarm_timeout = 0;
static alarm_id_t alarm_id = -1;

/* ----------------------- static functions ---------------------------------*/

static int64_t __isr prvvTIMERExpiredISR(alarm_id_t id, void *user_data);

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
    alarm_timeout = usTimeOut50us * 20;
    return TRUE;
}

inline void vMBMasterPortTimersT35Enable()
{
    if (alarm_id >= 0)
    {
        cancel_alarm(alarm_id);
        alarm_id = -1;
    }
    if (alarm_timeout)
    {
        alarm_id = add_alarm_in_ms(alarm_timeout, prvvTIMERExpiredISR, NULL, true);
    }
}

void vMBMasterPortTimersConvertDelayEnable()
{
    if (alarm_id >= 0)
    {
        cancel_alarm(alarm_id);
        alarm_id = -1;
    }
    if (alarm_timeout)
    {
         /* Set current timer mode, don't change it.*/
        vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
        alarm_id = add_alarm_in_ms(MB_MASTER_DELAY_MS_CONVERT, prvvTIMERExpiredISR, NULL, true);
    }
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
    if (alarm_id >= 0)
    {
        cancel_alarm(alarm_id);
        alarm_id = -1;
    }
    if (alarm_timeout)
    {
         /* Set current timer mode, don't change it.*/
        vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
        alarm_id = add_alarm_in_ms(MB_MASTER_TIMEOUT_MS_RESPOND, prvvTIMERExpiredISR, NULL, true);
    }
}

void vMBMasterPortTimersDisable(void)
{
    if (alarm_id >= 0)
    {
        cancel_alarm(alarm_id);
        alarm_id = -1;
    }
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static int64_t __isr prvvTIMERExpiredISR(alarm_id_t id, void *user_data)
{
    ( void )pxMBMasterPortCBTimerExpired(  );
    return 0;
}

