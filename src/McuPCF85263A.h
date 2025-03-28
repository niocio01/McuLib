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

#define McuPCF85263A_FLAGS_TSF1F_MASK (1<<0) /* Timestamp 1 Flag */
#define McuPCF85263A_FLAGS_TSF2F_MASK (1<<1) /* Timestamp 2 Flag */
#define McuPCF85263A_FLAGS_TSF3F_MASK (1<<2) /* Timestamp 3 Flag */
#define McuPCF85263A_FLAGS_BSF_MASK   (1<<3) /* Battery Switch Flag */
#define McuPCF85263A_FLAGS_WDF_MASK   (1<<4) /* Watchdog Flag */
#define McuPCF85263A_FLAGS_A1F_MASK   (1<<5) /* Alarm 1 Flag */
#define McuPCF85263A_FLAGS_A2F_MASK   (1<<6) /* Alarm 2 Flag */
#define McuPCF85263A_FLAGS_PIF_MASK   (1<<7) /* Periodic Interrupt Flag */

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
 * \brief Get the RTC date.
 * \param date Pointer to store the date information
 * \return Error code, ERR_OK if no failure
 */
uint8_t McuPCF85263A_GetDate(DATEREC *date);

/*!
 * \brief Set the RTC time.
 * \param time Pointer to time information
 * \return Error code, ERR_OK if no failure
 */
uint8_t McuPCF85263A_SetTime(TIMEREC *time);

/*!
 * \brief Set the RTC date.
 * \param date Pointer to date information
 * \return Error code, ERR_OK if no failure
 */
uint8_t McuPCF85263A_SetDate(DATEREC *date);

/*!
 * \brief Get the current time and date from the RTC
 * \param time Pointer where to store the time
 * \param date Pointer where to store the date
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_GetTimeDate(TIMEREC *time, DATEREC *date);

/*!
 * \brief Get a saved timestamp from the RTC
 * \param timestamp number 0-2
 * \param time Pointer where to store the time
 * \param date Pointer where to store the date
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_GetTimestamp(uint8_t tsNum, TIMEREC *time, DATEREC *date);

/*!
 * \brief Set the RTC time information
 * \param Hour The hour, in the range of 0-23
 * \param Min The minute, in the range of 0-59
 * \param Sec The second, in the range of 0-59
 * \param Sec100 The 0.01 sec part, in the range of 0-99
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_SetTimeInfo(uint8_t Hour, uint8_t Min, uint8_t Sec, uint8_t Sec100);

/*!
 * \brief set the RTC date information
 * \param Year The year, for example 2024
 * \param Month The month, for example 11
 * \param Day The day of the month, in the range of 0-31, depending of the month
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_SetDateInfo(uint16_t Year, uint8_t Month, uint8_t Day);

/*! \brief different RTC clock output frequencies */
typedef enum McuPCF85263A_COF_Frequency_e {
  McuPCF85263A_COF_FREQ_32768 = 0,
  McuPCF85263A_COF_FREQ_16384 = 1,
  McuPCF85263A_COF_FREQ_8192  = 2,
  McuPCF85263A_COF_FREQ_4096  = 3,
  McuPCF85263A_COF_FREQ_2048  = 4,
  McuPCF85263A_COF_FREQ_1024  = 5,
  McuPCF85263A_COF_FREQ_1     = 6,
  McuPCF85263A_COF_FREQ_OFF   = 7
} McuPCF85263A_COF_Frequency_e;

/*!
 * \brief Configure a new clock output frequency
 * \param Frequency Frequency to use
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteClockOutputFrequency(McuPCF85263A_COF_Frequency_e frequency);

/*!
 * \brief Read the alarm second time value
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param second Pointer to store the value
 * \param enabled Returns if the alarm is enabled or not.
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmSecond(uint8_t alarmNr, uint8_t *second, bool *enabled);

/*!
 * \brief Write the Alarm second time value
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param second Second time alarm value
 * \param enabled true to enable the alarm, false otherwise
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmSecond(uint8_t alarmNr, uint8_t second, bool enable);

/*!
 * \brief Read the alarm minute value
 * \param alarmNr Alarm to use (0 or 1)
 * \param minute Where to store the minute value
 * \param enabled Returns if the alarm is enabled or not.
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmMinute(uint8_t alarmNr, uint8_t *minute, bool *enabled);

/*!
 * \brief Write the alarm minute value
 * \param alarmNr Alarm to use (0 or 1)
 * \param minute Minute value to write
 * \param enable If the alarm has to be enabled or not
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmMinute(uint8_t alarmNr, uint8_t minute, bool enable);

/*!
 * \brief Read the alarm hour value
 * \param alarmNr Alarm to use (0 or 1)
 * \param hour Where to store the alarm hour value
 * \param enabled Returns if the alarm is enabled or not.
 * \param is24h Returns true if the hour format is 24 hours, false otherwise
 * \param isAM Returns true if the hour is an AM value, false otherwise
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmHour(uint8_t alarmNr, uint8_t *hour, bool *enabled, bool *is24h, bool *isAM);

/*!
 * \brief Write the alarm hour value
 * \param alarmNr Alarm to use (0 or 1)
 * \param hour Hour value
 * \param enable If the alarm has to be enabled or not
 * \param is24h true if hour value is for 24h format, false otherwise
 * \param isAM true if hour value is AM, false otherwise
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmHour(uint8_t alarmNr, uint8_t hour, bool enable, bool is24h, bool isAM);

/*!
 * \brief Read the alarm day
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param day Used to store the alarm day value
 * \param enabled Returns if the alarm is enabled or not.
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmDay(uint8_t alarmNr, uint8_t *day, bool *enabled);

/*!
 * \brief Write the alarm month value
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param day Day value for the alarm
 * \param enable If the alarm has to be enabled or not
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmMonth(uint8_t alarmNr, uint8_t day, bool enable);

/*!
 * \brief Read the alarm month
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param month Month Used to store the alarm day value
 * \param enabled Returns if the alarm is enabled or not.
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmMonth(uint8_t alarmNr, uint8_t *day, bool *enabled);

/*!
 * \brief Write the alarm day value
 * \param alarmNr Alarm to use. Here only nr 0 is supported
 * \param day Day value for the alarm (1-31)
 * \param enable If the alarm has to be enabled or not
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmDay(uint8_t alarmNr, uint8_t day, bool enable);

/*!
 * \brief Read the alarm week day
 * \param alarmNr Alarm to use, here only nr 1 is supported
 * \param weekDay Used to store the day
 * \param enabled Returns if the alarm is enabled or not.
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_ReadAlarmWeekDay(uint8_t alarmNr, uint8_t *weekDay, bool *enabled);

/*!
 * \brief Write the alarm week day
 * \param alarmNr Alarm to use, here only nr 1 is supported
 * \param weekDay The day of week (0-6)
 * \param enable If alarm is enabled
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmWeekDay(uint8_t alarmNr, uint8_t weekDay, bool enable);

/*!
 * \brief Write the alarm interrupt flag setting
 * \param alarmNr Alarm to use (0 or 1)
 * \param enable Use true to enable it, false otherwise
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteAlarmInterrupt(uint8_t alarmNr, bool enable);

/*!
 * \brief Reset the alarm interrupt flag
 * \param alarmNr Alarm to use (0 or 1)
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteResetAlarmFlag(uint8_t alarmNr);

/*!
 * \brief Perform a software reset
 * \return Error code, ERR_OK if no failure.
 */
uint8_t McuPCF85263A_WriteSoftwareReset(void);

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
