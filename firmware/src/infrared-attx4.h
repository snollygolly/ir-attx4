#include <avr/interrupt.h> // Supplied Interrupt Macros
#include <avr/wdt.h> // Supplied Watch Dog Timer Macros 
#include "spi_via_usi_driver.c" // Supplied SPI Driver

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define SYSCLOCK 8000000  // attiny clock
#define KHz 1000

#define _GAP 5000 // Minimum map between transmissions
#define GAP_TICKS (_GAP/USECPERTICK)

#define MARK  1
#define SPACE 0

// Port B, Pin 2
#define CS 2
// Port A, Pin 1
#define IR_RX 1
// Port A, pin 7
#define IR_TX 7
// Port B, pin 1
#define IRQ 1

// Response codes
#define ALIVE_CODE 0x55
#define ACK_CODE  0x33
#define FIN_CODE 0x16

// Firmware version to increment
#define FIRMWARE_VERSION 0x01

// Available commands
#define ACK_CMD 0
#define FIRMWARE_CMD 1
#define TX_DATA_CMD 2
#define RX_DATA_AVAIL_CMD 3
#define RX_DATA_CMD 4
#define START_RX 5
#define STOP_RX 6

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define MAX_BUF_LEN 100 // Length of raw duration buffer

// receiver states
#define STATE_IDLE		2
#define STATE_RX_MARK	3
#define STATE_RX_SPACE	4
#define STATE_RX_STOP	5
#define STATE_TX		6

// Struct for keeping tracking of the receive state
typedef struct {
  uint8_t state;          		// receiving state machine
  uint8_t enabled;
  unsigned int timer;     		// state timer, counts 50uS ticks.
  int rxbuf[MAX_BUF_LEN]; 		// raw data receiving (in 50uS ticks)
  uint8_t rxlen;         		// counter of entries in rxbuf
} 
receiver_t;

// Struct for keeping tracking of the transmit state
typedef struct {
  uint8_t frequency;          	// frequency for transmission
  uint8_t state;          		// state machine
  int txbuf[MAX_BUF_LEN]; 		// data to send
  uint8_t txlen;				// Counter of entries in txbuf
} 
transmitter_t;

extern volatile receiver_t receiver;
extern volatile transmitter_t transmitter;