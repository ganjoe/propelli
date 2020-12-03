/*
 * terminal.c
 *
 *  Created on: Jul 7, 2020
 *      Author: daniel
 *
 */

#include "terminal.h"
#include "usart.h"
#include "datatypes.h"
#include "delay.h"

// TODO: formalisieren und aufräumen


static TD_TERMINAL_CALLBACKS callbacks[CALLBACK_LEN];
static int callback_write = 0;

void mfinit_terminal(TD_TERMINAL* term)
{
	modflag_init(&term->mf_cmd, HALTICK, 1);
	term->callback_len = 40;
	term->maxArguments = 4;
	term->uart_buffer_rx_len = 32;
	term->uart_buffer_tx_len = 128;
	term->string_rx = calloc(term->uart_buffer_rx_len, 1);
	term->string_tx = calloc(term->uart_buffer_tx_len, 1);
	term->sep  = strdup(" ");
	term->eoc = (char*)13;

}
void mftask_terminal(TD_TERMINAL* term)
	{
    if (&term->mf_cmd.flag && term->mf_cmd.init_done)
		{
    	term->mf_cmd.repeat = modflag_tickdiff(&term->mf_cmd);

		term_lol_readbyte(&btTerm);

		term->mf_cmd.duration = modflag_tickdiff(&term->mf_cmd);
		term->mf_cmd.callcount++;
		term->mf_cmd.flag = false;
		}
	}
void mftick_terminal(TD_TERMINAL* term)
{
modflag_upd_regular(&term->mf_cmd);
}
void term_printf	(TD_TERMINAL* term, const char *fmt, ...)
    {
    //http://openbook.rheinwerk-verlag.de/c_von_a_bis_z/018_c_stdarg_h_001.htm
    va_list argp;
    va_start(argp, fmt);
    term_lol_vprint(fmt, argp, term);
    va_end(argp);
    }

//empfangspuffer für uart-dma


float term_lol_delay(int len)
    {
    int halbaudrate = huart1.Init.BaudRate;
    float Txmillis = 1024 * (float) len / (float) halbaudrate;	//transmission time[s]: 10 bit / n baud
    return Txmillis;
    }
void term_lol_setCallback(const char *command, const char *help,
	const char *arg_names, void (*cbf)(int argc, const char **argv))
    {
    int callback_num = callback_write;
    for (int i = 0; i < callback_write; i++)
	{
	// First check the address in case the same callback is registered more than once.
	if (callbacks[i].command == command)
	    {
	    callback_num = i;
	    break;
	    }
	// Check by string comparison.
	if (strcmp(callbacks[i].command, command) == 0)
	    {
	    callback_num = i;
	    break;
	    }
	// Check if the callback is empty (unregistered)
	if (callbacks[i].cbf == 0)
	    {
	    callback_num = i;
	    break;
	    }
	}
    callbacks[callback_num].command = command;
    callbacks[callback_num].help = help;
    callbacks[callback_num].arg_names = arg_names;
    callbacks[callback_num].cbf = cbf;

    if (callback_num == callback_write)
	{
	callback_write++;
	if (callback_write >= CALLBACK_LEN)
	    {
	    callback_write = 0;
	    }
	}
    }

void term_lol_parse(TD_TERMINAL* term)
    {
    int argc = 0;
    char *argv[term->maxArguments];
    //cmd ist der erste stringabschnitt von links
    char *p2 = strtok(term->string_rx, term->sep);
    //argumente separieren, und in ptr-array speichern
    while (p2 && argc < term->maxArguments)
		{
		argv[argc++] = p2;
		//sachen passieren
		p2 = strtok(0,term->sep);
		}
    if (argc == 0)
		{
		term_printf(term, "No command received\n");
		return;
		}
    if (strcmp(argv[0], "help") == 0)
	{
	term_printf(term, "registered commands:\n");

	for (int i = 0; i < callback_write; i++)
	    {
	    term_printf(term, callbacks[i].command);
	    term_printf(term, " help: ");
	    term_printf(term, callbacks[i].help);

	    }

	}

    for (int i = 0; i < callback_write; i++)
	{
	if (callbacks[i].cbf != 0 && strcmp(argv[0], callbacks[i].command) == 0)
	    {
	    callbacks[i].cbf(argc, (const char**) argv);
	    return;
	    }
	}
    }
void term_lol_vprint(const char *fmt, va_list argp, TD_TERMINAL* term)
    {
	HAL_StatusTypeDef stat;
	int txlen = strlen(fmt);
	utils_truncate_number_int(&txlen, 1,term->uart_buffer_tx_len);
	uint8_t* localbuff = calloc(txlen,1);
	//uint8_t localbuff[txlen];

	if (0 < vsprintf(localbuff, fmt, argp))
	    {
	    float del;
	    //TODO: auf abschluss vorhandener übertragung warten
	    stat = HAL_UART_Transmit_DMA(&huart1, localbuff, txlen);
	    del = term_lol_delay(txlen)*1000;
	    delay_us(&delay, (uint32_t)del);

	    }
	free (localbuff);
    }

void term_lol_writebuff		(TD_TERMINAL* term)
	{
	int strlen;
	strlen =sizeof(term->string_tx);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*) term->string_tx, strlen);
	}
int  term_lol_readbyte		(TD_TERMINAL* term)
{
	//isr scharfschalten für byteempfang
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)&term->byte_received, 1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
    btTerm.string_rx[btTerm.TerminalBufferItr] = btTerm.byte_received;

	if (btTerm.TerminalBufferItr < btTerm.uart_buffer_rx_len)
		{
		btTerm.TerminalBufferItr++	;
		}
	else
		{
		btTerm.TerminalBufferItr=0;
		//delay_us(&delay,100);
		}

	if (btTerm.byte_received == 13)
		{
		term_lol_parse(&btTerm);
		btTerm.TerminalBufferItr = 0;
		}
	term_lol_readbyte(&btTerm);
    }

TD_TERMINAL btTerm;
