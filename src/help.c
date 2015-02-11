/*****************************************************************************

	Conquest! - The original IRC war game
	Copyright (C) 1993 James D. Bennett

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include"conquest.h"

void help(char *name, char *arg1, char *arg2, char *arg3)
{
	char cmds[][16] =
	{
		"abilities", "describe", "levels", "structures", "troops", ""
	};
	char dftfname[16] = {"help"};
	char dirname[16] = {"help"};
	char fname[256];
	char input[64];
	char *ptr = NULL;
	FILE *fp;
	int count;
	int len;
	int nohelp = 0;
	int plynum;

	len = strlen(arg1);
	for(count = 0; len > 0 && strlen(cmds[count]) > 0; ++count)
		if((STRNCASECMP(cmds[count], arg1, len)) == 0)
		{
			if((plynum = find_player(name)) < 0)
				break;
			switch(count)
			{
				case 0:
					abilities(arg2, &players[plynum]);
					return;
				case 1:
					strcpy(dftfname, "describe");
					strcpy(dirname, "describe");
					strcpy(arg1, arg2);
					break;
				case 2:
					levels(arg2, &players[plynum]);
					return;
				case 3:
					show_structures(arg2, &players[plynum]);
					return;
				case 4:
					troop_info(arg2, &players[plynum]);
					return;
			}
		}
	scroll_head(name);
	if(strlen(arg1) < 1 || strchr(arg1, '.') || strchr(arg1, '/') || strchr(arg1, '*'))
		strcpy(arg1, dftfname);
	if((ptr = strtolower(arg1)) == NULL)
		ptr = strdup("help");
	if(ptr == NULL)
		nohelp = 1;
	else
	{
		sprintf(fname, "%s/%s", dirname, ptr);
		if((fp = fopen(fname, "r")) == NULL)
		{
			sprintf(fname, "help/%s", dftfname);
			fp = fopen(fname, "r");
		}
		if(fp == NULL)
			nohelp = 1;
		else
		{
			while((fgets(input, 59, fp)) != NULL)
			{
				strip(input);
				sprintf(message, "20004%c%-58s%c", MSGDEL, input, MSGDEL);
				send_message(message, name, 0);
			}
			fclose(fp);
		}
	}
	if(nohelp == 1)
	{
		sprintf(message, "20004%c%-58s%c", MSGDEL, "No Help Available.", MSGDEL);
		send_message(message, name, 0);
	}
	scroll_tail(name);
	if(ptr)
		free(ptr);
}

void scroll_head(char *name)
{
	sprintf(message, "20000%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "20001%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "20002%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "20003%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void scroll_tail(char *name)
{
	sprintf(message, "20005%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "20006%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}
