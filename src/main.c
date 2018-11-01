//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8LB1_Register_Enums.h>
#include "InitDevice.h"
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Global Variable
//-----------------------------------------------------------------------------
uint8_t limit = 0;
uint8_t n, d, k;
uint32_t eepromArray[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
uint32_t rearEEPROM[6];

void readArr (unsigned char start_address);
void writeArr (unsigned char start_address);

void SiLabs_Startup (void) {

}
 
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main(void) {
//   uint32_t f_byte, s_byte, t_byte, fo_byte, fi_byte, si_byte;
   uint8_t error_flag = 0;
   uint8_t startAddress = 0;
   enter_BusFreeMode_from_RESET();

   EEPROM_CheckSDA ();

   enter_DefaultMode_from_BusFreeMode();

   IE_EA = 1;                          // Global interrupt enable
///----------------------------------------------------------------------------

   // Write the value 0x05 to location 0x003 in the EEPROM
//   EEPROM_ByteWrite(0x00, 0x03, 0x05);
   // Read the value from location 0x003 in the EEPROM
//   f_byte = EEPROM_ByteRead(0x00, 0x03);
//   if(f_byte != 0x05) {
//	   error_flag++;
//   }

//   f_byte = EEPROM_ByteRead(0x00, 0x02);
//   t_byte = EEPROM_ByteRead(0x00, 0x05);
//   fi_byte = EEPROM_ByteRead(0x00, 0x01);
//   si_byte = EEPROM_ByteRead(0x00, 0x00);

//   for(i = 0; i < 6; i++) {
//	   EEPROM_ByteWrite(0x00, i, eepromArray[i]);
//	   for(d = 0; d < 150; d++);
//   }

   writeArr(0x05);
   readArr(0x05);
///----------------------------------------------------------------------------

   while(1);
}

// Read 6 byte
void readArr (unsigned char start_address) {
	limit = start_address + 6;

	for (n = start_address; n < limit; n++) {

		if(k < 6) {
			rearEEPROM[k] = EEPROM_ByteRead(0x00, n);
			k++;
		} else {
			k = 0;
		}

		for(d = 0; d < 150; d++);
	}
}

// Write 6 byte
void writeArr (unsigned char start_address) {
	limit = start_address + 6;

	for(n = start_address; n < limit; n++) {

		if(k < 6) {
			EEPROM_ByteWrite(0x00, n, eepromArray[k]);
			k++;
		} else {
			k = 0;
		}

		for(d = 0; d < 150; d++);
	}
}
