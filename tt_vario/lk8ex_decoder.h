
/**
 * @file decoder.h
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Decodes LK8EX1 protocol for getting vario info
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __DECODER_H
#define __DECODER_H

#include <stdlib.h>

/**
 * @brief  Initializes decoder module
 *
 * @param ser Hardware serial peripheral pointer
 */
void lk8ex_decoder_setup(HardwareSerial &ser);

/**
 * @brief Vario data getter
 *
 * @return int32_t vario data (cm/s)
 */
int32_t lk8ex_decoder_get_vario(void);

/**
 * @brief Decodes incoming serial data
 *
 * @return true if new data available
 * @return false if no data available
 */
bool lk8ex_decoder_update(void);

#endif //__DECODER_H