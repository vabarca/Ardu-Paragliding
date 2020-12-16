/**
 * @file wave_ten.cpp
 * @author vabarca (viabfer@gmail.com)
 * @brief Generates signal given specific frequency
 * @version 0.1
 * @date 2020-09-28
 *
 * @copyright Copyright (c) 2020
 *
 */

/* Includes --------------------------------------------------------------------------------------*/

#include <Arduino.h>
#include <avr/pgmspace.h>

#include "snd_gen.h"

/* Defines ---------------------------------------------------------------------------------------*/

#define NUM_SAMPLES 128

/* Typedefs --------------------------------------------------------------------------------------*/

/**
 * @brief DMAC descriptor structure
 *
 */
typedef struct
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

/**
 * @brief variables used inside interrupt service declared as voilatile
 *
 */
typedef struct wave_gen_lv_s
{
    volatile uint16_t sintable1[NUM_SAMPLES];
    volatile uint32_t freq;

    // Write-back DMAC descriptors
    volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));
    // DMAC channel descriptors
    volatile dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
    // Place holder descriptor
    dmacdescriptor descriptor __attribute__ ((aligned (16)));

}wave_gen_lv_t;

/* Private func declaration ----------------------------------------------------------------------*/


/* variables -------------------------------------------------------------------------------------*/

/**
 * @brief Local module variable
 *
 */
wave_gen_lv_t wave_gen_lv = {0};

/* Private func definitions ----------------------------------------------------------------------*/



/* Public func definitions -----------------------------------------------------------------------*/

/**
 * @brief Sets up the wave gen functionality
 */
void snd_gen_setup(void)
{
  /*
// Calculate the sine table with 32 entries
    for (uint16_t i = 0; i < NUM_SAMPLES; i++)
    {
        wave_gen_lv.sintable1[i] = (uint16_t)((sinf(2 * PI * (float)i / NUM_SAMPLES) * 511) + 512);
    }

    // Set the DAC's resolution to 10-bits
    analogWriteResolution(10);
    // Initialise the DAC
    analogWrite(A0, 0);

    //===============
    // Initialize DMA
    //===============

    DMAC->BASEADDR.reg = (uint32_t)wave_gen_lv.descriptor_section;                // Set the descriptor section base address
    DMAC->WRBADDR.reg = (uint32_t)wave_gen_lv.wrb;                                // Set the write-back descriptor base adddress
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);      // Enable the DMAC and priority levels

    DMAC->CHID.reg = DMAC_CHID_ID(0);                                 // Select DMAC channel 0
    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                      // Enable suspend channel interrupts on each channel
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) |                         // Set DMAC priority to level 0 (lowest)
                        DMAC_CHCTRLB_TRIGSRC(TCC0_DMAC_ID_OVF) |      // Trigger on timer TCC0 overflow
                        DMAC_CHCTRLB_TRIGACT_BEAT;                    // Trigger every beat
    wave_gen_lv.descriptor.descaddr = (uint32_t)&wave_gen_lv.descriptor_section[0];                  // Set up a circular descriptor
    wave_gen_lv.descriptor.srcaddr = (uint32_t)&(wave_gen_lv.sintable1[0]) + NUM_SAMPLES * sizeof(uint16_t);    // Read the current value in the sine table
    wave_gen_lv.descriptor.dstaddr = (uint32_t)&DAC->DATA.reg;                           // Copy it into the DAC data register
    wave_gen_lv.descriptor.btcnt = NUM_SAMPLES;                                                   // This takes the number of sine table entries = 256 beats
    wave_gen_lv.descriptor.btctrl = DMAC_BTCTRL_BLOCKACT_SUSPEND |                // Suspend DMAC channel at end of block transfer
                        DMAC_BTCTRL_BEATSIZE_HWORD |                  // Set the beat size to 16-bits (Half Word)
                        DMAC_BTCTRL_SRCINC |                          // Increment the source address every beat
                        DMAC_BTCTRL_VALID;                            // Flag the descriptor as valid
    memcpy((void*)&(wave_gen_lv.descriptor_section[0]), &wave_gen_lv.descriptor, sizeof(dmacdescriptor));  // Copy to the channel 0 descriptor

    NVIC_SetPriority(DMAC_IRQn, 0);           // Set the Nested Vector Interrupt Controller (NVIC) priority for the DMAC to 0 (highest)
    NVIC_EnableIRQ(DMAC_IRQn);                // Connect the DMAC to the Nested Vector Interrupt Controller (NVIC)

    //=============================
    // Initialize clocks and timer
    //=============================

    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                        GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                        GCLK_GENCTRL_GENEN |         // Enable GCLK4
                        GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                        GCLK_GENCTRL_ID(4);          // Select GCLK4
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                        GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                        GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
    while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NFRQ;          // Setup TCC0 in Normal Frequency (NFRQ) mode
    while (TCC0->SYNCBUSY.bit.WAVE);                 // Wait for synchronization

    wave_gen_lv.freq = 800;                                 // 800hz

    TCC0->PER.reg = 48000000/(wave_gen_lv.freq *NUM_SAMPLES) - 1;

    while(TCC0->SYNCBUSY.bit.PER);                   // Wait for synchronization

    TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1;      // Set the TCC0 prescaler to 1 giving 48MHz (20.83ns) timer tick
    TCC0->CTRLA.bit.ENABLE = 1;                      // Enable the TCC0 output
    while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization

    DMAC->CHID.reg = DMAC_CHID_ID(0);                // Select DMAC channel
    DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;        // Enable DMAC channel
    */
}

/**
 * @brief Sets frequency
 *
 * @param freq frequency value (0 < freq < MAX_FREQ)
 */
void snd_gen_set_frequency(double freq)
{

}

/**
 * @brief DMAC interrupt handler
 *
 */
void DMAC_Handler()
{
  /*
    static uint16_t period = 2999;                          // Corresponds to 500 Hz, gets overwritten

    DMAC->CHID.reg = DMAC_CHID_ID(DMAC->INTPEND.bit.ID);    // Find the DMAC channel generating the interrupt
    wave_gen_lv.descriptor_section[0].btctrl &= ~DMAC_BTCTRL_VALID;     // Disable the descriptor

    period = 48000000/(wave_gen_lv.freq * NUM_SAMPLES) -1;                     // then calc out the best fit period
    TCC0->PERB.reg = period;                                // 2kHz sine wave, 128 samples: 48MHz / (2000 * 128) - 1
    while(TCC0->SYNCBUSY.bit.PERB); // Wait for synchronization

    wave_gen_lv.descriptor_section[0].btctrl |= DMAC_BTCTRL_VALID;      // Enable the descriptor
    DMAC->CHCTRLB.reg |= DMAC_CHCTRLB_CMD_RESUME;           // Resume the DMAC channel
    DMAC->CHINTFLAG.bit.SUSP = 1;                           // Clear the DMAC channel suspend (SUSP) interrupt flag
    */
}
