/*
 ================================================================================================
 Name        : main_Mc2.c
 Author      : Abdelrahman
 Description : MC2 application  (Control_ECU)
 ================================================================================================
 */

#include "uart.h"
#include "timer1.h"
#include "external_eeprom.h"
#include "DC_motor.h"
#include "buzzer.h"
#include "std_types.h"
#include <util/delay.h>
#include "twi.h"
#include "pir.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define MC1_READY     0x11
#define MC2_READY     0x10
#define T              1
#define F              0
#define set_pass      0xFF
#define check_pass    0xF1
#define open          0xF2
#define ERRORR        0xF3
#define ADDRESS_1     0xF0

/*******************************************************************************
 *                     Global Variables                                        *
 *******************************************************************************/
uint8 count = 0;
uint8 stay = 0;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void set_password(void);
void check_password(void);
void open_door(void);
void error_handling(void);
void time_handler(void);

int main(void)
{
	uint8 response;

	/*******************************************************************************
	 *                           Initializations                                   *
	 *******************************************************************************/
	Buzzer_init();
	DcMotor_Init();
	PIR_init();

	TWI_ConfigType config_twi = {0x01,rate_400k};
	TWI_init(&config_twi);

	UART_ConfigType config_uart = {bits_8,parity_disabled,stop_bit_1,BaudRate_9600};
	UART_init(&config_uart);

	Timer1_setCallBack(time_handler,TIMER1);

	while(UART_recieveByte() != MC1_READY){} // waiting for MC1 to be ready for synchronization

	while(1)
	{
		stay = 0; // to set stay to 0 for the while loop inside open_door function

		response = UART_recieveByte(); // store the request of MC1 and choose what function to call according to it
		if (response == set_pass)
		{
			set_password(); //function to set the password
		}
		else if (response == check_pass)
		{
			check_password(); //function to check if the password is true
		}

		else if (response == open)
		{
			open_door(); // function to open the door
		}
		else if (response == ERRORR)
		{

			error_handling(); //function to make an alarm by the buzzer when the password is entered 3 times wrong
		}
	}

}
/*
 * Description :
 *function to set password by saving it inside the EEPROM.
 */
void set_password(void)
{
	uint8 i,key;
	for(i=0;i<5;i++)
	{
		UART_sendByte(MC2_READY);
		key = UART_recieveByte();
		EEPROM_writeByte(ADDRESS_1 + 4 * i, key);
	}
	while(UART_recieveByte() != MC1_READY){}
	UART_sendByte(T);
}

/*
 * Description :
 *function to check if the password entered is matched with the stored one in the EEPROM by checking each entered key
 and compare it with it's one of the pin in the EEPROM and will send by the UART to MC1 to inform it
 that the entered password is true or false.
 */

void check_password(void)
{
	uint8 i,key,check,response = T;

	for(i=0;i<5;i++)
	{
		UART_sendByte(MC2_READY);
		key = UART_recieveByte();
		EEPROM_readByte(ADDRESS_1 + 4 * i,&check);
		if(key != check)
		{
			response = F;
		}
		_delay_ms(500);
	}
	while(UART_recieveByte() != MC1_READY);
	UART_sendByte(response);
}

/*
 * Description :
 *function to open the vault as it make the motor rotate clockwise for 15seconds then it will wait for 3 seconds then
 *return anti-clockwise to close the vault
 */
void open_door(void)
{
	//uint8 check_pir =PIR_getState();
	DcMotor_Rotate(CW,252); // will order the motor to rotate clockwise direction
	Timer1_ConfigType config_timer = {0,58594,TIMER1,CLOCK_1024,COMPARE}; // set timer to give interrupt after 7.5seconds
	Timer1_init(&config_timer); // calling the initialization function of timer1 giving it the structure
	while (stay == 0); // we will be in this while until we are done with opening and closing the door
}
/*
 * Description :
 *function to make buzzer on for 1-min upon entering the passworg wrong for 3 consecutive times
 */
void error_handling(void)
{
	Buzzer_on();
	Timer1_deInit(TIMER1);
	_delay_ms(60000);
	Buzzer_off();
}


/*
 * Description :
 *the callback function of the timer.
 * it is 1st set to give interrupt every 7.5seconds so after 2 interrupts it change to the waiting state then change the
 * timer interrupt to 3 seconds on waiting state then change it again to 7.5 and give will return after 15 seconds after
 * de-initializing the timer to continue the program.
 */

void time_handler(void)
{
	count++;
	if(count == 2)
	{
		DcMotor_Rotate(stop,0);  // will order the motor to stop rotation
		Timer1_ConfigType config_timer = {0,23438,TIMER1,CLOCK_1024,COMPARE}; // set timer to give interrupt after 3seconds
		Timer1_init(&config_timer);
	}
	if (count == 3)
	{
		DcMotor_Rotate(CCW,252); // will order the motor to rotate anti-clockwise direction
		Timer1_ConfigType config_timer = {0,58594,TIMER1,CLOCK_1024,COMPARE}; // set timer to give interrupt after 7.5seconds
		Timer1_init(&config_timer);

	}
	if (count == 5)
	{
		DcMotor_Rotate(stop,0);  // will order the motor to stop rotation
		count = 0; // setting to 0 to reset everything to default
		Timer1_deInit(TIMER1); // deinitialization of the timer
		stay = 1; // setting it to 1 will make us leave the while loop that we are stucked in the open door function

	}


}
