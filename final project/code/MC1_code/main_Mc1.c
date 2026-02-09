/*
 ================================================================================================
 Name        : main_Mc1.c
 Author      : Abdelrahman
 Description : MC1 application to control human machine interface
 ================================================================================================
 */
#include "lcd.h"
#include "keypad.h"
#include "timer1.h"
#include "uart.h"
#include "std_types.h"
#include <util/delay.h>

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

/*******************************************************************************
 *                     Global Variables                                        *
 *******************************************************************************/
uint8 count = 0;
uint8 stay = 0;
uint8 choice;
uint8 set;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

uint8 set_password(void);
uint8 step_2(void);
void step_1(void);
void time_handler (void);
void testing_password(void);

int main(void)
{

	/*******************************************************************************
	 *                           Initializations                                   *
	 *******************************************************************************/
	LCD_init();

	UART_ConfigType config_uart = {bits_8,parity_disabled,stop_bit_1,BaudRate_9600};
	UART_init(&config_uart);

	Timer1_setCallBack(time_handler,TIMER1);


	UART_sendByte(MC1_READY); // to inform MC2 that MC1 is ready and have finished all initiliazations

	while(1)
	{

		step_1();
		set = 0;
		while(set == 0)
		{
			stay = 0;
			choice = step_2();
			testing_password();

		}
	}
}

/*
 * Description :
 *function to allow user to enter the password and send it then to MC2 either for setting or confirming
 */
uint8 set_password(void)
{
	uint8 i;
	uint8 key;
	for(i=0;i<5;i++)
	{
		_delay_ms(400);
		key = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		while(UART_recieveByte() != MC2_READY){}
		UART_sendByte(key);

	}

	while(KEYPAD_getPressedKey() != '=');
	LCD_clearScreen();
	UART_sendByte(MC1_READY);
	return UART_recieveByte();

}

/*
 * Description :
 *function to set password for the 1st time
 */
void step_1(void)
{
	uint8 response = F;
	do{
		LCD_clearScreen();
		LCD_displayString("Plz set Pass:");
		LCD_moveCursor(1,0);
		UART_sendByte(set_pass);
		set_password();
		LCD_displayString("Plz re-enter the");
		LCD_moveCursor(1,0);
		LCD_displayString("Same Pass:");
		UART_sendByte(check_pass);
		response = set_password();
	}while(response == F);
}
/*
 * Description :
 *function to ask user either to open the door or change the password and then return his choice to a variable (caller)
 */

uint8 step_2(void)
{

	uint8 key;
	LCD_moveCursor(0,0);
	LCD_displayString("+  :  Open Door");
	LCD_moveCursor(1, 0);
	LCD_displayString("-  : change Pass");

	key = KEYPAD_getPressedKey();
	LCD_clearScreen();
	LCD_moveCursor(0,7);
	LCD_displayCharacter(key);
	LCD_moveCursor(1,3);
	if(key == '+')
	{
		LCD_displayString("open Door");
	}
	else if(key == '-')
	{
		LCD_displayString("change Pass");
	}
	_delay_ms(2000);
	LCD_clearScreen();
	return key;
}

/*
 * Description :
 *function to test if the password if true or not
 */


void testing_password(void)
{
	uint8 response = F;


	LCD_displayString("Plz enter Pass:");
	LCD_moveCursor(1,0);

	UART_sendByte(check_pass);
	response = set_password();
	if (response == F)
	{
		LCD_displayString("Wrong Password");
		LCD_moveCursor(1, 0);
		LCD_displayString("2 Trials left");
		_delay_ms(1000);
		LCD_clearScreen();

		LCD_displayString("Plz enter Pass:");
		LCD_moveCursor(1,0);

		UART_sendByte(check_pass);
		response = set_password();
	}
	if (response == F)
	{
		LCD_displayString("Wrong Password");
		LCD_moveCursor(1, 0);
		LCD_displayString("1 Trial left");
		_delay_ms(1000);
		LCD_clearScreen();
		LCD_displayString("Plz enter Pass:");
		LCD_moveCursor(1,0);

		UART_sendByte(check_pass);
		response = set_password();
	}
	if (response == F)
	{
		UART_sendByte(ERRORR);

		LCD_moveCursor(0,6);
		LCD_displayString("Error"); //for 1 minute
		_delay_ms(60000);

		choice = 0;
	}

	if (choice == '+')
	{
		Timer1_ConfigType config_timer = {0,58594,TIMER1,CLOCK_1024,COMPARE}; // setting timer1 init (will be changed)
		UART_sendByte(open);
		LCD_displayStringRowColumn(0, 4, "Door is");
		LCD_displayStringRowColumn(1, 4, "Unlocking");

		Timer1_init(&config_timer);
		while(stay == 0){}
		LCD_clearScreen();
	}
	else if (choice == '-')
	{
		set = 1; //will return us to step 1 again
	}

}

/*
 * Description :
 *the callback function of the timer.
 * it is 1st set to give interrupt every 7.5seconds so after 2 interrupts it change to the waiting state then change the
 * timer interrupt to 3 seconds on waiting state then change it again to 7.5 and give will return after 15 seconds after
 * de-initializing the timer to continue the program.
 */
void time_handler (void)
{
	count++;
	if(count == 2)
	{
		LCD_clearScreen(); // hold for 3 seconds

		LCD_displayStringRowColumn(1, 4, "Waiting");


		Timer1_ConfigType config_timer = {0,23438,CLOCK_1024,COMPARE};// set timer to give interrupt after 3seconds
		Timer1_init(&config_timer);
	}
	if (count == 3)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 4, "Door is");
		LCD_displayStringRowColumn(1, 4, "locking");

		Timer1_ConfigType config_timer = {0,58594,CLOCK_1024,COMPARE};// set timer to give interrupt after 7.5seconds
		Timer1_init(&config_timer);

	}
	if (count == 5)
	{
		count = 0;
		Timer1_deInit(TIMER1);
		stay = 1;

	}
}
