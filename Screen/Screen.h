#ifndef Screen_h
#define Screen_h
#include "arduino.h"
#include "Drawable.h"
#include "SPI.h"
#include "RAM.h"

#define COLS 128
#define ROWS 8

#define DISP_CS 5
#define DISP_DC 6

#define DISP_OFF 0xAE
#define DISP_ON 0xAF
#define DISP_NOOP 0xE3 

#define MEM_ADD_MODE 0x20
#define PAGE_ADD_MODE 0x02

#define PAGE_START_ADD 0xB0
#define LCOL_START_ADD 0x00
#define HCOL_START_ADD 0x10

#define NORMAL_DIP 0xA6
#define INVERSE_DISP 0xA7

#define ENTIRE_DISP_ON 0xA5
#define RAM_CONTENT 0xA4


class Screen{
	public:
		Screen();
		void init();
		uint8_t draw(uint8_t*, int16_t, int16_t, uint8_t);
		void writeBuffer();
		void clearBuffer();
	
	private:
		uint8_t (*buf)[COLS];
		void sendCommand(uint8_t);
		void sendData(uint8_t);
		void send(uint8_t);
		void sendDataArray(uint8_t*, uint16_t);
		void sendCommand2Byte(uint8_t, uint8_t);
		void readArrayPgm(uint8_t*, uint8_t*, uint16_t);
		
		
};

#endif