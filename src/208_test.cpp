#include <ch32v003fun.h>
#include "funny_defs.h"

#include "funny_time.h"
#include "SystemInit120_HSE32.h"


#include <stdbool.h>
#include <cstdlib>
#include <cstring>


#include "eth_driver.h"


#include "tcpServer.h"




int main()
{  
    SystemInit120_HSE32();


#ifdef WCH_FAST_INTERRUPT_ENABLED
	__set_INTSYSCR(0x3); // [Experimental] enable fast interrupt feature
#endif
	system_initSystick();

    //funGpioInitAll();

	
	u8 i;

    WCHNET_GetMacAddr(MACAddr);                                   //get the chip MAC address

    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);           //Ethernet library initialize
    //mStopIfError(i);
    if (i != WCHNET_ERR_SUCCESS)
    {
        while (true)
		{
			/* code */
		}
    }
		
#if KEEPALIVE_ENABLE                                               //Configure keep alive parameters
    {
        struct _KEEP_CFG cfg;

        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    WCHNET_CreateTcpSocketListen();                               //Create TCP Socket for Listening

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }
    }
}

