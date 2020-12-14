/**
 * @file self_pwr_off.cpp
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Self power off functionality. Requires external circuit
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <Arduino.h>
#include <avr/io.h>

#include "utils.h"
#include "self_pwr_off.h"

typedef struct self_pwr_off_lv_s
{
    uint8_t pin_bit;
    volatile uint8_t *port_reg;
}self_pwr_off_lv_t;

self_pwr_off_lv_t self_pwr_off_lv =
{
    .pin_bit = 0,
    .port_reg = NULL,
};

/**
 * @brief Initializes self power off module
 *
 * @param pin self power off pin
 */
void self_pwr_off_setup(uint8_t pin)
{
    pinMode(pin, OUTPUT);

    self_pwr_off_lv.pin_bit = digitalPinToPort(pin);
    self_pwr_off_lv.port_reg = portOutputRegister(digitalPinToBitMask(pin));

    sbi(self_pwr_off_lv.port_reg, self_pwr_off_lv.pin_bit); // put pin port to HIGH
}

/**
 * @brief Updates self power engine
 *
 */
void self_pwr_off_update(void)
{
    static uint32_t cnt = 0;



}
