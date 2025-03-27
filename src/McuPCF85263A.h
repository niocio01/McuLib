/*
 * Copyright (c) 2025, Nico Zuber
 *
 * Driver for the NXP PCF85263A I2C RTC / Watchdog.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCUPCF85263A_H_
#define MCUPCF85263A_H_

#include "McuPCF85263A_config.h"
#include "McuShell.h"
#include "McuTimeDate.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Shell command line parser.
 * \param cmd Shell command string
 * \param handled Set to true if command was handled, untouched otherwise
 * \param io Standard I/O handler to be used
 * \return Error code, ERR_OK if no failure
 */
uint8_t McuPCF85263A_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief Get the RTC time.
 * \param time Pointer store the time information
 * \return Error code, ERR_OK if no failure
 */
uint8_t McuPCF85263A_GetTime(TIMEREC *time);

/*!
 * \brief Driver de-initialization.
 */
void McuPCF85263A_Deinit(void);

/*!
 * \brief Driver initialization.
 */
void McuPCF85263A_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* McuPCF85263A_H_ */
