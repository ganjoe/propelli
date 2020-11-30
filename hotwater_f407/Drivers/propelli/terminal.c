/*
 * terminal.c
 *
 *  Created on: Jul 7, 2020
 *      Author: danie
 *      vesc fork
 *
 */

#include "terminal.h"
#include "usart.h"
#include "datatypes.h"
#include "delay.h"

// TODO: formalisieren und aufräumen


void mfinit_terminal(TD_TERMINAL* term)
{
	modflag_init(&term->mf_cmd, HALTICK, 1);
	term->callback_len = 40;
	term->maxArguments = 4;
	term->buffer_receive_len = 20;
	term->uart_buffer_tx_len = 64;
	term->huart =huart1;
	term->string_rx = malloc(term->buffer_receive_len);
	term->string_tx = malloc(term->uart_buffer_tx_len);
	term->sep = (char*)10;
	term->eoc = (char*)13;
}
void mftask_terminal(TD_TERMINAL* term)
	{
    if (&term->mf_cmd.flag && term->mf_cmd.init_done)
		{
    	term->mf_cmd.repeat = modflag_tickdiff(&term->mf_cmd);

		term_lol_readbyte(&cmdkeen);

		term->mf_cmd.duration = modflag_tickdiff(&term->mf_cmd);
		term->mf_cmd.callcount++;
		term->mf_cmd.flag = false;
		}
	}
void mftick_terminal(TD_TERMINAL* term)
{
	modflag_upd_regular(&term->mf_cmd);
}

#define CALLBACK_LEN		40
//empfangspuffer für uart-dma
#define TERM_BUFF_RX_LEN 	20
int flagTerminal_newTransmission;
enum    {    kMaxArgs = 4    };

char TerminalCharBufferRx[TERM_BUFF_RX_LEN];
char *strptr;
typedef struct td_callbacks terminal_callback_struct;
extern TIM_HandleTypeDef htim1;

static volatile int fault_vec_write = 0;
static terminal_callback_struct callbacks[CALLBACK_LEN];
static int callback_write = 0;

float term_lol_delay(int len)
    {
    int halbaudrate = huart1.Init.BaudRate;
    float Txmillis = 10000000 * (float) len / (float) halbaudrate;	//transmission time[s]: 10 bit / n baud
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

void term_lol_parse(char *str, TD_TERMINAL* term)
    {
    int argc = 0;
    char *argv[term->maxArguments];
    //cmd ist der erste stringabschnitt von links
    char *p2 = strtok(str, " ");
    //argumente separieren, durchzählen, und in ptr-array speichern
    while (p2 && argc < term->maxArguments)
		{
		argv[argc++] = p2;
		p2 = strtok(0, " ");
		}
    if (argc == 0)
		{
		term_printf(&cmdkeen, "No command received\n");
		return;
		}
    if (strcmp(argv[0], "help") == 0)
	{
	term_printf(&cmdkeen, "registered commands:\n");

	for (int i = 0; i < callback_write; i++)
	    {
	    //TODO: blocking variante von ..print mit dma, flag, berechnete transferzeit
	    term_printf(&cmdkeen, callbacks[i].command);
	    term_printf(&cmdkeen, " help: ");
	    term_printf(&cmdkeen, callbacks[i].help);

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
void term_lol_vprint(const char *fmt, va_list argp)
    {
    static char string[120]={0};
    if (flagTerminal_newTransmission)
	{
	if (0 < vsprintf(string, fmt, argp))
	    {
		//term_lol_writebuff(term);
	    int len = strlen(string);
	    float del;
	    //HAL_UART_Transmit(&huart1, (uint8_t*)string, len,199);
	    //HAL_UART_Transmit_IT(&huart1, (uint8_t*)string, len);
	    flagTerminal_newTransmission = false;
	    HAL_UART_Transmit_DMA(&huart1, (uint8_t*) string, len);
	    //TODO: mit mf delay austauschen
	    del = term_lol_delay(len);
	    //HAL_Delay(del);
	    //delay_us(&delay, (uint32_t)del);


	    }
	}
    }
void term_printf(TD_TERMINAL* term, const char *fmt, ...) // custom printf() function
    {
    //http://openbook.rheinwerk-verlag.de/c_von_a_bis_z/018_c_stdarg_h_001.htm
    va_list argp;
    va_start(argp, fmt);
    term_lol_vprint(fmt, argp);
    //term_lol_writebuff
    va_end(argp);
    }
//TODO: für utils_ formalisieren
int term_lol_searchstring	(TD_TERMINAL* term)
    {
	char* charptr;

	charptr = memchr(term->string_rx, (int)term->eoc, term->TerminalBufferItr);

	return *charptr;
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
    cmdkeen.string_rx[cmdkeen.TerminalBufferItr] = cmdkeen.byte_received;

	if (cmdkeen.TerminalBufferItr < cmdkeen.buffer_receive_len)
		{
		cmdkeen.TerminalBufferItr++	;
		}
	else
		{
		cmdkeen.TerminalBufferItr=0;
		HAL_Delay(100);
		}

	if (cmdkeen.byte_received == 13)
		{
		term_lol_parse(cmdkeen.string_rx, &cmdkeen);
		cmdkeen.TerminalBufferItr = 0;
		}
	term_lol_readbyte(&cmdkeen);

    }
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
    {

    }

TD_TERMINAL cmdkeen;
