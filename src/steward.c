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

#include "conquest.h"

void check_steward(int cont, int type, player *enemy, player *plyptr)
{
	char cmd[32] = {0};
	char cmdstr[256] = {0};
	char *ptr = NULL;
	char tmpstr[256] = {0};
	int oldval;

	if(plyptr->validate != NOT_VAL && plyptr->validate != VAL_QUESTCHAR)
		return;
	if((strlen(plyptr->steward[cont][type])) < 1 || plyptr->level[cont] < 0)
		return;
	sprintf(message, "4338%c%s%c%s%c", MSGDEL, conts[cont].name, MSGDEL, plyptr->steward[cont][type], MSGDEL);
	log_event("", message, plyptr->name);
	oldval = plyptr->validate;
	plyptr->validate = VAL_STEWARD;
	plyptr->sock = sockets[sockout].sock;
	if(type == STEWARD_HK && plyptr->cont != cont)
	{
		plyptr->cont = cont;
		plyptr->city = conts[cont].port;
	}
	strcpy(tmpstr, plyptr->steward[cont][type]);
	ptr = strtok(tmpstr, ";");
	while(ptr != NULL)
	{
		memset(cmdstr, 0, strlen(cmdstr));
		strncpy(cmdstr, ptr, 128);
		if((strlen(cmdstr)) > 0)
		{
			parse_cmdstr(cmdstr, cont, enemy, plyptr);
			sscanf(cmdstr, "%31s", cmd);
			parser(cmd, plyptr->host, cmdstr + strlen(cmd) + 1, XMLTYPE, plyptr->nick);
		}
		ptr = strtok(NULL, ";");
	}
	plyptr->sock = INVALID_SOCKET;
	plyptr->validate = oldval;
}

void parse_cmdstr(char *cmdstr, int cont, player *enemy, player *plyptr)
{
	char list[][4] =
	{
		"%DN", "%MF", ""
	};
	char newcmd[128] = {0};
	char *ptr = NULL;
	int amount;
	int count = 0;
	int len;
	int total;
	int troopnum;

	strncpy(newcmd, cmdstr, 127);
	while(strlen(list[count]) > 0)
	{
		while((ptr = strstr(cmdstr, list[count])) != NULL)
		{
			len = ptr - cmdstr;
			strncpy(newcmd, cmdstr, len);
			newcmd[len] = 0;
			switch(count)
			{
				case 0:
					if(enemy)
						strcat(newcmd, enemy->name);
					break;
				case 1:
					for(amount = 0, total = 0, troopnum = 0; troopnum < MAXTRP; ++troopnum)
					{
						amount += plyptr->army[ARMY_DEFENSE][cont].total[troopnum] * units[plyptr->troop_type][troopnum].food;
						amount += plyptr->army[ARMY_CAMPAIGN][cont].total[troopnum] * units[plyptr->troop_type][troopnum].food;
					}
					if((total = plyptr->food[cont] - amount) < 0)
						total = 0;
					if(cities[plyptr->city].copper < total * cities[plyptr->city].food_buy)
						total = cities[plyptr->city].copper / cities[plyptr->city].food_buy;
					sprintf(newcmd, "%s%d", newcmd, total);
					break;
				default:
					break;
			}
			strcat(newcmd, cmdstr + len + strlen(list[count]));
			strcpy(cmdstr, newcmd);
			if(strlen(cmdstr) > 128)
				return;
		}
		++count;
	}
}

void show_steward(int cont, player *plyptr)
{
	char types[][16] = 
	{
		"HK", "Lose battle", "Win battle"
	};
	int count;

	sprintf(message, "4340%c%s%c%d%c%s%c", MSGDEL, conts[cont].name, MSGDEL, (plyptr->level[cont] + 1) * 5000, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4333%c%s%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = STEWARD_HK; count <= STEWARD_WIN; ++count)
	{
		sprintf(message, "4334%c%-16s%c%s%c", MSGDEL, types[count], MSGDEL, strlen(plyptr->steward[cont][count]) > 0?plyptr->steward[cont][count]:"NOT SET", MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void steward(char *cmd, char *cmdstr, player *plyptr)
{
	char cmds[][16] = 
	{
		"hk", "lose", "win", ""
	};
	char newcmd[32] = {0};
	char notallowed[][16] = 
	{
		"attack", "berserk", "cast", "charm", "crusade", 
		"espionage", "loot", "hk", "pestilence", "quest", "seek", 
		"slay", "summon", "use", ""
	};
	char *ptr = NULL;
	char tmpstr[128] = {0};
	char types[][16] = 
	{
		"HK", "Lose battle", "Win battle"
	};
	int cmdnum = 0;
	int cont = -1;
	int count = 0;

	if((strlen(cmd)) < 1 || ((cont = find_cont(cmd)) >= 0))
	{
		show_steward((cont < 0 || plyptr->level[cont] < 0)?plyptr->cont:cont, plyptr);
		return;
	}
	while((strlen(cmds[cmdnum])) > 0)
	{
		if((STRCASECMP(cmd, cmds[cmdnum])) == 0)
			break;
		++cmdnum;
	}
	if((strlen(cmds[cmdnum])) == 0)
	{
		sprintf(message, "4335%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_word(plyptr->name, cmdstr, 64, 0))
		return;
	if((strlen(cmdstr)) > 0 && ((strchr(cmdstr, ';')) == NULL || cmdstr[strlen(cmdstr) - 1] != ';'))
	{
		sprintf(message, "4339%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < STEWARD)
	{
		sprintf(message, "1%c%d%cto change your steward's orders%c", MSGDEL, STEWARD, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cmdnum == STEWARD_HK && strlen(cmdstr) > 0)
	{
		strcpy(tmpstr, cmdstr);
		ptr = strtok(tmpstr, ";");
		while(ptr != NULL)
		{
			sscanf(ptr, "%31s", newcmd);
			for(count = 0; strlen(notallowed[count]) > 0; ++count)
				if((STRNCASECMP(newcmd, notallowed[count], strlen(notallowed[count]))) == 0)
				{
					sprintf(message, "4336%c%s%c", MSGDEL, ptr, MSGDEL);
					send_message(message, plyptr->name, 1);
					return;
				}
			ptr = strtok(NULL, ";");
		}
	}
	plyptr->movepts -= STEWARD;
	strcpy(plyptr->steward[plyptr->cont][cmdnum], cmdstr);
	sprintf(message, "4337%c%s%c%s%c", MSGDEL, types[cmdnum], MSGDEL, strlen(cmdstr) > 0?cmdstr:"UN-SET", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void steward_hk(int cont, player *plyptr)
{
	int amount;
	int count;

	for(count = STEWARD_HK; count <= STEWARD_WIN; ++count)
		if((strlen(plyptr->steward[cont][count])) > 0)
			break;
	if(count > STEWARD_WIN)
		return;
	amount = (plyptr->level[cont] + 1) * 5000;
	if(plyptr->copper[cont] < amount)
	{
		for(count = STEWARD_HK; count <= STEWARD_WIN; ++count)
			memset(plyptr->steward[cont][count], 0, strlen(plyptr->steward[cont][count]));
		sprintf(message, "4341%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
	}
	else
	{
		plyptr->copper[cont] -= amount;
		sprintf(message, "4342%c%s%c%d%c%s%c", MSGDEL, conts[cont].name, MSGDEL, amount, MSGDEL, money_name, MSGDEL);
	}
	log_event("", message, plyptr->name);
}
