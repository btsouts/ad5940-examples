#ifndef SIGNALGEN_H
#define SIGNALGEN_H

#include "stdio.h"
#include <stdint.h>
//#include "math.h"

typedef enum {TRIANGLE, SINE, SQUARE, SQUARE_2} MODE ;
typedef enum {FREQ0, FREQ1} FREQREG;
typedef enum {PHASE0, PHASE1} PHASEREG;
typedef enum {FULL, COARSE, FINE} FREQADJUSTMODE;

//MiniGen();
//MiniGen(int16_t FSYNCPin);
void signalGenConfigSPIPeripheral(void);
void signalGenSPIWrite(uint16_t RegData);

void signalGenReset(void);
void signalGenSetMode(MODE newMode);
void signalGenSelectFreqReg(FREQREG reg);
void signalGenSelectPhaseReg(PHASEREG reg);
void signalGenSetFreqAdjustMode(FREQADJUSTMODE newMode);
void signalGenAdjustPhaseShift(PHASEREG reg, uint16_t newPhase);
//void signalGenAdjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint32_t newFreq);
void signalGenAdjustFreq(FREQREG reg, uint32_t newFreq);
//void adjustFreq(FREQREG reg, uint32_t newFreq);
//void adjustFreq(FREQREG reg, uint16_t newFreq);
uint32_t signalGenFreqCalc(float desiredFrequency);

#endif
