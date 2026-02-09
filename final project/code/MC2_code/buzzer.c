/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: Buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 *******************************************************************************/

#include "buzzer.h"
#include <avr/io.h>
#include "gpio.h"
#include "common_macros.h"

void Buzzer_init(){

	GPIO_setupPinDirection(buzzerPORT_ID,buzzerPIN_ID, PIN_OUTPUT);
	GPIO_writePin(buzzerPORT_ID,buzzerPIN_ID, LOGIC_LOW);

}

void Buzzer_on(void){
	GPIO_writePin(buzzerPORT_ID,buzzerPIN_ID, LOGIC_HIGH);
}

void Buzzer_off(void){
	GPIO_writePin(buzzerPORT_ID,buzzerPIN_ID, LOGIC_LOW);
}
