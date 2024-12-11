/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : McuRNet.h
**     Component   : McuRNet
**     Version     : Component 01.096, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2021-07-06, 16:35, # CodeGen: 0
**     Abstract    :
**          This components implements the RNet Stack.
**     Settings    :
**          Component name                                 : McuRNet
**          Transceiver                                    : 
**            Transceiver Type                             : nRF24L01+
**            nRF24L01+                                    : Enabled
**              nRF24L01+                                  : McuNRF24L01
**              Radio Channel                              : 81
**              Data Rate                                  : 2000 kBit
**              Payload Size                               : 32
**              Address                                    : 0x11, 0x22, 0x33, 0x44, 0x55
**            SMAC                                         : Disabled
**          Network                                        : 
**            Address Size                                 : 8 Bits
**          Queues                                         : 
**            Rx Message Queue Size                        : 15
**            Tx Message Queue Size                        : 15
**            Message Queue Blocking Time (ms)             : 200
**          Send Retry Count                               : 3
**          Send Timeout (ms)                              : 250
**          System                                         : 
**            Utility                                      : UTIL1
**            RTOS                                         : FRTOS1
**            Shell                                        : Enabled
**              Remote StdIO                               : Enabled
**                Queue length                             : 48
**                Queue Timeout (ms)                       : 500
**              Shell                                      : McuShell
**     Contents    :
**         SetChannel   - uint8_t McuRNet_SetChannel(uint8_t channel);
**         ParseCommand - uint8_t McuRNet_ParseCommand(const unsigned char *cmd, bool *handled, const...
**         Init         - void McuRNet_Init(void);
**         Deinit       - void McuRNet_Deinit(void);
**
** * Copyright (c) 2014-2019, Erich Styger
**  * Web:         https://mcuoneclipse.com
**  * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**  * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**  * All rights reserved.
**  *
**  * Redistribution and use in source and binary forms, with or without modification,
**  * are permitted provided that the following conditions are met:
**  *
**  * - Redistributions of source code must retain the above copyright notice, this list
**  *   of conditions and the following disclaimer.
**  *
**  * - Redistributions in binary form must reproduce the above copyright notice, this
**  *   list of conditions and the following disclaimer in the documentation and/or
**  *   other materials provided with the distribution.
**  *
**  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**  * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** \file McuRNet.c
** \version 01.00
** \brief This module implements the RNet Stack.
*/         
/*!
**  @addtogroup McuRNet_module McuRNet module documentation
**  @{
*/         

/* MODULE McuRNet. */

#include "McuRNetConfig.h"
#if McuRNET_CONFIG_IS_ENABLED
#include "McuRNet.h"
#include "RStack.h"
#include "Radio.h"
#include "RNWK.h"
#include "RMSG.h"
#include "RApp.h"
#include "RStdIO.h"
#include "McuNRF24L01.h"

/*
** ===================================================================
**     Method      :  McuRNet_OnInterrupt (component RNet)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void McuNRF24L01_OnInterrupt(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Method      :  Init (component RNet)
**
**     Description :
**         Initializes the RNet Stack
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuRNet_Init(void)
{
  RSTACK_Init();
}

/*
** ===================================================================
**     Method      :  Deinit (component RNet)
**
**     Description :
**         Deinitializes the RNet Stack
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuRNet_Deinit(void)
{
  RSTACK_Deinit();
}

/*
** ===================================================================
**     Method      :  SetChannel (component RNet)
**
**     Description :
**         Sets the radio channel
**     Parameters  :
**         NAME            - DESCRIPTION
**         channel         - Channel number
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t McuRNet_SetChannel(uint8_t channel)
{
  return RADIO_SetChannel(channel);
}

/*
** ===================================================================
**     Method      :  ParseCommand (component RNet)
**
**     Description :
**         Shell Command Line parser. This method is enabled/disabled
**         depending on if you have the Shell enabled/disabled in the
**         properties.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * cmd             - Pointer to command string
**       * handled         - Pointer to variable which tells if
**                           the command has been handled or not
**       * io              - Pointer to I/O structure
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t McuRNet_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io)
{
  static const McuShell_ParseCommandCallback CmdParserTable[] =
  {
    RADIO_ParseCommand,
    RNWK_ParseCommand,
    RMSG_ParseCommand,
  #if RNET_CONFIG_REMOTE_STDIO
    #if RSTDIO_PARSE_COMMAND_ENABLED
    RSTDIO_ParseCommand,
    #endif
  #endif
    McuNRF24L01_ParseCommand,
    NULL /* sentinel */
  };
  return McuShell_IterateTable(cmd, handled, io, CmdParserTable);
}

#if PL_HAS_RADIO
void McuRNet_OnRadioEvent(McuRNet_RadioEvent event)
{
  (void)event;
  /* Write your code here ... */
}
#endif

#endif /* McuRNET_CONFIG_IS_ENABLED */

/* END McuRNet. */

/*!
** @}
*/
