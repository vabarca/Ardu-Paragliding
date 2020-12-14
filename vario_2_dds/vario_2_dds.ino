/**
 * @file app.ino
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Customize vario audio sound based on LK8EX1 decoded data read from serial port
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "dds.h"
#include "decoder.h"
#include "self_pwr_off.h"

int ledPin = 13; // LED pin 13
int pwr_off_pin = 7; // Pin 7
int dds_pin = 11; // Pin 11

void setup()
{
    pinMode(ledPin, OUTPUT); // sets the digital pin as output

    Serial.begin(115200);    // connect to the serial port
    Serial.println("Vario custom sound");

    self_pwr_off_setup(pwr_off_pin);
    dds_setup(dds_pin);
    decoder_setup(Serial);
}

void loop()
{
    while(1)
    {
        if(decoder_update())
        {
            int32_t vario = decoder_get_vario();
            wave_type_t wave_type = WAVE_TYPE_SINE;

            //negative value
            if(vario < 0)
            {
                //select square wave
                wave_type = WAVE_TYPE_SQUARE;
                //Invert value
                vario = -vario
            }

            uint32_t freq = 1000; //map((uint32_t)vario, fromLow, fromHigh, toLow, toHigh);
            dds_set_frequency(wave_type, (double)freq );

            self_pwr_off_update();
        }
    }
}

ISR(TIMER2_OVF_vect)
{
    DDS_ISR_TIM2();
}