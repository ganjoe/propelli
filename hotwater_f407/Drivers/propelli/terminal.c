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


#define CALLBACK_LEN		40
#define TERM_BUFF_RX_LEN 20
enum    {    kMaxArgs = 4    };

char TerminalCharBufferRx[TERM_BUFF_RX_LEN];
char *strptr;
typedef struct _terminal_callback_struct terminal_callback_struct;
extern TIM_HandleTypeDef htim1;

static volatile int fault_vec_write = 0;
static terminal_callback_struct callbacks[CALLBACK_LEN];
static int callback_write = 0;

int flagTerminal_newString = 0;
int flagTerminal_newTransmission = 2;


float _calc_uart_tx_time(int len)
    {
    int halbaudrate = huart1.Init.BaudRate;
    float Txmillis = 10000000 * (float) len / (float) halbaudrate;	//transmission time[s]: 10 bit / n baud
    return Txmillis;
    }

void terminal_register_command_callback(const char *command, const char *help,
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
void terminal_unregister_callback(void (*cbf)(int argc, const char **argv))
    {
    for (int i = 0; i < callback_write; i++)
	{
	if (callbacks[i].cbf == cbf)
	    {
	    callbacks[i].cbf = 0;
	    }
	}
    }
void terminal_process_string(char *str)
    {
    int argc = 0;	//argument count
    char *argv[kMaxArgs];
    char *p2 = strtok(str, " ");	//argument seperator.
    while (p2 && argc < kMaxArgs)
		{
		argv[argc++] = p2;
		p2 = strtok(0, " ");
		}
    if (argc == 0)
		{
		commands_printf("No command received\n");
		return;
		}
    if (strcmp(argv[0], "help") == 0)
	{
	commands_printf("registered commands:\n");

	for (int i = 0; i < callback_write; i++)
	    {
	    //TODO: blocking variante von ..print mit dma, flag, berechnete transferzeit
	    commands_printf(callbacks[i].command);
	    commands_printf(" help: ");
	    commands_printf(callbacks[i].help);

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
void vprint(const char *fmt, va_list argp)
    {
    static char string[120]={0};
    if (flagTerminal_newTransmission)
	{
	if (0 < vsprintf(string, fmt, argp))
	    {
	    int len = strlen(string);
	    float del;
	    //HAL_UART_Transmit(&huart1, (uint8_t*)string, len,199);
	    //HAL_UART_Transmit_IT(&huart1, (uint8_t*)string, len);
	    flagTerminal_newTransmission = false;
	    HAL_UART_Transmit_DMA(&huart1, (uint8_t*) string, len);
	    //TODO: mit mf delay austauschen
	    del = _calc_uart_tx_time(len);
	    //HAL_Delay(del);
	    //delay_us(&delay, (uint32_t)del);


	    }
	}
    }
void commands_printf(const char *fmt, ...) // custom printf() function
    {
    //http://openbook.rheinwerk-verlag.de/c_von_a_bis_z/018_c_stdarg_h_001.htm
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
    }
void task_terminalGetKey()
    {

    if (!flagTerminal_newString)
	{
	HAL_UART_Receive_DMA(&huart1, TerminalCharBufferRx, TERM_BUFF_RX_LEN);
	}
    if (flagTerminal_newString) 	// string is terminated by cr
	{
	flagTerminal_newString = false;
	terminal_process_string(strptr);
	}
    }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
    strptr = strtok(TerminalCharBufferRx, "\n");
    flagTerminal_newString = true;

    }
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
    {
    flagTerminal_newTransmission++;
    }
