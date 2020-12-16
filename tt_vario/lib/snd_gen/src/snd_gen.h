/**
 * @file wave_gen.h
 * @author vabarca (viabfer@gmail.com)
 * @brief Generates sin/square signal given specific frequency
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __WAVE_GEN_H
#define __WAVE_GEN_H

#include <stdlib.h>

/**
 * @brief Sets up the sound generator functionality
 */
void snd_gen_setup(void);

/**
 * @brief Sets sound frequency
 *
 * @param[in] freq frequency value (0 < freq < MAX_FREQ)
 */
void snd_gen_set_frequency(double freq);

#endif //__WAVE_GEN_H