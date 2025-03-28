/*
 * Copyright (c) 2025, Nico Zuber
 *
 * * Driver for the NXP PCF85263A I2C RTC.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuPCF85263A.h"
#include "McuShell.h"
#include "McuUtility.h"
#include "McuGenericI2C.h"
#include "McuXFormat.h"

#define McuPCF85263A_I2C_DEVICE_ADDRESS   (0x51) /* 7bit I2C address, not-shifted value */

#pragma region RegisterMap
/* RTC mode (default) register memory map */

// RTC time and date
#define McuPCF85263A_ADDRESS_TIME_DATE_START_ADDR   0x00  /* start address of time and date registers */
#define McuPCF85263A_ADDRESS_TIME_START_ADDR        0x00  /* start address of time registers */
#define McuPCF85263A_ADDRESS_DATE_START_ADDR        0x04  /* start address of time registers */

// RTC alarms
#define McuPCF85263A_ADDRESS_ALARM1_START_ADDR       0x08  /* start address of alarm1 registers */
#define McuPCF85263A_ADDRESS_ALARM1_SECOND           0x08  /* Second_alarm1 */
#define McuPCF85263A_ADDRESS_ALARM1_MINUTE           0x09  /* Minute_alarm1 */
#define McuPCF85263A_ADDRESS_ALARM1_HOUR             0x0A  /* Hour_alarm1 */
#define McuPCF85263A_ADDRESS_ALARM1_DAY              0x0B  /* Day_alarm1 */
#define McuPCF85263A_ADDRESS_ALARM1_MONTH            0x0C  /* Month_alarm1 */

#define McuPCF85263A_ADDRESS_ALARM2_START_ADDR       0x0D  /* start address of alarm2 registers */
#define McuPCF85263A_ADDRESS_ALARM2_MINUTE           0x0D  /* Minute_alarm2 */
#define McuPCF85263A_ADDRESS_ALARM2_HOUR             0x0E  /* Hour_alarm2 */
#define McuPCF85263A_ADDRESS_ALARM2_WEEKDAY          0x0F  /* Weekday_alarm2 */

#define McuPCF85263A_ADDRESS_ALARM_ENABLES           0x10  /* Alarm_enables */

// RTC Timestamps
#define McuPCF85263A_ADDRESS_TSR1_START_ADDR         0x11  /* start address of timestamp1 registers */
#define McuPCF85263A_ADDRESS_TSR1_SECONDS            0x11  /* TSR1_seconds */
#define McuPCF85263A_ADDRESS_TSR1_MINUTES            0x12  /* TSR1_minutes */
#define McuPCF85263A_ADDRESS_TSR1_HOURS              0x13  /* TSR1_hours */
#define McuPCF85263A_ADDRESS_TSR1_DAYS               0x14  /* TSR1_days */
#define McuPCF85263A_ADDRESS_TSR1_MONTHS             0x15  /* TSR1_months */
#define McuPCF85263A_ADDRESS_TSR1_YEARS              0x16  /* TSR1_years */

#define McuPCF85263A_ADDRESS_TSR2_START_ADDR         0x17  /* start address of timestamp2 registers */
#define McuPCF85263A_ADDRESS_TSR2_SECONDS            0x17  /* TSR2_seconds */
#define McuPCF85263A_ADDRESS_TSR2_MINUTES            0x18  /* TSR2_minutes */
#define McuPCF85263A_ADDRESS_TSR2_HOURS              0x19  /* TSR2_hours */
#define McuPCF85263A_ADDRESS_TSR2_DAYS               0x1A  /* TSR2_days */
#define McuPCF85263A_ADDRESS_TSR2_MONTHS             0x1B  /* TSR2_months */
#define McuPCF85263A_ADDRESS_TSR2_YEARS              0x1C  /* TSR2_years */

#define McuPCF85263A_ADDRESS_TSR3_START_ADDR         0x1D  /* start address of timestamp3 registers */
#define McuPCF85263A_ADDRESS_TSR3_SECONDS            0x1D  /* TSR3_seconds */
#define McuPCF85263A_ADDRESS_TSR3_MINUTES            0x1E  /* TSR3_minutes */
#define McuPCF85263A_ADDRESS_TSR3_HOURS              0x1F  /* TSR3_hours */
#define McuPCF85263A_ADDRESS_TSR3_DAYS               0x20  /* TSR3_days */
#define McuPCF85263A_ADDRESS_TSR3_MONTHS             0x21  /* TSR3_months */
#define McuPCF85263A_ADDRESS_TSR3_YEARS              0x22  /* TSR3_years */

#define McuPCF85263A_ADDRESS_TSR_MODE                0x23  /* TSR_mode */

/* Control and Function register memory map */
#define McuPCF85263A_ADDRESS_OFFSET                  0x24  /* Offset */

#define McuPCF85263A_ADDRESS_CONTROL_Start_ADDR      0x25  /* start address of control registers */
#define McuPCF85263A_ADDRESS_CONTROL_OSCILATOR       0x25  /* Oscilator */
#define McuPCF85263A_ADDRESS_CONTROL_BATTERY_SWITCH  0x26  /* Battery_switch */
#define McuPCF85263A_ADDRESS_CONTROL_PIN_IO          0x27  /* Pin_IO */
#define McuPCF85263A_ADDRESS_CONTROL_Function        0x28  /* Function */
#define McuPCF85263A_ADDRESS_CONTROL_INTA_ENABLE     0x29  /* INT_A_enable */
#define McuPCF85263A_ADDRESS_CONTROL_INTB_ENABLE     0x2A  /* INT_B_enable */
#define McuPCF85263A_ADDRESS_CONTROL_FLAGS           0x2B  /* Flags */
#define McuPCF85263A_ADDRESS_CONTROL_RAM_Byte        0x2C  /* RAM_Byte */
#define McuPCF85263A_ADDRESS_CONTROL_WATCHDOG        0x2D  /* WatchDog */
#define McuPCF85263A_ADDRESS_CONTROL_STOP_ENABLE     0x2E  /* Stop_enable */
#define McuPCF85263A_ADDRESS_CONTROL_RESETS          0x2F  /* Resets */


#define McuPCF85263A_TTIME_AMPM_AM   0  /* AM */
#define McuPCF85263A_TTIME_AMPM_PM   1  /* PM */
/* hour mode is set in 12_24 bit in Control_1 */
#define McuPCF85263A_TTIME_MODE_24H  0  /* 24 hour clock mode (default) */
#define McuPCF85263A_TTIME_MODE_12H  1  /* 12 hour clock mode */

#define McuPCF85263A_MEMORY_TIME_SIZE       (4) /* 0x00-0x03, number of bytes for time information on device */
#define McuPCF85263A_MEMORY_DATE_SIZE       (4) /* 0x04-0x07, number of bytes for date information on device */
#define McuPCF85263A_MEMORY_TIME_DATE_SIZE  (8) /* 0x00-0x07, number of bytes for data and time information on device */

#pragma endregion 

#pragma region Typedefs
typedef struct McuPCF85263A_TTIME {  /* Time in binary format (24h) */
  uint8_t hour;                      /* hours */
  uint8_t min;                       /* minutes */
  uint8_t sec;                       /* seconds */
  uint8_t cs;                        /* centiseconds (hundredths) */
  uint8_t am_pm;                     /* 0: AM, 1: PM */
  uint8_t mode;                      /* hour mode is set in 12_24 bit in Control_1 */
} McuPCF85263A_TTIME;

typedef struct McuPCF85263A_TDATE {  /* Date in binary format */
  uint8_t year;                      /* year */
  uint8_t month;                     /* month */
  uint8_t day;                       /* day */
  uint8_t dayOfWeek;                 /* Day of week, where 0 is the first day. In the range of 0..6 */
} McuPCF85263A_TDATE;

#pragma endregion

#pragma region Register_ReadWrites
uint8_t McuPCF85263A_ReadBlock(uint8_t addr, uint8_t *buf, size_t bufSize) {
  return McuGenericI2C_ReadAddress(McuPCF85263A_I2C_DEVICE_ADDRESS, &addr, sizeof(addr), buf, bufSize);
}

uint8_t McuPCF85263A_WriteBlock(uint8_t addr, uint8_t *buf, size_t bufSize) {
  return McuGenericI2C_WriteAddress(McuPCF85263A_I2C_DEVICE_ADDRESS, &addr, sizeof(addr), buf, bufSize);
}

uint8_t McuPCF85263A_ReadByte(uint8_t addr, uint8_t *data) {
  return McuGenericI2C_ReadAddress(McuPCF85263A_I2C_DEVICE_ADDRESS, &addr, sizeof(addr), data, sizeof(*data));
}

uint8_t McuPCF85263A_WriteByte(uint8_t addr, uint8_t data) {
  return McuGenericI2C_WriteAddress(McuPCF85263A_I2C_DEVICE_ADDRESS, &addr, sizeof(addr), &data, sizeof(data));
}


uint8_t McuPCF85263A_ReadOffset(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_OFFSET, data);
}
uint8_t McuPCF85263A_WriteOffset(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_OFFSET, data);
}

uint8_t McuPCF85263A_ReadOscilator(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_OSCILATOR, data);
}
uint8_t McuPCF85263A_WriteOscilator(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_OSCILATOR, data);
}

uint8_t McuPCF85263A_ReadBatterySwitch(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_BATTERY_SWITCH, data);
}
uint8_t McuPCF85263A_WriteBatterySwitch(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_BATTERY_SWITCH, data);
}

uint8_t McuPCF85263A_ReadPinIO(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_PIN_IO, data);
}
uint8_t McuPCF85263A_WritePinIO(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_PIN_IO, data);
}

uint8_t McuPCF85263A_ReadFunction(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_Function, data);
}
uint8_t McuPCF85263A_WriteFunction(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_Function, data);
}

uint8_t McuPCF85263A_ReadINTAEnable(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_INTA_ENABLE, data);
}
uint8_t McuPCF85263A_WriteINTAEnable(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_INTA_ENABLE, data);
}

uint8_t McuPCF85263A_ReadINTBEnable(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_INTB_ENABLE, data);
}
uint8_t McuPCF85263A_WriteINTBEnable(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_INTB_ENABLE, data);
}

uint8_t McuPCF85263A_ReadFlags(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_FLAGS, data);
}
uint8_t McuPCF85263A_WriteFlags(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_FLAGS, data);
}

uint8_t McuPCF85263A_ReadRamByte(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_RAM_Byte, data);
}
uint8_t McuPCF85263A_WriteRamByte(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_RAM_Byte, data);
}

uint8_t McuPCF85263A_ReadWatchDog(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_WATCHDOG, data);
}
uint8_t McuPCF85263A_WriteWatchDog(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_WATCHDOG, data);
}

uint8_t McuPCF85263A_ReadStopEnable(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_STOP_ENABLE, data);
}
uint8_t McuPCF85263A_WriteStopEnable(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_STOP_ENABLE, data);
}

uint8_t McuPCF85263A_ReadResets(uint8_t *data) {
  return McuPCF85263A_ReadByte(McuPCF85263A_ADDRESS_CONTROL_RESETS, data);
}
uint8_t McuPCF85263A_WriteResets(uint8_t data) {
  return McuPCF85263A_WriteByte(McuPCF85263A_ADDRESS_CONTROL_RESETS, data);
}

#pragma endregion Register_ReadWrites

#pragma region TimeDateFunctions

bool McuPCF85263A_Is24hMode(void) {
  uint8_t res, osc;

  res = McuPCF85263A_ReadOscilator(&osc);
  if (res!=ERR_OK) {
    return true; /* default is 24h mode */
  }
  return (osc&(1<<5))==0; /* 0: 24-hour mode, 1: 12-hour mode */
}


uint8_t McuPCF85263A_ReadTimeDate(McuPCF85263A_TTIME *time, McuPCF85263A_TDATE *date) {
  uint8_t buf[McuPCF85263A_MEMORY_TIME_DATE_SIZE];
  bool is24hMode;

  if (McuPCF85263A_ReadBlock(McuPCF85263A_ADDRESS_TIME_DATE_START_ADDR, buf, sizeof(buf))!=ERR_OK) {
    return ERR_FAILED;
  }
  is24hMode = McuPCF85263A_Is24hMode();
  time->cs = (uint8_t)(((buf[0]&0xF0)>>4)*10 + (buf[0]&0x0F)); /* centiseconds in BCD format */
  time->sec = (uint8_t)(((buf[1]&0x70)>>4)*10 + (buf[1]&0x0F)); /* seconds in BCD format */
  time->min = (uint8_t)((buf[2]>>4)*10 + (buf[2]&0x0F)); /* minutes in BCD format */
  if (is24hMode) {
    time->hour = (uint8_t)(((buf[3]&0x30)>>4)*10 + (buf[3]&0x0F)); /* hour in BCD format */
    time->mode = McuPCF85263A_TTIME_MODE_24H;
  } else {
    time->hour = (uint8_t)(((buf[3]&0x10)>>4)*10 + (buf[3]&0x0F)); /* hour in BCD format */
    time->am_pm = (uint8_t)((buf[3]&(1<<5))>>5); /* bit 5: AM/PM indicator. 0: AM, 1: PM */
    time->mode = McuPCF85263A_TTIME_MODE_12H;
  }
  date->day = (uint8_t)((buf[4]>>4)*10 + (buf[4]&0x0F)); /* BCD format */
  date->dayOfWeek =(uint8_t)(buf[5]); /* 0: Sunday, 1: Monday, ... 6: Saturday */
  date->month = (uint8_t)((buf[6]>>4)*10 + (buf[6]&0x0F)); /* 1: January */
  date->year = (uint8_t)((buf[7]>>4)*10 + (buf[7]&0x0F));
  return ERR_OK;
}

uint8_t McuPCF85263A_WriteTimeDate(McuPCF85263A_TTIME *time, McuPCF85263A_TDATE *date) {
  uint8_t buf[McuPCF85263A_MEMORY_TIME_DATE_SIZE];
  if (   ((time->mode==McuPCF85263A_TTIME_MODE_12H)&&((time->hour>12)||(time->hour==0)))
      || ((time->mode==McuPCF85263A_TTIME_MODE_24H)&&(time->hour>23))
      || (time->min>59)||(time->sec>59)
      || (date->year>99)||(date->month>12)||(date->month==0)
      || (date->day>31)||(date->day==0)
      || (date->dayOfWeek>6))
  {
    return ERR_RANGE;
  }
  buf[0] = (uint8_t)(((time->cs/10)<<4) | (time->cs%10));
  buf[1] = (uint8_t)(((time->sec/10)<<4) | (time->sec%10));
  buf[2] = (uint8_t)(((time->min/10)<<4) | (time->min%10));
  buf[3] = (uint8_t)(((time->hour/10)<<4) | (time->hour%10));
  if (time->mode==McuPCF85263A_TTIME_MODE_12H) {
    buf[3] |= (time->am_pm)?(1<<5):0;
  }
  buf[4] = (uint8_t)(((date->day/10)<<4) | (date->day%10));
  buf[5] = (uint8_t)(date->dayOfWeek);
  buf[6] = (uint8_t)(((date->month/10)<<4) | (date->month%10));
  buf[7] = (uint8_t)(((date->year/10)<<4) | (date->year%10));
  if (McuPCF85263A_WriteBlock(McuPCF85263A_ADDRESS_TIME_DATE_START_ADDR, buf, sizeof(buf))!=ERR_OK) {
    return ERR_FAILED;
  }
  return ERR_OK;
}

uint8_t McuPCF85263A_WriteDate(McuPCF85263A_TDATE *date) {
  uint8_t buf[McuPCF85263A_MEMORY_DATE_SIZE];
  if (   (date->year>99)
      || (date->month>12)||(date->month==0)
      || (date->day>31)||(date->day==0)
      || (date->dayOfWeek>6)
     )
  {
    return ERR_RANGE;
  }
  buf[0] = (uint8_t)(((date->day/10)<<4) | (date->day%10));
  buf[1] = (uint8_t)(date->dayOfWeek);
  buf[2] = (uint8_t)(((date->month/10)<<4) | (date->month%10));
  buf[3] = (uint8_t)(((date->year/10)<<4) | (date->year%10));
  if (McuPCF85263A_WriteBlock(McuPCF85263A_ADDRESS_DATE_START_ADDR, buf, sizeof(buf))!=ERR_OK) {
    return ERR_FAILED;
  }
  return ERR_OK;
}

uint8_t McuPCF85263A_WriteTime(McuPCF85263A_TTIME *time) {
  uint8_t buf[McuPCF85263A_MEMORY_TIME_SIZE];

  if (   ((time->mode==McuPCF85263A_TTIME_MODE_12H)&&((time->hour>12)||(time->hour==0)))
      || ((time->mode==McuPCF85263A_TTIME_MODE_24H)&&(time->hour>23))
      || (time->min>59) || (time->sec>59)
     )
  {
    return ERR_RANGE;
  }
  buf[0] = (uint8_t)(((time->cs/10)<<4) | (time->cs%10));
  buf[1] = (uint8_t)(((time->sec/10)<<4) | (time->sec%10));
  buf[2] = (uint8_t)(((time->min/10)<<4) | (time->min%10));
  buf[3] = (uint8_t)(((time->hour/10)<<4) | (time->hour%10));
  if (time->mode==McuPCF85263A_TTIME_MODE_12H) {
    buf[3] |= (time->am_pm)?(1<<5):0;
  }
  if (McuPCF85263A_WriteBlock(McuPCF85263A_ADDRESS_TIME_START_ADDR, buf, sizeof(buf))!=ERR_OK) {
    return ERR_FAILED;
  }
  return ERR_OK;
}

uint8_t McuPCF85263A_GetTime(TIMEREC *time) {
  McuPCF85263A_TTIME ttime;
  McuPCF85263A_TDATE tdate;

  if (McuPCF85263A_ReadTimeDate(&ttime, &tdate)!=ERR_OK) {
    return ERR_FAILED;
  }
  time->Hour = ttime.hour;
  time->Min = ttime.min;
  time->Sec = ttime.sec;
  time->Sec100 = ttime.cs;
  return ERR_OK;
}

uint8_t McuPCF85263A_SetTimeInfo(uint8_t Hour, uint8_t Min, uint8_t Sec, uint8_t Sec100) {
  McuPCF85263A_TTIME ttime;

  ttime.hour = Hour;
  ttime.min = Min;
  ttime.sec = Sec;
  ttime.cs = Sec100;
  ttime.mode = McuPCF85263A_TTIME_MODE_24H;
  ttime.am_pm = McuPCF85263A_TTIME_AMPM_AM;
  return McuPCF85263A_WriteTime(&ttime);
}

uint8_t McuPCF85263A_SetTime(TIMEREC *time) {
  return McuPCF85263A_SetTimeInfo(time->Hour, time->Min, time->Sec, time->Sec100);
}

uint8_t McuPCF85263A_GetDate(DATEREC *date) {
  McuPCF85263A_TTIME ttime;
  McuPCF85263A_TDATE tdate;

  if (McuPCF85263A_ReadTimeDate(&ttime, &tdate)!=ERR_OK) {
    return ERR_FAILED;
  }
  date->Year = (uint16_t)(tdate.year+2000); /* assume we are after the year 2000 */
  date->Month = tdate.month;
  date->Day = tdate.day;
  return ERR_OK;
}

uint8_t McuPCF85263A_SetDateInfo(uint16_t Year, uint8_t Month, uint8_t Day) {
  McuPCF85263A_TDATE tdate;

  if (Year>=2000) {
    tdate.year = (uint8_t)(Year-2000);
  } else if (Year>=1900) {
    tdate.year = (uint8_t)(Year-1900);
  } else {
    tdate.year = Year%100;
  }
  tdate.month = Month;
  tdate.day = Day;
  tdate.dayOfWeek = McuUtility_WeekDay(Year, Month, Day);
  return McuPCF85263A_WriteDate(&tdate);
}

uint8_t McuPCF85263A_SetDate(DATEREC *date) {
  return McuPCF85263A_SetDateInfo(date->Year, date->Month, date->Day);
}

uint8_t McuPCF85263A_GetTimeDate(TIMEREC *time, DATEREC *date) {
  McuPCF85263A_TTIME ttime;
  McuPCF85263A_TDATE tdate;

  if (McuPCF85263A_ReadTimeDate(&ttime, &tdate)!=ERR_OK) {
    return ERR_FAILED;
  }
  time->Hour = ttime.hour;
  time->Min = ttime.min;
  time->Sec = ttime.sec;
  time->Sec100 = ttime.cs;
  date->Year = (uint16_t)(tdate.year+2000);
  date->Month = tdate.month;
  date->Day = tdate.day;
  return ERR_OK;
}

static const char *GetWeekDayString(uint8_t weekday) {
  static const char *const weekDays[]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  if (weekday<=sizeof(weekDays)/sizeof(weekDays[0])) {
    return weekDays[weekday];
  } else {
    return "error";
  }
}

static uint8_t StrCatHWTimeDate(uint8_t *buf, size_t bufSize) {
  McuPCF85263A_TDATE tdate;
  McuPCF85263A_TTIME ttime;

  if (McuPCF85263A_ReadTimeDate(&ttime, &tdate)!=ERR_OK) {
    return ERR_FAILED;
  }
  if (tdate.dayOfWeek<=6) {
    McuUtility_strcat(buf, bufSize, (unsigned char*)GetWeekDayString(tdate.dayOfWeek));
  }
  McuUtility_chcat(buf, bufSize, ' ');
  McuUtility_strcatNum16uFormatted(buf, bufSize, tdate.day, '0', 2);
  McuUtility_chcat(buf, bufSize, '.');
  McuUtility_strcatNum16uFormatted(buf, bufSize, tdate.month, '0', 2);
  McuUtility_chcat(buf, bufSize, '.');
  McuUtility_strcatNum16u(buf, bufSize, (uint16_t)tdate.year);
  McuUtility_strcat(buf, bufSize, (unsigned char*)", ");
  McuUtility_strcatNum16sFormatted(buf, bufSize, ttime.hour, '0', 2);
  McuUtility_chcat(buf, bufSize, ':');
  McuUtility_strcatNum16sFormatted(buf, bufSize, ttime.min, '0', 2);
  McuUtility_chcat(buf, bufSize, ':');
  McuUtility_strcatNum16sFormatted(buf, bufSize, ttime.sec, '0', 2);
  McuUtility_chcat(buf, bufSize, '.');
  McuUtility_strcatNum8u(buf, bufSize, ttime.cs);
  if (ttime.mode==McuPCF85263A_TTIME_MODE_24H) {
    McuUtility_strcat(buf, bufSize, (unsigned char*)" (24h)");
  } else {
    if (ttime.am_pm==McuPCF85263A_TTIME_AMPM_AM) {
    McuUtility_strcat(buf, bufSize, (unsigned char*)"am");
    } else {
      McuUtility_strcat(buf, bufSize, (unsigned char*)"pm");
    }
  }
  return ERR_OK;
}
#pragma endregion TimeDateFunctions

#pragma region ShellCommands

static uint8_t DateCmd(const unsigned char *cmd, McuShell_ConstStdIOType *io) {
  uint8_t day, month;
  uint16_t year;
  uint8_t res = ERR_OK;

  if (McuUtility_ScanDate(&cmd, &day, &month, &year) == ERR_OK) { /* ok, format fine */
    /* update real time clock */
    res = McuPCF85263A_SetDateInfo(year, month, day);
    if (res!=ERR_OK) {
      McuShell_SendStr((unsigned char*)"*** Failure setting RTC\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  } else {
    McuShell_SendStr((unsigned char*)"*** Error while reading command! ***", io->stdErr);
    McuShell_SendStr((void *)cmd, io->stdErr);
    McuShell_SendStr((unsigned char*)"\r\n", io->stdErr);
    res = ERR_FAILED;
  }
  return res;
}

static uint8_t TimeCmd(const unsigned char *cmd, McuShell_ConstStdIOType *io) {
  uint8_t hour, minute, second, hSecond;
  uint8_t res = ERR_OK;

  if (McuUtility_ScanTime(&cmd, &hour, &minute, &second, &hSecond)==ERR_OK) { /* format fine */
    /* set RTC time */
    res = McuPCF85263A_SetTimeInfo(hour, minute, second, hSecond);
    if (res != ERR_OK) {
      McuShell_SendStr((unsigned char*)"*** Failure setting RTC time\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  } else {
    McuShell_SendStr((unsigned char*)"*** Error while reading command: ", io->stdErr);
    McuShell_SendStr(cmd, io->stdErr);
    McuShell_SendStr((unsigned char*)"\r\n", io->stdErr);
    res = ERR_FAILED;
  }
  return res;
}

uint8_t McuPCF85263A_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
  int32_t val;
  const unsigned char *p;
  bool enabled;
  uint8_t dummy;

  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "rtc help")==0) {
    *handled = true;
    McuShell_SendHelpStr((unsigned char*)"rtc", (const unsigned char*)"Group of PCF85263 RTC commands\r\n", io->stdOut);
    // McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
    McuShell_SendHelpStr((unsigned char*)"  get datetime", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
    McuShell_SendHelpStr((unsigned char*)"  set time [hh:mm:ss[,z]]", (const unsigned char*)"Set the current time\r\n", io->stdOut);
    McuShell_SendHelpStr((unsigned char*)"  set date [dd.mm.yyyy]", (const unsigned char*)"Set the current date\r\n", io->stdOut);

    return ERR_OK;
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "rtc get datetime")==0)) {
    *handled = true;
    unsigned char buf[96];
    buf[0] = '\0';
    if (StrCatHWTimeDate(buf, sizeof(buf))!=ERR_OK) {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
    } else {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
    }
    McuShell_SendStatusStr((unsigned char*)"  time/date", buf, io->stdOut);
  } else if (McuUtility_strncmp((char*)cmd, "rtc set date ", sizeof("rtc set date ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("rtc set date ")-1;
    return DateCmd(p, io);
  } else if (McuUtility_strncmp((char*)cmd, "rtc set time ", sizeof("rtc set time ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("rtc set time ")-1;
    return TimeCmd(p, io);
  }
  return ERR_OK;
}
#pragma endregion ShellCommands






void McuPCF85263A_Init(void) {
}
