/*
 * terminal.h
 *
 *  Created on: Jul 7, 2020
 *      Author: daniel
 */

#ifndef INC_TERMINAL_H_
#define INC_TERMINAL_H_

#include "datatypes.h"
#include "datatypes_tasks.h"
#include "main.h"
//datei exisitiert nur wenn von cubeIDE generiert. "generate pair of .c .h files"
#include "usart.h"

typedef struct
	{
	const char *command;
	const char *help;
	const char *arg_names;
	void (*cbf)(int argc, const char **argv);
	}
TD_TERMINAL_CALLBACKS;

typedef struct
{
	int callback_len, buffer_receive_len, maxArguments, TerminalBufferItr;
	char byte_received;
	char* sep;
	char* eoc;
	char* string_rx;
	char* string_tx;
	int uart_buffer_tx_len;
	int flag_newString, flag_newTransmission;

	modflag mf_cmd;
	TD_TERMINAL_CALLBACKS td_callbacks;
	UART_HandleTypeDef* huart;
}
TD_TERMINAL;


extern int flagTerminal_newKey;
extern int flagTerminal_newString;
extern int flagTerminal_newTransmission;
extern int TerminalBufferItr;

void mfinit_terminal(TD_TERMINAL* term);
void mftick_terminal(TD_TERMINAL* term);
void mftask_terminal(TD_TERMINAL* term);

//wrapper für sprintf. erzeugt strings zum senden
void term_printf(TD_TERMINAL* term, const char *fmt, ...);
//cmd-string separieren und callbacks aufrufen
void term_lol_parse(char *str, TD_TERMINAL* term);
//funktionsnamen und deren namensstrings mit funktionspointern verknüpfen
//TODO:
void term_lol_setCallback	(const char* command,
							 const char *help,
							 const char *arg_names,
							 void(*cbf)(int argc,
							 const char **argv));

//obsolet
float term_lol_delay(int len);
//transferzeit für uart in us zurückgeben
int term_lol_txtime_us		(TD_TERMINAL* term);
//aufruf nach jeden neuen byte durch isr o.ä.
int  term_lol_readbyte		(TD_TERMINAL* term);
void term_lol_writebuff		(TD_TERMINAL* term);

void term_lol_vprint(const char *fmt, va_list argp, TD_TERMINAL term);
extern TD_TERMINAL cmdkeen;

#endif /* INC_TERMINAL_H_ */
