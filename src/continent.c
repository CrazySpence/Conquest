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

void add_ship(int shpnum, int type, player *plyptr)
{
	if(type < 1 || type >= num_ships)
		return;
	++plyptr->fleet;
	plyptr->ships[shpnum].type = type;
	plyptr->ships[shpnum].hull = shiptypes[type].hull;
	plyptr->ships[shpnum].cargo = 0;
	plyptr->ships[shpnum].cargotype = NOCARGO;
	if((strlen(plyptr->ships[shpnum].name)) < 1)
		strcpy(plyptr->ships[shpnum].name, dft_sname);
}

void atlantis(player *plyptr)
{
	char desc[16] = {0};
	char type[16] = {0};
	int amount = 0;
	int cargotype = NOCARGO;
	int chance = 1;
	int count;
	int gift;
	int inum;
	int maxcargo = 0;
	int newamount = 0;

	if(check_hero(HERO_TITAN, plyptr))
		chance += 9;
	if(randomnum(100, 1) <= chance)
	{
		sprintf(message, "13016%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
		if(randomnum(100, 1) > 5)
			return;
	sprintf(message, "5000%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "5001%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "5002%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	switch((gift = randomnum(4, 0)))
	{
		case 0:
			strcpy(desc, "buried");
			strcpy(type, money_name);
			amount = randomnum(100000, 1000);
			cargotype = CARGOMONEY;
			break;
		case 1:
			for(amount = 0, count = randomnum(8, 2); count > 0; --count)
				if((inum = find_randomitem()) > 0 && !has_item(inum, plyptr))
				{
					++amount;
					add_item(inum, plyptr);
				}
			if(amount == 0)
				sprintf(message, "5108%c%c", MSGDEL, MSGDEL);
			else
				sprintf(message, "5003%c%d%c", MSGDEL, amount, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		case 2:
			strcpy(desc, "stranded");
			strcpy(type, peasant_name);
			amount = randomnum(1000, 100);
			cargotype = CARGOPEASANT;
			break;
		case 3:
			strcpy(desc, "stranded");
			sprintf(type, "%s(s)", units[plyptr->troop_type][SOLDIERNUM].name);
			amount = randomnum(500, 100);
			cargotype = CARGOSOLDIER;
			break;
		case 4:
			strcpy(desc, "extra");
			strcpy(type, "move");
			amount = randomnum(10, 2);
			plyptr->movepts += amount;
			sprintf(message, "5004%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, desc, MSGDEL, type, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
	}
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type < 1 || plyptr->ships[count].hull < 1)
			continue;
		maxcargo = find_maxcargo(count, cargotype, plyptr);
		if(plyptr->ships[count].cargotype == NOCARGO || (plyptr->ships[count].cargotype == cargotype && plyptr->ships[count].cargo < maxcargo))
		{
			newamount = amount;
			if(plyptr->ships[count].cargo + amount > maxcargo)
				newamount = maxcargo - plyptr->ships[count].cargo;
			if(newamount < 100)
				continue;
			amount = newamount;
			plyptr->ships[count].cargotype = cargotype;
			plyptr->ships[count].cargo += amount;
			break;
		}
	}
	if(count == MAXFLT)
		sprintf(message, "5093%c%s%c%s%c", MSGDEL, desc, MSGDEL, type, MSGDEL);
	else
		sprintf(message, "5004%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, desc, MSGDEL, type, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void blockade(player *plyptr)
{
	int amount = 0;
	struct tm *pnt;
	time_t current;

	if(!fighter(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(FIGHTER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.cont_special[plyptr->cont] & SPC_BLOCKADE)
	{
		sprintf(message, "4390%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BLOCKADE)
	{
		sprintf(message, "1%c%d%cto use Blockade%c", MSGDEL, BLOCKADE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(num_ships > 1)
		amount = shiptypes[(num_ships - 1)].cost * MAXFLT;
	if(plyptr->copper[plyptr->cont] < amount)
	{
		sprintf(message, "16%c%s%c%d%cblockade %s%c", MSGDEL, money_name, MSGDEL, amount, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= BLOCKADE;
	if((randomnum(100, 1)) <= 70)
	{
		sprintf(message, "4391%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->copper[plyptr->cont] -= amount;
	time(&current);
	pnt = localtime(&current);
	conq_info.cont_special[plyptr->cont] += SPC_BLOCKADE;
	save_info();
	sprintf(message, "4392%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4393%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "4394%cblockade%c%d%c", MSGDEL, MSGDEL, 60 - pnt->tm_min, MSGDEL);
	send_channel(message, 0);
}

void build_ships()
{
	int cont;
	int count;

	for(cont = 0; cont < num_conts; ++cont)
	{
		for(count = 0; count < MAXSHP; ++count)
		{
			if(conq_info.ship_qty[cont][count] < MAXSHPINV / 4)
			{
				conq_info.ship_qty[cont][count] += randomnum(MAXSHPINV / 4, MAXSHPINV / 10);
				if(conq_info.ship_qty[cont][count] > MAXSHPINV / 4)
					conq_info.ship_qty[cont][count] = MAXSHPINV / 4;
			}
		}
	}
	save_info();
}

void buy_ship(char *typename, player *plyptr)
{
	int count;
	int shpnum;

	if(plyptr->fleet == MAXFLT)
	{
		sprintf(message, "10047%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(count = 1; count < num_ships; ++count)
		if((STRCASECMP(typename, shiptypes[count].name)) == 0)
			break;
	if(count == num_ships)
	{
		sprintf(message, "5006%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5007%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5101%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 1; count < num_ships; ++count)
		{
			sprintf(message, "5102%c%-16s%c%-6s%c%04d%c%05d%c%07d%c%d%c", MSGDEL, shiptypes[count].name, MSGDEL, shiptypes[count].type == 0?"Cargo":"Attack", MSGDEL, shiptypes[count].hull, MSGDEL, shiptypes[count].maxcargo, MSGDEL, 
			shiptypes[count].cost, MSGDEL, conq_info.ship_qty[plyptr->cont][count], MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		sprintf(message, "56%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.ship_qty[plyptr->cont][count] < 1)
	{
		sprintf(message, "5106%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < shiptypes[count].mpcost)
	{
		sprintf(message, "1%c%d%cto purchase a ship%c", MSGDEL, shiptypes[count].mpcost, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < shiptypes[count].cost)
	{
		sprintf(message, "5008%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5009%c%d%c%s%c", MSGDEL, shiptypes[count].cost, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	--conq_info.ship_qty[plyptr->cont][count];
	save_info();
	shpnum = find_shpnum(plyptr);
	add_ship(shpnum, count, plyptr);
	plyptr->movepts -= shiptypes[count].mpcost;
	plyptr->copper[plyptr->cont] -= shiptypes[count].cost;
	sprintf(message, "5010%c%s%c%s%c", MSGDEL, shiptypes[count].modifier, MSGDEL, shiptypes[count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

int check_blockade(int cont, player *plyptr)
{
	struct tm *pnt;
	time_t current;

	if(!(conq_info.cont_special[cont] & SPC_BLOCKADE))
		return(1);
	if(randomnum(100, 1) <= 12 - (plyptr->fleet * 2))
	{
		sprintf(message, "4397%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	time(&current);
	pnt = localtime(&current);
	sprintf(message, "4395%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4394%cblockade of %s%c%d%c", MSGDEL, conts[cont].name, MSGDEL, 60 - pnt->tm_min, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(0);
}

void check_contspc(int cont)
{
	int save = 0;

	if(conq_info.cont_special[cont] & SPC_BLOCKADE)
	{
		save = 1;
		conq_info.cont_special[cont] -= SPC_BLOCKADE;
		sprintf(message, "4396%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
	}
	if(conq_info.cont_special[cont] & SPC_BLITZ)
	{
		save = 1;
		conq_info.cont_special[cont] -= SPC_BLITZ;
	}
	if(conq_info.cont_special[cont] & SPC_CEASEFIRE)
	{
		save = 1;
		conq_info.cont_special[cont] -= SPC_CEASEFIRE;
		sprintf(message, "16027%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
	}
	if(save == 1)
		save_info();
}

int check_siren(player *plyptr)
{
	int shpnum;

	if(!has_item(ITEM_SIREN, plyptr))
		return(1);
	if(randomnum(100, 1) <= 20 && (shpnum = find_randomshpnum(ANYSHIP, plyptr)) >= 0)
	{
		sprintf(message, "10064%c%s%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL, itemd[ITEM_SIREN].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		remove_ship(shpnum, plyptr);
	}
	if(plyptr->fleet < 1)
		return(0);
	else
		return(1);
}

int check_weather(int cont, int to, player *plyptr)
{
	int chance = 0;
	int damage = 0;
	int shpnum;

	if(forecast[cont].sail_damage == 0)
		return(1);
	switch(forecast[cont].type)
	{
		case WT_FREEZE:
			chance = 4;
			damage = randomnum(5, 1);
			break;
		case WT_SNOW:
			chance = 6;
			damage = randomnum(6, 2);
			break;
		case WT_STORM:
		case WT_FOG:
			if(forecast[cont].violent == 1)
			{
				chance = 100;
				damage = conq_info.kraken.maxhealth / 3;
			}
			else
			{
				chance = 6;
				damage = randomnum(6, 2);
			}
			break;
		case WT_WIND:
			chance = 6;
			damage = randomnum(6, 2);
			break;
		default:
			return(1);
	}
	if(randomnum(100, 1) <= chance * plyptr->fleet && (shpnum = find_randomshpnum(ANYSHIP, plyptr)) >= 0)
	{
		sprintf(message, "5072%c%s%c%s%c", MSGDEL, conts[to].name, MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		plyptr->ships[shpnum].hull -= damage;
		damage_cargo(damage, shpnum, plyptr);
		if(plyptr->ships[shpnum].hull <= 0)
		{
			remove_ship(shpnum, plyptr);
			sprintf(message, "5059%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
	}
	if(plyptr->fleet < 1)
		return(0);
	else
		return(1);
}

int compare_cargo(char *typename, int type, player *plyptr)
{
	char cargo[][32] =
	{
		"", "", "", "", "", "", "", "", "", "", "" 
	};
	int count;
	int found = NOCARGO;
	int len;

	len = strlen(typename);
	for(count = CARGOREN; count <= CARGOFOOD; ++count)
	{
		switch(count)
		{
			case CARGOPEASANT:
				strcpy(cargo[count], peasant_name);
				break;
			case CARGOFOOD:
				strcpy(cargo[count], food_name);
				break;
			case CARGOMONEY:
				strcpy(cargo[count], money_name);
				break;
			default:
				strcpy(cargo[count], units[plyptr->troop_type][count].name);
				break;
		}
		if((STRNCASECMP(typename, cargo[count], len)) == 0)
		{
			found = count;
			if(type >= CARGOREN && type != count)
				found = -2;
			break;
		}
	}
	return(found);
}

int compute_shpdmg(int shpnum, player *plyptr)
{
	int addfire = 0;
	int fire;

	fire = shiptypes[plyptr->ships[shpnum].type].maxdamage;
	switch(plyptr->ships[shpnum].cargotype)
	{
		case NOCARGO:
		case CARGOFOOD:
		case CARGOPEASANT:
		case CARGOMONEY:
			break;
		default:
			if(plyptr->ships[shpnum].cargo < 1 || units[plyptr->troop_type][plyptr->ships[shpnum].cargotype].range == 0)
				break;
			if(shiptypes[plyptr->ships[shpnum].type].type == ATTACKSHIP)
				addfire = plyptr->ships[shpnum].cargo / 15;
			else
				addfire = plyptr->ships[shpnum].cargo / 150;
			if(addfire < 1)
				addfire = 1;
			fire += addfire;
			break;
	}
	return(randomnum(fire, 1));
}

void damage_cargo(int damage, int shpnum, player *plyptr)
{
	switch(plyptr->ships[shpnum].cargotype)
	{
		case NOCARGO:
			break;
		case CARGOFOOD:
		case CARGOPEASANT:
			plyptr->ships[shpnum].cargo -= damage * 5;
			break;
		case CARGOCAT:
			plyptr->ships[shpnum].cargo -= damage / 2;
			break;
		case CARGOMONEY:
			plyptr->ships[shpnum].cargo -= damage * 1000;
			break;
		default:
			plyptr->ships[shpnum].cargo -= (damage * 10) / units[plyptr->troop_type][plyptr->ships[shpnum].cargotype].defend;
			break;
	}
	if(plyptr->ships[shpnum].cargo <= 0)
	{
		plyptr->ships[shpnum].cargo = 0;
		plyptr->ships[shpnum].cargotype = NOCARGO;
	}
}

void engage(char *name, player *plyptr)
{
	int plynum;
	int round = 0;
	player *result = NULL;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "engage", "", "");
		return;
	}
	if(plyptr->movepts < ENGAGE)
	{
		sprintf(message, "1%c%d%cto engage another ship%c", MSGDEL, ENGAGE, MSGDEL, MSGDEL);
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
		sprintf(message, "11%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(players[plynum].fleet < 1)
	{
		sprintf(message, "5046%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.kraken.alive == 1 && plyptr->cont != players[plynum].cont)
		if(!kraken(plyptr))
			return;
	plyptr->movepts -= ENGAGE;
	sprintf(message, "5047%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, 
	select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "5048%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	show_fleet(plyptr->name, "crew", &players[plynum]);
	show_fleet(players[plynum].name, "crew", plyptr);
	while(result == NULL && round++ < 500)
	{
		result = seabattle(plyptr, &players[plynum]);
		if(result == NULL)
			result = seabattle(&players[plynum], plyptr);
	}
	if(result == NULL)
	{
		sprintf(message, "5091%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
		return;
	}
	if(result == plyptr)
	{
		sprintf(message, "5052%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5054%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
	}
	else
	{
		sprintf(message, "5052%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
		send_message(message, players[plynum].name, 1);
		sprintf(message, "5054%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	save_player(plynum);
}

int find_maxcargo(int shpnum, int type, player *plyptr)
{
	float pct;
	int maxcargo;

	switch(type)
	{
		case CARGOMONEY:
			maxcargo = shiptypes[plyptr->ships[shpnum].type].maxcargo * 1000;
			break;
		case CARGOCAT:
			maxcargo = shiptypes[plyptr->ships[shpnum].type].maxcargo / 10;
			break;
		default:
			maxcargo = shiptypes[plyptr->ships[shpnum].type].maxcargo;
			break;
	}
	pct = (float)((plyptr->ships[shpnum].hull * 100 / shiptypes[plyptr->ships[shpnum].type].hull) / 100.0);
	return((int)(maxcargo * pct));
}

int find_randomshpnum(int type, player *plyptr)
{
	int count;
	int shps[MAXFLT];
	int current;

	if(plyptr->fleet < 1)
		return(-1);
	for(count = 0, current = 0; count < MAXFLT; ++count)
		if(plyptr->ships[count].type > 0 && (type == ANYSHIP || shiptypes[plyptr->ships[count].type].type == type))
			shps[current++] = count;
		else
			shps[count] = -1;
	return(shps[randomnum(current - 1, 0)]);
}

int find_shpnum(player *plyptr)
{
	int count;

	for(count = 0; count < MAXFLT; ++count)
		if(plyptr->ships[count].type == 0)
			break;
	if(count == MAXFLT)
		return(-1);
	else
		return(count);
}

void fleet(player *plyptr)
{
	char cargo[][32] =
	{
		"", "", "", "", "", "", "", "", "", "", "" 
	};
	char str[32] = {0};
	int count;
	int maxcargo;
	struct tm *pnt;
	time_t current;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < FLEET)
	{
		sprintf(message, "1%c%d%cto view your fleet%c", MSGDEL, FLEET, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= FLEET;
	sprintf(message, "5012%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "5013%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type == 0)
			continue;
		switch(plyptr->ships[count].cargotype)
		{
			case NOCARGO:
				break;
			case CARGOPEASANT:
				strcpy(cargo[plyptr->ships[count].cargotype], peasant_name);
				break;
			case CARGOFOOD:
				strcpy(cargo[plyptr->ships[count].cargotype], food_name);
				break;
			case CARGOMONEY:
				strcpy(cargo[plyptr->ships[count].cargotype], money_name);
				break;
			case CARGOCAT:
				strcpy(cargo[plyptr->ships[count].cargotype], units[plyptr->troop_type][plyptr->ships[count].cargotype].name);
				break;
			default:
				strcpy(cargo[plyptr->ships[count].cargotype], units[plyptr->troop_type][plyptr->ships[count].cargotype].name);
				break;
		}
		maxcargo = find_maxcargo(count, plyptr->ships[count].cargotype, plyptr);
		sprintf(str, "%d/%d", plyptr->ships[count].hull, shiptypes[plyptr->ships[count].type].hull);
		sprintf(message, "5014%c%02d%c%-16s%c%-12s%c%-6s%c%d%c%d%c%s%s%c", MSGDEL, count + 1, MSGDEL, plyptr->ships[count].name, MSGDEL, shiptypes[plyptr->ships[count].type].name, MSGDEL, 
		str, MSGDEL, plyptr->ships[count].cargo, MSGDEL, maxcargo, MSGDEL, plyptr->ships[count].cargotype == NOCARGO?"":cargo[plyptr->ships[count].cargotype], 
		plyptr->ships[count].cargotype == NOCARGO || plyptr->ships[count].cargotype >= CARGOMONEY?"":"(s)", MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(conq_info.cont_special[plyptr->cont] & SPC_BLOCKADE)
	{
		time(&current);
		pnt = localtime(&current);
		sprintf(message, "4394%cblockade of %s%c%d%c", MSGDEL, conts[plyptr->cont].name, MSGDEL, 60 - pnt->tm_min, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	if(conq_info.kraken.alive == 1)
	{
		sprintf(message, "5016%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void invade(char *name, player *plyptr)
{
	int amount;
	int cont;
	int count;
	int move;
	int plynum;
	int troopnum;

	if(strlen(name) < 1)
	{
		help(plyptr->name, "invade", "", "");
		return;
	}
	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	move = sail_move(plyptr);
	if(plyptr->movepts < move + INVADE)
	{
		sprintf(message, "1%c%d%cto launch an invasion!%c", MSGDEL, move + INVADE, MSGDEL, MSGDEL);
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
		sprintf(message, "11%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont == players[plynum].cont)
	{
		sprintf(message, "5099%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((allow_attack(name, 1, plyptr)) == 0)
		return;
	cont = players[plynum].cont;
	sprintf(message, "5100%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL);
	send_channel(message, 0);
	sail(conts[cont].name, plyptr);
	if(plyptr->cont != cont)
	{
		plyptr->movepts -= INVADE;
		return;
	}
	engage(name, plyptr);
	if(plyptr->fleet < 1)
	{
		plyptr->movepts -= (INVADE - ENGAGE);
		return;
	}
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type < 1)
			continue;
		if(plyptr->ships[count].cargotype < CARGOREN || plyptr->ships[count].cargotype > CARGOMERC2)
			continue;
		plyptr->army[ARMY_CAMPAIGN][cont].total[plyptr->ships[count].cargotype] += plyptr->ships[count].cargo;
	}
	attack(name, plyptr);
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type < 1)
			continue;
		if(plyptr->ships[count].cargotype < CARGOREN || plyptr->ships[count].cargotype > CARGOMERC2)
			continue;
		troopnum = plyptr->ships[count].cargotype;
		if(plyptr->cont == cont)
		{
			amount = plyptr->ships[count].cargo - plyptr->army[ARMY_CAMPAIGN][cont].total[troopnum];
			plyptr->army[ARMY_CAMPAIGN][cont].total[troopnum] -= plyptr->ships[count].cargo;
			if(plyptr->army[ARMY_CAMPAIGN][cont].total[troopnum] < 0)
				plyptr->army[ARMY_CAMPAIGN][cont].total[troopnum] = 0;
			if(amount > 0)
				plyptr->ships[count].cargo -= amount;
		}
		else
			plyptr->ships[count].cargo = 0;
		if(plyptr->ships[count].cargo <= 0)
			plyptr->ships[count].cargotype = NOCARGO;
	}
	plyptr->movepts -= (INVADE - ATTACK - ENGAGE);
}

int kraken(player *plyptr)
{
	int chance;
	int condition;
	int hit;
	int fire = 0;
	int shpnum;
	int won = -1;

	if((shpnum = find_randomshpnum(ANYSHIP, plyptr)) < 0)
		return(1);
	if(randomnum(100, 1) + (shiptypes[plyptr->ships[shpnum].type].speed * 2) <= 25 || titan(plyptr))
	{
		sprintf(message, "5017%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5018%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_channel(message, 0);
		return(1);
	}
	sprintf(message, "5019%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "5020%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, shiptypes[plyptr->ships[shpnum].type].name, MSGDEL);
	send_channel(message, 0);
	while(won == -1)
	{
		if((chance = randomnum(100, 1)) <= conq_info.kraken.damage)
		{
			hit = 1;
			fire = randomnum(conq_info.kraken.maxdamage, 1);
			plyptr->ships[shpnum].hull -= fire;
			damage_cargo(fire / 2, shpnum, plyptr);
			if(plyptr->ships[shpnum].hull <= 0)
			{
				won = 0;
				break;
			}
		}
		else
			hit = 0;
		if(randomnum(100, 1) <= 10)
		{
			switch(randomnum(5 - (hit * 3), 3 - (hit * 3)))
			{
				case 0:
					sprintf(message, "5021%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 1:
					sprintf(message, "5022%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 2:
					sprintf(message, "5023%c%s%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL, randomnum(100, 1) <= 50?"starboard":"port", MSGDEL);
					break;
				case 3:
					sprintf(message, "5024%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 4:
					sprintf(message, "5025%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 5:
					sprintf(message, "5026%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
			}
			send_channel(message, 0);
			if(hit == 1)
			{
				condition = plyptr->ships[shpnum].hull * 100 / shiptypes[plyptr->ships[shpnum].type].hull;
				if(condition > 0)
				{
					sprintf(message, "5027%c%s%c%d%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL, condition, MSGDEL);
					send_channel(message, 0);
				}
			}
		}
		chance = shiptypes[plyptr->ships[shpnum].type].damage;
		if(check_hero(HERO_BARBLEADER, plyptr))
			chance += 10;
		if(plyptr->ships[shpnum].cargotype >= CARGOREN && plyptr->ships[shpnum].cargotype <= CARGOMERC2)
			chance += 5;
		if(randomnum(100, 1) <= chance)
		{
			hit = 1;
			fire = compute_shpdmg(shpnum, plyptr);
			conq_info.kraken.health -= fire;
			if(conq_info.kraken.health <= 0)
			{
				won = 1;
				break;
			}
		}
		else
			hit = 0;
		if(randomnum(100, 1) <= 10)
		{
			switch(randomnum(5 - (hit * 3), 3 - (hit * 3)))
			{
				case 0:
					sprintf(message, "5028%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 1:
					sprintf(message, "5029%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 2:
					sprintf(message, "5030%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 3:
					sprintf(message, "5031%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 4:
					sprintf(message, "5032%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
				case 5:
					sprintf(message, "5033%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
					break;
			}
			send_channel(message, 0);
		}
	}
	if(won == 0)
	{
		remove_ship(shpnum, plyptr);
		sprintf(message, "5034%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5035%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_channel(message, 0);
	}
	else
	{
		conq_info.kraken.alive = 0;
		save_info();
		sprintf(message, "5036%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5037%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "5090%c%d%c%s%c", MSGDEL, KRAKENCOPPER, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		plyptr->copper[plyptr->cont] += KRAKENCOPPER;
	}
	save_info();
	return(won);
}

void loadship(char *typename, int amount, int shpnum, player *plyptr)
{
	char cargo[][32] =
	{
		"", "", "", "", "", "", "", "", "", "", "" 
	};
	int count;
	int food = 0;
	int *item = NULL;
	int maxcargo;
	int space;
	int type;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(typename) < 1)
	{
		help(plyptr->name, "loadship", "", "");
		return;
	}
	if(plyptr->movepts < LOADSHIP)
	{
		sprintf(message, "1%c%d%cto load your fleet%c", MSGDEL, LOADSHIP, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount < 1)
	{
		help(plyptr->name, "loadship", "", "");
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(--shpnum < 0 || shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5015%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((type = compare_cargo(typename, plyptr->ships[shpnum].cargotype, plyptr)) == NOCARGO)
	{
		sprintf(message, "5038%c%s%c", MSGDEL, typename, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	strcpy(cargo[CARGOPEASANT], peasant_name);
	strcpy(cargo[CARGOFOOD], food_name);
	strcpy(cargo[CARGOMONEY], money_name);
	for(count = 0; count < MAXTRP; ++count)
		strcpy(cargo[count], units[plyptr->troop_type][count].name);
	if(type < CARGOREN)
	{
		sprintf(message, "5039%c%s%c", MSGDEL, shiptypes[plyptr->ships[shpnum].type].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5040%c%s%c%s%c", MSGDEL, cargo[plyptr->ships[shpnum].cargotype], MSGDEL, plyptr->ships[shpnum].cargotype >= CARGOMONEY?"":"(s)", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	maxcargo = find_maxcargo(shpnum, type, plyptr);
	if(plyptr->ships[shpnum].cargo + amount > maxcargo)
	{
		sprintf(message, "5041%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5042%c%s%c%s%c%s%c%d%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, shiptypes[plyptr->ships[shpnum].type].name, MSGDEL, plyptr->ships[shpnum].name, MSGDEL, amount, MSGDEL);
		send_message(message, plyptr->name, 1);
		if((space = maxcargo - plyptr->ships[shpnum].cargo) < 0)
			space = 0;
		sprintf(message, "5043%c%d%c", MSGDEL, space, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(type)
	{
		case CARGOPEASANT:
			food = amount;
			item = &plyptr->peasant[plyptr->cont];
			break;
		case CARGOFOOD:
			item = &plyptr->food[plyptr->cont];
			break;
		case CARGOMONEY:
			item = &plyptr->copper[plyptr->cont];
			break;
		default:
			food = amount * units[plyptr->troop_type][type].food;
			item = &plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[type];
			break;
	}
	if(*item < amount)
	{
		sprintf(message, "5045%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, cargo[type], MSGDEL, type >= CARGOMONEY?" ":"s ", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->food[plyptr->cont] < food)
	{
		sprintf(message, "5067%c%s%c%d%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, food, MSGDEL, food_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5068%c%d%c%s%c", MSGDEL, plyptr->food[plyptr->cont], MSGDEL, food_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	*item -= amount;
	plyptr->food[plyptr->cont] -= food;
	plyptr->movepts -= LOADSHIP;
	plyptr->ships[shpnum].cargo += amount;
	plyptr->ships[shpnum].cargotype = type;
	sprintf(message, "5044%c%s%c%d%c%s%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL, amount, MSGDEL, cargo[plyptr->ships[shpnum].cargotype], plyptr->ships[shpnum].cargotype >= CARGOMONEY?"":"(s)", MSGDEL);
	send_message(message, plyptr->name, 1);
}

int pirates(player *plyptr)
{
	char numbers[][4] = {"I", "II", "III"};
	int chance = 1;
	int count;
	int round = 0;
	player *result = NULL;
	player *pirate;

	if((randomnum(100, 1)) > 8 || has_item(ITEM_LETTER, plyptr) || check_hero(HERO_BARBLEADER, plyptr))
		return(1);
	pirate = (void *)plybuf[ATTACKER];
	memset(pirate, 0, PLYSIZ);
	for(count = 0; count < randomnum(3, 1); ++count)
	{
		add_ship(count, randomnum(num_ships - 1, 1), pirate);
		sprintf(pirate->ships[count].name, "The Jolly Roger %s", numbers[count]);
	}
	strcpy(pirate->name, pirate_names[randomnum(3, 0)]);
	strcpy(pirate->nick, pirate->name);
	sprintf(message, "5055%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, plyptr->nick, MSGDEL, pirate->nick, MSGDEL);
	send_channel(message, 0);
	show_fleet(plyptr->name, "crew", pirate);
	if(check_hero(HERO_TITAN, plyptr))
		chance += 9;
	if(randomnum(100, 1) <= chance)
	{
		sprintf(message, "5056%c%s%c", MSGDEL, pirate->nick, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "13016%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		result = plyptr;
	}
	while(result == NULL && round++ < 500)
	{
		result = seabattle(plyptr, pirate);
		if(result == NULL)
			result = seabattle(pirate, plyptr);
	}
	if(result == NULL)
	{
		sprintf(message, "5091%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
		return(1);
	}
	if(result == plyptr)
	{
		sprintf(message, "5057%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "5058%c%s%c", MSGDEL, pirate->nick, MSGDEL);
		send_channel(message, 0);
		return(1);
	}
	else
	{
		sprintf(message, "5060%c%s%c%s%c", MSGDEL, pirate->nick, MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		return(0);
	}
}

void population(char *contname, player *plyptr)
{
	int count;
	int cont;
	int found;
        int hit[10] =
	{
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
	int maxcycles = 0;
	int next = 0;
	int plynum;
	int popnum;

	if(plyptr->movepts < POPULATION)
	{
		sprintf(message, "1%c%d%cto view the population%c", MSGDEL, POPULATION, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((cont = find_cont(contname)) < 0)
		cont = plyptr->cont;
	plyptr->movepts -= POPULATION;
	sprintf(message, "5103%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "5104%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "5105%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	plynum = playernum(plyptr);
	while(next < 10 && maxcycles++ < 1000)
	{
		if((popnum = find_randomplayer(cont, -1)) < 1 || plynum == popnum)
			continue;
		for(count = 0, found = 0; count < next; ++count)
			if(hit[count] == popnum)
			{
				found = 1;
				break;
			}
		if(!found)
		{
			hit[next++] = popnum;
			sprintf(message, "5061%c%-16s%c%-16s%c%-16s%s%c", MSGDEL, 
			select_class(players[popnum].classnum), MSGDEL, select_title(players[popnum].classnum, 
			players[popnum].level[cont], players[popnum].gender), MSGDEL, players[popnum].nick, 
			(restrict_combat == 1 && players[popnum].validate != NOT_VAL)?"*":"", MSGDEL),
			send_message(message, plyptr->name, 1);
		}
	}
	if(next == 0)
	{
		sprintf(message, "5062%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(restrict_combat == 1)
	{
		sprintf(message, "4319%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void read_ships()
{
	char input[1024] = {0};
	FILE *fp;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, SHPFIL);
	if((fp = fopen(SHPFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, SHPFIL, strerror(errno));
		exit_conquest();
	}
	conq_info.kraken.damage = 1;
	conq_info.kraken.maxhealth = 1;
	conq_info.kraken.maxdamage = 1;
	memset(&shiptypes[num_ships], 0, SHTSIZ);
	strcpy(shiptypes[num_ships].modifier, "None");
	strcpy(shiptypes[num_ships++].name, "None");
	while((fgets(input, 1024, fp)) != NULL && num_ships < MAXSHP)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%15s %15s %d %d %d %d %d %d %d %d", 
		shiptypes[num_ships].modifier, shiptypes[num_ships].name, &shiptypes[num_ships].cost, &shiptypes[num_ships].damage, &shiptypes[num_ships].hull, &shiptypes[num_ships].maxcargo, 
		&shiptypes[num_ships].maxdamage, &shiptypes[num_ships].mpcost, &shiptypes[num_ships].speed, &shiptypes[num_ships].type);
		if(shiptypes[num_ships].cost < 0)
			shiptypes[num_ships].cost = 0;
		if(shiptypes[num_ships].damage < 0)
			shiptypes[num_ships].damage = 0;
		if(shiptypes[num_ships].hull < 1)
			shiptypes[num_ships].hull = 1;
		if(shiptypes[num_ships].maxcargo < 0)
			shiptypes[num_ships].maxcargo = 0;
		if(shiptypes[num_ships].maxdamage < 0)
			shiptypes[num_ships].maxdamage = 0;
		if(shiptypes[num_ships].mpcost < 0)
			shiptypes[num_ships].mpcost = 0;
		if(shiptypes[num_ships].speed < 0)
			shiptypes[num_ships].speed = 0;
		if(shiptypes[num_ships].type < CARGOSHIP || shiptypes[num_ships].type > ATTACKSHIP)
			shiptypes[num_ships].type = CARGOSHIP;
		if(shiptypes[num_ships].damage > conq_info.kraken.damage)
			conq_info.kraken.damage = shiptypes[num_ships].damage;
		if(shiptypes[num_ships].maxdamage > conq_info.kraken.maxdamage)
			conq_info.kraken.maxdamage = shiptypes[num_ships].maxdamage;
		if(shiptypes[num_ships].hull > conq_info.kraken.maxhealth)
			conq_info.kraken.maxhealth = shiptypes[num_ships].hull;
		if((strlen(shiptypes[num_ships].modifier)) > 0 && (strlen(shiptypes[num_ships].name)) > 0)
			++num_ships;
	}
	fclose(fp);
	conq_info.kraken.damage *= 2;
	conq_info.kraken.maxdamage *= 2;
	conq_info.kraken.maxhealth *= 3;
}

void remove_ship(int shpnum, player *plyptr)
{
	--plyptr->fleet;
	plyptr->ships[shpnum].type = 0;
}

void repair(int shpnum, player *plyptr)
{
	int amount;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(--shpnum < 0 || shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5015%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->ships[shpnum].hull >= shiptypes[plyptr->ships[shpnum].type].hull)
	{
		sprintf(message, "5063%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < REPAIR)
	{
		sprintf(message, "1%c%d%cto repair %s%c", MSGDEL, REPAIR, MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	amount = (shiptypes[plyptr->ships[shpnum].type].hull - plyptr->ships[shpnum].hull) * 1000;
	if(plyptr->copper[plyptr->cont] < amount)
	{
		sprintf(message, "5065%c%s%c%d%c%s%c%d%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL, amount, MSGDEL, money_name, MSGDEL, plyptr->copper[plyptr->cont], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->ships[shpnum].cargotype != NOCARGO)
	{
		sprintf(message, "5064%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= REPAIR;
	plyptr->copper[plyptr->cont] -= amount;
	plyptr->ships[shpnum].hull = shiptypes[plyptr->ships[shpnum].type].hull;
	sprintf(message, "5066%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void sail(char *dest, player *plyptr)
{
	int cont;
	int from;
	int move;
	int shpnum = 0;

	if((strlen(dest)) < 1)
	{
		help(plyptr->name, "sail", "", "");
		return;
	}
	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(num_conts < 2)
	{
		sprintf(message, "5069%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((cont = find_cont(dest)) < 0)
	{
		sprintf(message, "5070%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, dest, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cont == plyptr->cont)
	{
		sprintf(message, "5071%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	move = sail_move(plyptr);
	if(plyptr->movepts < move)
	{
		sprintf(message, "1%c%d%cto launch your fleet%c", MSGDEL, move, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= move;
	if(!check_blockade(plyptr->cont, plyptr))
		return;
	if(!check_blockade(cont, plyptr))
		return;
	if(!check_weather(plyptr->cont, cont, plyptr))
		return;
	if(!check_siren(plyptr))
		return;
	if(conq_info.kraken.alive == 1)
		kraken(plyptr);
	else
		pirates(plyptr);
	if(plyptr->fleet < 1)
		return;
	from = plyptr->cont;
	atlantis(plyptr);
	plyptr->cont = cont;
	plyptr->city = conts[cont].port;
	if(plyptr->level[cont] < 0)
	{
		++plyptr->kingdoms;
		if(titan(plyptr))
			plyptr->level[cont] = plyptr->level[from];
		else
			plyptr->level[cont] = 0;
		++totlvl[cont][plyptr->level[cont]];
		plyptr->first_play[cont] = conq_info.today;
		plyptr->honor[cont] = 0;
		plyptr->land[cont] = 100;
		plyptr->food[cont] = 100;
		plyptr->peasant[cont] = 100;
		default_formation(cont, plyptr);
		plyptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] = 50;
		plyptr->army[ARMY_CAMPAIGN][cont].total[SOLDIERNUM] = 50;
		if(units[plyptr->troop_type][CATAPULTNUM].attack > 0)
			plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] = 1;
		plyptr->attacks_lost[cont] = 0;
		plyptr->castle[cont] = 1;
		plyptr->tax[cont] = 2;
		plyptr->spy[cont] = -1;
		plyptr->copper[cont] = 15000;
		if(sail_protection == 1)
			plyptr->isprotected = 0;
		plyptr->tavern[cont] = 0;
		plyptr->event[cont] = 0;
		plyptr->promotion[cont] = 0;
		plyptr->surrender[cont] = (float)dft_surrender;
		strcpy(plyptr->kname[cont], dft_kname);
		sprintf(message, "5073%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5074%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5075%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
		send_channel(message, 0);
		sprintf(message, "5076%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		log_event("", message, plyptr->name);
	}
	else
	{
		sprintf(message, "5077%c%s%c%s%c", MSGDEL, conts[cont].name, MSGDEL, peasant_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		if((demote(cont, plyptr)) == 0)
			promote(cont, plyptr);
	}
	spy_report(cont, plyptr);
}

int sail_move(player *plyptr)
{
	int count;
	int move = 0;

	for(count = 0; count < MAXFLT; ++count)
		if((plyptr->ships[count].type > 0) && shiptypes[plyptr->ships[count].type].speed > move)
			move = shiptypes[plyptr->ships[count].type].speed;
	move += SAIL;
	if(check_hero(HERO_GIANT, plyptr))
		move /= 2;
	if(move < 1)
		move = 1;
	return(move);
}

void scuttle(int shpnum, player *plyptr)
{
	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(--shpnum < 0 || shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5015%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	remove_ship(shpnum, plyptr);
	sprintf(message, "5078%c%s%c", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

player *seabattle(player *attacker, player *defender)
{
	int ashpnum;
	int chance;
	int condition;
	int dshpnum;
	int fire = 0;
	player *result = NULL;

	if((ashpnum = find_randomshpnum(ATTACKSHIP, attacker)) < 0)
		ashpnum = find_randomshpnum(ANYSHIP, attacker);
	if((dshpnum = find_randomshpnum(ATTACKSHIP, defender)) < 0)
		dshpnum = find_randomshpnum(ANYSHIP, defender);
	if(ashpnum < 0 || dshpnum < 0)
		return(result);
	chance = shiptypes[attacker->ships[ashpnum].type].damage;
	if(check_hero(HERO_BARBLEADER, attacker))
		chance += 10;
	if(attacker->ships[ashpnum].cargotype >= CARGOREN && attacker->ships[ashpnum].cargotype <= CARGOMERC2)
		chance += 5;
	if(randomnum(100, 1) <= chance)
	{
		fire = compute_shpdmg(ashpnum, attacker);
		defender->ships[dshpnum].hull -= fire;
		if(defender->ships[dshpnum].hull < 0)
			defender->ships[dshpnum].hull = 0;
		if(randomnum(100, 1) <= 10)
		{
			switch(randomnum(3, 0))
			{
				case 1:
					sprintf(message, "5079%c%s%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL, shiptypes[defender->ships[dshpnum].type].name, MSGDEL);
					break;
				case 2:
					sprintf(message, "5029%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL);
					break;
				case 3:
					sprintf(message, "5080%c%s%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL, shiptypes[defender->ships[dshpnum].type].name, MSGDEL);
					break;
				default:
					sprintf(message, "5081%c%s%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL, shiptypes[defender->ships[dshpnum].type].name, MSGDEL);
					break;
			}
			send_channel(message, 0);
			condition = defender->ships[dshpnum].hull * 100 / shiptypes[defender->ships[dshpnum].type].hull;
			if(condition > 0)
			{
				sprintf(message, "5027%c%s%c%d%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL, condition, MSGDEL);
				send_channel(message, 0);
			}
			if(condition > 35)
			{
				switch(randomnum(2, 0))
				{
					case 1:
						sprintf(message, "5082%c%s%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL);
						break;
					case 2:
						sprintf(message, "5083%c%s%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL);
						break;
					default:
						sprintf(message, "5084%c%s%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL);
						break;
				}
				send_channel(message, 0);
			}
		}
		damage_cargo(fire, dshpnum, defender);
		if(defender->ships[dshpnum].hull <= 0)
		{
			take_cargo(dshpnum, defender, attacker);
			remove_ship(dshpnum, defender);
			sprintf(message, "5059%c%s%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL);
			send_message(message, defender->name, 1);
			sprintf(message, "5053%c%s%c%s%c", MSGDEL, defender->ships[dshpnum].name, MSGDEL, attacker->nick, MSGDEL);
			send_channel(message, 0);
		}
	}
	else
		if(randomnum(100, 1) <= 10)
		{
			switch(randomnum(2, 0))
			{
				case 1:
					sprintf(message, "5085%c%s%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL, shiptypes[defender->ships[dshpnum].type].name, MSGDEL);
					break;
				case 2:
					sprintf(message, "5031%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL);
					break;
				default:
					sprintf(message, "5032%c%s%c", MSGDEL, attacker->ships[ashpnum].name, MSGDEL);
					break;
			}
			send_channel(message, 0);
		}
	if(defender->fleet < 1)
		result = attacker;
	return(result);
}

void sell_ship(int shpnum, player *plyptr)
{
	float pct;
	int amount;
	int count;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(--shpnum < 0 || shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5008%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5094%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5095%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "5096%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 0; count < MAXFLT; ++count)
		{
			if(plyptr->ships[count].type == 0)
				continue;
			pct = (float)((float)plyptr->ships[count].hull / (float)shiptypes[plyptr->ships[count].type].hull);
			amount = (int)(shiptypes[plyptr->ships[count].type].cost / 2 * pct);
			sprintf(message, "5097%c%02d%c%-16s%c%d%c", MSGDEL, count + 1, MSGDEL, plyptr->ships[count].name, MSGDEL, amount, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		sprintf(message, "56%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.ship_qty[plyptr->cont][plyptr->ships[shpnum].type] >= MAXSHPINV) 
	{
		sprintf(message, "5107%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	++conq_info.ship_qty[plyptr->cont][plyptr->ships[shpnum].type];
	save_info();
	pct = (float)((float)plyptr->ships[shpnum].hull / (float)shiptypes[plyptr->ships[shpnum].type].hull);
	amount = (int)(shiptypes[plyptr->ships[shpnum].type].cost / 2 * pct);
	plyptr->copper[plyptr->cont] += amount;
	sprintf(message, "5098%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	remove_ship(shpnum, plyptr);
}

void show_fleet(char *name, char *type, player *plyptr)
{
	int count;
	int total[MAXSHP];

	for(count = 0; count < MAXSHP; ++count)
		total[count] = 0;
	for(count = 0; count < MAXFLT; ++count)
		++total[plyptr->ships[count].type];
	sprintf(message, "5092%c%s%c", MSGDEL, type, MSGDEL);
	for(count = 1; count < num_ships; ++count)
	{
		if(total[count] < 1)
			continue;
		sprintf(message, "%s%d %s ", message, total[count], shiptypes[count].name);
	}
	sprintf(message, "%s%c", message, MSGDEL);
	send_message(message, name, 1);
}

void sname(char *name, int shpnum, player *plyptr)
{
	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((strlen(name)) < 1 || shpnum == 0)
	{
		help(plyptr->name, "shipname", "", "");
		return;
	}
	if(--shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5015%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_word(plyptr->name, name, 31, 1))
	{
		strcpy(plyptr->ships[shpnum].name, name);
		sprintf(message, "5086%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void take_cargo(int dshpnum, player *loser, player *winner)
{
	int amount = 0;
	int count;
	int maxcargo = 0;

	if(loser->ships[dshpnum].cargotype <= CARGOMERC2)
		return;
	switch(loser->ships[dshpnum].cargotype)
	{
		case CARGOMONEY:
			amount = randomnum(shiptypes[loser->ships[dshpnum].type].maxcargo * 1000 / 4, 2);
			break;
		default:
			amount = randomnum(shiptypes[loser->ships[dshpnum].type].maxcargo / 4, 2);
			break;
	}
	for(count = 0; count < MAXFLT; ++count)
	{
		if(winner->ships[count].type < 1 || winner->ships[count].hull < 1)
			continue;
		maxcargo = find_maxcargo(count, loser->ships[dshpnum].cargotype, winner);
		if(winner->ships[count].cargotype == NOCARGO || (winner->ships[count].cargotype == loser->ships[dshpnum].cargotype && winner->ships[count].cargo < maxcargo))
			break;
	}
	if(count == MAXFLT)
		return;
	winner->ships[count].cargotype = loser->ships[dshpnum].cargotype;
	if(winner->ships[count].cargo + amount > maxcargo)
		amount = maxcargo - winner->ships[count].cargo;
	winner->ships[count].cargo += amount;
	switch(loser->ships[dshpnum].cargotype)
	{
		case CARGOPEASANT:
			sprintf(message, "5049%c%d%c%ss%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL);
			send_message(message, winner->name, 1);
			break;
		case CARGOFOOD:
			sprintf(message, "5050%c%d%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL);
			send_message(message, winner->name, 1);
			break;
		case CARGOMONEY:
			sprintf(message, "5051%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
			send_message(message, winner->name, 1);
			break;
		default:
			break;
	}
}

void unload(int amount, int shpnum, player *plyptr)
{
	char cargo[][32] =
	{
		"", "", "", "", "", "", "", "", "", "", "" 
	};
	int ok = 0;

	if(plyptr->fleet < 1)
	{
		sprintf(message, "5011%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(--shpnum < 0 || shpnum >= MAXFLT || plyptr->ships[shpnum].type == 0)
	{
		sprintf(message, "5015%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < UNLOAD)
	{
		sprintf(message, "1%c%d%cto unload %s%c", MSGDEL, UNLOAD, MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->ships[shpnum].cargotype == NOCARGO)
	{
		sprintf(message, "5087%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount == 0)
		amount = plyptr->ships[shpnum].cargo;
	if(amount < 0 || amount > plyptr->ships[shpnum].cargo)
	{
		sprintf(message, "5088%c%s%c%d%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, plyptr->ships[shpnum].cargo, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(plyptr->ships[shpnum].cargotype)
	{
		case CARGOPEASANT:
			strcpy(cargo[plyptr->ships[shpnum].cargotype], peasant_name);
			plyptr->peasant[plyptr->cont] += amount;
			ok = 1;
			break;
		case CARGOFOOD:
			strcpy(cargo[plyptr->ships[shpnum].cargotype], food_name);
			plyptr->food[plyptr->cont] += amount;
			ok = 1;
			break;
		case CARGOMONEY:
			strcpy(cargo[plyptr->ships[shpnum].cargotype], money_name);
			plyptr->copper[plyptr->cont] += amount;
			ok = 1;
			break;
		case CARGOCAT:
			strcpy(cargo[plyptr->ships[shpnum].cargotype], units[plyptr->troop_type][plyptr->ships[shpnum].cargotype].name);
			plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[plyptr->ships[shpnum].cargotype] += amount;
			ok = 1;
			break;
		default:
			strcpy(cargo[plyptr->ships[shpnum].cargotype], units[plyptr->troop_type][plyptr->ships[shpnum].cargotype].name);
			if(combined_army(plyptr->cont, plyptr) + amount <= plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum))
			{
				plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[plyptr->ships[shpnum].cargotype] += amount;
				ok = 1;
			}
			else
			{
				sprintf(message, "31%c%s%c%d%c%d%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum), MSGDEL, combined_army(plyptr->cont, plyptr), MSGDEL);
				send_message(message, plyptr->name, 1);
			}
			break;
	}
	if(ok)
	{
		plyptr->movepts -= UNLOAD;
		sprintf(message, "5089%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, cargo[plyptr->ships[shpnum].cargotype], MSGDEL, plyptr->ships[shpnum].cargotype > CARGOMONEY?" ":"(s) ", MSGDEL, plyptr->ships[shpnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		plyptr->ships[shpnum].cargo -= amount;
		if(plyptr->ships[shpnum].cargo == 0)
			plyptr->ships[shpnum].cargotype = NOCARGO;
	}
}
