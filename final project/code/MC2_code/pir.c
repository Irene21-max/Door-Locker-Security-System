/*
 * pir.c
 *
 *  Created on: Nov 6, 2024
 *      Author: irene
 */

#include "pir.h"
#include <avr/io.h>
#include "gpio.h"
#include "common_macros.h"

void PIR_init(void){

	GPIO_setupPinDirection(PIR_PORT_ID,PIR_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(PIR_PORT_ID,PIR_PIN_ID, LOGIC_LOW);

}

uint8 PIR_getState(void){
	uint8 state=0;
	state= GPIO_readPin(PIR_PORT_ID,PIR_PIN_ID);
	return state;
}
