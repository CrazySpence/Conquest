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

void add_incr_move()
{
	int cont;
	int count;
	int max;
	int mod = 0;

	if(mps_15 < 1)
		return;
	for(count = 1; count < current_player; ++count)
	{
		if(strlen(players[count].name) < 1 || players[count].special_misc & SPC_VACATION)
			continue;
		cont = players[count].cont;
		switch(BASE_CLASSNUM(players[count].classnum))
		{
			case MAGE:
			case RANGER:
				mod = 16;
				break;
			case VAMPIRE:
				mod = 10;
				break;
			default:
				mod = 12;
				break;
		}
		max = ((players[count].levelmax + 1) * mod + mod) * 2;
		if(players[count].movepts >= max)
			continue;
		players[count].movepts += mps_15;
		save_player(count);
	}
}

void add_move(player *plyptr)
{
	int count;
	int plynum;
	struct tm *ptr;
	time_t current;

	plyptr->movepts /= 2;
	switch(BASE_CLASSNUM(plyptr->classnum))
	{
		case MAGE:
		case RANGER:
			plyptr->movepts += (plyptr->levelmax + 1) * 12;
			break;
		case VAMPIRE:
			plyptr->movepts += (plyptr->levelmax + 1) * 8;
			time(&current);
			ptr = localtime(&current);
			if(ptr->tm_hour >= 0 && ptr->tm_hour <= 6)
				plyptr->movepts += 6 - ptr->tm_hour;
			if(ptr->tm_hour >= 18 && ptr->tm_hour <= 23)
				plyptr->movepts += ptr->tm_hour;
			break;
		default:
			plyptr->movepts += (plyptr->levelmax + 1) * 10;
			break;
	}
	plyptr->movepts += plyptr->kingdoms * 4;
	plyptr->movepts += plyptr->remorts * 4;
	plynum = playernum(plyptr);
	for(count = 0; count < 5; ++count)
		if(conq_info.ranks[count] == plynum)
		{
			plyptr->movepts += (5 - count);
			break;
		}
	if(check_artifact(BOOTS, plyptr))
		plyptr->movepts += 10;
}

void cast_ar()
{
	int plynum;
	player *plyptr;

	plynum = add_player(DFTMAGE, "unknown@unknown", randomnum(20 + (num_conts - 1), 20), -1, -1);
	plyptr = &players[plynum];
	change_level(plyptr->cont, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1 - plyptr->level[plyptr->cont], plyptr);
	sprintf(message, "9000%c%d%c%s%c", MSGDEL, conq_info.today, MSGDEL, select_classname(MAGE), MSGDEL);
	send_channel(message, 0);
	sprintf(message, "9001%c%s%c", MSGDEL, age, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "9002%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
	spell_alterreality(1, plyptr);
}

void castle_income(int cont, player *plyptr)
{
	int amount;
	int income;

	if((amount = plyptr->peasant[cont] / 50) > plyptr->castle[cont])
		amount = plyptr->castle[cont];
	if(amount < 1 || titan(plyptr) || structures[BASE_CLASSNUM(plyptr->classnum)].tax == 0)
		return;
	income = amount * (BASETAX * plyptr->tax[cont]);
	plyptr->copper[cont] += income;
	sprintf(message, "9003%c%d%c%s%c%s%c", MSGDEL, income, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
}

void check_castles(int cont, player *plyptr)
{
	int castles;
	int lost;

	if(plyptr->land[cont] >= plyptr->castle[cont] * structures[BASE_CLASSNUM(plyptr->classnum)].land || titan(plyptr))
		return;
	lost = plyptr->land[cont] / structures[BASE_CLASSNUM(plyptr->classnum)].land;
	castles = plyptr->castle[cont] - lost;
	plyptr->castle[cont] = lost;
	if(plyptr->castle[cont] < 1)
		plyptr->castle[cont] = 1;
	sprintf(message, "9005%c%d%c%s%c%s%c", MSGDEL, castles, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
}

void check_chest(int cont, player *plyptr)
{
	int amount;

	if(!check_artifact(CHEST, plyptr))
		return;
	if(check_hero(HERO_ALCHEMIST, plyptr))
		amount = randomnum(200000, 100000);
	else
		amount = randomnum(150000, 75000);
	plyptr->copper[cont] += amount;
	sprintf(message, "9007%c%s%c%d%c%s%c%s%c", MSGDEL, artifacts[CHEST].name, MSGDEL, amount, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
	return;
}

void check_crown(int cont, player *plyptr)
{
	int amount;
	int count;
	int total = 0;

	if(!check_artifact(CROWN, plyptr) || barbarian(plyptr) || plyptr->troop_type == VAMPIRE)
		return;
	for(count = SOLDIERNUM; count <= KNIGHTNUM; ++count)
	{
		if(units[plyptr->troop_type][count].attack == 0 && units[plyptr->troop_type][count].defend == 0)
			continue;
		amount = randomnum(200, 100);
		if((amount = fill_castles(amount, cont, plyptr)) <= 0)
			break;
		total += amount;
		plyptr->army[ARMY_DEFENSE][cont].total[count] += amount;
	}
	if(total > 0)
	{
		sprintf(message, "9006%c%s%c%d%c%s%c", MSGDEL, artifacts[CROWN].name, MSGDEL, total, MSGDEL, conts[cont].name, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

void check_reign(int cont, player *plyptr)
{
	FILE *fp;
	int count;
	int hero;

	if(plyptr->level[cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1 || titan(plyptr))
		return;
	if(conq_info.today - plyptr->king[cont] < REIGN && plyptr->king[cont] > 0)
		return;
	sprintf(message, "9010%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL);
	send_message(message, plyptr->name, 1);
	log_event("", message, plyptr->name);
	if(plyptr->remorts + 1 <= num_remorts && (plyptr->land[cont] >= promotions[BASE_CLASSNUM(plyptr->classnum)][maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1][1] || vampire(plyptr)))
		++plyptr->remorts;
	if(conq_info.agewon == 0 && plyptr->remorts >= num_remorts && conq_info.today > end_date)
	{
		conq_info.agewon = 1;
		save_info();
		sprintf(message, "9011%c%s%c%s%c%s%c%d%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->name, MSGDEL, age, MSGDEL, conq_info.today, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "%s %s conquered the %s Age in %d.", select_class(plyptr->classnum), plyptr->name, age, conq_info.today);
		strcpy(conq_info.motd, message);
		if((fp = fopen(TITFIL, "a")) != NULL)
		{
			fprintf(fp, "%s\n", message);
			fclose(fp);
		}
		if(vampire(plyptr))
		{
			destroy_vampires(plyptr);
			conq_info.vampire = 0;
			save_info();
			strcpy(plyptr->nick, plyptr->name);
		}
		make_titan(0, plyptr);
		return;
	}
	if(plyptr->kingdoms == 1)
	{
		--totply[plyptr->classnum];
		if(conq_info.vampire == playernum(plyptr))
		{
			destroy_vampires(plyptr);
			conq_info.vampire = 0;
			save_info();
			strcpy(plyptr->nick, plyptr->name);
		}
		switch(BASE_CLASSNUM(plyptr->classnum))
		{
			case BARBARIAN:
			case VAMPIRE:
				plyptr->classnum = randomnum(total_races[FIGHTER] - 1, 0);
				break;
			case MAGE:
				if(associate_races == 1)
					plyptr->classnum -= 20;
				else
					plyptr->classnum = randomnum(20 + total_races[MAGE] - 1, 20);
				break;
			case CLERIC:
				if(associate_races == 1)
					plyptr->classnum -= 40;
				else
					plyptr->classnum = randomnum(40 + total_races[CLERIC] - 1, 40);
				break;
			case RANGER:
				if(associate_races == 1)
					plyptr->classnum -= 50;
				else
					plyptr->classnum = randomnum(50 + total_races[RANGER] - 1, 50);
				break;
			default:
				break;
		}
		++totply[plyptr->classnum];
		plyptr->ally = 0;
		plyptr->ally_request = 0;
		for(count = 0; count < 6; ++count)
			plyptr->components[count] = 0;
		remove_all_items(plyptr);
	}
	change_level(cont, -(plyptr->level[cont]), plyptr);
	plyptr->movepts = 30;
	plyptr->copper[cont] = 10000 + (plyptr->copper[cont] * randomnum(3, 1) / 100);
	plyptr->food[cont] = 150 + (plyptr->food[cont] * randomnum(3, 1) / 100);
	plyptr->peasant[cont] = 100 + (plyptr->peasant[cont] * randomnum(3, 1) / 100);
	plyptr->honor[cont] /= 20;
	plyptr->isprotected = 0;
	plyptr->event[cont] = 0;
	plyptr->promotion[cont] = 0;
	if((hero = find_hero(plyptr)) >= 0 && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	if(!check_hero(HERO_NECROMANCER, plyptr))
		plyptr->troop_type = troop_type(plyptr->classnum);
	for(count = 0; count < MAXTRP; ++count)
		switch(count)
		{
			case SOLDIERNUM:
				plyptr->army[ARMY_DEFENSE][cont].total[count] = 50;
				plyptr->army[ARMY_CAMPAIGN][cont].total[count] = 50;
				break;
			case BLOODGUARDNUM:
				if(units[plyptr->troop_type][count].attack > 0 && units[plyptr->troop_type][count].defend > 0)
					plyptr->army[ARMY_DEFENSE][cont].total[count] = 25;
				break;
			case CATAPULTNUM:
				if(units[plyptr->troop_type][count].attack > 0)
					plyptr->army[ARMY_CAMPAIGN][cont].total[count] = 2;
				break;
			default:
				plyptr->army[ARMY_DEFENSE][cont].total[count] = 0;
				plyptr->army[ARMY_CAMPAIGN][cont].total[count] = 0;
				break;
		}
	if((plyptr->castle[cont] = combined_army(cont, plyptr) /castle_max(plyptr->classnum)) < 1)
		plyptr->castle[cont] = 1;
	plyptr->land[cont] = structures[BASE_CLASSNUM(plyptr->classnum)].land * plyptr->castle[cont] + (plyptr->land[cont] * randomnum(3, 1) / 100);
	remove_all_ret(plyptr);
	reset_special(plyptr);
	if(plyptr->kingdoms == 1)
		default_formation(cont, plyptr);
	if(plyptr->attacks > attacks_allowed(plyptr))
		plyptr->attacks = attacks_allowed(plyptr);
	check_vassals(plyptr);
	sprintf(message, "9051%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL, plyptr->nick, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL);
	send_channel(message, 0);
}

void check_rod(int cont, int type, player *plyptr)
{
	if(!check_artifact(ROD, plyptr))
		return;
	if(vampire(plyptr) && !type)
		return;
	change_level(cont, 1, plyptr);
	plyptr->promotion[cont] = 1;
	plyptr->land[cont] += (plyptr->level[cont] + 1) * 100;
	plyptr->peasant[cont] += (plyptr->level[cont] + 1) * 100;
	plyptr->copper[cont] += (plyptr->level[cont] + 1) * 10000;
	if(plyptr->level[cont] >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "9008%c%s%c%s%c%s%c%s%c", MSGDEL, 
		artifacts[ROD].name, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
		send_channel(message, 0);
		delete_player(playernum(plyptr));
	}
	else
	{
		sprintf(message, "9009%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, artifacts[ROD].name, MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
	}
	return;
}

void create_questchar()
{
	char name[16];
	char str[256] = {0};
	int classnum;
	int count;
	int num;
	int plynum;
	player *plyptr;

	if(quest_chars == 0 || current_player > (int)(max_player * .90))
		return;
	for(count = 0; count < num_conts; ++count)
	{
		num = randomnum(quest_chars, 1);
		while(num > 0)
		{
			sprintf(name, "QuestChar%d", randomnum(5000, 1000));
			if((find_player(name)) > 0)
				continue;
			if(num_conts == 1)
				classnum = count;
			else
				while((classnum = randomnum(total_races[FIGHTER] - 1, 0)) == count);
			plynum = add_player(name, "questchar@conquestgamesite.com", classnum, count, -1);
			plyptr = &players[plynum];
			strcpy(plyptr->kname[count], "Quest Character");
			sprintf(plyptr->password, "QuestChar%d", randomnum(9999, 1000));
			plyptr->gender = randomnum(1, 0);
			plyptr->isprotected = 1;
			plyptr->special_misc += SPC_QUESTCHAR;
			if(allow_surrender == 1)
				plyptr->surrender[count] = (float)1.0;
			plyptr->validate = VAL_QUESTCHAR;
			change_level(count, (randomnum(3, 1) - plyptr->level[count]), plyptr);
			plyptr->copper[count] = randomnum(plyptr->levelmax * 50000, plyptr->levelmax * 10000);
			plyptr->land[count] = promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][1];
			plyptr->peasant[count] = promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][2];
			plyptr->food[count] = (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * 1.25);
			plyptr->army[ARMY_DEFENSE][count].total[SOLDIERNUM] = (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * .20);
			if(units[plyptr->troop_type][ARCHERNUM].attack > 0 && units[plyptr->troop_type][ARCHERNUM].defend > 0)
				plyptr->army[ARMY_DEFENSE][count].total[ARCHERNUM] = (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * .40);
			else
				plyptr->army[ARMY_DEFENSE][count].total[SOLDIERNUM] += (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * .40);
			if(units[plyptr->troop_type][KNIGHTNUM].attack > 0 && units[plyptr->troop_type][KNIGHTNUM].defend > 0)
				plyptr->army[ARMY_DEFENSE][count].total[KNIGHTNUM] = (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * .40);
			else
				plyptr->army[ARMY_DEFENSE][count].total[SOLDIERNUM] += (int)(promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[count]][0] * .40);
			if(units[plyptr->troop_type][CATAPULTNUM].attack > 0)
				plyptr->army[ARMY_DEFENSE][count].total[CATAPULTNUM] = randomnum(8, 2);
			plyptr->castle[count] = army_size(count, ARMY_DEFENSE, plyptr) / castle_max(plyptr->classnum) + 1;
			plyptr->honor[count] = randomnum(50, 25);
			plyptr->tax[count] = BASETAX / BASETAX;
			sprintf(str, "sell %%MF %s;", food_name);
			steward("hk", str, plyptr);
			save_player(plynum);
			sprintf(message, "9012%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[count].name, MSGDEL);
			send_channel(message, 0);
			--num;
		}
	}
}

void feed_army(int cont, int type, player *plyptr)
{
	int amount = 0;
	int count;

	if(titan(plyptr))
		return;
	for(count = MAXTRP - 1; count >= 0; --count)
	{
		if(units[plyptr->troop_type][count].food < 1 || plyptr->army[type][cont].total[count] < 1)
			continue;
		plyptr->food[cont] -= plyptr->army[type][cont].total[count] * units[plyptr->troop_type][count].food;
		if(plyptr->food[cont] < 0)
		{
			if((amount = abs(plyptr->food[cont] / units[plyptr->troop_type][count].food)) < 1)
				amount = 1;
			plyptr->army[type][cont].total[count] -= amount;
			plyptr->food[cont] = 0;
			sprintf(message, "9013%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, conts[cont].name, MSGDEL);
		}
		else
			sprintf(message, "9024%c%s%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, conts[cont].name, MSGDEL, plyptr->army[type][cont].total[count] * units[plyptr->troop_type][count].food, MSGDEL, food_name, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

void feed_army_ships(int cont, player *plyptr)
{
	int amount = 0;
	int cargo;
	int count;

	if(plyptr->fleet < 1)
		return;
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type == 0)
			continue;
		cargo = plyptr->ships[count].cargotype;
		if(cargo < CARGOREN || cargo > CARGOMERC2)
			continue;
		if(units[plyptr->troop_type][cargo].food < 1)
			continue;
		plyptr->food[cont] -= plyptr->ships[count].cargo * units[plyptr->troop_type][cargo].food;
		if(plyptr->food[cont] < 0)
		{
			if((amount = abs(plyptr->food[cont] / units[plyptr->troop_type][cargo].food)) < 1)
				amount = 1;
			plyptr->ships[count].cargo -= amount;
			if(plyptr->ships[count].cargo < 1)
				plyptr->ships[count].cargotype = NOCARGO;
			plyptr->food[cont] = 0;
			sprintf(message, "9013%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][cargo].name, MSGDEL, plyptr->ships[count].name, MSGDEL);
		}
		else
			sprintf(message, "9024%c%s%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][cargo].name, MSGDEL, plyptr->ships[count].name, MSGDEL, plyptr->ships[count].cargo * units[plyptr->troop_type][cargo].food, MSGDEL, food_name, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

int find_market(int ctynum, int num, int type)
{
	int count;

	for(count = 0; count < MAXMRK; ++count)
		if(cities[ctynum].troop_num[count] == num && (restrict_markets == 0 || cities[ctynum].troop_type[count] == type))
			break;
	if(count == MAXMRK)
		return(-1);
	else
		return(count);
}

int find_merc_market(int ctynum, int num, int type)
{
	int count;

	for(count = 0; count < MAXMEC; ++count)
		if(cities[ctynum].merc_num[count] == num && cities[ctynum].merc_type[count] == type)
			break;
	if(count == MAXMEC)
		return(-1);
	else
		return(count);
}

void hk_main(int cont, int type, player *plyptr)
{
	int amount;
	int count;
	int max = 50;
	int maxtrp = SOLDIERNUM;
	int min = 25;
	int total = 0;

	plyptr->attacks_lost[cont] = 0;
	if(plyptr->honor[cont] > MAXHNR)
		plyptr->honor[cont] = MAXHNR;
	plyptr->promotion[cont] = 0;
	plyptr->tavern[cont] = 0;
	if(plyptr->food[cont] > MAXFOOD)
		random_event(cont, EVENT_LOCUSTS, plyptr);
	if(plyptr->peasant[cont] > MAXPEASANT)
		random_event(cont, EVENT_UPRISING, plyptr);
	if(plyptr->copper[cont] > MAXCOPPER)
		random_event(cont, EVENT_THIEVES, plyptr);
	random_event(cont, EVENT_NONE, plyptr);
	weather_event(cont, plyptr);
	if(type == 1)
		new_year(cont, plyptr);
	check_rod(cont, type, plyptr);
	if(strlen(plyptr->name) < 1)
		return;
	check_chest(cont, plyptr);
	feed_army(cont, 0, plyptr);
	feed_army(cont, 1, plyptr);
	if(cont == plyptr->cont)
		feed_army_ships(cont, plyptr);
	pay_army(cont, 0, plyptr);
	pay_army(cont, 1, plyptr);
	if(cont == plyptr->cont)
		pay_army_ships(cont, plyptr);
	item_hk(cont, plyptr);
	steward_hk(cont, plyptr);
	produce_food(cont, plyptr);
	castle_income(cont, plyptr);
	check_crown(cont, plyptr);
	check_castles(cont, plyptr);
	tithe(cont, plyptr);
	if(type == 1 && promote_knights == 1 && conq_info.agewon == 0)
		promote_knight(cont, plyptr);
	if((difftime(time(NULL), plyptr->lastplay)) >= 259200 && plyptr->isprotected == 0)
	{
		plyptr->isprotected = 1;
		plyptr->validate = VAL_QUESTCHAR;
		sprintf(message, "9014%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
		promote_knight(cont, plyptr);
	}
	else if(plyptr->level[cont] > 2 && plyptr->isprotected == 0)
	{
		plyptr->isprotected = 1;
		sprintf(message, "9014%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
	}
	if(fighter(plyptr) && plyptr->level[cont] >= 2 && plyptr->troop_type == FIGHTER)
	{
		if(plyptr->level[cont] >= 6)
		{
			max = 100;
			maxtrp = KNIGHTNUM;
			min = 75;
		} else if(plyptr->level[cont] >= 4)
		{
			max = 75;
			maxtrp = ARCHERNUM;
			min = 50;
		}
		for(count = 0; count <= maxtrp; ++count)
		{
			if(units[plyptr->troop_type][count].attack == 0 && units[plyptr->troop_type][count].defend == 0)
				continue;
			amount = randomnum(max, min);
			if((amount = fill_castles(amount, cont, plyptr)) <= 0)
				break;
			total += amount;
			plyptr->army[ARMY_DEFENSE][cont].total[count] += amount;
		}
		if(total > 0)
		{
			sprintf(message, "9052%c%d%c%s%c", MSGDEL, total, MSGDEL, conts[cont].name, MSGDEL);
			log_event("", message, plyptr->name);
		}
	}
}

void housekeeping(int type)
{
	char str[128] = {0};
	FILE *fp;
	int cont;
	int count;
	time_t current;

	SET_FUNCTION;
	memset(conq_info.hktime, 0, 32);
	strncpy(conq_info.hktime, NOWSTR, 31);
	if(type == 1)
		remove(HKLOG);
	if((fp = fopen(HKLOG, "a")) != NULL)
	{
		fprintf(fp, "%s Housekeeping ran %s by %s.\n", type == 0?"Regular":"New Year", conq_info.hktime, conq_info.hkby);
		fclose(fp);
	}
	sprintf(message, "9049%c%c", MSGDEL, MSGDEL);
	send_channel(message, 1);
	if(type == 1)
	{
		++conq_info.today;
		if(end_date > 0)
		{
			strcpy(message, "");
			switch(end_date - conq_info.today)
			{
				case 5:
				case 3:
				case 1:
					sprintf(message, "9015%c%s%c%d%c", MSGDEL, age, MSGDEL, end_date - conq_info.today, MSGDEL);
					break;
				default:
					break;
			}
			if(strlen(message) > 0)
				send_channel(message, 0);
		}
		artifacts[AEGIS].owner = 0;
		save_art();
		item_decay();
	}
	remove_all_hosts();
	set_mode();
	for(count = 0; count < num_cities; ++count)
		init_city(count, type);
	hero_hk(type);
	build_ships();
	regen_kraken();
	time(&current);
	for(count = 1; count < current_player; ++count)
	{
		if((strlen(players[count].name)) < 1)
			continue;
		players[count].attacks = 0; 
		players[count].done = 0;
		players[count].sock = INVALID_SOCKET;
		players[count].trynum = 0;
		if(players[count].validate == VAL_NORMAL)
			players[count].validate = NOT_VAL;
		reset_special(&players[count]);
		if(players[count].vacation > 0)
		{
			if(players[count].special_misc & SPC_VACATION)
			{
				if(players[count].vacation <= current)
				{
					players[count].lastplay = current;
					players[count].special_misc -= SPC_VACATION;
					players[count].vacation = 0;
					log_event("", "End vacation.", players[count].name);
				}
				else
					continue;
			}
			else if(players[count].vacation > current)
			{
				drop(&players[count]);
				dismiss(&players[count]);
				players[count].special_misc += SPC_VACATION;
				log_event("", "Begin vacation.", players[count].name);
				save_player(count);
				continue;
			}
		}
		for(cont = 0; cont < num_conts; ++cont)
			if(players[count].level[cont] >= 0)
				hk_main(cont, type, &players[count]);
		if((strlen(players[count].name)) < 1)
			continue;
		add_move(&players[count]);
		get_components(type, &players[count]);
		for(cont = 0; cont < num_conts; ++cont)
			check_steward(cont, STEWARD_HK, NULL, &players[count]);
		save_player(count);
	}
	for(count = 1; count < current_player; ++count)
		if((strlen(players[count].name)) > 0)
			vassal_hk(type, &players[count]);
	if(type == 1)
	{
		create_questchar();
		revive_heroes();
	}
	for(count = 0; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock != INVALID_SOCKET)
			newtopic("", 1, sockets[count].sock, &players[0]);
#ifdef WIN32
	sprintf(str, "copy %s %s", FSAVE, FBACK);
#else
	sprintf(str, "cp %s %s 2>/dev/null", FSAVE, FBACK);
#endif
	system(str);
	sprintf(message, "9050%c%c", MSGDEL, MSGDEL);
	send_channel(message, 1);
	if(type == 1)
	{
		if(logfile != NULL)
		{
			fclose(logfile);
			sprintf(str, "%s.old", log_file);
			rename(log_file, str);
			if((logfile = fopen(log_file, "w")) == NULL)
				fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, log_file, strerror(errno));
		}
		if(conq_info.today == auto_ar)
			cast_ar();
	}
	if(conq_info.today == end_date)
	{
		conq_info.agewon = 1;
		sprintf(message, "9017%c%s%c", MSGDEL, age, MSGDEL);
		send_channel(message, 0);
	}
	save_info();
}

void init_city(int ctynum, int type)
{
	char itemnames[256] = {0};
	int base[] = {0, 25, 75, 150, 0, 150};
	int count;
	int even[] = {0, 1000, 1000, 500, 0, 50};
	int maxqty[] = {0, 2000, 2000, 1000, 1000, 100};
	int price;

	if(cities[ctynum].copper < 50000 || cities[ctynum].copper > 50000000)
		cities[ctynum].copper = 500000;
	if(cities[ctynum].lock == 1 && type)
	{
		cities[ctynum].lock = 0;
		sprintf(message, "9018%c%s%c", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
		send_channel(message, 0);
	}
	if(cities[ctynum].food_pr > 0)
	{
		price = make_price(cities[ctynum].food_qty, cities[ctynum].food_pr, 4000);
		if(price != 0 && ((price > 0 && cities[ctynum].food_pr + price <= 400) || (price < 0 && cities[ctynum].food_pr + price >= 100)))
		{
			cities[ctynum].food_pr += price;
			if(price > 0)
				sprintf(itemnames, "%s+%s ", itemnames, food_name);
			else
				sprintf(itemnames, "%s-%s ", itemnames, food_name);
		}
		cities[ctynum].food_buy = cities[ctynum].food_pr / 2;
		if(cities[ctynum].food_buy < 1)
			cities[ctynum].food_buy = 1;
		cities[ctynum].food_qty = randomnum(3500, 2500);
	}
	for(count = 0; count < MAXMRK; ++count)
	{
		if(cities[ctynum].troop_pr[count] < 1)
			continue;
		if(cities[ctynum].troop_num[count] == BLOODGUARDNUM)
			price = 0;
		else
			price = make_price(cities[ctynum].troop_qty[count], cities[ctynum].troop_pr[count], even[cities[ctynum].troop_num[count]]);
		if(price != 0 && cities[ctynum].troop_pr[count] + price >= base[cities[ctynum].troop_num[count]])
		{
			cities[ctynum].troop_pr[count] += price;
			if(restrict_markets == 0)
				cities[ctynum].troop_type[count] = FIGHTER;
			if(price > 0)
				sprintf(itemnames, "%s+%ss ", itemnames, units[cities[ctynum].troop_type[count]][cities[ctynum].troop_num[count]].name);
			else
				sprintf(itemnames, "%s-%ss ", itemnames, units[cities[ctynum].troop_type[count]][cities[ctynum].troop_num[count]].name);
		}
		cities[ctynum].troop_qty[count] = randomnum(maxqty[cities[ctynum].troop_num[count]], maxqty[cities[ctynum].troop_num[count]] / 2);
	}
	if(cities[ctynum].spy_pr > 0)
	{
		price = make_price(cities[ctynum].spy_qty, cities[ctynum].spy_pr, 175);
		if(price != 0 && cities[ctynum].spy_pr + price >= 75)
		{
			cities[ctynum].spy_pr += price;
			if(price > 0)
				strcat(itemnames, "+Spies ");
			else
				strcat(itemnames, "-Spies ");
		}
		cities[ctynum].spy_qty = randomnum(225, 175);
	}
	if(cities[ctynum].peasant_pr > 0)
		cities[ctynum].peasant_qty = randomnum(3000, 1000);
	if(cities[ctynum].movepts_pr > 0)
		cities[ctynum].movepts_qty = randomnum(75, 25);
	if(strlen(itemnames) > 0)
	{
		sprintf(message, "999%cThe merchants of %s change the price of the following: %s", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, itemnames);
		send_channel(message, 0);
	}
	for(count = 0; count < MAXMEC; ++count)
		if(cities[ctynum].merc_num[count] >= MERC)
			cities[ctynum].merc_qty[count] = randomnum(1000, 500);
	if(open_othermarket(ctynum) == 0)
		open_troopmarket(ctynum, -1, -1);
	save_city(ctynum);
}

int make_price(int current, int price, int qty)
{
	int iprice = 0;
	int min = 0;
	int max = 9999;

	min = (int)(qty * .85);
	max = qty + (int)(qty * .15);
	if(current < min)
	{
		if(price > 500)
		{
			if(current < (int)(qty * .10))
				iprice = 50;
		}
		else
			iprice = 50;
	}
	else
		if(current > max)
			iprice = -50;
	if(price + iprice < 1)
		iprice = 0;
	return(iprice);
}

int market_type()
{
	int type = -1;

	if(restrict_markets == 0)
		return(FIGHTER);
	while(type == -1)
	{
		switch(randomnum(RANGER, FIGHTER))
		{
			case FIGHTER:
				type = FIGHTER;
				break;
			case BARBARIAN:
				if(allow_barb == 1)
					type = BARBARIAN;
				break;
			case MAGE:
				if(allow_mage == 1)
					type = MAGE;
				break;
			case VAMPIRE:
				if(allow_vampire == 1)
					type = VAMPIRE;
				break;
			case CLERIC:
				if(allow_cleric == 1)
					type = CLERIC;
				break;
			case RANGER:
				if(allow_ranger == 1)
					type = RANGER;
				break;
		}
	}
	return(type);
}

void new_year(int cont, player *plyptr)
{
	int amount;

	if(remove_items == 1 && !check_hero(HERO_ALCHEMIST, plyptr))
		remove_all_items(plyptr);
	plyptr->ally_request = 0;
	if(plyptr->tax[cont] == 4 && !barbarian(plyptr) && !vampire(plyptr))
		--plyptr->honor[cont];
	produce_peasant(cont, plyptr);
	if(check_hero(HERO_LORDCHAOS, plyptr) && randomnum(100, 1) <= 15)
	{
		amount = plyptr->peasant[cont] * randomnum(25, 1) / 100;
		plyptr->peasant[cont] -= amount;
		sprintf(message, "9019%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
		log_event("", message, plyptr->name);
		if(!barbarian(plyptr) && !vampire(plyptr))
			plyptr->honor[cont] -= 2;
	}
	check_reign(cont, plyptr);
	if(vampire(plyptr) && plyptr->level[cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		if(conq_info.vampire == playernum(plyptr))
			sprintf(message, "9020%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, adjectives[plyptr->gender][5], MSGDEL);
		else
			sprintf(message, "9021%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, conts[cont].name, MSGDEL);
		send_channel(message, 0);
		change_level(cont, 1, plyptr);
		promote_max(cont, plyptr);
	}
	if(cleric(plyptr) && plyptr->fleet == 0 && num_ships > 1 && plyptr->levelmax < 2)
	{
		add_ship(0, 1, plyptr);
		sprintf(message, "9022%c%s%c%s%c", MSGDEL, shiptypes[1].modifier, MSGDEL, shiptypes[1].name, MSGDEL);
		log_event("", message, plyptr->name);
	}
	remove_all_ret(plyptr);
}

int open_othermarket(int ctynum)
{
	char types[][64] = 
	{
		"", "Spies"
	};
	int cont;
	int count;
	int food_markets = 0;
	int spy_markets = 0;
	int num;

	if(cities[ctynum].food_pr > 0 && cities[ctynum].spy_pr > 0)
		return(0);
	cont = cities[ctynum].cont;
	for(count = 0; count < conts[cont].totalcty; ++count)
	{
		if(cities[conts[cont].cities[count]].food_pr > 0)
			++food_markets;
		if(cities[conts[cont].cities[count]].spy_pr > 0)
			++spy_markets;
	}
	count = conts[cont].totalcty / 2;
	if(randomnum(100, 1) <= 75 || (food_markets >= count && spy_markets >= count))
		return(0);
	if(food_markets >= count || cities[ctynum].food_pr > 0)
		num = 1;
	else if(spy_markets >= count || cities[ctynum].spy_pr > 0)
		num = 0;
	else
		num = randomnum(1, 0);
	switch(num)
	{
		case 0:
			strcpy(types[num], food_name);
			cities[ctynum].food_pr = randomnum(400, 100);
			cities[ctynum].food_buy = cities[ctynum].food_pr - 50;
			cities[ctynum].food_qty = randomnum(20000, 4000);
			break;
		case 1:
			cities[ctynum].spy_pr = randomnum(600, 300);
			cities[ctynum].spy_qty = randomnum(99, 10);
			break;
	}
	sprintf(message, "9023%c%s%c%s%c", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL, types[num], MSGDEL);
	send_channel(message, 0);
	return(1);
}

void open_troopmarket(int ctynum, int force_num, int force_type)
{
	char types[][64] = 
	{
		"", "", "", "", "", ""
	};
	int count;
	int found = 0;
	int markets = 0;
	int maxtries = 0;
	int next = 0;
	int num = -1;
	int price_max[] = {0, 500, 500, 500, 2000, 800};
	int price_min[] = {0, 100, 100, 200, 2000, 200};
	int qty_max[] = {0, 30000, 20000, 16000, 4000, 200};
	int qty_min[] = {0, 8000, 6000, 4000, 4000, 100};
	int type = 0;

	if((next = find_openmarket(ctynum)) == -1)
		return;
	if(force_num >= 0 && force_type >= 0)
	{
		if(units[force_type][force_num].attack == 0 && units[force_type][force_num].defend == 0)
			return;
		cities[ctynum].troop_num[next] = force_num;
		cities[ctynum].troop_type[next] = force_type;
		cities[ctynum].troop_pr[next] = randomnum(price_max[force_num], price_min[force_num]);
		cities[ctynum].troop_qty[next] = randomnum(qty_max[force_num], qty_min[force_num]);
		return;
	}
	for(count = 0; count < MAXMRK; ++count)
		if(cities[ctynum].troop_pr[count] > 0)
			++markets;
	if(restrict_markets == 0)
	{
		if(markets == 4 || (markets > 2 && randomnum(100, 1) <= 75))
			return;
	}
	else 
		if(markets == MAXMRK || (markets > (MAXMRK - 3) && randomnum(100, 1) <= 75))
			return;
	while(found == 0 && maxtries++ < 1000)
	{
		num = randomnum(CATAPULTNUM, SOLDIERNUM);
		type = market_type();
		switch(num)
		{
			case SOLDIERNUM:
			case ARCHERNUM:
			case KNIGHTNUM:
				if(type == VAMPIRE || (find_market(ctynum, num, type)) >= 0)
					break;
				if(units[type][num].attack == 0 && units[type][num].defend == 0)
					break;
				found = 1;
				cities[ctynum].troop_pr[next] = randomnum(price_max[num], price_min[num]);
				cities[ctynum].troop_qty[next] = randomnum(qty_max[num], qty_min[num]);
				break;
			case BLOODGUARDNUM:
				break;
			case CATAPULTNUM:
				if((find_market(ctynum, num, type)) >= 0)
					break;
				if(units[type][num].attack == 0 && units[type][num].defend == 0)
					break;
				found = 1;
				cities[ctynum].troop_pr[next] = randomnum(price_max[num], price_min[num]);
				cities[ctynum].troop_qty[next] = randomnum(qty_max[num], qty_min[num]);
				break;
		}
	}
	if(found)
	{
		cities[ctynum].troop_num[next] = num;
		cities[ctynum].troop_type[next] = type;
		strcpy(types[num], units[type][num].name);
		sprintf(message, "9023%c%s%c%s%c", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL, types[num], MSGDEL);
		send_channel(message, 0);
	}
}

void pay_army(int cont, int type, player *plyptr)
{
	int amount = 0;
	int count;

	if(titan(plyptr))
		return;
	for(count = MAXTRP - 1; count >= 0; --count)
	{
		if(units[plyptr->troop_type][count].copper < 1 || plyptr->army[type][cont].total[count] < 1)
			continue;
		plyptr->copper[cont] -= plyptr->army[type][cont].total[count] * units[plyptr->troop_type][count].copper;
		if(plyptr->copper[cont] < 0)
		{
			if((amount = abs(plyptr->copper[cont] / units[plyptr->troop_type][count].copper)) < 1)
				amount = 1;
			plyptr->army[type][cont].total[count] -= amount;
			plyptr->copper[cont] = 0;
			sprintf(message, "9025%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, conts[cont].name, MSGDEL);
		}
		else
			sprintf(message, "9024%c%s%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][count].name, MSGDEL, conts[cont].name, MSGDEL, plyptr->army[type][cont].total[count] * units[plyptr->troop_type][count].copper, MSGDEL, money_name, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

void pay_army_ships(int cont, player *plyptr)
{
	int amount = 0;
	int cargo;
	int count;

	if(plyptr->fleet < 1)
		return;
	for(count = 0; count < MAXFLT; ++count)
	{
		if(plyptr->ships[count].type == 0)
			continue;
		cargo = plyptr->ships[count].cargotype;
		if(cargo < CARGOREN || cargo > CARGOMERC2)
			continue;
		if(units[plyptr->troop_type][cargo].copper < 1)
			continue;
		plyptr->copper[cont] -= plyptr->ships[count].cargo * units[plyptr->troop_type][cargo].copper;
		if(plyptr->copper[cont] < 0)
		{
			if((amount = abs(plyptr->copper[cont] / units[plyptr->troop_type][cargo].copper)) < 1)
				amount = 1;
			plyptr->ships[count].cargo -= amount;
			if(plyptr->ships[count].cargo < 1)
				plyptr->ships[count].cargotype = NOCARGO;
			plyptr->copper[cont] = 0;
			sprintf(message, "9025%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][cargo].name, MSGDEL, plyptr->ships[count].name, MSGDEL);
		}
		else
			sprintf(message, "9024%c%s%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][cargo].name, MSGDEL, plyptr->ships[count].name, MSGDEL, plyptr->ships[count].cargo * units[plyptr->troop_type][cargo].copper, MSGDEL, money_name, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

void produce_food(int cont, player *plyptr)
{
	int amount;
	int spoilage = 50;
	float max;
	float min;

	if(ranger(plyptr) && !titan(plyptr))
	{
		if(plyptr->level[cont] >= 2)
			spoilage = 0;
		else if(plyptr->level[cont] >= 1)
			spoilage = 25;
		else
			spoilage = 40;
	}
	if(check_hero(HERO_FARMER, plyptr))
		spoilage -= 15;
	if(spoilage < 0)
		spoilage = 0;
	plyptr->food[cont] -= plyptr->food[cont] * spoilage / 100;
	if(plyptr->land[cont] < 1 || plyptr->peasant[cont] < 1)
		return;
	switch(plyptr->troop_type)
	{
		case BARBARIAN:
		case MAGE:
			min = (float).70;
			max = 1;
			break;
		case VAMPIRE:
			min = (float).40;
			max = (float).60;
			break;
		case CLERIC:
			min = .75;
			max = (float)1.10;
			break;
		case RANGER:
			min = (float).80;
			max = (float)1.15;
			break;
		default:
			min = (float).70;
			max = (float)1.05;
			break;
	}
	switch(plyptr->tax[cont])
	{
		case 0:
			min += (float).20;
			max += (float).20;
			break;
		case 1:
			min += (float).10;
			max += (float).10;
			break;
		case 3:
			min -= (float).10;
			max -= (float).10;
			break;
		case 4:
			min -= (float).20;
			max -= (float).20;
			break;
		default:
			break;
	}
	if(check_artifact(PLOW, plyptr))
	{
		min += (float).50;
		max += (float).50;
	}
	min += forecast[cont].food;
	max += forecast[cont].food;
	if(min < 0)
		min = 0;
	if(max < 0)
		max = 0;
	if(plyptr->land[cont] > plyptr->peasant[cont])
		amount = randomnum((int)(plyptr->peasant[cont] * max), (int)(plyptr->peasant[cont] * min));
	else
		amount = randomnum((int)(plyptr->land[cont] * max), (int)(plyptr->land[cont] * min));
	if(amount < 1)
		amount = 1;
	plyptr->food[cont] += amount;
	sprintf(message, "9026%c%s%c%s%c%d%c%s%c", MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL, amount, MSGDEL, food_name, MSGDEL);
	log_event("", message, plyptr->name);
}

void produce_peasant(int cont, player *plyptr)
{
	int amount;
	float maxb = 0;
	float maxd = 0;
	float minb = 0;
	float mind = 0;

	if(plyptr->peasant[cont] < 2)
		return;
	switch(plyptr->troop_type)
	{
		case BARBARIAN:
			minb = (float).15;
			maxb = (float).25;
			mind = (float).10;
			maxd = (float).20;
			break;
		case VAMPIRE:
			minb = (float).05;
			maxb = (float).15;
			mind = (float).15;
			maxd = (float).25;
			break;
		case CLERIC:
			minb = (float).15;
			maxb = (float).25;
			mind = (float).05;
			maxd = (float).15;
			break;
		default:
			minb = (float).10;
			maxb = (float).20;
			mind = (float).10;
			maxd = (float).20;
			break;
	}
	switch(plyptr->tax[cont])
	{
		case 0:
			minb += (float).10;
			maxb += (float).10;
			mind -= (float).10;
			maxd -= (float).10;
			if(!barbarian(plyptr) && !vampire(plyptr))
				++plyptr->honor[cont];
		case 1:
			minb += (float).05;
			maxb += (float).05;
			mind -= (float).05;
			maxd -= (float).05;
			break;
		case 3:
			minb -= (float).05;
			maxb -= (float).05;
			mind += (float).05;
			maxd += (float).05;
			break;
		case 4:
			minb -= (float).10;
			maxb -= (float).10;
			mind += (float).10;
			maxd += (float).10;
			if(!barbarian(plyptr) && !vampire(plyptr))
				--plyptr->honor[cont];
			break;
		default:
			break;
	}
	if(minb < 0)
		minb = (float).01;
	if(maxb < 0)
		maxb = (float).01;
	if(mind < 0)
		mind = (float).01;
	if(maxd < 0)
		maxd = (float).01;
	amount = randomnum((int)(plyptr->peasant[cont] * maxb), (int)(plyptr->peasant[cont] * minb));
	plyptr->peasant[cont] += amount;
	sprintf(message, "9027%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
	amount = randomnum((int)(plyptr->peasant[cont] * maxd), (int)(plyptr->peasant[cont] * mind));
	if(amount > plyptr->peasant[cont])
		amount = plyptr->peasant[cont];
	plyptr->peasant[cont] -= amount;
	sprintf(message, "9028%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
}

void promote_knight(int cont, player *plyptr)
{
	if(plyptr->level[cont] > 0)
		return;
	if(difftime(time(NULL), plyptr->lastplay) < 259200 && plyptr->first_play[cont] + 1 != conq_info.today && plyptr->remorts < 1)
		return;
	change_level(cont, 1, plyptr);
	sprintf(message, "9029%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, plyptr->nick, MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, conts[cont].name, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
	send_channel(message, 0);
	if(!barbarian(plyptr) && plyptr->troop_type != VAMPIRE)
		add_bg(plyptr->cont, 20, plyptr);
	sprintf(message, "9030%c%s%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, conts[cont].name, MSGDEL);
	log_event("", message, plyptr->name);
}

void random_event(int cont, int setnum, player *plyptr)
{
	int amount;
	int count;
	int evtnum;
	int shpnum;
	int type;

	type = randomnum(ARMY_CAMPAIGN, ARMY_DEFENSE);
	if(setnum > EVENT_NONE)
		evtnum = setnum;
	else
	{
		if(plyptr->event[cont] > 0)
			evtnum = plyptr->event[cont];
		else
		{
			if(randomnum(100, 1) >= (plyptr->level[cont] + 1) * 5)
				return;
			if(randomnum(100, 1) + (plyptr->honor[cont] / 2) >= 50)
				evtnum = randomnum(EVENT_FINDCASTLE, EVENT_MERCHANT);
			else
				evtnum = randomnum(EVENT_POISON, EVENT_PLAGUE);
		}
		plyptr->event[cont] = EVENT_NONE;
	}
	switch(evtnum)
	{
		case EVENT_MERCHANT:
			amount = randomnum(50000, 10000);
			sprintf(message, "9032%c%s%c%d%c%s%c", MSGDEL, conts[cont].name, MSGDEL, amount, MSGDEL, money_name, MSGDEL);
			plyptr->copper[cont] += amount;
			break;
		case EVENT_SOLDIERS:
			amount = randomnum(500, 100);
			if((amount = fill_castles(amount, cont, plyptr)) <= 0)
				return;
			sprintf(message, "9033%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][SOLDIERNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			plyptr->army[type][cont].total[SOLDIERNUM] += amount;
			break;
		case EVENT_RECOVERFOOD:
			amount = randomnum(1000, 100);
			sprintf(message, "9034%c%s%c%s%c%d%c%s%c", MSGDEL, conts[cont].name, MSGDEL, food_name, MSGDEL, amount, MSGDEL, food_name, MSGDEL);
			plyptr->food[cont] += amount;
			break;
		case EVENT_REPAIRCAT:
			if(units[plyptr->troop_type][CATAPULTNUM].attack == 0)
				break;
			sprintf(message, "9035%c%s%c%s%c", MSGDEL, units[plyptr->troop_type][CATAPULTNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			++plyptr->army[type][cont].total[CATAPULTNUM];
			break;
		case EVENT_PEASANTS:
			amount = randomnum(500, 100);
			sprintf(message, "9036%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			plyptr->peasant[cont] += amount;
			break;
		case EVENT_EXPLORERS:
			amount = randomnum(500, 100);
			sprintf(message, "9037%c%d%c%s%c", MSGDEL, amount, MSGDEL, conts[cont].name, MSGDEL);
			plyptr->land[cont] += amount;
			break;
		case EVENT_FINDSHIP:
			if((shpnum = find_shpnum(plyptr)) >= 0 && num_ships > 1)
			{
				add_ship(shpnum, 1, plyptr);
				sprintf(message, "9038%c%s%c", MSGDEL, shiptypes[1].name, MSGDEL);
			}
			else
				return;
			break;
		case EVENT_GRACE:
			if(barbarian(plyptr) || vampire(plyptr))
				return;
			plyptr->honor[cont] += randomnum(5, 1);
			sprintf(message, "9039%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_FINDCASTLE:
			if(plyptr->land[cont] <= plyptr->castle[cont] * structures[BASE_CLASSNUM(plyptr->classnum)].land)
				return;
			++plyptr->castle[cont];
			sprintf(message, "9040%c%s%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_PLAGUE:
			plyptr->peasant[cont] -= plyptr->peasant[cont] * randomnum(30, 10) / 100;
			for(count = 0; count < MAXTRP; ++count)
				if(count != CATAPULTNUM)
					plyptr->army[type][cont].total[count] -= plyptr->army[type][cont].total[count] * randomnum(30, 10) / 100;
			plyptr->event[cont] = EVENT_SICKNESS;
			sprintf(message, "9041%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_LOCUSTS:
			if(plyptr->food[cont] > MAXFOOD)
				amount = 50;
			else
				amount = randomnum(30, 10);
			plyptr->food[cont] -= plyptr->food[cont] * amount / 100;
			sprintf(message, "9044%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_UPRISING:
			if(plyptr->peasant[cont] > MAXPEASANT)
				amount = 50;
			else
				amount = randomnum(30, 10);
			plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
			sprintf(message, "9045%c%s%c%s%c%s%c", MSGDEL, peasant_name, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_ARSON:
			if(plyptr->castle[cont] < 2)
				return;
			--plyptr->castle[cont];
			sprintf(message, "9046%c%s%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_THIEVES:
			if(plyptr->copper[cont] > MAXCOPPER)
				amount = 50;
			else
				amount = randomnum(30, 10);
			plyptr->copper[cont] -= plyptr->copper[cont] * amount / 100;
			sprintf(message, "9047%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_SICKNESS:
			plyptr->peasant[cont] -= plyptr->peasant[cont] * 5 / 100;
			for(count = 0; count < MAXTRP; ++count)
				if(count != CATAPULTNUM)
					plyptr->army[type][cont].total[count] -= plyptr->army[type][cont].total[count] * 5 / 100;
			if(randomnum(100, 1) >= (60 - plyptr->level[cont] * 5) + plyptr->honor[cont] / 2)
				plyptr->event[cont] = 15;
			sprintf(message, "9048%c%s%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			break;
		case EVENT_POISON:
			amount = randomnum(25, 10);
			plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
			for(count = 0; count < MAXTRP; ++count)
				if(count != CATAPULTNUM)
					plyptr->army[type][cont].total[count] -= (plyptr->army[type][cont].total[count] * amount / 100);
			sprintf(message, "10085%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	log_event("", message, plyptr->name);
}

void regen_kraken()
{
	int amount;
	int diff;

	if(conq_info.kraken.alive == 0)
		return;
	diff = conq_info.kraken.maxhealth - conq_info.kraken.health;
	if(diff < 1)
		return;
	amount = randomnum(diff / 2, 1);
	conq_info.kraken.health += amount;
	sprintf(message, "9031%c%d%c", MSGDEL, (int)((float)amount / (float)conq_info.kraken.maxhealth  * 100.0), MSGDEL);
	send_channel(message, 0);
}

void weather_event(int cont, player *plyptr)
{
	int amount;
	int count;
	int found;
	int type;

	if(forecast[cont].violent == 0)
		return;
	if(randomnum(100, 1) >= (plyptr->level[cont] + 1) * 4)
		return;
	type = randomnum(ARMY_CAMPAIGN, ARMY_DEFENSE);
	switch(forecast[cont].type)
	{
		case WT_FREEZE:
			if(randomnum(100, 1) <= 50)
			{
				amount = randomnum(10, 5);
				plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
				sprintf(message, "9059%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			}
			else
			{
				amount = randomnum(15, 10);
				plyptr->food[cont] -= plyptr->food[cont] * amount / 100;
				sprintf(message, "9060%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL, conts[cont].name, MSGDEL);
			}
			break;
		case WT_SNOW:
			if(randomnum(100, 1) <= 50)
			{
				amount = randomnum(20, 15);
				plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
				sprintf(message, "9056%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			}
			else
			{
				amount = randomnum(10, 5);
				for(count = 0, found = 0; count < MAXTRP; ++count)
					if(units[plyptr->troop_type][count].food > 0)
					{
						found = 1;
						plyptr->army[type][cont].total[count] -= plyptr->army[type][cont].total[count] * amount / 100;
					}
				if(!found)
					return;
				sprintf(message, "9057%c%d%c%s%c", MSGDEL, amount, MSGDEL, conts[cont].name, MSGDEL);
			}
			break;
		case WT_STORM:
			amount = randomnum(10, 5);
			plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
			plyptr->food[cont] -= plyptr->food[cont] * amount / 100;
			sprintf(message, "9042%c%s%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			break;
		case WT_HOT:
			if(randomnum(100, 1) <= 50)
			{
				amount = randomnum(8, 3);
				plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
				sprintf(message, "9054%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			}
			else
			{
				amount = randomnum(5, 1);
				for(count = 0; count < MAXTRP; ++count)
					plyptr->army[type][cont].total[count] -= plyptr->army[type][cont].total[count] * amount / 100;
				sprintf(message, "9055%c%d%c%s%c", MSGDEL, amount, MSGDEL, conts[cont].name, MSGDEL);
			}
			break;
		case WT_DRY:
			amount = randomnum(10, 5);
			plyptr->food[cont] -= plyptr->food[cont] * amount / 100;
			sprintf(message, "9058%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case WT_WIND:
			if(randomnum(100, 1) <= 50)
			{
				amount = randomnum(8, 3);
				plyptr->peasant[cont] -= plyptr->peasant[cont] * amount / 100;
				plyptr->food[cont] -= plyptr->food[cont] * amount / 100;
				sprintf(message, "9043%c%s%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
			}
			else
			{
				if(plyptr->castle[cont] > 1)
				{
					--plyptr->castle[cont];
					sprintf(message, "9053%c%s%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
				}
				else
					return;
			}
			break;
		
		default:
			return;
	}
	log_event("", message, plyptr->name);
}
