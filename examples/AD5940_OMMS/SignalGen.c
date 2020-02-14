#include "SignalGen.h"
//#include "stdio.h"
#include "string.h"
#include "ad5940.h"
#include "ADuCM3029.h"

//meld src/boot/ksdk1.1.0/devAD9837.c ~/Desktop/ad5940-examples/examples/AD5940_Impedance/SignalGen.c
static uint16_t configReg;
//static uint16_t _FSYNCPin;
//static uint32_t CSPin = 0x00000000F;

// reset the AD part. This will disable all function generation and set the
//  output to approximately mid-level, constant voltage. Since we're resetting,
//  we can also forego worrying about maintaining the state of the other bits
//  in the config register.
void signalGenReset()
{
  uint32_t defaultFreq = signalGenFreqCalc(100.0);
	signalGenSetFreqAdjustMode(FULL);
  //signalGenAdjustFreq(FREQ0, FULL, defaultFreq);
  //signalGenAdjustFreq(FREQ1, FULL, defaultFreq);
	signalGenSetFreqAdjustMode(FULL);
  signalGenAdjustFreq(FREQ0, defaultFreq);
  signalGenSetFreqAdjustMode(FULL);
  signalGenAdjustFreq(FREQ1, defaultFreq);
  signalGenAdjustPhaseShift(PHASE0, 0x0000);
  signalGenAdjustPhaseShift(PHASE1, 0x0000);
  signalGenSPIWrite(0x0100);
  signalGenSPIWrite(0x0000);
}

// Set the mode of the part. The mode (trinagle, sine, or square) is set by
//  three bits in the status register: D5 (OPBITEN), D3 (DIV2), and D1 (MODE).
//  Here's a nice truth table for those settings:
//  D5 D1 D3
//  0  0  x   Sine wave output
//  0  1  x   Triangle wave output
//  1  0  0   Square wave @ 1/2 frequency
//  1  0  1   Square wave @ frequency
//  1  1  x   Not allowed
void signalGenSetMode(MODE newMode)
{
  // We want to adjust the three bits in the config register that we're
  //  interested in without screwing up anything else. Unfortunately, this
  //  part is write-only, so we need to maintain a local shadow, adjust that,
  //  then write it.
  configReg &= ~0x002A; // Clear D5, D3, and D1.
  // This switch statement sets the appropriate bit in the config register.
  switch(newMode)
  {
    case TRIANGLE:
      configReg |= 0x0002;
    break;
    case SQUARE_2:
      configReg |=0x0020;
    break;
    case SQUARE:
      configReg |=0x0028;
    break;
    case SINE:
      configReg |=0x0000;
    break;
  }
  signalGenSPIWrite(configReg); // Now write our shadow copy to the part. 
}

// The AD9837 has two frequency registers that can be independently adjusted.
//  This allows us to fiddle with the value in one without affecting the output
//  of the device. The register used for calculating the output is selected by
//  toggling bit 11 of the config register.
void signalGenSelectFreqReg(FREQREG reg)
{
  // For register FREQ0, we want to clear bit 11.
  if (reg == FREQ0) configReg &= ~0x0800;
  // Otherwise, set bit 11.
  else              configReg |= 0x0800;
  signalGenSPIWrite(configReg); 
}

// Similarly, there are two phase registers, selected by bit 10 of the config
//  register.
void signalGenSelectPhaseReg(PHASEREG reg)
{
  if (reg == PHASE0) configReg &= ~0x0400;
  else               configReg |= 0x0400;
  signalGenSPIWrite(configReg); 
}

// The frequency registers are 28 bits in size (combining the lower 14 bits of
//  two 16 bit writes; the upper 2 bits are the register address to write).
//  Bits 13 and 12 of the config register select how these writes are handled:
//  13 12
//  0  0   Any write to a frequency register is treated as a write to the lower
//          14 bits; this allows for fast fine adjustment.
//  0  1   Writes are send to upper 14 bits, allowing for fast coarse adjust.
//  1  x   First write of a pair goes to LSBs, second to MSBs. Note that the
//          user must, in this case, be certain to write in pairs, to avoid
//          unexpected results!
void signalGenSetFreqAdjustMode(FREQADJUSTMODE newMode)
{
  // Start by clearing the bits in question.
  configReg &= ~0x3000;
  // Now, adjust the bits to match the truth table above.
  switch(newMode)
  {
    case COARSE:  // D13:12 = 01
      configReg |= 0x1000;
    break;
    case FINE:    // D13:12 = 00
    break;
    case FULL:    // D13:12 = 1x (we use 10)
      configReg |= 0x2000;
    break;
  }
  signalGenSPIWrite(configReg); 
}

// The phase shift value is 12 bits long; it gets routed to the proper phase
//  register based on the value of the 3 MSBs (4th MSB is ignored).
void signalGenAdjustPhaseShift(PHASEREG reg, uint16_t newPhase)
{
  // First, let's blank the top four bits. Just because it's the right thing
  //  to do, you know?
  newPhase &= ~0xF000;
  // Now, we need to set the top three bits to properly route the data.
  //  D15:D13 = 110 for PHASE0...
  if (reg == PHASE0) newPhase |= 0xC000;
  // ... and D15:D13 = 111 for PHASE1.
  else               newPhase |= 0xE000;
  signalGenSPIWrite(newPhase);
}

void signalGenSPIWrite(uint16_t RegData) {
	 uint8_t SendBuffer[2];
   uint8_t RecvBuffer[2];

//	 AD5940_CsClr();
//	 //pADI_GPIO1->CLR = (1<<12); /* P1_12 - GPIO28 - IO28 - Arduino DIO High 10 */
//   SendBuffer[0] = (RegData>>8) & 0xff;
//	 AD5940_ReadWriteNBytes(SendBuffer,RecvBuffer,1);
//	
//   SendBuffer[0] = RegData & 0xff;
//   AD5940_ReadWriteNBytes(SendBuffer,RecvBuffer,1);
//	 AD5940_CsSet();
//	 //pADI_GPIO1->SET = (1<<12); /* P1_12 - GPIO28 - IO28 - Arduino DIO High 10 */
	
	 AD5940_SPI1CsClr();
	 SendBuffer[0] = (RegData>>8) & 0xff;
	 AD5940_SPI1ReadWriteNBytes(SendBuffer,RecvBuffer,1);
	
   SendBuffer[0] = RegData & 0xff;
   AD5940_SPI1ReadWriteNBytes(SendBuffer,RecvBuffer,1);
	 AD5940_SPI1CsSet();
}

//void signalGenAdjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint32_t newFreq)
void signalGenAdjustFreq(FREQREG reg, uint32_t newFreq)
{
  //signalGenSetFreqAdjustMode(mode);
  // Now, we can just call the normal 32-bit write.
  //adjustFreq(reg, newFreq);
	
	// We need to split the 32-bit input into two 16-bit values, blank the top
  //  two bits of those values, and set the top two bits according to the
  //  value of reg.
  // Start by acquiring the low 16-bits...
  uint16_t temp = (uint16_t)newFreq;
  // ...and blanking the first two bits.
  temp &= ~0xC000;
  // Now, set the top two bits according to the reg parameter.
  if (reg==FREQ0) temp |= 0x4000;
  else            temp |= 0x8000;
  // Now, we can write temp out to the device.
  signalGenSPIWrite(temp); 
  // Okay, that's the lower 14 bits. Now let's grab the upper 14.
  temp = (uint16_t)(newFreq>>14);
  // ...and now, we can just repeat the process.
  temp &= ~0xC000;
  // Now, set the top two bits according to the reg parameter.
  if (reg==FREQ0) temp |= 0x4000;
  else            temp |= 0x8000;
  // Now, we can write temp out to the device.
  signalGenSPIWrite(temp); 
}

//// Adjust the coarse or fine register, depending on the current mode. Note that
////  if the current adjust mode is FULL, this is going to cause undefined
////  behavior, as it will leave one transfer hanging. Maybe that means only
////  half the register gets loaded? Maybe nothing happens until another write
////  to that register? Either way, it's not going to be good.
//void signalGenAdjustFreq(FREQREG reg, uint16_t newFreq)
//{
//  // We need to blank the first two bits...
//  newFreq &= ~0xC000;
//  // Now, set the top two bits according to the reg parameter.
//  if (reg==FREQ0) newFreq |= 0x4000;
//  else            newFreq |= 0x8000;
//  // Now, we can write newFreq out to the device.
//  signalGenSPIWrite(reg, newFreq);
//}

// Helper function, used to calculate the integer value to be written to a
//  freq register for a desired output frequency.
// The output frequency is fclk/2^28 * FREQREG. For us, fclk is 16MHz. We can
//  save processor time by specifying a constant for fclk/2^28- .0596. That is,
//  in Hz, the smallest step size for adjusting the output frequency.
uint32_t signalGenFreqCalc(float desiredFrequency)
{
  return (uint32_t) (desiredFrequency/.0596);
}
