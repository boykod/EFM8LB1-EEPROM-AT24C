#include <SI_EFM8LB1_Register_Enums.h>
#include "eeprom.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// SMBUS0_ISR
//---------------------------------------------------------------------------------------------------------------------------------------------------------
SI_INTERRUPT (SMBUS0_ISR, SMBUS0_IRQn) {
	static bool FAIL = 0;

	static uint8_t i;

	static uint8_t f_s_byte = 0;

	static bool SEND_START = 0;     		  		// Send a start

	switch (SMB0CN0 & 0xF0) {				  		// Status vector
		// Master Transmitter/Receiver: START condition transmitted.
		case SMB_MTSTA:
		SMB0DAT = TARGET;					  		// Load address of the target slave
		SMB0DAT &= 0xFE;					  		// Clear the LSB of the address for the
											  	  	// R/W bit
		SMB0DAT |= (uint8_t) SMB_RW;  		  		// Load R/W bit
		SMB0CN0_STA = 0;					  		// Manually clear START bit
		i = 0;								  		// Reset data byte counter
		break;
///--------------------------------------------------------------------------------------------------------------------------------------------------------
		// Master Transmitter: Data byte (or Slave Address) transmitted
		case SMB_MTDB:
		if (SMB0CN0_ACK) { 					  		// Slave Address or Data Byte
		                             	 	  	  	// Acknowledged?
			if (SEND_START) {
				SMB0CN0_STA = 1;
				SEND_START = 0;
				break;
			}
			if(SMB_SENDWORDADDR) {       	  		// Are we sending the word address?
//				SMB_SENDWORDADDR = 0;   	  		// Clear flag
//				SMB0DAT = WORD_ADDR_1;		  		// Send word address

				if (f_s_byte) {
					SMB_SENDWORDADDR = 0;   	  	// Clear flag
					SMB0DAT = WORD_ADDR_2;		  	// Send word address
					if (SMB_RANDOMREAD) {
						SEND_START = 1; 		  	// Send a START after the next SMB0CN_ACK cycle
						SMB_RW = READ;
					}
				} else {
					SMB0DAT = WORD_ADDR_1;		  	// Send word address
					f_s_byte++;
				}

//				if (SMB_RANDOMREAD) {
//					SEND_START = 1; 		  		// Send a START after the next SMB0CN_ACK cycle
//					SMB_RW = READ;
//				}

				break;
			}
///--------------------------------------------------------------------------------------------------------------------------------------------------------
			if (SMB_RW==WRITE) {        	  		// Is this transfer a WRITE?
				if (i < SMB_DATA_LEN) {  	  		// Is there data to send?
					// send data byte
					SMB0DAT = *pSMB_DATA_OUT;

					// increment data out pointer
					pSMB_DATA_OUT++;

					// increment number of bytes sent
					i++;
				} else {
					SMB0CN0_STO = 1;          		// Set SMB0CN_STO to terminte transfer
					SMB_BUSY = 0;			  		// Clear software busy flag
					f_s_byte = 0;
				}
			} else {}                    		  	// If this transfer is a READ,
									   	   	  	  	// then take no action. Slave
									   	      	  	// address was transmitted. A
									   	      	  	// separate 'case' is defined
									   	      	  	// for data byte recieved.
		} else {                         	  		// If slave NACK,
			if(SMB_ACKPOLL) {
				SMB0CN0_STA = 1;              		// Restart transfer
			} else {
				FAIL = 1;               	  		// Indicate failed transfer
			}                          		  		// and handle at end of ISR
		}
		break;
///--------------------------------------------------------------------------------------------------------------------------------------------------------
		// Master Receiver: byte received
		case SMB_MRDB:
		if ( i < SMB_DATA_LEN ) {			  		// Is there any data remaining?
			*pSMB_DATA_IN = SMB0DAT;   		  		// Store received byte
			pSMB_DATA_IN++;					  		// Increment data in pointer
			i++;							  		// Increment number of bytes received
			SMB0CN0_ACK = 1;				  		// Set SMB0CN_ACK bit (may be cleared later
											  	  	// in the code)
		}

		if (i == SMB_DATA_LEN) {       		  		// This is the last byte
			SMB_BUSY = 0;              		  		// Free SMBus interface
			SMB0CN0_ACK = 0;				  		// Send NACK to indicate last byte
											  	  	// of this transfer
			f_s_byte = 0;
			SMB0CN0_STO = 1;				  		// Send STOP to terminate transfer
		}

		break;

		default:
		FAIL = 1;                     		  		// Indicate failed transfer
									  	      	  	// and handle at end of ISR
		break;
	}
///--------------------------------------------------------------------------------------------------------------------------------------------------------
	if (FAIL) {                          	  		// If the transfer failed,
		SMB0CF &= ~0x80;                 	  		// Reset communication
		SMB0CF |= 0x80;
		SMB0CN0_STA = 0;
		SMB0CN0_STO = 0;
		SMB0CN0_ACK = 0;

		SMB_BUSY = 0;						  		// Free SMBus

		FAIL = 0;
	}
///--------------------------------------------------------------------------------------------------------------------------------------------------------
	SMB0CN0_SI = 0;                           		// Clear interrupt flag
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMER3_ISR
//---------------------------------------------------------------------------------------------------------------------------------------------------------
SI_INTERRUPT (TIMER3_ISR, TIMER3_IRQn) {
	SMB0CF &= ~0x80;                    	  		// Disable SMBus
	SMB0CF |= 0x80;							  		// Re-enable SMBus
	TMR3CN0 &= ~0x80;						  		// Clear Timer3 interrupt-pending flag
	SMB_BUSY = 0;							  		// Free bus
}


