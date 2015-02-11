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

void add_item(int itemnum, player *plyptr)
{
	if(check_artifact(ORB, plyptr) || plyptr->city == antimagic_city)
	{
		sprintf(message, "10010%c%s%c%s%c", MSGDEL, itemd[itemnum].name, MSGDEL, check_artifact(ORB, plyptr)?artifacts[ORB].name:cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		log_event("", message, plyptr->name);
	}
	else
	{
		++plyptr->items[0];
		plyptr->items[itemnum] = itemd[itemnum].uses;
	}
}

void buy_item(char *item, player *plyptr)
{
	int count;

/*
	if(ranger(plyptr) && !titan(plyptr))
	{
		sprintf(message, "10007%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
*/
	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((strlen(item)) < 1)
	{
		sprintf(message, "10052%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10053%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10054%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 1; count < TOTITM; ++count)
		{
			if(itemd[count].active == 0 || itemd[count].cost == -1)
				continue;
			sprintf(message, "10055%c%-32s%c%06d%c%d%c%s%c", MSGDEL, itemd[count].name, MSGDEL, itemd[count].cost, MSGDEL, conq_info.item_qty[plyptr->cont][count], MSGDEL, itemd[count].keyword, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		sprintf(message, "56%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	strip(item);
	for(count = 1; count < TOTITM; ++count)
		if((STRCASECMP(item, itemd[count].keyword)) == 0)
			break;
	if(count == TOTITM)
	{
		sprintf(message, "10001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(has_item(count, plyptr))
	{
		sprintf(message, "10056%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(itemd[count].cost < 0)
	{
		sprintf(message, "10063%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.item_qty[plyptr->cont][count] < 1)
	{
		sprintf(message, "10057%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BUY_ITEM)
	{
		sprintf(message, "1%c%d%cto purchase an item%c", MSGDEL, BUY_ITEM, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < itemd[count].cost)
	{
		sprintf(message, "10058%c%s%c%d%c", MSGDEL, itemd[count].name, MSGDEL, itemd[count].cost, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	--conq_info.item_qty[plyptr->cont][count];
	save_info();
	plyptr->movepts -= BUY_ITEM;
	plyptr->copper[plyptr->cont] -= itemd[count].cost;
	sprintf(message, "10059%c%s%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, itemd[count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	add_item(count, plyptr);
}

void destroy(char *item, player *plyptr)
{
	int count;

	if(strlen(item) < 1)
	{
		help(plyptr->name, "destroy", "", "");
		return;
	}
	if(plyptr->items[0] < 1)
	{
		sprintf(message, "10000%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	strip(item);
	for(count = 1; count < TOTITM; ++count)
		if((STRCASECMP(item, itemd[count].keyword)) == 0)
			break;
	if(count == TOTITM)
	{
		sprintf(message, "10001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!has_item(count, plyptr))
	{
		sprintf(message, "10002%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10003%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < DESTROY_ITEM)
	{
		sprintf(message, "1%c%d%cto destroy an item%c", MSGDEL, DESTROY_ITEM, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= DESTROY_ITEM;
	sprintf(message, "10004%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	remove_item(count, plyptr);
}

void dsp_items(player *plyptr)
{
	int count;
	char str[16] = {0};

	if(plyptr->items[0] < 1)
	{
		sprintf(message, "10000%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "10005%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10090%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10091%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 1; count < TOTITM; ++count)
		if(has_item(count, plyptr))
		{
			if(plyptr->items[count] == -1)
				strcpy(str, "Unlimited");
			else
				sprintf(str, "%d", plyptr->items[count]);
			sprintf(message, "10006%c%-32s%c%-16s%c%s%c", MSGDEL, itemd[count].name, MSGDEL, itemd[count].keyword, MSGDEL, str, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

int find_randomitem()
{
	int count;
	int itms[TOTITM];
	int current = 0;
 
	for(count = 1; count < TOTITM; ++count)
		if(itemd[count].active == 1)
			itms[current++] = count;
	if(current == 0)
		return(0);
	else
		return(itms[randomnum(current - 1, 0)]);
}

int has_item(int itemnum, player *plyptr)
{
	if(plyptr->items[itemnum] != 0)
		return(1);
	else
		return(0);
}

void item_decay()
{
	int cont;
	int count;

	for(cont = 0; cont < num_conts; ++cont)
	{
		for(count = 0; count < TOTITM; ++count)
			if(conq_info.item_qty[cont][count] > 0)
				conq_info.item_qty[cont][count] -= randomnum(conq_info.item_qty[cont][count], 0);
	}
	save_info();
}

void item_hk(int cont, player *plyptr)
{
	int amount;

	if(has_item(ITEM_AXE, plyptr) && randomnum(100, 1) >= 50)
	{
		amount = randomnum(10, 1);
		plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
		sprintf(message, "10016%c%c", MSGDEL, MSGDEL);
		log_event("", message, plyptr->name);
		sprintf(message, "10017%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL);
		log_event("", message, plyptr->name);
		remove_charge(ITEM_AXE, plyptr);
	}
	if(has_item(ITEM_TOME, plyptr) && randomnum(100, 1) >= 50)
	{
		use_tome(plyptr);
		remove_charge(ITEM_TOME, plyptr);
	}
}

void quest_3(player *plyptr)
{
	int inum = 0;

	if(ranger(plyptr) && !titan(plyptr))
	{
		sprintf(message, "10007%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < (plyptr->level[plyptr->cont] + 1) * 2)
	{
		sprintf(message, "1%c%d%cto search for an item%c", MSGDEL, (plyptr->level[plyptr->cont] + 1) * 2, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= (plyptr->level[plyptr->cont] + 1) * 2;
	if((inum = find_randomitem()) < 1 || has_item(inum, plyptr) || randomnum(100, 1) <= 55 - (plyptr->level[plyptr->cont] + 1) * 2 || (inum == ITEM_ORB && plyptr->isprotected == 0))
	{
		sprintf(message, "10008%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "10009%c%s%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, itemd[inum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	add_item(inum, plyptr);
}

void read_items()
{
	char input[1024];
	char str[16] = {0};
	char *mid;
	char *start;
	FILE *fp;
	int num_items = 1;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, ITEMFIL);
	if((fp = fopen(ITEMFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, ITEMFIL, strerror(errno));
		return;
	}
	while((fgets(input, 1024, fp)) != NULL && num_items < TOTITM)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%d %d %d %d %15s", &itemd[num_items].active, &itemd[num_items].uses, &itemd[num_items].mpcost, &itemd[num_items].cost, str);
		if((start = strstr(input, str)) == NULL)
			continue;
		if((mid = strchr(start, '@')) == NULL)
			continue;
		if(itemd[num_items].active < 0 || itemd[num_items].active > 1)
			itemd[num_items].active = 0;
		if(itemd[num_items].uses < -1)
			itemd[num_items].uses = 1;
		if(itemd[num_items].mpcost < 0)
			itemd[num_items].mpcost = 0;
		if(itemd[num_items].cost < -1)
			itemd[num_items].cost = 0;
		strncpy(itemd[num_items].keyword, start, (mid - start) > 15?15:(mid - start));
		strncpy(itemd[num_items++].name, mid + 1, 63);
	}
	fclose(fp);
}

void remove_all_items(player *plyptr)
{
	int count;

	for(count = 0; count < TOTITM; ++count)
		plyptr->items[count] = 0;
}

void remove_charge(int itemnum, player *plyptr)
{
	if(plyptr->items[itemnum] > 0)
	{
		--plyptr->items[itemnum];
		if(plyptr->items[itemnum] == 0)
			remove_item(itemnum, plyptr);
	}
}

void remove_item(int itemnum, player *plyptr)
{
	--plyptr->items[0];
	plyptr->items[itemnum] = 0;
}

void sell_item(char *item, player *plyptr)
{
	int count;

	if(plyptr->city != conts[plyptr->cont].port)
	{
		sprintf(message, "5005%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, cities[conts[plyptr->cont].port].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((strlen(item)) < 1)
	{
		sprintf(message, "10060%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10053%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10054%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 1; count < TOTITM; ++count)
		{
			if(itemd[count].active == 0 || itemd[count].cost == -1)
				continue;
			sprintf(message, "10055%c%-32s%c%06d%c%d%c", MSGDEL, itemd[count].name, MSGDEL, itemd[count].cost / 2, MSGDEL, conq_info.item_qty[plyptr->cont][count], MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		return;
	}
	strip(item);
	for(count = 1; count < TOTITM; ++count)
		if((STRCASECMP(item, itemd[count].keyword)) == 0)
			break;
	if(count == TOTITM)
	{
		sprintf(message, "10001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!has_item(count, plyptr))
	{
		sprintf(message, "10002%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10003%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(itemd[count].cost < 0)
	{
		sprintf(message, "10063%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.item_qty[plyptr->cont][count] >= MAXITMINV)
	{
		sprintf(message, "10062%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	++conq_info.item_qty[plyptr->cont][count];
	save_info();
	plyptr->copper[plyptr->cont] += itemd[count].cost / 2;
	sprintf(message, "10061%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	remove_item(count, plyptr);
}

void use(char *item, char *name, player *plyptr)
{
	int count;
	int useok = 0;

	if(plyptr->items[0] < 1)
	{
		sprintf(message, "10000%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	strip(item);
	for(count = 1; count < TOTITM; ++count)
		if((STRCASECMP(item, itemd[count].keyword)) == 0)
			break;
	if(count == TOTITM)
	{
		sprintf(message, "10001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!has_item(count, plyptr))
	{
		sprintf(message, "10002%c%s%c", MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10003%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < itemd[count].mpcost)
	{
		sprintf(message, "1%c%d%cto use %s%c", MSGDEL, itemd[count].mpcost, MSGDEL, itemd[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_artifact(ORB, plyptr) || plyptr->city == antimagic_city)
	{
		sprintf(message, "10011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(count)
	{
		case ITEM_EYE:
			useok = use_eye(name, plyptr);
			break;
		case ITEM_SPEED:
			useok = use_speed(plyptr);
			break;
		case ITEM_STONE:
			useok = use_stone(plyptr);
			break;
		case ITEM_ORB:
			useok = use_orb(plyptr);
			break;
		case ITEM_COW:
			useok = use_cow(plyptr);
			break;
		case ITEM_TRUMPET:
			useok = use_trumpet(plyptr);
			break;
		case ITEM_CARPET:
			useok = use_carpet(name, plyptr);
			break;
		case ITEM_SCEPTRE:
			useok = use_sceptre(plyptr);
			break;
		case ITEM_ENDURANCE:
			useok = use_endurance(plyptr);
			break;
		case ITEM_QUIVER:
			useok = use_quiver(plyptr);
			break;
		case ITEM_BUILDERS:
			useok = use_builders(plyptr);
			break;
		case ITEM_AXE:
			useok = use_axe(plyptr);
			break;
		case ITEM_THIEVES:
			useok = use_thieves(plyptr);
			break;
		case ITEM_LURE:
			useok = use_lure(plyptr);
			break;
		case ITEM_MASK:
			useok = use_mask(name, plyptr);
			break;
		case ITEM_WAND:
			useok = use_wand(name, plyptr);
			break;
		case ITEM_DUST:
			useok = use_dust(name, plyptr);
			break;
		case ITEM_BOAT:
			useok = use_boat(plyptr);
			break;
		case ITEM_AMULET:
			useok = use_amulet(name, plyptr);
			break;
		case ITEM_RING:
			useok = use_ring(name, plyptr);
			break;
		case ITEM_POUCH:
			useok = use_pouch(plyptr);
			break;
		case ITEM_TRICKS:
			useok = use_tricks(plyptr);
			break;
		case ITEM_TOME:
			useok = use_tome(plyptr);
			break;
		case ITEM_CHAOS:
		case ITEM_BLADE:
		case ITEM_GAUNTLETS:
		case ITEM_HOLDING:
		case ITEM_SIREN:
		case ITEM_LETTER:
		case ITEM_BANNER:
		case ITEM_WART:
			useok = use_always(count, plyptr);
			break;
		case ITEM_SHIELD:
			useok = use_shield(name, plyptr);
			break;
		case ITEM_FLAG:
			useok = use_flag(plyptr);
			break;
		case ITEM_EMBLEM:
			useok = use_emblem(name, plyptr);
			break;
		case ITEM_BOULDER:
			useok = use_boulder(name, plyptr);
			break;
		case ITEM_COWL:
			useok = use_cowl(name, plyptr);
			break;
		case ITEM_VIAL:
			useok = use_vial(name, plyptr);
			break;
	}
	if(useok)
	{
		plyptr->movepts -= itemd[count].mpcost;
		remove_charge(count, plyptr);
		if(useok & 2)
		{
			plyptr->lastattack = time(NULL);
			++plyptr->attacks;
			plyptr->isprotected = 1;
		}
	}
}

int use_always(int itemnum, player *plyptr)
{
	sprintf(message, "10018%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(0);

}

int use_amulet(char *name, player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int num_castles = 0;
	int plynum;
	int round = 1;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	sprintf(message, "10012%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->ally == 0)
	{
		combatants[ATTACKER] = plyptr;
		combatants[DEFENDER] = &players[plynum];
		combatants[ATTACKER_HORDE1] = (void *)plybuf[ATTACKER_HORDE1];
		summon_creature(0, randomnum(LION, WOLF), ARMY_CAMPAIGN, ARMY_CAMPAIGN, -1, combatants[ATTACKER_HORDE1], plyptr);
		sprintf(message, "10013%c%s%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, players[plynum].nick, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "10014%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		start_battle();
	}
	else
	{
		sprintf(message, "10015%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
		log_event("", message, players[plynum].name);
		combatants[ATTACKER] = (void *)plybuf[ATTACKER];
		combatants[DEFENDER] = &players[plynum];
		summon_creature(1, randomnum(LION, WOLF), ARMY_DEFENSE, ARMY_CAMPAIGN, -1, combatants[ATTACKER], &players[plynum]);
		if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
			beast_won(&players[plynum], "Pack", PACK);
		else
			beast_lost(&players[plynum], "Pack", PACK);
		reset_combatants();
		sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
	}
	save_player(plynum);
	return(2);
}

int use_axe(player *plyptr)
{
	sprintf(message, "10016%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10017%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	random_event(plyptr->cont, EVENT_SICKNESS, plyptr);
	return(1);
}


int use_boat(player *plyptr)
{
	int shpnum;

	if(num_ships < 1)
	{
		sprintf(message, "10051%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	if((shpnum = find_shpnum(plyptr)) < 0)
	{
		sprintf(message, "10047%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	sprintf(message, "10048%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	add_ship(shpnum, 1, plyptr);
	return(1);
}

int use_boulder(char *cityname, player *plyptr)
{
	int ctynum;

	if(strlen(cityname) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "18%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == plyptr->city)
	{
		sprintf(message, "10072%c%s%c", MSGDEL, itemd[ITEM_BOULDER].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "20%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].lock == 0)
	{
		sprintf(message, "10073%c%s%c%s%c", MSGDEL, itemd[ITEM_BOULDER].name, MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	if(randomnum(100, 1) <= 50)
	{
		cities[ctynum].lock = 0;
		sprintf(message, "10077%c%s%c%s%c", MSGDEL, itemd[ITEM_BOULDER].name, MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10078%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL, itemd[ITEM_BOULDER].name, MSGDEL);
		send_channel(message, 0);
		return(1);
	}
	sprintf(message, "10074%c%s%c%s%c%s%c", MSGDEL, itemd[ITEM_BOULDER].name, MSGDEL, cities[ctynum].name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	if(randomnum(100, 1) <= 10 && plyptr->castle[plyptr->cont] > 1)
	{
		--plyptr->castle[plyptr->cont];
		sprintf(message, "10075%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
	{
		sprintf(message, "10076%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	return(1);
}

int use_builders(player *plyptr)
{
	int amount;

	if(units[plyptr->troop_type][CATAPULTNUM].attack == 0)
	{
		sprintf(message, "10050%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	amount = randomnum(4, 1);
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[CATAPULTNUM] += amount;
	sprintf(message, "10019%c%d%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][CATAPULTNUM].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_carpet(char *cityname, player *plyptr)
{
	int ctynum;

	if(strlen(cityname) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "18%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == plyptr->city)
	{
		sprintf(message, "19%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "20%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->city = ctynum;
	sprintf(message, "10020%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_cow(player *plyptr)
{
	int amount;

	amount = randomnum(500, 100);
	plyptr->food[plyptr->cont] += amount;
	sprintf(message, "10021%c%d%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_cowl(char *name, player *plyptr)
{
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = allow_spell(name, plyptr)) == 0)
		return(0);
	if(players[plynum].level[plyptr->cont] == 0 || titan(&players[plynum]) || vampire(&players[plynum]))
	{
		sprintf(message, "10079%c%s%c%s%c", MSGDEL, itemd[ITEM_COWL].name, MSGDEL, select_title(players[plynum].classnum, players[plynum].level[plyptr->cont], players[plynum].gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(randomnum(100, 1) > 10 - (players[plynum].level[plyptr->cont] + 1))
	{
		sprintf(message, "10080%c%s%c%s%c", MSGDEL, itemd[ITEM_COWL].name, MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	change_level(plyptr->cont, -1, &players[plynum]);
	sprintf(message, "10081%c%s%c%s%c", MSGDEL, itemd[ITEM_COWL].name, MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10082%c%s%c%s%c", MSGDEL, itemd[ITEM_COWL].name, MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	sprintf(message, "10083%c%c", MSGDEL, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "10084%c%c", MSGDEL, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	check_ret(plyptr->cont, plyptr, &players[plynum]);
	update_ret(plyptr->cont, plyptr, &players[plynum]);
	save_player(plynum);
	return(1);
}

int use_dust(char *name, player *plyptr)
{
	return(spell_move(name, plyptr));
}

int use_emblem(char *name, player *plyptr)
{
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	sprintf(message, "10070%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	combatants[ATTACKER] = plyptr;
	combatants[DEFENDER] = &players[plynum];
	combatants[ATTACKER_HORDE1] = (void *)plybuf[ATTACKER_HORDE1];
	enemy(0, IMMORTAL, ARMY_CAMPAIGN, ARMY_CAMPAIGN, WEAK, combatants[ATTACKER_HORDE1], plyptr);
	sprintf(message, "10071%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	send_channel(message, 0);
	start_battle();
	save_player(plynum);
	return(2);
}

int use_endurance(player *plyptr)
{
	plyptr->attacks -= 2;
	sprintf(message, "10022%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_eye(char *name, player *plyptr)
{
	int cont;
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = find_player(name)) < 1)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(playernum(plyptr) == plynum)
	{
		sprintf(message, "21%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return(0);
	if(check_artifact(MIRROR, &players[plynum]))
	{
		sprintf(message, "10023%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(check_artifact(ORB, &players[plynum]) || players[plynum].city == antimagic_city)
	{
		sprintf(message, "10011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	sprintf(message, "10024%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	person("", plyptr, &players[plynum]);
	cont = players[plynum].cont;
	show_surrender(plyptr->name, &players[plynum]);
	sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
	send_message(message, plyptr->name, 1);
	review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_NOBG, ARMY_DEFENSE, &players[plynum], &players[plynum]);
	show_formation(plyptr->name, cont, ARMY_DEFENSE, &players[plynum]);
	if(advanced_armies == 1)
	{
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_NOBG, ARMY_CAMPAIGN, &players[plynum], &players[plynum]);
		show_formation(plyptr->name, cont, ARMY_CAMPAIGN, &players[plynum]);
	}
	return(1);
}

int use_flag(player *plyptr)
{
	if(attack_reprieve == 0)
	{
		sprintf(message, "10066%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->attacks_lost[plyptr->cont] >= attacks_allowed(plyptr))
	{
		sprintf(message, "10067%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	++plyptr->attacks;
	++plyptr->attacks_lost[plyptr->cont];
	if(!barbarian(plyptr) && !vampire(plyptr))
		--plyptr->honor[plyptr->cont];
	sprintf(message, "10068%c%s%c%s%c", MSGDEL, itemd[ITEM_FLAG].name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10069%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, 
	itemd[ITEM_FLAG].name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	return(1);
}

int use_lure(player *plyptr)
{
	char lord[32] = {0};
	char str[32] = {0};
	char str2[32] = {0};

	if(plyptr->lord == 0 || conq_info.vampire == plyptr->lord)
		strcpy(lord, "???");
	else
		strcpy(lord, players[plyptr->lord].name);
	sprintf(message, "10026%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "22%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "23%c%-8s%c%s%c%s%c%s%c", MSGDEL, symbols[0][plyptr->classnum], MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	sprintf(str, "%d Movept(s)", plyptr->movepts);
	sprintf(str2, "%d %s(s)", plyptr->castle[plyptr->cont], structures[BASE_CLASSNUM(plyptr->classnum)].name);
	sprintf(message, "24%c%-8s%c%-16s%c%-16s%c%d%c%s%c", MSGDEL, symbols[1][plyptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, plyptr->copper[plyptr->cont], MSGDEL, money_name, MSGDEL);
	send_channel(message, 0);
	sprintf(str, "%d Land", plyptr->land[plyptr->cont]);
	sprintf(str2, "%d %s(s)", plyptr->peasant[plyptr->cont], peasant_name);
	sprintf(message, "25%c%-8s%c%-16s%c%-16s%c%d%c%s%c", MSGDEL, symbols[2][plyptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, plyptr->food[plyptr->cont], MSGDEL, food_name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "26%c%-8s%c%s%c%s%c%s%c", MSGDEL, symbols[3][plyptr->classnum], MSGDEL, plyptr->nick, MSGDEL, lord, MSGDEL, plyptr->lord == 0?"???":players[plyptr->lord].kname[plyptr->cont], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "27%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, plyptr->winmsg, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "28%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, plyptr->city == secret_city?"??????":cities[plyptr->city].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	return(1);
}

int use_mask(char *name, player *plyptr)
{
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	return(spell_polymorph(name, plyptr));
}

int use_orb(player *plyptr)
{
	if(plyptr->isprotected == 0)
	{
		sprintf(message, "10027%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->isprotected = 0;
	sprintf(message, "10028%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][2], MSGDEL);
	send_channel(message, 0);
	return(1);
}

int use_pouch(player *plyptr)
{
	int count;

	if(!mage(plyptr) && !titan(plyptr) && !check_hero(HERO_DRAGON, plyptr))
	{
		sprintf(message, "10029%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	for(count = 0; count < MAXCMP; ++count)
		if(plyptr->components[count] < MAXCOMPONENT)
			++plyptr->components[count];
	sprintf(message, "10030%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_quiver(player *plyptr)
{
	int amount;
	int count;

	for(count = 0; count < CATAPULTNUM; ++count)
		if(units[plyptr->troop_type][count].attack > 0 && units[plyptr->troop_type][count].defend > 0 && units[plyptr->troop_type][count].range == 1)
			break;
	if(count == CATAPULTNUM)
	{
		sprintf(message, "10049%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	amount = randomnum(100, 25);
	if((amount = fill_castles(amount, plyptr->cont, plyptr)) <= 0)
	{
		sprintf(message, "10049%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[count] += amount;
	sprintf(message, "10031%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10032%c%d%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_ring(char *wish, player *plyptr)
{
	char wishes[][16] = {"", "", "", "land", "move", ""};
	int amount;
	int count;

	strcpy(wishes[0], structures[BASE_CLASSNUM(plyptr->classnum)].name);
	strcpy(wishes[1], money_name);
	strcpy(wishes[2], food_name);
	strcpy(wishes[5], peasant_name);
	sprintf(message, "10033%c", MSGDEL);
	for(count = 0; count < 6; ++count)
	{
		sprintf(message, "%s %s", message, wishes[count]);
		if((STRCASECMP(wish, wishes[count])) == 0)
			break;
	}
	if(count == 6)
	{
		sprintf(message, "%s%c", message, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	sprintf(message, "10034%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	switch(count)
	{
		case 0:
			amount = randomnum(5, 1);
			if((plyptr->castle[plyptr->cont] + amount) * structures[BASE_CLASSNUM(plyptr->classnum)].land > plyptr->land[plyptr->cont])
				amount = plyptr->land[plyptr->cont] / structures[BASE_CLASSNUM(plyptr->classnum)].land - plyptr->castle[plyptr->cont];
			if(amount < 1)
			{
				sprintf(message, "10035%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			}
			plyptr->castle[plyptr->cont] += amount;
			sprintf(message, "10036%c%d%c%s(s)%c", MSGDEL, amount, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
			break;
		case 1:
			amount = randomnum(100000, 10000);
			plyptr->copper[plyptr->cont] += amount;
			sprintf(message, "10036%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
			break;
		case 2:
			amount = randomnum(500, 100);
			plyptr->food[plyptr->cont] += amount;
			sprintf(message, "10036%c%d%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL);
			break;
		case 3:
			amount = randomnum(500, 100);
			plyptr->land[plyptr->cont] += amount;
			sprintf(message, "10036%c%d%c%s%c", MSGDEL, amount, MSGDEL, "land", MSGDEL);
			break;
		case 4:
			amount = randomnum(25, 10);
			plyptr->movepts += amount;
			sprintf(message, "10036%c%d%c%s%c", MSGDEL, amount, MSGDEL, "Movement Points", MSGDEL);
			break;
		default:
			amount = randomnum(500, 100);
			plyptr->peasant[plyptr->cont] += amount;
			sprintf(message, "10036%c%d%c%ss%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_sceptre(player *plyptr)
{
	sprintf(message, "10037%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10038%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10039%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	delete_player(playernum(plyptr));
	return(0);
}

int use_shield(char *name, player *plyptr)
{
	int count;
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	sprintf(message, "10065%c%s%c", MSGDEL, itemd[ITEM_SHIELD].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < CATAPULTNUM; ++count)
		batmod[ATTACKER][count].shield = 1;
	combatants[ATTACKER] = plyptr;
	combatants[DEFENDER] = &players[plynum];
	start_battle();
	save_player(plynum);
	return(2);
}

int use_speed(player *plyptr)
{
	int amount;

	amount = randomnum(20, 10);
	plyptr->movepts += amount + itemd[ITEM_SPEED].mpcost;
	sprintf(message, "10040%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int use_stone(player *plyptr)
{
	plyptr->movepts += 5;
	plyptr->castle[plyptr->cont] += 1;
	plyptr->land[plyptr->cont] += 50;
	plyptr->peasant[plyptr->cont] += 50;
	plyptr->food[plyptr->cont] += 100;
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[SOLDIERNUM] += 75;
	if(units[plyptr->troop_type][ARCHERNUM].attack > 0 && units[plyptr->troop_type][ARCHERNUM].defend > 0)
		plyptr->army[ARMY_DEFENSE][plyptr->cont].total[ARCHERNUM] += 25;
	if(units[plyptr->troop_type][CATAPULTNUM].attack > 0)
		++plyptr->army[ARMY_DEFENSE][plyptr->cont].total[CATAPULTNUM];
	sprintf(message, "10041%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][2], MSGDEL);
	send_channel(message, 0);
	return(1);
}

int use_thieves(player *plyptr)
{
	int amount;

	if(cleric(plyptr) && !titan(plyptr))
	{
		sprintf(message, "3059%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	amount = randomnum(100000, 10000);
	plyptr->copper[plyptr->cont] += amount;
	sprintf(message, "10042%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	save_city(plyptr->city);
	return(1);
}

int use_tome(player *plyptr)
{
	player *caster;

	sprintf(message, "10043%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	caster = (void *)plybuf[ATTACKER];
	memset(caster, 0, PLYSIZ);
	caster->classnum = randomnum(20 + (num_conts - 1), 20);
	caster->cont = plyptr->cont;
	caster->level[caster->cont] = plyptr->level[plyptr->cont];
	strcpy(caster->name, DFTMAGE);
	strcpy(caster->nick, DFTMAGE);
	switch(randomnum(1, 0))
	{
		case 0:
			spell_giant(plyptr->name, caster);
			break;
		default:
			spell_dragon(plyptr->name, caster);
			break;
	}
	return(1);
}

int use_tricks(player *plyptr)
{
	sprintf(message, "10044%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	switch(randomnum(9, 0))
	{
		case 0:
			use_axe(plyptr);
			break;
		case 1:
			use_builders(plyptr);
			break;
		case 2:
			use_cow(plyptr);
			break;
		case 3:
			use_endurance(plyptr);
			break;
		case 4:
			use_lure(plyptr);
			break;
		case 5:
			use_quiver(plyptr);
			break;
		case 6:
			use_speed(plyptr);
			break;
		case 7:
			use_stone(plyptr);
			break;
		case 8:
			use_tome(plyptr);
			break;
		default:
			use_thieves(plyptr);
			break;
	}
	return(1);
}

int use_trumpet(player *plyptr)
{
	int amount;

	if(!fighter(plyptr) || plyptr->troop_type == VAMPIRE)
	{
		sprintf(message, "10045%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	amount = (plyptr->level[plyptr->cont] + 1) * 10;
	if((amount = fill_castles(amount, plyptr->cont, plyptr)) <= 0)
	{
		sprintf(message, "10045%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[BLOODGUARDNUM] += amount;
	sprintf(message, "10046%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	return(1);
}

int use_vial(char *name, player *plyptr)
{
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = allow_spell(name, plyptr)) == 0)
		return(0);
	if(randomnum(100, 1) > 40 + ((players[plynum].level[plyptr->cont] + 1) * 2))
	{
		sprintf(message, "10086%c%s%c%s%c", MSGDEL, players[plynum].kname[plyptr->cont], MSGDEL, itemd[ITEM_VIAL].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "10087%c%s%c%s%c", MSGDEL, players[plynum].kname[plyptr->cont], MSGDEL, plyptr->name, MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		return(1);
	}
	players[plynum].event[plyptr->cont] = 16;
	check_ret(plyptr->cont, plyptr, &players[plynum]);
	update_ret(plyptr->cont, plyptr, &players[plynum]);
	save_player(plynum);
	sprintf(message, "10088%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "10089%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, players[plynum].kname[plyptr->cont], MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	return(1);
}

int use_wand(char *name, player *plyptr)
{
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	return(spell_fireball(name, plyptr));
}
