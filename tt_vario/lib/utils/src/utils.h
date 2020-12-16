

#ifndef __UTILS_H
#define __UTILS_H

#include <Arduino.h>

#ifndef bit
#define bit(a)  (1UL << (a))
#endif

#define cbi(sfr, b) (_SFR_BYTE(sfr) &= ~bit(b))
#define sbi(sfr, b) (_SFR_BYTE(sfr) |= bit(b))
#define tbi(sfr, b) (_SFR_BYTE(sfr) ^= bit(b))

#endif //__UTILS_H
