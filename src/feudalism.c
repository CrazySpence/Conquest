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

void add_vassal(int lord, player *plyptr)
{
	int count;
	player *lordptr;

	if(lord < 1)
		return;
	lordptr = &players[lord];
	for(count = 0; count < MAXVAS; ++count)
		if(lordptr->vassals.plynum[count] == 0)
		{
			lordptr->vassals.plynum[count] = playernum(plyptr);
			++lordptr->vassals.total;
			sprintf(message, "7000%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, lordptr->kname[plyptr->cont], MSGDEL);
			send_message(message, lordptr->name, 1);
			log_event("", message, lordptr->name);
			save_player(lord);
			break;
		}
}

int attack_vassal(player *plyptr, player *target)
{
	int plynum;

	plynum = playernum(target);
	if(plyptr->lord > 0)
	{
		if(plyptr->lord == plynum)
		{
			sprintf(message, "7001%c%s%c", MSGDEL, adjectives[target->gender][4], MSGDEL);
			send_message(message, plyptr->name, 1);
			return(0);
		}
		if(plyptr->lord == target->lord)
		{
			sprintf(message, "7002%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, adjectives[players[plyptr->lord].gender][4], MSGDEL, adjectives[players[plyptr->lord].gender][0], MSGDEL);
			send_message(message, plyptr->name, 1);
			return(0);
		}
	}
	if(is_vassal(plyptr, &players[plynum]))
		{
			sprintf(message, "7003%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return(0);
		}
	return(1);
}

void check_vassals(player *plyptr)
{
	int count;

	if(plyptr->vassals.total < 1 || vampire(plyptr))
		return;
	if(plyptr->levelmax < MINVASRANK)
	{
		sprintf(message, "5%c%s%c", MSGDEL, select_title(plyptr->classnum, MINVASRANK, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		remove_all_vassal(plyptr);
		return;
	} 
	for(count = 0; count < MAXVAS; ++count)
	{
		if(plyptr->vassals.plynum[count] == 0)
			continue;
		if(!equal_class(plyptr->classnum, players[plyptr->vassals.plynum[count]].classnum))
			remove_vassal(&players[plyptr->vassals.plynum[count]]);
	}
}

int is_vassal(player *plyptr, player *vassal)
{
	int count;
	int plynum;

	plynum = playernum(vassal);
	for(count = 0; count < MAXVAS; ++count)
		if(plyptr->vassals.plynum[count] == plynum)
			return(1);
	return(0);
}

void log_vassalmsg(char *msg, player *plyptr)
{
	int count;

	if(plyptr->vassals.total < 1)
		return;
	for(count = 0; count < MAXVAS; ++count)
	{
		if(plyptr->vassals.plynum[count] < 1)
			continue;
		send_message(msg, players[plyptr->vassals.plynum[count]].name, 1);
		log_event("", msg, players[plyptr->vassals.plynum[count]].name);
	}
}

void release(char *name, player *plyptr)
{
	int plynum;

	if(plyptr->vassals.total < 1)
	{
		sprintf(message, "7004%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(vampire(plyptr))
	{
		sprintf(message, "7005%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr == &players[plynum])
	{
		sprintf(message, "7006%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!is_vassal(plyptr, &players[plynum]))
	{
		sprintf(message, "7010%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	remove_vassal(&players[plynum]);
}

void remove_all_vassal(player *plyptr)
{
	int count;

	for(count = 0; count < MAXVAS; ++count)
		if(plyptr->vassals.plynum[count] > 0)
			remove_vassal(&players[plyptr->vassals.plynum[count]]);
}

void remove_vassal(player *plyptr)
{
	int count;
	int plynum;
	player *lord;

	if(plyptr->lord < 1)
		return;
	lord = &players[plyptr->lord];
	plynum = playernum(plyptr);
	for(count = 0; count < MAXVAS; ++count)
		if(lord->vassals.plynum[count] == plynum)
		{
			lord->vassals.plynum[count] = 0;
			lord->vassals.release[count] = 0;
			--lord->vassals.total;
			plyptr->lord = 0;
			sprintf(message, "7008%c%s%c%s%c%s%c%s%c", 
			MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL, lord->kname[plyptr->cont], MSGDEL);
			send_message(message, lord->name, 1);
			log_event("", message, lord->name);
			sprintf(message, "7011%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, adjectives[lord->gender][4], MSGDEL, lord->nick, MSGDEL);
			send_message(message, plyptr->name, 1);
			log_event("", message, plyptr->name);
			save_player(playernum(lord));
			save_player(plynum);
			break;
		}
}

void vassalaid(char *arg1, player *plyptr)
{
	if(plyptr->vassals.total < 1)
	{
		sprintf(message, "7004%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((STRCASECMP(arg1, "on")) == 0 && !(plyptr->special_kingdom & spcaid[plyptr->cont]))
		plyptr->special_kingdom += spcaid[plyptr->cont];
	else if((STRCASECMP(arg1, "off")) == 0 && plyptr->special_kingdom & spcaid[plyptr->cont])
		plyptr->special_kingdom -= spcaid[plyptr->cont];
	sprintf(message, "7027%c%s%c", MSGDEL, plyptr->special_kingdom & spcaid[plyptr->cont]?"On":"Off", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void vassal_hk(int type, player *plyptr)
{
	int cont;
	int copper;
	int count;
	int food;
	player *vassal;

	if(plyptr->vassals.total < 1 || vampire(plyptr))
		return;
	for(count = 0; count < MAXVAS; ++count)
		if(plyptr->vassals.plynum[count] > 0 && plyptr->vassals.release[count] == 0)
			plyptr->vassals.release[count] = 1;
	if(!type)
		return;
	for(cont = 0; cont < num_conts; ++cont)
	{
		if(plyptr->level[cont] < 0)
			continue;
		copper = plyptr->copper[cont] * 2 / 100;
		if(copper < 1)
			copper = 1;
		if(plyptr->copper[cont] - (plyptr->vassals.total * copper) < 0)
		{
			if(!barbarian(plyptr))
				plyptr->honor[cont] -= plyptr->vassals.total;
			sprintf(message, "7012%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			continue;
		}
		for(count = 0; count < MAXVAS; ++count)
		{
			if(plyptr->vassals.plynum[count] == 0)
				continue;
			vassal = &players[plyptr->vassals.plynum[count]];
			if(vassal->level[cont] < 0)
				continue;
			food = vassal->food[cont] * 5 / 100;
			if(food < 1)
				food = 1;
			if(vassal->food[cont] < food)
			{
				if(!barbarian(plyptr))
					plyptr->honor[cont] -= plyptr->level[cont] + 1;
				sprintf(message, "7013%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][4], MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL);
				log_event("", message, vassal->name);
				sprintf(message, "7014%c%s%c%s%c%s%c", 
				MSGDEL, vassal->name, MSGDEL, conts[cont].name, MSGDEL, adjectives[plyptr->gender][4], MSGDEL);
				log_event("", message, plyptr->name);
				save_player(plyptr->vassals.plynum[count]);
				continue;
			}
			plyptr->copper[cont] -= copper;
			vassal->food[cont] -= food;
			plyptr->food[cont] += food;
			vassal->copper[cont] += copper;
 			sprintf(message, "7015%c%s%c%d%c%s%c%d%c%s%c%s%c", 
			MSGDEL, vassal->name, MSGDEL, food, MSGDEL, food_name, MSGDEL, copper, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			sprintf(message, "7016%c%s%c%s%c%d%c%s%c%d%c%s%c%s%c", 
			MSGDEL, adjectives[plyptr->gender][4], MSGDEL, plyptr->nick, MSGDEL, food, MSGDEL, food_name, MSGDEL, copper, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
			log_event("", message, vassal->name);
			save_player(plyptr->vassals.plynum[count]);
		}
	}
	save_player(playernum(plyptr));
}

void vassals(char *name, player *plyptr)
{
	char land[16] = {0};
	char level[4] = {0};
	char peasant[16] = {0};
	char troops[16] = {0};
	int count;
	int current = 1;
	int plynum;
	player *lord;
	player *vassal;

	if(strlen(name) > 0)
	{
		if((plynum = find_player(name)) <= 0)
		{
			sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		lord = &players[plynum];
	}
	else
		lord = plyptr;
	if(vampire(lord) && lord != plyptr && !cleric(plyptr))
	{
		sprintf(message, "7017%c%s%c%s%c", MSGDEL, adjectives[lord->gender][5], MSGDEL, adjectives[lord->gender][0], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(lord->levelmax < MINVASRANK && !vampire(lord))
	{
		if(plyptr == lord)
			sprintf(message, "7018%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, select_title(plyptr->classnum, MINVASRANK, plyptr->gender), MSGDEL);
		else
			sprintf(message, "7019%c%s%c%s%c", MSGDEL, lord->name, MSGDEL, select_title(lord->classnum, MINVASRANK, lord->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(lord->vassals.total < 1)
	{
		if(plyptr == lord)
			sprintf(message, "7020%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		else
			sprintf(message, "7021%c%s%c", MSGDEL, lord->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr == lord)
		sprintf(message, "7027%c%s%c", MSGDEL, plyptr->special_kingdom & spcaid[plyptr->cont]?"On":"Off", MSGDEL);
	else
		sprintf(message, "7027%c%s%c", MSGDEL, lord->special_kingdom & spcaid[lord->cont]?"On":"Off", MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr == lord)
		sprintf(message, "7022%c%c", MSGDEL, MSGDEL);
	else
		sprintf(message, "7023%c%s%c", MSGDEL, lord->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "7024%c%-8s%c", MSGDEL, peasant_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "7025%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXVAS; ++count)
	{
		if(lord->vassals.plynum[count] == 0)
			continue;
		vassal = &players[lord->vassals.plynum[count]];
		sprintf(level, "%d", vassal->level[vassal->cont] + 1);
		if(plyptr == lord || titan(plyptr))
		{
			sprintf(land, "%d", vassal->land[vassal->cont]);
			sprintf(peasant, "%d", vassal->peasant[vassal->cont]);
			sprintf(troops, "%d", combined_army(vassal->cont, vassal));
		}
		else
		{
			strcpy(land, "?????");
			strcpy(peasant, "?????");
			strcpy(troops, "?????");
		}
		sprintf(message, "7026%c%02d%c%-16s%c%-5s%c%-16s%c%-5s%c%-8s%c%s%c", 
		MSGDEL, current++, MSGDEL, vassal->name, MSGDEL, level, MSGDEL, lord == plyptr || titan(plyptr)?conts[vassal->cont].name:"??????", MSGDEL, 
		land, MSGDEL, peasant, MSGDEL, troops, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}
