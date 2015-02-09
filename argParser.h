/**
 * @file argParser.h
 *
 * @date 12.09.2014
 * @author Roy Freytag
 */

#ifndef ARGPARSER_H_
#define ARGPARSER_H_

#include "list.h"
#include "stack.h"

/**
 * structure for command-line switches to be parsed
 */
typedef struct
{
	char name; ///< short-hand name(only one character)
	char *lname; ///< long name
	int switched; ///< when name or lname is found among the arguments, this will be switched to 1
} ArgSwitch_t;

/**
 * structure for command-line parameters to be parsed
 */
typedef struct
{
	char name; ///< @see ArgSwitch_t
	char *lname; ///< @see ArgSwitch_t
	char *value; ///< will hold the pointer to the next element in the argv array after the found argument if it doesn't start with -
} ArgParam_t;

/**
 * struct for command-line arguments that are not defined
 */
typedef struct
{
	char *value;
} ArgLoose_t;

/**
 * struct for the parsed and unparsed arguments
 */
typedef struct ArgList
{
	Stack_t *argv; ///< stack of all arguments in the argv array to be parsed
	int argc; ///< argument count
	int dirty; ///< set to 1 if there are undefined arguments
	List_t *switchReg; ///< register of all switches
	List_t *argReg; ///< register of all parameters
	List_t *lReg; ///< list of loose arguments
} ArgList_t;

ArgList_t   *arg_initArgs(int argc, char **argv);
int         arg_parseArgs(ArgList_t *arg);
void        arg_destroyArgs(ArgList_t *arg);

ArgSwitch_t *arg_addSwitch(ArgList_t *arg, char name, char *lname);
ArgParam_t  *arg_addParam(ArgList_t *arg, char name, char *lname);

void        arg_destroySwitch(ArgSwitch_t *sw);
void        arg_destroyParam(ArgParam_t *parm);

int         arg_getLooseCount(ArgList_t *arg);
char        *arg_getLoose(ArgList_t *arg, int idx);

/*
int         arg_isSwitched(ArgList_t *arg, char sw, char *lsw);
char        *arg_getArgumentForStr(ArgList_t *arg, char args, char *largs);
char        *arg_getArgument(ArgList_t *arg, int i);
*/



#endif /* ARGPARSER_H_ */
