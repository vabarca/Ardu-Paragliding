/**
 * @file self_pwr_off.h
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Self power off functionality. Requires external circuit
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __SELF_PWR_OFF_H
#define __SELF_PWR_OFF_H

#include <stdlib.h>

/**
 * @brief Initializes self power off module
 *
 * @param pin self power off pin
 */
void self_pwr_off_setup(uint8_t pin);

/**
 * @brief Updates self power engine
 *
 */
void self_pwr_off_update(void);

#endif //__SELF_PWR_OFF_H