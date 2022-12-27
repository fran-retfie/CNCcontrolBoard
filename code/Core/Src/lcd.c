/*
 * lcd.c
 *
 *  Created on: Dec 1, 2020
 *      Author: fatay
 */
#include <stdint.h>
#include "lcd.h"

#define SET_IF(expr)  ((expr) ? GPIO_PIN_SET : GPIO_PIN_RESET)
char display_settings;

//Sending falling edge signal to EPin for waking up LCD
static void fallingEdge(void) {
    HAL_GPIO_WritePin(CMD_PORT, E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CMD_PORT, E_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CMD_PORT, E_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}


static void send4Bits(char data) {
	HAL_GPIO_WritePin(DATA_PORT, DATA5_Pin, SET_IF(data&0x01));
	HAL_GPIO_WritePin(DATA_PORT, DATA6_Pin, SET_IF(data&0x02));
	HAL_GPIO_WritePin(DATA_PORT, DATA7_Pin, SET_IF(data&0x04));
	HAL_GPIO_WritePin(DATA_PORT, DATA8_Pin, SET_IF(data&0x08));

	fallingEdge();
}

static void sendCommand(char cmd) {
	HAL_GPIO_WritePin(CMD_PORT, RS_Pin, GPIO_PIN_RESET);
	send4Bits(cmd >> 4);
	send4Bits(cmd);
}

static void sendData(char data) {
	HAL_GPIO_WritePin(CMD_PORT, RS_Pin, GPIO_PIN_SET);
	send4Bits(data >> 4);
	send4Bits(data);

}

void clearLCD(void) {
	sendCommand(LCD_CLEARDISPLAY);
	HAL_Delay(5);
}

void putLCD(char c) {
	sendData(c);
}

void writeLCD (char *str) {
	for(; *str != 0; ++str) {
		sendData(*str);
	}
}

void initLCD(void) {
    HAL_GPIO_WritePin(CMD_PORT, E_Pin,  	 GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CMD_PORT, RS_Pin, 	 GPIO_PIN_RESET);

	HAL_Delay(50);

	display_settings = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	send4Bits(0x03);
	HAL_Delay(5);
	send4Bits(0x03);
	HAL_Delay(5);
	send4Bits(0x03);
	HAL_Delay(2);
	send4Bits(0x02);
	HAL_Delay(2);

	sendCommand(LCD_FUNCTIONSET | display_settings);
	display_settings = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	sendCommand(LCD_DISPLAYCONTROL | display_settings);
	HAL_Delay(2);

	clearLCD();
	display_settings =  LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	sendCommand(LCD_ENTRYMODESET | display_settings);
	HAL_Delay(2);
}


void setCursor(char x, char y) {

	uint8_t base = 0;

	if(y==1) {
		base = 0x40;
	}else {
		base = 0;
	}

	sendCommand( 0x80 | (base + x));


}

void cursorOn(void) {
	sendCommand(0x08 | 0x04 | 0x02);
}

void blinkOn(void) {
	sendCommand(0x08 | 0x04 | 0x01);
}
