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
//
void sdwrite(int argc, const char **argv);
void sdread(int argc, const char **argv);
void readinit(int argc, const char **argv);
void writeinit(int argc, const char **argv);
void clearinit(int argc, const char **argv);
void nlog(int argc, const char **argv);
void nlogn(int argc, const char **argv);
// zeile aus init-datei drucken/schreiben/ausführen


#endif /* PROPELLI_COMMAND_H_ */
