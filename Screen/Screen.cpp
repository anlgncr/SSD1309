#include "Screen.h"

Screen::Screen(){
	buf = (uint8_t(*)[COLS])RAM::malloc(ROWS * COLS);
	pinMode(DISP_CS, OUTPUT);
	pinMode(DISP_DC, OUTPUT);
	SPI.begin();
	
	sendCommand(DISP_NOOP);
	//sendCommand(ENTIRE_DISP_ON);
	sendCommand(RAM_CONTENT);
	sendCommand(MEM_ADD_MODE);
	sendCommand(PAGE_ADD_MODE);
	//sendCommand(INVERSE_DISP);
	sendCommand(NORMAL_DIP);
	writeBuffer();
	sendCommand(DISP_ON);
};

void Screen::writeBuffer(){
	sendCommand2Byte(LCOL_START_ADD, HCOL_START_ADD);
	
	uint8_t temp[128];
	for(uint8_t rows = 0; rows < ROWS; rows++){	
		sendCommand(PAGE_START_ADD | rows);
		RAM::readArray(&buf[rows][0], temp, 128);
		sendDataArray(temp, 128);
	}
}

void Screen::clearBuffer(){
	RAM::writeLoop(buf, 0, ROWS * COLS);
}

void Screen::sendCommand(uint8_t cmd){
	digitalWrite(DISP_DC, LOW);
	send(cmd);
}

void Screen::sendCommand2Byte(uint8_t cmd1, uint8_t cmd2){
	SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
	digitalWrite(DISP_DC, LOW);
	digitalWrite(DISP_CS, LOW);
	SPI.transfer(cmd1);
	SPI.transfer(cmd2);
	digitalWrite(DISP_CS, HIGH);
	SPI.endTransaction();
}

void Screen::sendDataArray(uint8_t *data, uint16_t length){
	SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
	digitalWrite(DISP_DC, HIGH);
	digitalWrite(DISP_CS, LOW);
	
	for(uint16_t i=0; i<length; i++){
		SPI.transfer(data[i]);
	}
	
	digitalWrite(DISP_CS, HIGH);
	SPI.endTransaction();
}

void Screen::sendData(uint8_t data){
	digitalWrite(DISP_DC, HIGH);
	send(data);
}

void Screen::send(uint8_t data){
	SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
	digitalWrite(DISP_CS, LOW);
	SPI.transfer(data);
	digitalWrite(DISP_CS, HIGH);
	SPI.endTransaction();
}

uint8_t Screen::draw(uint8_t* image, int16_t x, int16_t y, uint8_t memory){
	if(!image)
		return 0;
	
	uint8_t image_width, image_height;
	
	if(memory == PGMEM){
		image_width = pgm_read_byte(image);
		image_height = pgm_read_byte(image + 1);
	}
	else if(memory == SPIMEM){
		image_width = RAM::read(image);
		image_height = RAM::read(image + 1);
	}
	
	if(image_height == 0){ return 0; }
	image_height *= 8;
	
	uint8_t image_start_col, image_end_col, buffer_start_col;

	if(x < 0 && ((x + image_width) > COLS)){
		image_start_col = abs(x);
		image_end_col = COLS - x;
		buffer_start_col = 0;
	}
	else if(x < 0){
		if((x + image_width) <= 0){ return 0; }
		else{
			image_start_col = abs(x);
			image_end_col = image_width;
			buffer_start_col = 0;
		}
	}
	else if(x + image_width > COLS){
		if(x >= COLS){ return 0; }
		else{
			image_start_col = 0;
			image_end_col = COLS - x;
			buffer_start_col = x;
		}
	}
	else{ 
		image_start_col = 0;
		image_end_col = image_width;
		buffer_start_col = x; 
	}
	
	int16_t buffer_start_row, buffer_end_row, image_start_row;
	uint8_t image_shift;
	
	if(y < 0 && ((y + image_height) > ROWS*8)){ 	//Serial.println("Alttan ustten tasti");
		image_start_row = abs(y) / 8;
		image_shift = 8 - abs(y) % 8;
		buffer_start_row = -1;
		buffer_end_row = ROWS;
	}
	else if(y < 0){
		if((y + image_height) <= 0){ 				//Serial.println("Ustten disarida");
			return 0; 
			}
		else{										//Serial.println("Ustten tasti");
			image_start_row = abs(y) / 8;
			image_shift = 8 - abs(y) % 8;
			buffer_start_row = -1;
			buffer_end_row = (y / 8 - 1) + (image_height / 8);
		}
	}
	else if((y + image_height) > ROWS*8)			
	{
		if(y >= ROWS*8){ 							//Serial.println("Alttan disarida");
			return 0; 
		}
		else{										//Serial.println("Alttan tasti");
			image_start_row = 0;
			image_shift = abs(y) % 8;
			buffer_start_row = y / 8;
			buffer_end_row = ROWS;
		}
	}
	else{
		image_start_row = 0;						//Serial.println(y);
		image_shift = abs(y) % 8;
		buffer_start_row = y / 8;
		buffer_end_row = (y / 8) + (image_height / 8);
	}
		
	uint8_t(*image_array)[image_width];
	image_array = (uint8_t(*)[image_width])(image + 2);
	
	uint8_t visible_width = image_end_col - image_start_col;
	uint8_t temp_image[visible_width];
	uint8_t temp_buffer[visible_width]; // screen buffer in spi ram
		
	int16_t next_image_row = image_start_row;
	for(int16_t rows = buffer_start_row; rows < buffer_end_row; rows++)
	{
		if(rows >= 0)
		{
			if(memory == SPIMEM)
				RAM::readArray(&image_array[next_image_row][image_start_col], temp_image, visible_width);
			else if(memory == PGMEM)
				readArrayPgm(&image_array[next_image_row][image_start_col], temp_image, visible_width);		
	
			RAM::readArray(&buf[rows][buffer_start_col], temp_buffer, visible_width);
			
			for(uint8_t i=0; i<visible_width; i++){
				temp_image[i] <<= image_shift;
				temp_image[i] |= temp_buffer[i];
			}
			
			RAM::writeArray(&buf[rows][buffer_start_col], temp_image, visible_width);
		}
		if((rows + 1) < ROWS && image_shift)
		{
			if(memory == SPIMEM)
				RAM::readArray(&image_array[next_image_row][image_start_col], temp_image, visible_width);
			else if(memory == PGMEM)
				readArrayPgm(&image_array[next_image_row][image_start_col], temp_image, visible_width);		
	
			RAM::readArray(&buf[rows + 1][buffer_start_col], temp_buffer, visible_width);
			
			for(uint8_t i=0; i<visible_width; i++){
				temp_image[i] >>= (8 - image_shift);
				temp_image[i] |= temp_buffer[i];
			}
			
			RAM::writeArray(&buf[rows + 1][buffer_start_col], temp_image, visible_width);
		}
		next_image_row++;
	}
}


void Screen::readArrayPgm(uint8_t *address, uint8_t *data, uint16_t length){
	for(uint16_t i=0; i<length; i++){
		data[i] = pgm_read_byte(&address[i]);
	}
}

























