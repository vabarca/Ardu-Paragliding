

#ifndef __UTILS_H
#define __UTILS_H

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit))

#endif //__UTILS_H