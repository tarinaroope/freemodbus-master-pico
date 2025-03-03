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
 */

/* ----------------------- System includes --------------------------------*/

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#include <stdio.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"

/* ----------------------- Defines ------------------------------------------*/

#define UART_ID0 uart0
#define UART_ID1 uart1

#define STOP_BITS 1

#define UART_TX_PIN0 0
#define UART_RX_PIN0 1
#define UART_TX_PIN1 4
#define UART_RX_PIN1 5

/* ----------------------- Static variables ---------------------------------*/

static volatile bool isRx = false;
static volatile bool isTx = false;

static uart_inst_t* UART_ID = UART_ID0;


/* ----------------------- static functions ---------------------------------*/

static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
static void prvvUARTISR( void );
static int64_t prvvOneTimeTimerExpiredISR(alarm_id_t id, void *user_data);
static int64_t prvvOneTimeTimerExpiredISR2(alarm_id_t id, void *user_data);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    
    uint UART_TX_PIN;
    uint UART_RX_PIN;

    if (ucPORT == 0)
    {
        UART_ID = UART_ID0;
        UART_TX_PIN = UART_TX_PIN0;
        UART_RX_PIN = UART_RX_PIN0;
    }
    else if (ucPORT == 1)
    {
        UART_ID = UART_ID1;
        UART_TX_PIN = UART_TX_PIN1;
        UART_RX_PIN = UART_RX_PIN1;
    }
    else 
    {
        // Not supported "COM" port
        return false;
    }

    uart_init(UART_ID, ulBaudRate);
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
   
    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);
    
    // Set our data format
    uart_parity_t parity = UART_PARITY_NONE;
    switch (eParity)
    {
        case MB_PAR_NONE:
            parity = UART_PARITY_NONE;
            break;
        case MB_PAR_ODD:
            parity = UART_PARITY_ODD;
            break;
        case MB_PAR_EVEN:
            parity = UART_PARITY_EVEN;
            break;
        default:
            return FALSE;
    }
 
    uart_set_format(UART_ID, ucDataBits, STOP_BITS, parity);
    
    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);
    
    // Select correct interrupt for the UART we are using
    uint UART_IRQ = (ucPORT == 0) ? UART0_IRQ : UART1_IRQ;
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, prvvUARTISR);
    irq_set_enabled(UART_IRQ, true);
   
    return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */    
    uart_set_irq_enables(UART_ID, xRxEnable, xTxEnable);
    isRx = xRxEnable;
    isTx = xTxEnable;
    
    if (xTxEnable)
    {
        add_alarm_in_ms(1, prvvOneTimeTimerExpiredISR, NULL, TRUE);
    }
}

void vMBMasterPortClose(void)
{
    /* Release any allocated resources. */
    uart_deinit(UART_ID);
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    uart_putc_raw(UART_ID, ucByte);
       
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = uart_getc(UART_ID);

    return TRUE;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void)
{
   pxMBMasterFrameCBTransmitterEmpty();
}

/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBMasterFrameCBByteReceived();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void __isr prvvUARTISR( void )
{
    if ( isRx )
    {
        prvvUARTRxISR(  );
    }
    if ( isTx )
    {
        prvvUARTTxReadyISR(  );
    }
}

static int64_t __isr prvvOneTimeTimerExpiredISR(alarm_id_t id, void *user_data)
{
    prvvUARTTxReadyISR();
    return 0;
}

static int64_t __isr prvvOneTimeTimerExpiredISR2(alarm_id_t id, void *user_data)
{
    pxMBMasterFrameCBByteReceived();
    return 0;
}



