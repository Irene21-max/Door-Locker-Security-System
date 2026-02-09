/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: Timer1.c
 *
 * Description: Source file for the AVR Timer1 driver
 *
 *******************************************************************************/

#include "timer1.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static volatile void (*g_Timer0_CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer1_CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer2_CallBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	SREG |= (1<<7);

if(Config_Ptr ->timer_ID == TIMER1){
	TCNT1 = Config_Ptr->initial_value;

	if(Config_Ptr->mode == COMPARE)
	{
		OCR1A = Config_Ptr ->compare_value;
		TIMSK = (1<<OCIE1A);
		TCCR1A = (1<<FOC1A);
		TCCR1B = (1<<WGM12)|(Config_Ptr->prescaler);
	}
	else if (Config_Ptr->mode == NORMAL)
	{
		TIMSK = (1<<TOIE1);
		TCCR1A = (1<<FOC1A);
		TCCR1B = (Config_Ptr->prescaler);

	}
}
else if(Config_Ptr ->timer_ID == TIMER0){
	TCNT0 = Config_Ptr->initial_value;
	if(Config_Ptr->mode == COMPARE)
		{
			OCR0 = Config_Ptr ->compare_value;
			TIMSK = (1<<OCIE0);
			TCCR0 = (1<<WGM01)|(Config_Ptr->prescaler)|(1<<FOC0);

		}
		else if (Config_Ptr->mode == NORMAL)
		{
			TIMSK = (1<<TOIE0);
			TCCR0 = (Config_Ptr->prescaler)|(1<<FOC0);

		}
}
else if(Config_Ptr ->timer_ID == TIMER0){
	TCNT2 = Config_Ptr->initial_value;
	if(Config_Ptr->mode == COMPARE)
		{
			OCR2 = Config_Ptr ->compare_value;
			TIMSK = (1<<OCIE2);
			TCCR0 = (1<<WGM21)|(Config_Ptr->prescaler)|(1<<FOC2);

		}
		else if (Config_Ptr->mode == NORMAL)
		{
			TIMSK = (1<<TOIE2);
			TCCR0 = (Config_Ptr->prescaler)|(1<<FOC2);

		}
}

}

void Timer1_deInit(Timer_ID_TYPE timer_type)
{
	if(timer_type == TIMER1){
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK = 0;
	}
	else if(timer_type == TIMER0){
		TCNT0 =0;
		TIMSK = 0;
		TCCR0 = 0;
	}
	else if(timer_type == TIMER2){
			TCNT2 =0;
			TIMSK = 0;
			TCCR2 = 0;
		}


}

void Timer1_setCallBack(void(*a_ptr)(void),Timer_ID_TYPE timer_ID)
{

	switch(timer_ID) {
		case TIMER0:
			g_Timer0_CallBackPtr = a_ptr;
			break;
		case TIMER1:
			g_Timer1_CallBackPtr = a_ptr;
			break;
		case TIMER2:
			g_Timer2_CallBackPtr = a_ptr;
			break;
		}
}

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect) {
	if(g_Timer0_CallBackPtr != NULL_PTR) {
		(*g_Timer0_CallBackPtr)();
	}
}

ISR(TIMER0_COMP_vect) {
	if(g_Timer0_CallBackPtr != NULL_PTR) {
		(*g_Timer0_CallBackPtr)();
	}
}

ISR(TIMER1_OVF_vect) {
	if(g_Timer1_CallBackPtr != NULL_PTR) {
		(*g_Timer1_CallBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect) {
	if(g_Timer1_CallBackPtr != NULL_PTR) {
		(*g_Timer1_CallBackPtr)();
	}
}

ISR(TIMER2_OVF_vect) {
	if(g_Timer2_CallBackPtr != NULL_PTR) {
		(*g_Timer2_CallBackPtr)();
	}
}

ISR(TIMER2_COMP_vect) {
	if(g_Timer2_CallBackPtr != NULL_PTR) {
		(*g_Timer2_CallBackPtr)();
	}
}
