/**
 * @file dds.h
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Generates sin/square signal given specific frequency
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __DDS_H
#define __DDS_H

#include <stdlib.h>

typedef enum wave_type_e
{
    WAVE_TYPE_SINE,
    WAVE_TYPE_SQUARE
}wave_type_t;

/**
 * @brief Sets up the dds functionality
 *
 * @param pin DDS output pin
 */
void dds_setup(uint8_t pin);

/**
 * @brief Sets frequency
 *
 * @param freq frequency value (0 < freq < MAX_FREQ)
 * @param type wave type
 */
void dds_set_frequency(wave_type_t type, double freq);

/**
 * @brief Timer2 Interrupt Service at 31372,550 KHz = 32uSec
 this is the timebase REFCLOCK for the DDS generator
 FOUT = (M (REFCLK)) / (2 exp 32)
 runtime : 8 microseconds ( inclusive push and pop)
 *
 */
void dds_isr_tim2(void);

#endif //__DDS_H