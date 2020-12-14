/**
 * @file xiao_sinewave.ino
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Customize vario audio sound based on LK8EX1 decoded data read from serial port
 * @version 0.1
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "lk8ex_decoder.h"
#include "wave_gen.h"

int ledPin = 13; // LED pin 13

void setup()
{
    pinMode(ledPin, OUTPUT); // sets the digital pin as output

    Serial.begin(115200);    // connect to the serial port
    Serial.println("Vario custom sound");

    wave_gen_setup();
    lk8ex_decoder_setup(Serial);
}

void loop()
{
    if(lk8ex_decoder_update())
    {
        int32_t vario = lk8ex_decoder_get_vario();

        uint32_t freq = 1000; //map((uint32_t)vario, fromLow, fromHigh, toLow, toHigh);
        wave_gen_set_frequency((double)freq);
    }
}
