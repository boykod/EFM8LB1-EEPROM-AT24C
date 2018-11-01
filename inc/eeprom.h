#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

//-----------------------------------------------------------------------------
// Pin Definitions
//-----------------------------------------------------------------------------
#define LED_ON   0
#define LED_OFF  1

SI_SBIT (DISP_EN, SFR_P3, 4);          // Display Enable
#define DISP_BC_DRIVEN   0             // 0 = Board Controller drives display
#define DISP_EFM8_DRIVEN 1             // 1 = EFM8 drives display

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define  WRITE                    0x00 // SMBus WRITE command
#define  READ                     0x01 // SMBus READ command
// Device addresses (7 bits, lsb is a don't care)
#define  EEPROM_ADDR              0xA0 //See Datasheet
// SMBus Buffer Size
#define  SMB_BUFF_SIZE            0x08 // Defines the maximum number of bytes
                                       // that can be sent or received in a
                                       // single transfer

// Status vector - top 4 bits only
#define  SMB_MTSTA                0xE0 // (MT) start transmitted
#define  SMB_MTDB                 0xC0 // (MT) data byte transmitted
#define  SMB_MRDB                 0x80 // (MR) data byte received
// End status vector definition

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
extern uint8_t* pSMB_DATA_IN;          // Global pointer for SMBus data
                                       // All receive data is written here

extern uint8_t SMB_SINGLEBYTE_OUT;     // Global holder for single byte writes.

extern uint8_t* pSMB_DATA_OUT;         // Global pointer for SMBus data.
                                       // All transmit data is read from here

extern uint8_t SMB_DATA_LEN;           // Global holder for number of bytes
                                       // to send or receive in the current
                                       // SMBus transfer.

extern uint8_t WORD_ADDR_1;
extern uint8_t WORD_ADDR_2;

extern uint8_t TARGET;                 // Target SMBus slave address

extern volatile bool SMB_BUSY;         // Software flag to indicate when the
                                       // EEPROM_ByteRead() or
                                       // EEPROM_ByteWrite()
                                       // functions have claimed the SMBus

extern volatile bool SMB_RW;           // Software flag to indicate the
                                       // direction of the current transfer

extern volatile bool SMB_SENDWORDADDR; // When set, this flag causes the ISR
                                       // to send the 8-bit <WORD_ADDR>
                                       // after sending the slave address.

extern volatile bool SMB_RANDOMREAD;   // When set, this flag causes the ISR
                                       // to send a START signal after sending
                                       // the word address.

extern volatile bool SMB_ACKPOLL;      // When set, this flag causes the ISR
                                       // to send a repeated START until the
                                       // slave has acknowledged its address

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void EEPROM_CheckSDA ();
void EEPROM_ByteWrite(uint8_t addr_1, uint8_t addr_2, uint8_t dat);
uint8_t EEPROM_ByteRead(uint8_t addr_1, uint8_t addr_2);

#endif /* INC_EEPROM_H_ */
