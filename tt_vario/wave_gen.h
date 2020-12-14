/**
 * @file wave_gen.h
 * @author vabarcaf (viabfer@gmail.com)
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
 * @brief Sets up the wave gen functionality
 */
void wave_gen_setup(void);

/**
 * @brief Sets frequency
 *
 * @param freq frequency value (0 < freq < MAX_FREQ)
 */
void wave_gen_set_frequency(double freq);

#endif //__WAVE_GEN_H