/**
 * @file decoder.cpp
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Decodes LK8EX1 protocol for getting vario info
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <Arduino.h>

#include "utils.h"
#include "decoder.h"

typedef enum vario_sm_e
{
    SM_START = 0,
    SM_L,
    SM_K,
    SM_8,
    SM_E,
    SM_X,
    SM_1,
    SM_COMMA,
    SM_DATA_PRESS,
    SM_DATA_ALT,
    SM_DATA_VARIO,
    SM_DATA_TEMP,
    SM_DATA_BATT,
    SM_DATA_CRC
}vario_sm_t;

typedef struct data_s
{
    int32_t vario;
}data_t;

typedef struct decoder_lv_s
{
    vario_sm_t sm;
    HardwareSerial *ser;
    data_t data;
}decoder_lv_t;

decoder_lv_t decoder_lv =
{
    .sm = SM_START,
    .ser = NULL,
    .data =
    {
        .vario = 0,
    },
};

/**
 * @brief  Initializes decoder module
 *
 * @param ser Hardware serial peripheral pointer
 */
void decoder_setup(HardwareSerial &ser)
{
    decoder_lv.ser = &ser;
}

/**
 * @brief Vario data getter
 *
 * @return int32_t vario data (cm/s)
 */
int32_t decoder_get_vario(void)
{
    return decoder_lv.data.vario;
}

/**
 * @brief Decodes incoming serial data
 *
 * @return true if new data available
 * @return false if no data available
 */
bool decoder_update(void)
{
    static String str_data;
    bool ret = false;

    if(decoder_lv.ser)
    {
        if (decoder_lv.ser->available())
        {
            char data =  (char)decoder_lv.ser->read();
            switch (decoder_lv.sm)
            {
                case SM_START:
                    decoder_lv.sm = '$' == data? SM_L : SM_START;
                break;

                case SM_L:
                    decoder_lv.sm = 'L' == data? SM_K : SM_START;
                break;

                case SM_K:
                    decoder_lv.sm = 'K' == data? SM_8 : SM_START;
                break;

                case SM_8:
                    decoder_lv.sm = '8' == data? SM_E : SM_START;
                break;

                case SM_E:
                    decoder_lv.sm = 'E' == data? SM_X : SM_START;
                break;

                case SM_X:
                    decoder_lv.sm = 'X' == data? SM_1 : SM_START;
                break;

                case SM_1:
                    decoder_lv.sm = '1' == data? SM_COMMA : SM_START;
                break;

                case SM_COMMA:
                    decoder_lv.sm = ',' == data? SM_DATA_PRESS : SM_START;
                break;

                case SM_DATA_PRESS:
                    if(data == ',')
                    {
                        decoder_lv.sm = SM_DATA_ALT;
                    }
                    else if (data < '0' || data > '9')
                    {
                        decoder_lv.sm = SM_START;
                    }
                    else
                    {
                        /* save pressure data */
                    }
                break;

                case SM_DATA_ALT:
                    if(data == ',')
                    {
                        decoder_lv.sm = SM_DATA_VARIO;
                        str_data = "";
                    }
                    else if (data < '0' || data > '9')
                    {
                        decoder_lv.sm = SM_START;
                    }
                    else
                    {
                        /* save altitude data */
                    }
                break;

                case SM_DATA_VARIO:
                    if(data == ',')
                    {
                        decoder_lv.sm = SM_DATA_TEMP;
                        decoder_lv.data.vario = str_data.toInt();
                        str_data = "";
                        ret = true;
                    }
                    else if (data < '-' || data > '9')
                    {
                        decoder_lv.sm = SM_START;
                    }
                    else
                    {
                        /* save vario data */
                        str_data += String(data);
                    }
                break;

                case SM_DATA_TEMP:
                    if(data == ',')
                    {
                        decoder_lv.sm = SM_DATA_BATT;
                    }
                    else if (data < '-' || data > '9')
                    {
                        decoder_lv.sm = SM_START;
                    }
                    else
                    {
                        /* save vario data */
                    }
                break;

                case SM_DATA_BATT:
                    if(data == ',')
                    {
                        decoder_lv.sm = SM_DATA_CRC;
                    }
                    else if (data < '-' || data > '9')
                    {
                        decoder_lv.sm = SM_START;
                    }
                    else
                    {
                        /* save vario data */
                    }
                break;

                case SM_DATA_CRC:
                    decoder_lv.sm = SM_START;
                break;

                default:
                break;
            }
        }
    }

    return ret;
}
