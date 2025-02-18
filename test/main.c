#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#include "pico/stdlib.h"
#include <stdio.h>

#include "registers.h"
#include "user_mb_app_m.h"

#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define UART_ID uart1
#define BAUD_RATE 19200

volatile bool sent = false;

int main(void)
{

    timer_hw->dbgpause = 0; // hack!
    stdio_init_all();

    sleep_ms(1000);

    printf("\n\nSensorHub init...\n");

    eMBErrorCode eStatus = eMBMasterInit( MB_RTU, 1, 19200, MB_PAR_NONE );

    //eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 );
    eStatus = eMBMasterEnable(  );

    for( ;; )
    {
        ( void )eMBMasterPoll(  );

        if ( !sent && eMBMasterIsEstablished())
        {
            eMBMasterReqReadCoils( 1, 30, 1, -1 );
            sent = true;
        }
        eMBMasterReqErrCode cod;
        if(eMBMasterGetReqResult(&cod))
        {   
            Coil* coil = get_coil_buffer();
            int index = find_index_binary((BaseData*) coil, sizeof(Coil), DEFINED_NCOILS, 30);
            printf("\nRESULT %d\n",coil[index].value); 
        }
     
    }
}