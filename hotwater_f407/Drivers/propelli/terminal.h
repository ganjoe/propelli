/*
 * terminal.h
 *
 *  Created on: Jul 7, 2020
 *      Author: daniel
 */

#ifndef INC_TERMINAL_H_
#define INC_TERMINAL_H_

#include "datatypes.h"
#include "main.h"


extern int flagTerminal_newKey;
extern int flagTerminal_newString;
extern int flagTerminal_newTransmission;
extern int TerminalBufferItr;

typedef struct _terminal_callback_struct
	{
	const char *command;
	const char *help;
	const char *arg_names;
	void (*cbf)(int argc, const char **argv);
	} terminal_callback_struct;

void terminal_process_string(char *str);
void commands_printf(const char* format, ...);
void terminal_register_command_callback		(const char* command, const char *help,	const char *arg_names,	void(*cbf)(int argc, const char **argv)	);
void terminal_unregister_callback		(void(*cbf)(int argc, const char **argv));
void task_terminalGetKey();
float _calc_uart_tx_time(int len);

#endif /* INC_TERMINAL_H_ */
