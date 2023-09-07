#include "main.h"

const char *str_invalid = "//// Invalid option ////";

void process_command(Command_TypeDef *cmd);
int extract_command(Command_TypeDef *cmd);

void menu_task_handler(void *param)
{
	uint32_t cmd_addr;
	Command_TypeDef *cmd;
	int input;

	const char *str_menu = "=============================\n"
					   	   "|			Menu			|\n"
					   	   "=============================\n"
					   	   "LED Effect ---------------> 0\n"
					   	   "Date and Time ------------> 1\n"
					   	   "Exit ---------------------> 2\n"
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
	const char *str_led =  "=============================\n"
					   	   "|		LED Effect			 \n"
					   	   "=============================\n"
					   	   "Choices: none, e1, e2, e3, e4\n"
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

void rtc_task_handler(void *param)
{
	while (1)
	{

	}
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



