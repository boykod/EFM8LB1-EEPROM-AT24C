//   Timer1 - SMBus clock source
//   Timer3 - SMBus SCL low timeout detection
//   P1.2   - SMBus SDA
//   P1.3   - SMBus SCL

#include <SI_EFM8LB1_Register_Enums.h>
#include "InitDevice.h"
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Pin Definitions
//-----------------------------------------------------------------------------
SI_SBIT(LED1, SFR_P1, 4);                 	// DS5 P1.0 LED

#define LED_ON                       0
#define LED_OFF                      1

SI_SBIT(SDA, SFR_P0, 1);                  	// SMBus on P0.1
SI_SBIT(SCL, SFR_P0, 2);                  	// and P0.2

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
uint8_t i;                             		// Temporary counter variable

uint8_t* pSMB_DATA_IN;                 		// Global pointer for SMBus data
                                       	   	// All receive data is written here

uint8_t SMB_SINGLEBYTE_OUT;            		// Global holder for single byte writes.

uint8_t* pSMB_DATA_OUT;                		// Global pointer for SMBus data.
                                       	   	// All transmit data is read from here

uint8_t SMB_DATA_LEN;

uint8_t WORD_ADDR_1;
uint8_t WORD_ADDR_2;

uint8_t TARGET;                        		// Target SMBus slave address

volatile bool SMB_BUSY = 0;            		// Software flag to indicate when the
                                       	   	// EEPROM_ByteRead() or
                                       	    // EEPROM_ByteWrite()
                                       	    // functions have claimed the SMBus

volatile bool SMB_RW;                  		// Software flag to indicate the
                                       	    // direction of the current transfer

volatile bool SMB_SENDWORDADDR;        		// When set, this flag causes the ISR
                                       	    // to send the 8-bit <WORD_ADDR>
                                       	    // after sending the slave address.


volatile bool SMB_RANDOMREAD;          		// When set, this flag causes the ISR
                                       	    // to send a START signal after sending
                                       	    // the word address.

volatile bool SMB_ACKPOLL;             		// When set, this flag causes the ISR
                                       	    // to send a repeated START until the
                                       	    // slave has acknowledged its address

//-----------------------------------------------------------------------------
// EEPROM_CheckSDA ()
//-----------------------------------------------------------------------------
void EEPROM_CheckSDA () {
	while(!SDA) {
		XBR2 = 0x40;                     	// Enable Crossbar
		SCL = 0;                         	// Drive the clock low
		for(i = 0; i < 255; i++);        	// Hold the clock low
		SCL = 1;                         	// Release the clock
		while(!SCL);                     	// Wait for open-drain
		// clock output to rise
		for(i = 0; i < 10; i++);         	// Hold the clock high
		XBR2 = 0x00;                     	// Disable Crossbar
	}
}

//-----------------------------------------------------------------------------
// EEPROM_ByteWrite ()
//-----------------------------------------------------------------------------
void EEPROM_ByteWrite(uint8_t addr_1, uint8_t addr_2, uint8_t dat) {
   while (SMB_BUSY);                   		// Wait for SMBus to be free.
   SMB_BUSY = 1;                       		// Claim SMBus (set to busy)

   // Set SMBus ISR parameters
   TARGET = EEPROM_ADDR;               		// Set target slave address
   SMB_RW = WRITE;                     		// Mark next transfer as a write
   SMB_SENDWORDADDR = 1;               		// Send Word Address after Slave Address
   SMB_RANDOMREAD = 0;                 		// Do not send a START signal after

   SMB_ACKPOLL = 1;                    		// Enable Acknowledge Polling

   // Specify the Outgoing Data
   WORD_ADDR_1 = addr_1;                    // Set the target address in the
                                       	    // EEPROM's internal memory space

   WORD_ADDR_2 = addr_2;

   SMB_SINGLEBYTE_OUT = dat;

   // The outgoing data pointer points to the <dat> variable
   pSMB_DATA_OUT = &SMB_SINGLEBYTE_OUT;

   SMB_DATA_LEN = 1;

   // Initiate SMBus Transfer
   SMB0CN0_STA = 1;

}

//-----------------------------------------------------------------------------
// EEPROM_ByteRead ()
//-----------------------------------------------------------------------------
uint8_t EEPROM_ByteRead(uint8_t addr_1, uint8_t addr_2) {
   uint8_t retval;               	   		// Holds the return value

   while (SMB_BUSY);                   		// Wait for SMBus to be free.
   SMB_BUSY = 1;                       		// Claim SMBus (set to busy)

   // Set SMBus ISR parameters
   TARGET = EEPROM_ADDR;               		// Set target slave address
   SMB_RW = WRITE;

   SMB_SENDWORDADDR = 1;               		// Send Word Address after Slave Address
   SMB_RANDOMREAD = 1;                 		// Send a START after the word address
   SMB_ACKPOLL = 1;                    		// Enable Acknowledge Polling

   // Specify the Incoming Data
   WORD_ADDR_1 = addr_1;

   WORD_ADDR_2 = addr_2;

   pSMB_DATA_IN = &retval;

   SMB_DATA_LEN = 1;

   // Initiate SMBus Transfer
   SMB0CN0_STA = 1;
   while(SMB_BUSY);                    		// Wait until data is read

   return retval;
}
