
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

void disband(char *typestr, char *unit, int amount, player *plyptr)
{
	int count;
	int type = ARMY_DEFENSE;

	if(amount == 0)
	{
		help(plyptr->name, "disband", "", "");
		return;
	}
	if((strlen(unit)) < 1)
	{
		show_disband(plyptr);
		return;
	}
	for(count = 0; count < MAXTRP; ++count)
		if((STRCASECMP(unit, units[plyptr->troop_type][count].name)) == 0)
			break;
	if(count == MAXTRP)
	{
		show_disband(plyptr);
		return;
	}
	if((tolower(typestr[0])) == 'c')
		type = ARMY_CAMPAIGN;
	if(amount > plyptr->army[type][plyptr->cont].total[count])
	{
		sprintf(message, "12000%c%s%c%d%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, plyptr->army[type][plyptr->cont].total[count], MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, army_names[type], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->army[type][plyptr->cont].total[count] -= amount;
	sprintf(message, "12001%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, army_names[type], MSGDEL);
	send_message(message, plyptr->name, 1);
}

void hire(char *name, int amount, player *plyptr)
{
	int count;
	int type;
	int num;

	if((strlen(name)) < 1)
	{
		show_hire(plyptr->city, plyptr);
		return;
	}
	for(count = 0; count < MAXMEC; ++count)
	{
		if(restrict_markets == 0)
			type = plyptr->troop_type;
		else
			type = cities[plyptr->city].merc_type[count];
		if((STRCASECMP(name, units[type][cities[plyptr->city].merc_num[count]].name)) == 0)
			break;
	}
	if(count == MAXMEC)
	{
		show_hire(plyptr->city, plyptr);
		return;
	}
	if(plyptr->movepts < HIRE)
	{
		sprintf(message, "1%c%d%cto hire any mercenaries%c", MSGDEL, HIRE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	num = cities[plyptr->city].merc_num[count];
	if(restrict_markets == 1 && plyptr->troop_type != cities[plyptr->city].merc_type[count])
	{
		sprintf(message, "12002%c%s%c", MSGDEL, units[cities[plyptr->city].merc_type[count]][num].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount <= 0 || amount > cities[plyptr->city].merc_qty[count])
	{
		sprintf(message, "12003%c%s%c%d%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, cities[plyptr->city].merc_qty[count], MSGDEL, units[plyptr->troop_type][num].name, MSGDEL, cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(units[plyptr->troop_type][num].copper * amount > plyptr->copper[plyptr->cont])
	{
		sprintf(message, "12004%c%s%c%d%c%s%c%s%c%d%c%d%c%s%c", MSGDEL, 
		money_name, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][num].name, MSGDEL, units[plyptr->troop_type][num].name, MSGDEL, units[plyptr->troop_type][num].copper, MSGDEL, plyptr->copper[plyptr->cont], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(combined_army(plyptr->cont, plyptr) + amount > plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum))
	{
		sprintf(message, "31%c%s%c%d%c%d%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum), MSGDEL, combined_army(plyptr->cont, plyptr), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	cities[plyptr->city].merc_qty[count] -= amount;
	plyptr->copper[plyptr->cont] -= units[plyptr->troop_type][num].copper * amount;
	plyptr->movepts -= HIRE;
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[num] += amount;
	sprintf(message, "12005%c%d%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][num].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "12006%c%s%c", MSGDEL, units[plyptr->troop_type][num].desc, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void show_disband(player *plyptr)
{
	char num_attacks[8] = {0};
	char unit_info[64] = {0};
	int count;

	sprintf(message, "12007%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	strcpy(message, "");
	for(count = 0; count < MAXTRP; ++count)
	{
		if((strlen(units[plyptr->troop_type][count].name)) < 1)
			continue;
		sprintf(num_attacks, ",x%d", units[plyptr->troop_type][count].num_attacks);
		sprintf(unit_info, "%ss (%d/%d%s%s%s%s%s)", 
		units[plyptr->troop_type][count].name, units[plyptr->troop_type][count].attack, units[plyptr->troop_type][count].defend, 
		units[plyptr->troop_type][count].num_attacks > 1?num_attacks:"", 
		units[plyptr->troop_type][count].ambush == 1?",amb":"", units[plyptr->troop_type][count].range == 1?",rng":"", 
		units[plyptr->troop_type][count].regenerate == 1?",regen":"", 
		units[plyptr->troop_type][count].swarm == 1?",swarm":"");
		sprintf(message, "12008%c%-30s%c%d%c%s%c%d%c%s%c", MSGDEL, 
		unit_info, MSGDEL, units[plyptr->troop_type][count].food, MSGDEL, food_name, MSGDEL, units[plyptr->troop_type][count].copper, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		strcpy(unit_info, "");
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void show_hire(int ctynum, player *plyptr)
{
	char num_attacks[8] = {0};
	char unit_info[64] = {0};
	int count;
	int num;
	int type;

	sprintf(message, "12009%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	strcpy(message, "");
	for(count = 0; count < MAXMEC; ++count)
	{
		if(restrict_markets == 0)
			type = plyptr->troop_type;
		else
			type = cities[ctynum].merc_type[count];
		num = cities[ctynum].merc_num[count];
		if(num < MERC)
			continue;
		if((strlen(units[type][num].name)) < 1)
			continue;
		sprintf(num_attacks, ",x%d", units[type][num].num_attacks);
		sprintf(unit_info, "%ss (%d/%d%s%s%s%s%s)", 
		units[type][num].name, units[type][num].attack, units[type][num].defend, 
		units[type][num].num_attacks > 1?num_attacks:"", 
		units[type][num].ambush == 1?",amb":"", units[type][num].range == 1?",rng":"", 
		units[type][num].regenerate == 1?",regen":"", units[type][num].swarm == 1?",swarm":"");
		sprintf(message, "12008%c%-30s%c%d%c%s%c%d%c%s%c", MSGDEL,
		unit_info, MSGDEL, units[type][num].food, MSGDEL, food_name, MSGDEL, units[type][num].copper, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		strcpy(unit_info, "");
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}
