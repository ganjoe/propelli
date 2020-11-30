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

typedef struct td_callbacks
	{
	const char *command;
	const char *help;
	const char *arg_names;
	void (*cbf)(int argc, const char **argv);
	} terminal_callback_struct;

typedef struct
{
	int callback_len, buffer_receive_len, maxArguments;
	char byte_received, TerminalBufferItr;
	char* bytestring_received;
	int flag_newString, flag_newTransmission;
	modflag mf_term;
	terminal_callback_struct td_callbacks;
	UART_HandleTypeDef huart;

}
TD_TERMINAL;

extern int flagTerminal_newKey;
extern int flagTerminal_newString;
extern int flagTerminal_newTransmission;
extern int TerminalBufferItr;

mfinit_terminal(TD_TERMINAL* term);
mftick_terminal(TD_TERMINAL* term);
mftask_terminal(TD_TERMINAL* term);

//wrapper für sprintf. erzeugt strings zum senden
void term_printf(TD_TERMINAL* term, const char *fmt, ...);
//cmd-string separieren und callbacks aufrufen
void term_lol_parse			(char *str);
//funktionsnamen und deren namensstrings mit funktionspointern verknüpfen
//TODO:
void term_lol_setCallback	(const char* command,
							 const char *help,
							 const char *arg_names,
							 void(*cbf)(int argc,
							 const char **argv));
//empfangene bytes auf cmd-string prüfen
void term_lol_searchstring	(TD_TERMINAL* term);
//obsolet
float term_lol_delay(int len);
//transferzeit für uart in us zurückgeben
int term_lol_txtime_us		(TD_TERMINAL* term);
//aufruf nach jeden neuen byte durch isr o.ä.
int term_lol_readbyte		(TD_TERMINAL* term);
void term_lol_writebuff		(TD_TERMINAL* term);
extern TD_TERMINAL cmdkeen;

#endif /* INC_TERMINAL_H_ */
