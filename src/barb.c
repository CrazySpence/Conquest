
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

int barbarian(player *plyptr)
{
	if(!plyptr)
		return(0);
	if(plyptr->classnum >= 10 && plyptr->classnum < 20)
		return(1);
	else
		return(0);
}

void berserk(char *name, player *plyptr)
{
	int attack = 2;
	int count;
	int defend = 0;
	int loss;
	int plynum;

	if(!barbarian(plyptr))
	{
		sprintf(message, "1000%c%s%c%s%c", MSGDEL, vampire(plyptr)?"elusive":"honorable", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 3)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, 0), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BERSERK)
	{
		sprintf(message, "1%c%d%cto berserk your army%c", MSGDEL, BERSERK, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "berserk", "", "");
		return;
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return;
	plyptr->movepts -= BERSERK;
	plyptr->lastattack = time(NULL);
	++plyptr->attacks;
	plyptr->isprotected = 1;
	if(plyptr->level[plyptr->cont] >= 5)
	{
		attack = 4;
		defend = -2;
		loss = randomnum(5, 1);
	}
	else if(plyptr->level[plyptr->cont] >= 4)
	{
		attack = 3;
		defend = -1;
		loss = randomnum(10, 5);
	}
	else
		loss = randomnum(15, 10);
	for(count = 0; count < MAXTRP; ++count)
	{
		batmod[ATTACKER][count].attack = attack;
		batmod[ATTACKER][count].defend = defend;
	}
	combatants[ATTACKER] = plyptr;
	combatants[DEFENDER] = &players[plynum];
	sprintf(message, "1018%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "1001%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	start_battle();
	save_player(plynum);
	for(count = 0; count < MAXTRP; ++count)
		plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[count] -= (plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[count] * loss) / 100;
	sprintf(message, "1002%c%d%c", MSGDEL, loss, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void besiege(char *cityname, player *plyptr)
{
	int attack_damage;
	int ctynum;
	int defend_damage;
	int num_castles = 0;
	int round = 1;
	player *garrison;

	if(!barbarian(plyptr))
	{
		sprintf(message, "1000%c%s%c%s%c", MSGDEL, vampire(plyptr)?"elusive":"honorable", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 4)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 4, 0), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(cityname) < 1)
	{
		help(plyptr->name, "besiege", "", "");
		return;
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "3%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BESIEGE)
	{
		sprintf(message, "1%c%d%cto besiege %s%c", MSGDEL, BESIEGE, MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].lock == 0)
	{
		sprintf(message, "1004%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	++plyptr->attacks;
	plyptr->isprotected = 1;
	plyptr->lastattack = time(NULL);
	plyptr->movepts -= BESIEGE;
	garrison = (void *)plybuf[DEFENDER];
	combatants[ATTACKER] = plyptr;
	combatants[DEFENDER] = garrison;
	enemy(1, GARRISON, ARMY_CAMPAIGN, ARMY_DEFENSE, MIGHTY, combatants[DEFENDER], combatants[ATTACKER]);
	if((army_size(plyptr->cont, ARMY_DEFENSE, garrison)) < 1000)
	{
		if((garrison->castle[plyptr->cont] = 1000 / castle_max(FIGHTER)) < 1)
			garrison->castle[plyptr->cont] = 1;
		refresh_army(plyptr->cont, garrison);
	}
	if(ctynum == secret_city)
		strcpy(combatants[DEFENDER]->kname[cities[ctynum].cont], "??????");
	else
		strcpy(combatants[DEFENDER]->kname[cities[ctynum].cont], cities[ctynum].name);
	sprintf(message, "1005%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][0], MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
	send_channel(message, 0);
	if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
	{
		garrison_lost(ctynum, plyptr);
		loot_city(ctynum, plyptr);
		save_city(ctynum);
	}
	else
		garrison_won(ctynum, plyptr);
	if(count_casualties(ATTACKER))
		show_casualties(ATTACKER, ARMY_CAMPAIGN, combatants[ATTACKER]);
	if(combatants[ATTACKER_ALLY] && count_casualties(ATTACKER_ALLY))
		show_casualties(ATTACKER_ALLY, ARMY_CAMPAIGN, combatants[ATTACKER_ALLY]);
	reset_combatants();
	sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
}

void blitz(player *plyptr)
{
	int amount;
	int troopnum;
	struct tm *pnt;
	time_t current;

	if(!barbarian(plyptr))
	{
		sprintf(message, "1000%c%s%c%s%c", MSGDEL, vampire(plyptr)?"elusive":"honorable", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.cont_special[plyptr->cont] & SPC_BLITZ)
	{
		sprintf(message, "1020%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BLITZ)
	{
		sprintf(message, "1%c%d%cto use Blitz%c", MSGDEL, BLITZ, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr)) < promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][0])
	{
		sprintf(message, "16%ctroops%c%d%cblitz %s%c", MSGDEL, MSGDEL, promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][0], MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= BLITZ;
	if((randomnum(100, 1)) <= 70)
	{
		sprintf(message, "1021%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
	{
		if(plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[troopnum] < 1 || troopnum == CATAPULTNUM)
			continue;
		if((amount = plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[troopnum] * 5 / 100) < 1)
			amount = 1;
		plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[troopnum] -= amount;
	}
	time(&current);
	pnt = localtime(&current);
	conq_info.cont_special[plyptr->cont] += SPC_BLITZ;
	save_info();
	sprintf(message, "1022%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "1023%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL,select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "4394%cblitz%c%d%c", MSGDEL, MSGDEL, 60 - pnt->tm_min, MSGDEL);
	send_channel(message, 0);
}

void garrison_lost(int ctynum, player *plyptr)
{
	sprintf(message, "1006%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "1007%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
	send_channel(message, 0);
	cities[ctynum].lock = 0;
}

void garrison_won(int ctynum, player *plyptr)
{
	sprintf(message, "1008%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "1009%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
}

void loot(char *cityname, player *plyptr)
{
	int chance = 40;
	int ctynum;

	if(!barbarian(plyptr) && !check_hero(HERO_QUEENSPIDER, plyptr))
	{
		sprintf(message, "1000%c%s%c%s%c", MSGDEL, vampire(plyptr)?"elusive":"honorable", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2 && !check_hero(HERO_QUEENSPIDER, plyptr))
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, 0), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(cityname) < 1)
	{
		help(plyptr->name, "loot", "", "");
		return;
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "3%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < LOOT)
	{
		sprintf(message, "1%c%d%cto loot %s%c", MSGDEL, LOOT, MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].lock == 1)
	{
		sprintf(message, "1019%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= LOOT;
	if(check_hero(HERO_QUEENSPIDER, plyptr))
		chance = 80;
	else
	{
		if(plyptr->level[plyptr->cont] >= 4)
			chance = 80;
		else if(plyptr->level[plyptr->cont] >= 3)
			chance = 60;
	}
	if(randomnum(100, 1) >= chance)
	{
		
		sprintf(message, "1011%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	loot_city(ctynum, plyptr);
	sprintf(message, "1012%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
	send_channel(message, 0);
	if(randomnum(100, 1) <= 15)
	{
		cities[ctynum].lock = 1;
		sprintf(message, "1013%c%s%c", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
		send_channel(message, 0);
	}
	save_city(ctynum);
	if(check_hero(HERO_QUEENSPIDER, plyptr) && !barbarian(plyptr) && randomnum(100, 1) <= 2)
	{
		sprintf(message, "8025%c%s%c", MSGDEL, heroes[HERO_QUEENSPIDER].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		log_event("", message, plyptr->name);
		send_channel(message, 0);
		heroes[HERO_QUEENSPIDER].alive = 0;
		heroes[HERO_QUEENSPIDER].owner = 0;
		heroes[HERO_QUEENSPIDER].serve = conq_info.today;
		save_hero();
		lost_hero(HERO_QUEENSPIDER, plyptr);
	}
}

void loot_city(int ctynum, player *plyptr)
{
	char item[16] = {0};
	int amount = 0;
	int count;
	int found = 0;
	int num;

	while(!found)
	{
		num = randomnum(3, 0);
		switch(num)
		{
			case 0:
				if(cities[ctynum].food_qty > 0)
				{
					if((amount = cities[ctynum].food_qty * randomnum(15, 10) / 100) < 1)
						amount = 1;
					plyptr->food[plyptr->cont] += amount;
					cities[ctynum].food_qty -= amount;
					strcpy(item, food_name);
					found = 1;
				}
				break;
			case 1:
				if(cities[ctynum].peasant_qty > 0)
				{
					if((amount = cities[ctynum].peasant_qty * randomnum(15, 10) / 100) < 1)
						amount = 1;
					plyptr->peasant[plyptr->cont] += amount;
					cities[ctynum].peasant_qty -= amount;
					sprintf(item, "%s(s)", peasant_name);
					found = 1;
				}
				break;
			case 2:
				if(cities[ctynum].copper > 0)
				{
					if((amount = cities[ctynum].copper * randomnum(10, 8) / 100) < 1)
						amount = 1;
					plyptr->copper[plyptr->cont] += amount;
					cities[ctynum].copper -= amount;
					strcpy(item, money_name);
					found = 1;
				}
				break;
			case 3:
				if(units[plyptr->troop_type][CATAPULTNUM].attack == 0)
					break;
				for(count = 0; count < MAXMRK; ++count)
					if(cities[ctynum].troop_num[count] == CATAPULTNUM && cities[ctynum].troop_qty[count] > 0 && (restrict_markets == 0 || cities[ctynum].troop_type[count] == plyptr->troop_type))
						break;
				if(count < MAXMRK)
				{
					if((amount = cities[ctynum].troop_qty[count] * randomnum(5, 1) / 100) < 1)
						amount = 1;
					plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[CATAPULTNUM] += amount;
					cities[ctynum].troop_qty[count] -= amount;
					sprintf(item, "%s(s)", units[plyptr->troop_type][CATAPULTNUM].name);
					found = 1;
				}
				break;
		}
	}
	sprintf(message, "1014%c%s%c%d%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, amount, MSGDEL, item, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void make_barb(int level, player *plyptr)
{
	int count;
	int hero;
	int oldclass;

	if(!barbarian(&players[plyptr->lord]))
		remove_vassal(plyptr);
	oldclass = plyptr->classnum;
	--totply[plyptr->classnum];
	plyptr->classnum = randomnum(10 + total_races[BARBARIAN] - 1, 10);
	++totply[plyptr->classnum];
	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0)
			continue;
		plyptr->honor[count] = 0;
		if(plyptr->level[count] > 2)
			change_level(count, -(plyptr->level[count] - 2), plyptr);
		check_assets(count, level, plyptr);
		if(plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] > 0 || plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] > 0)
		{
			sprintf(message, "2072%c%s%c", MSGDEL, plyptr->kname[count], MSGDEL);
			send_message(message, plyptr->name, 1);
			plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] = 0;
			plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] = 0;
		}
	}	
	sprintf(message, "1015%c%s%c%s%c%s%c", MSGDEL, select_class(oldclass), MSGDEL, select_class(plyptr->classnum), MSGDEL, select_classname(BARBARIAN), MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "1016%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, select_class(oldclass), MSGDEL, plyptr->nick, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_classname(BARBARIAN), MSGDEL);
	send_channel(message, 0);
	if(plyptr->ally > 0)
		break_alliance(plyptr);
	if((hero = find_hero(plyptr)) >= 0 && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	if(!check_hero(HERO_NECROMANCER, plyptr))
		plyptr->troop_type = troop_type(plyptr->classnum);
	check_troops(plyptr);
	check_vassals(plyptr);
}

void rebel(player *plyptr)
{
	int hero;

	if(allow_barb == 0)
	{
		sprintf(message, "6%c%s%c", MSGDEL, select_classname(BARBARIAN), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "7%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "8%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!fighter(plyptr) || titan(plyptr))
	{
		sprintf(message, "9%c%s%c", MSGDEL, select_classname(BASE_CLASSNUM(plyptr->classnum)), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < REBEL)
	{
		sprintf(message, "1%c%d%cto become a %s%c", MSGDEL, REBEL, MSGDEL, select_classname(BARBARIAN), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < class_change[BARBARIAN])
	{
		sprintf(message, "1017%c%d%c%s%c%s%c", MSGDEL, class_change[BARBARIAN], MSGDEL, money_name,  MSGDEL, select_classname(BARBARIAN), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= REBEL;
	plyptr->copper[plyptr->cont] -= class_change[BARBARIAN];
	if((hero = find_hero(plyptr)) >= 0 && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	make_barb(CLASSCHANGE, plyptr);
}
