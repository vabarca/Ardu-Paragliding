/**
 * @file dds.cpp
 * @author vabarcaf (viabfer@gmail.com)
 * @brief Generates sin/square signal given specific frequency
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

/* Includes --------------------------------------------------------------------------------------*/

#include <Arduino.h>
#include <avr/pgmspace.h>

#include "utils.h"
#include "dds.h"

/* Defines ---------------------------------------------------------------------------------------*/

// const double refclk=15686.274;  // = 8MHz / 510
#define REF_CLK     (double)15527.9503 // measured
#define MAX_FREQ    REF_CLK/2.0
#define POW2_32     0xFFFFFFFF

/* Typedefs --------------------------------------------------------------------------------------*/

/**
 * @brief variables used inside interrupt service declared as voilatile
 *
 */
typedef struct dds_lv_s
{
    byte icnt;            // var inside interrupt
    unsigned long phaccu; // pahse accumulator
    unsigned long tword;  // dds tuning word m
    uint8_t* wave;  // dds tuning word m
    uint8_t pin_bit;
    uint8_t *port_reg;
}dds_lv_t;

/* Private func declaration ----------------------------------------------------------------------*/

static void Setup_timer2(void);

/* variables -------------------------------------------------------------------------------------*/

/**
 * @brief table of 256 sine values / one sine period / stored in flash memory
 *
 */
PROGMEM const uint8_t sine256[] =
{
    254, 254, 254, 254, 254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246,
    245, 244, 242, 241, 239, 238, 236, 235, 233, 231, 229, 228, 226, 224, 221, 219,
    217, 215, 213, 210, 208, 205, 203, 200, 198, 195, 193, 190, 187, 184, 182, 179,
    176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 136, 133, 130,
    127, 124, 121, 118, 115, 111, 108, 105, 102, 99,  96,  93,  90,  87,  84,  81,
    78,  75,  72,  70,  67,  64,  61,  59,  56,  54,  51,  49,  46,  44,  41,  39,
    37,  35,  33,  30,  28,  26,  25,  23,  21,  19,  18,  16,  15,  13,  12,  10,
    9,   8,   7,   6,   5,   4,   3,   3,   2,   1,   1,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   1,   1,   2,   3,   3,   4,   5,   6,   7,   8,
    9,   10,  12,  13,  15,  16,  18,  19,  21,  23,  25,  26,  28,  30,  33,  35,
    37,  39,  41,  44,  46,  49,  51,  54,  56,  59,  61,  64,  67,  70,  72,  75,
    78,  81,  84,  87,  90,  93,  96,  99,  102, 105, 108, 111, 114, 118, 121, 124,
    127, 127, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170,
    173, 176, 179, 182, 184, 187, 190, 193, 195, 198, 200, 203, 205, 208, 210, 213,
    215, 217, 219, 221, 224, 226, 228, 229, 231, 233, 235, 236, 238, 239, 241, 242,
    244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 254, 254
};

/**
 * @brief table of 256 sine values / one sine period / stored in flash memory
 *
 */
PROGMEM const uint8_t square256[] =
{
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

/**
 * @brief Local module variable
 *
 */
volatile dds_lv_t dds_lv =
{
    .icnt = 0,
    .phaccu = 0,
    .tword =  POW2_32 * 1000.0 / REF_CLK, // calulate DDS tuning word for 1000.0 Hz
    .wave = sine256,
    .pin_bit = 0,
    .port_reg = NULL,
};

/* Private func definitions ----------------------------------------------------------------------*/

/**
 * @brief Configures timer 2. Set prscaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
 */
static void Setup_timer2(void)
{
    // Timer2 Clock Prescaler to : 1
    sbi(TCCR2B, CS20);
    cbi(TCCR2B, CS21);
    cbi(TCCR2B, CS22);

    // Timer2 PWM Mode set to Phase Correct PWM
    cbi(TCCR2A, COM2A0); // clear Compare Match
    sbi(TCCR2A, COM2A1);

    sbi(TCCR2A, WGM20); // Mode 1  / Phase Correct PWM
    cbi(TCCR2A, WGM21);
    cbi(TCCR2B, WGM22);
}

/* Public func definitions -----------------------------------------------------------------------*/

/**
 * @brief Sets up the dds functionality
 *
 * @param pin DDS output pin
 */
void dds_setup(uint8_t pin)
{
    pinMode(pin, OUTPUT); // pin11= PWM  output / frequency output

    dds_lv.pin_bit = digitalPinToPort(pin);
    dds_lv.port_reg = portOutputRegister(digitalPinToBitMask(pin));

    Setup_timer2();

    // disable interrupts to avoid timing distortion
    cbi(TIMSK0, TOIE0); // disable Timer0 !!! delay() is now not available
    sbi(TIMSK2, TOIE2); // enable Timer2 Interrupt
}

/**
 * @brief Sets frequency
 *
 * @param freq frequency value (0 < freq < MAX_FREQ)
 * @param type wave type
 */
void dds_set_frequency(wave_type_t type, uint32_t freq)
{
    uint8_t* wave = sine256;
    double dfreq = (double)freq;

    switch (type)
    {
        case WAVE_TYPE_SQUARE:
            wave = square256;
        break;

        case WAVE_TYPE_SINE:
        default:
            wave = sine256;
        break;
    }

    dfreq = dfreq > MAX_FREQ? MAX_FREQ : dfreq;
    dfreq = dfreq < 0.0 ? 0.0 : dfreq;

    cbi(TIMSK2, TOIE2);                  // disble Timer2 Interrupt
    dds_lv.tword = POW2_32 * dfreq / REF_CLK; // calulate DDS new tuning word
    dds_lv.wave = wave;
    sbi(TIMSK2, TOIE2);                  // enable Timer2 Interrupt
}

/**
 * @brief Timer2 Interrupt Service at 31372,550 KHz = 32uSec
 this is the timebase REFCLOCK for the DDS generator
 FOUT = (M (REFCLK)) / (2 exp 32)
 runtime : 8 microseconds ( inclusive push and pop)
 *
 */
void dds_isr_tim2(void)
{
    dds_lv.phaccu = dds_lv.phaccu + dds_lv.tword; // soft DDS, phase accu with 32 bits
    dds_lv.icnt = dds_lv.phaccu >> 24;     // use upper 8 bits for phase accu as frequency information
    // read value fron ROM sine table and send to PWM DAC

    OCR2A = pgm_read_byte_near(dds_lv.wave + dds_lv.icnt);

    tbi(dds_lv.port_reg, dds_lv.pin_bit); // toggle dds output pin
}

