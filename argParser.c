/*
 * argParser.c
 *
 *  Created on: 12.09.2014
 *      Author: pfannkuchen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "argParser.h"

ArgList_t *arg_initArgs(int argc, char **argv)
{
	int i;
	ArgList_t *tmp = malloc(sizeof(ArgList_t));
	tmp->argc = argc;
	tmp->dirty = 0;
	tmp->argv = stk_createStack();

	for(i = argc-1; i >= 1; i--)
	{
		stk_push(tmp->argv, argv[i]);
	}
	tmp->argReg = lst_createList();
	tmp->switchReg = lst_createList();
	tmp->lReg = lst_createList();

	return tmp;
}

int argSwStrCmp(void *a, void *b)
{
	ArgSwitch_t *rs = (ArgSwitch_t*)a;
	if(!rs->lname) return -1;
	char *needle = b;
	if(!rs->lname) return 0;
	return strcmp(rs->lname, needle);
}

int argSwCharCmp(void *a, void *b)
{
	ArgSwitch_t *rs = (ArgSwitch_t*)a;
	char needle = *((char*)b);
	if(needle > rs->name)
		return -1;
	else if(needle < rs->name)
		return 1;
	else
		return 0;
}

int argParmStrCmp(void *a, void *b)
{
	ArgParam_t *rs = (ArgParam_t*)a;
	if(!rs->lname) return -1;
	char *needle = (char*)b;
	if(!rs->lname) return 0;
	return strcmp(rs->lname, needle);
}

int argParmCharCmp(void *a, void *b)
{
	ArgParam_t *rs = (ArgParam_t*)a;
	char needle = *((char*)b);
	if(needle > rs->name)
		return -1;
	else if(needle < rs->name)
		return 1;
	else
		return 0;
}

int			arg_parseArgs(ArgList_t *arg)
{
	if(!arg) return 0;
	int i;
	ArgSwitch_t *sw;
	ArgParam_t *pm;
	//ArgLoose_t *lo;
	char *carg;
	int carglen;
	char *cparm;
	while(arg->argv->stackSize)
	{
		carg = (char*)stk_pop(arg->argv);
		carglen = strlen(carg);
		if(carglen < 2) continue;

		if(carg[0] == '-' && carg[1] == '-')
		{
			if((sw = lst_getMatching(arg->switchReg, carg+2, argSwStrCmp)))
			{
				sw->switched = 1;
			}
			else if((pm = lst_getMatching(arg->argReg, carg+2, argParmStrCmp)))
			{
				cparm = stk_pop(arg->argv);
				if(cparm[0] == '-')
				{
					stk_push(arg->argv, cparm);
					continue;
				}
				pm->value = cparm;
			}
		}
		else if(carg[0] == '-')
		{
			if(carglen == 2)
			{
				if((sw = lst_getMatching(arg->switchReg, &(carg[1]), argSwCharCmp)))
				{
					sw->switched = 1;
				}
				else if((pm = lst_getMatching(arg->argReg, &(carg[1]), argParmCharCmp)))
				{
					cparm = stk_pop(arg->argv);
					if(cparm[0] == '-')
					{
						stk_push(arg->argv, cparm);
						continue;
					}
					pm->value = cparm;
				}
			}
			else
			{
				for(i = 1; i < carglen; i++)
				{
					if((sw = lst_getMatching(arg->switchReg, &(carg[i]), argSwCharCmp)))
					{
						sw->switched = 1;
					}
				}
			}
		}
		else
		{
			ArgLoose_t *tmp = malloc(sizeof(ArgLoose_t));
			tmp->value = carg;
			lst_insertTail(arg->lReg, tmp);
			arg->dirty = 1;
		}
	}

	return 1;
}

void arg_destroyArgs(ArgList_t *arg)
{
	//int i;
	ArgLoose_t *ltmp;
	ArgParam_t *ptmp;
	ArgSwitch_t *stmp;
	while((ltmp = lst_getFirst(arg->lReg)))
	{
		free(ltmp);
		lst_removeItem(arg->lReg);
	}

	while((ptmp = lst_getFirst(arg->argReg)))
	{
		arg_destroyParam(ptmp);
		lst_removeItem(arg->argReg);
	}

	while((stmp = lst_getFirst(arg->switchReg)))
	{
		arg_destroySwitch(stmp);
		lst_removeItem(arg->switchReg);
	}

	stk_destroyStack(arg->argv);
	lst_deleteList(arg->argReg);
	lst_deleteList(arg->lReg);
	lst_deleteList(arg->switchReg);
	free(arg);
}

int argsw_cmp(void* a, void* b)
{
	ArgSwitch_t *aa = (ArgSwitch_t*)a, *bb = (ArgSwitch_t*)b;
	return (aa->name > bb->name);
}

int argparm_cmp(void* a, void* b)
{
	ArgParam_t *aa = (ArgParam_t*)a, *bb = (ArgParam_t*)b;
	return (aa->name > bb->name);
}

ArgSwitch_t *arg_addSwitch(ArgList_t *arg, char name, char *lname)
{
	ArgSwitch_t *tmp = malloc(sizeof(ArgSwitch_t));
	tmp->name = name;
	tmp->lname = malloc(strlen(lname)+1);
	strcpy(tmp->lname, lname);
	tmp->switched = 0;

	if(!lst_addItemToList(arg->switchReg, tmp, argsw_cmp)) return NULL;
	else return tmp;
}

void arg_destroySwitch(ArgSwitch_t *sw)
{
	if(sw->lname) free(sw->lname);
	free(sw);
}

void arg_destroyParam(ArgParam_t *parm)
{
	if(parm->lname) free(parm->lname);
	//if(parm->value) free(parm->value);
	free(parm);
}

ArgParam_t  *arg_addParam(ArgList_t *arg, char name, char *lname)
{
	ArgParam_t *tmp = malloc(sizeof(ArgParam_t));
	tmp->name = name;
	tmp->lname = malloc(strlen(lname)+1);
	strcpy(tmp->lname, lname);
	tmp->value = 0;

	if(!lst_addItemToList(arg->argReg, tmp, argparm_cmp)) return NULL;
	else return tmp;
}

int         arg_getLooseCount(ArgList_t *arg)
{
	return lst_getNodeCount(arg->lReg);
}

char        *arg_getLoose(ArgList_t *arg, int idx)
{
	if(idx < 0) return NULL;
	ArgLoose_t *tmp = lst_getIndexed(arg->lReg, idx);
	if(!tmp) return NULL;
	return tmp->value;
}

/*
int arg_isSwitched(ArgList_t *arg, char sw, char *lsw)
{
	if(!arg) return 0;
	if(!lsw) return 0;

	int i, j;
	int al;
	for(i = 1; i < arg->argc; i++)
	{
		al = strlen(arg->argv[i]);
		if(al < 2) continue;
		if(arg->argv[i][0] == '-' && arg->argv[i][1] == '-' && lsw)
		{
			if(!strncmp((arg->argv[i])+2, lsw, strlen(lsw)))
				return i;
			else return 0;
		}
		else if(arg->argv[i][0] == '-' && arg->argv[i][1] != '-')
		{
			for(j = 1; j < al; j++)
			{
				if(arg->argv[i][j] == sw) return i;
			}
		}
	}

	return 0;
}
char *arg_getArgumentForStr(ArgList_t *arg, char args, char *largs)
{
	if(!arg) return NULL;
	if(!args && !largs) return NULL;
	int n = arg_isSwitched(arg, args, largs);
	if(n < (arg->argc-1))
	{
		if(arg->argv[n+1][0] != '-')
		{
			return arg->argv[n+1];
		}
	}
	return NULL;
}
char *arg_getArgument(ArgList_t *arg, int i)
{
	if(!arg || (arg && i >= arg->argc)) return NULL;
	return arg->argv[i];
}
*/

