#include "main.h"

#define HH_CONFIG 		0
#define MM_CONFIG 		1
#define SS_CONFIG 		2

#define DATE_CONFIG 	0
#define MONTH_CONFIG 	1
#define YEAR_CONFIG 	2
#define DAY_CONFIG 		3

const char *str_invalid = "//// Invalid option ////";

void process_command(Command_TypeDef *cmd);
int extract_command(Command_TypeDef *cmd);

void menu_task_handler(void *param)
{
	uint32_t cmd_addr;
	Command_TypeDef *cmd;
	int input;

	const char *str_menu = "\n********** Menu **********\n"
					   	   "LED Effect       : 0\n"
					   	   "Date and Time    : 1\n"
					   	   "Exit             : 2\n"
					   	   "Enter your choice: ";

	while (1)
	{
		// Note that it is not the menu string itself that gets sent to the print queue,
		// but it is the POINTER to the menu string '&str_menu'. See the API documentation.
		xQueueSend(q_print, &str_menu, portMAX_DELAY);

		// Go to blocked state indefinitely until it gets the notification
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

		// When there has been a notification, decode the command
		cmd = (Command_TypeDef *)cmd_addr;
		if (cmd->len == 1)
		{
			// Valid user input

			input = cmd->payload[0] - '0';

			// Decode
			switch (input)
			{
			case 0:	// LED Effect
				curr_state = sLedEffect;
				xTaskNotify(led_task_handle, 0, eNoAction);
				break;
			case 1:	// Date and Time
				curr_state = sRtcMenu;
				xTaskNotify(rtc_task_handle, 0, eNoAction);
				break;
			case 2:	// Exit
				break;
			default:
				xQueueSend(q_print, &str_invalid, portMAX_DELAY);
				continue;	// Continue waiting for the command
			}
		}
		else
		{
			// Invalid user input

			xQueueSend(q_print, &str_invalid, portMAX_DELAY);
			continue;
		}

		// Go to blocked state until gets a notification from other tasks
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Upon unblocking, it will run the while loop again!
	}
}

void cmd_task_handler(void *param)
{
	BaseType_t status;
	Command_TypeDef cmd;

	while (1)
	{
		// Wait for command
		status = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		if (status == pdTRUE)
		{
			// Process the command stored in the input data queue
			process_command(&cmd);
		}
	}
}

void print_task_handler(void *param)
{
	uint32_t *msg;

	while (1)
	{
		// Go to blocked state until there's any message in the print queue
		xQueueReceive(q_print, &msg, portMAX_DELAY);

		// Retrieve the item in the queue, and write it over UART
		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen((char *)msg), HAL_MAX_DELAY);
	}
}

void led_task_handler(void *param)
{
	uint32_t cmd_addr;
	Command_TypeDef *cmd;
	const char *str_led =  "\n********** LED Effect **********\n"
					   	   "Effects: none, e1, e2, e3, e4\n"
					   	   "Enter your choice: ";

	while (1)
	{
		// Wait for notification (Notify wait)
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Print LED menu
		xQueueSend(q_print, &str_led, portMAX_DELAY);

		// Wait for LED command in the blocked state (Notify wait)
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (Command_TypeDef *)cmd_addr;


		if (cmd->len <= 4)
		{
			if (!strcmp((char *)cmd->payload, "none"))
				stop_led_effect();
			else if (!strcmp((char *)cmd->payload, "e1"))
				led_effect(1);
			else if (!strcmp((char *)cmd->payload, "e2"))
				led_effect(2);
			else if (!strcmp((char *)cmd->payload, "e3"))
				led_effect(3);
			else if (!strcmp((char *)cmd->payload, "e4"))
				led_effect(4);
			else
				xQueueSend(q_print, &str_invalid, portMAX_DELAY);	// Print invalid message
		}
		else
		{
			xQueueSend(q_print, &str_invalid, portMAX_DELAY);
		}

		// Update the current state so the program can transition back
		// to the main menu
		curr_state = sMainMenu;

		// Notify the menu task
		xTaskNotify(menu_task_handle, 0, eNoAction);
	}
}

// Convert the user input (ASCII) to a number
uint8_t getnumber(uint8_t *p, int len)
{
	int value;

	if (len > 1)
		value = (((p[0] - '0') * 10) + (p[1] - '0'));
	else
		value = p[0] - '0';

	return value;
}

void rtc_task_handler(void *param)
{
	// To improve this code, you can define a char** array to manage all strings

	const char *str_rtc1 = "\n********** RTC **********\n";
	const char *str_rtc2 = "Configure Time   : 0\n"
						   "Configure Date   : 1\n"
						   "Enable Reporting : 2\n"
						   "Exit             : 3\n"
						   "Enter your choice: ";

	const char *str_rtc_hh = "Enter hour (1-12): ";
	const char *str_rtc_mm = "Enter minutes (0-59): ";
	const char *str_rtc_ss = "Enter seconds (0-59): ";

	const char *str_rtc_dd = "Enter date (1-31): ";
	const char *str_rtc_mo = "Enter month (1-12): ";
	const char *str_rtc_dow = "Enter day (1-7 sun:1): ";
	const char *str_rtc_yr = "Enter year (0-99): ";

	const char *str_conf_succ = "Configuration successful\n";
	const char *str_rtc_report = "Enable time & date reporting (y/n)?: ";

	uint32_t cmd_addr;
	Command_TypeDef *cmd;

	static int rtc_state = 0;
	int menu_code;

	RTC_TimeTypeDef time;	// Holds the time information
	RTC_DateTypeDef date;	// Holds the date information

	while (1)
	{
		// Wait till someone notifies
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Print the menu and display current date and time information
		xQueueSend(q_print, &str_rtc1, portMAX_DELAY);
		display_time_date();
		xQueueSend(q_print, &str_rtc2, portMAX_DELAY);

		while (curr_state != sMainMenu)
		{
			// Wait for command notification
			xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
			cmd = (Command_TypeDef *)cmd_addr;

			switch (curr_state)
			{
			case sRtcMenu:
				// Process RTC menu commands
				if (cmd->len == 1)
				{
					menu_code = cmd->payload[0] - '0';
					switch (menu_code)
					{
					case 0:
						curr_state = sRtcTimeConfig;
						// Ask for the hour information
						xQueueSend(q_print, &str_rtc_hh, portMAX_DELAY);
						break;
					case 1:
						curr_state = sRtcDateConfig;
						// Ask for the date information
						xQueueSend(q_print, &str_rtc_dd, portMAX_DELAY);
						break;
					case 2:
						curr_state = sRtcReport;
						xQueueSend(q_print, &str_rtc_report, portMAX_DELAY);
						break;
					case 3:
						curr_state = sMainMenu;
						break;
					default:
						curr_state = sMainMenu;
						xQueueSend(q_print, &str_invalid, portMAX_DELAY);
						break;
					}
				}
				else
				{
					// Invalid input

					curr_state = sMainMenu;
					xQueueSend(q_print, &str_invalid, portMAX_DELAY);
				}
				break;
			case sRtcTimeConfig:
				// Ask for hh, mm, ss information and configure RTC
				// Handle invalid entries
				switch (rtc_state)
				{
				case HH_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t hour = getnumber(cmd->payload, cmd->len);
					time.Hours = hour;
					rtc_state = MM_CONFIG;	// Change to miniutes configuration
					// Ask for 'minutes' information
					xQueueSend(q_print, &str_rtc_mm, portMAX_DELAY);
					break;
				case MM_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t min = getnumber(cmd->payload, cmd->len);
					time.Minutes = min;
					rtc_state = SS_CONFIG;	// Change to seconds configuration
					// Ask for 'seconds' information
					xQueueSend(q_print, &str_rtc_ss, portMAX_DELAY);
					break;
				case SS_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t sec = getnumber(cmd->payload, cmd->len);
					time.Seconds = sec;
					// Do a validity check for the entered RTC information
					if (!validate_rtc_information(&time, NULL))
					{
						// Valid RTC information
						rtc_configure_time(&time);
						xQueueSend(q_print, &str_conf_succ, portMAX_DELAY);
						display_time_date();
					}
					else
					{
						// Invalid RTC information
						xQueueSend(q_print, &str_invalid, portMAX_DELAY);
					}

					curr_state = sMainMenu;
					rtc_state = 0;
					break;
				}
				break;
			case sRtcDateConfig:
				// Get date, month, day, year information and configure RTC
				// Handle invalid entries
				switch (rtc_state)
				{
				case DATE_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t d = getnumber(cmd->payload, cmd->len);
					date.Date = d;
					rtc_state = MONTH_CONFIG;
					xQueueSend(q_print, &str_rtc_mo, portMAX_DELAY);
					break;
				case MONTH_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t month = getnumber(cmd->payload, cmd->len);
					date.Month = month;
					rtc_state = DAY_CONFIG;
					xQueueSend(q_print, &str_rtc_dow, portMAX_DELAY);
					break;
				case DAY_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t day = getnumber(cmd->payload, cmd->len);
					date.WeekDay = day;
					rtc_state = YEAR_CONFIG;
					xQueueSend(q_print, &str_rtc_yr, portMAX_DELAY);
					break;
				case YEAR_CONFIG:
					; // The C language standard only allows statements​ to follow a label. (Declaration is not a statement)
					uint8_t year = getnumber(cmd->payload, cmd->len);
					date.Year = year;
					// Do a validity check for the entered RTC information
					if (!validate_rtc_information(NULL, &date))
					{
						// Valid RTC information
						rtc_configure_date(&date);
						xQueueSend(q_print, &str_conf_succ, portMAX_DELAY);
						display_time_date();
					}
					else
					{
						// Invalid RTC information
						xQueueSend(q_print, &str_invalid, portMAX_DELAY);
					}

					curr_state = sMainMenu;
					rtc_state = 0;
					break;
				}
				break;
			case sRtcReport:
				// Enable or disable RTC current time reporting over ITM printf
				if (cmd->len == 1)
				{
					if (cmd->payload[0] == 'y')
					{
						if (xTimerIsTimerActive(rtc_timer) == pdFALSE)
							xTimerStart(rtc_timer, portMAX_DELAY);
						else if (cmd->payload[0] == 'n')
							xTimerStop(rtc_timer, portMAX_DELAY);
						else
							xQueueSend(q_print, &str_invalid, portMAX_DELAY);
					}
				}
				else
				{
					xQueueSend(q_print, &str_invalid, portMAX_DELAY);
				}

				curr_state = sMainMenu;
				break;
			} // End of switch
		} // End of while

		// Notify the menu task
		xTaskNotify(menu_task_handle, 0, eNoAction);

	} // End of while super loop
}

void process_command(Command_TypeDef *cmd)
{
	extract_command(cmd);

	switch (curr_state)
	{
	case sMainMenu:
		xTaskNotify(menu_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	case sLedEffect:
		xTaskNotify(led_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(rtc_task_handle, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	}
}

int extract_command(Command_TypeDef *cmd)
{
	uint8_t item;	// Temporary variable to hold a data-byte
	BaseType_t status;

	// Check if there's any data item present in the queue
	status = uxQueueMessagesWaiting(q_data);	// Returns the # of data waiting in the queue
	if (!status)
	{
		// No data in the queue, return!
		return -1;
	}

	uint8_t i = 0;

	// Read in the data items into the 'payload' field of the cmd structure
	do
	{
		status = xQueueReceive(q_data, &item, 0);
		if (status == pdTRUE)
			cmd->payload[i++] = item;
	} while (item != '\n');

	cmd->payload[i - 1] = '\0'; // Make sure that the payload ends with '\0' char
	cmd->len = i - 1;	// Save length of the command excluding the null char

	return 0;
}



