/*
 * pir.h
 *
 *  Created on: Nov 6, 2024
 *      Author: irene
 */

#ifndef PIR_H_
#define PIR_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PIR_PORT_ID PORTC_ID
#define PIR_PIN_ID PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void PIR_init(void);
uint8 PIR_getState(void);

#endif /* PIR_H_ */
