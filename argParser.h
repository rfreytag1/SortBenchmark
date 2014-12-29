/*
 * argParser.h
 *
 *  Created on: 12.09.2014
 *      Author: pfannkuchen
 */

#ifndef ARGPARSER_H_
#define ARGPARSER_H_

#include "list.h"
#include "stack.h"

typedef struct
{
	char name;
	char *lname;
	int switched;
} ArgSwitch_t;

typedef struct
{
	char name;
	char *lname;
	char *value;
} ArgParam_t;

typedef struct
{
	char *value;
} ArgLoose_t;




typedef struct ArgList
{
	Stack_t *argv;
	int argc;
	int dirty;
	List_t *switchReg;
	List_t *argReg;
	List_t *lReg;
} ArgList_t;

ArgList_t   *arg_initArgs(int argc, char **argv);
int			arg_parseArgs(ArgList_t *arg);
void        arg_destroyArgs(ArgList_t *arg);

ArgSwitch_t *arg_addSwitch(ArgList_t *arg, char name, char *lname);
ArgParam_t  *arg_addParam(ArgList_t *arg, char name, char *lname);

void arg_destroySwitch(ArgSwitch_t *sw);
void arg_destroyParam(ArgParam_t *parm);

int         arg_getLooseCount(ArgList_t *arg);
char        *arg_getLoose(ArgList_t *arg, int idx);

/*
int         arg_isSwitched(ArgList_t *arg, char sw, char *lsw);
char        *arg_getArgumentForStr(ArgList_t *arg, char args, char *largs);
char        *arg_getArgument(ArgList_t *arg, int i);
*/



#endif /* ARGPARSER_H_ */
