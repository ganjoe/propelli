/*
 * command.h
 *
 *  Created on: Nov 15, 2020
 *      Author: danie
 */

#ifndef PROPELLI_COMMAND_H_
#define PROPELLI_COMMAND_H_

void Command_init();

//	config
void reset(int argc, const char **argv);
void selterm(int argc, const char **argv);
void settime(int argc, const char **argv);
void setdate(int argc, const char **argv);
//	gpios setzen
void setword(int argc, const char **argv);
void writepin(int argc, const char **argv);
void readpin(int argc, const char **argv);
void setallin(int argc, const char **argv);
void setword(int argc, const char **argv);
// neue logdatei, filename ist parameter oder timestring
void sdwrite(int argc, const char **argv);
void newlog(int argc, const char **argv);
// zeile aus init-datei drucken/schreiben/ausführen
void sdread(int argc, const char **argv);
void writeconf(int argc, const char **argv);
void readconf(int argc, const char **argv);

#endif /* PROPELLI_COMMAND_H_ */
