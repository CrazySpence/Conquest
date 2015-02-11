/******************************************************************

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

void enemy(int ally, int army_type, int enemy_army, int horde_army, int strength, player *eptr, player *plyptr)
{
	char h_winmsg[4][128] = 
	{
		"The Horde commander sneers, \"The rape of your land and people will be most gratifying!\"", 
		"The Horde commander jests, \"Bah!  My 2 year old son can do better!\"",
		"The Horde commander scoffs, \"You are such fools, the Barbarian Hordes will crush you all!\"",
		"The Horde commander jeers, \"Your land is ripe.  Yes, we will enjoy taking this land...\""
	};
	float pct;
	int amount;
	int carry;
	int count;
	int cont;
	int size;

	memset(eptr, 0, PLYSIZ);
	if((size = strength) < 0)
		size = randomnum(MIGHTY, WEAK);
	switch(army_type)
	{
		case UNDEAD:
			strcpy(eptr->name, "Undead");
			strcpy(eptr->nick, "Undead Horde");
			eptr->troop_type = VAMPIRE;
			break;
		case CRUSADERS:
			strcpy(eptr->name, "Band");
			strcpy(eptr->nick, "Band of Crusaders");
			eptr->troop_type = CLERIC;
			break;
		case GARRISON:
			strcpy(eptr->name, "Garrison");
			strcpy(eptr->nick, "Garrison");
			eptr->troop_type = FIGHTER;
			break;
		case MERCENARY:
			strcpy(eptr->name, "Mercenary");
			strcpy(eptr->nick, "Mercenary Army");
			eptr->troop_type = plyptr->troop_type;
			break;
		case IMMORTAL:
			strcpy(eptr->name, "Immortal");
			strcpy(eptr->nick, "Immortal Army");
			eptr->troop_type = TITAN;
			break;
		default:
			strcpy(eptr->name, "Horde");
			if(barbarian(plyptr))
				sprintf(eptr->nick, "%s Horde", select_class(plyptr->classnum));
			else
				sprintf(eptr->nick, "%s Horde", select_class(randomnum(14, 10)));
			strcpy(eptr->winmsg, h_winmsg[randomnum(3, 0)]);
			eptr->troop_type = BARBARIAN;
			break;
	}
	cont = plyptr->cont;
	eptr->classnum = 70;
	eptr->level[cont] = size;
	eptr->kingdoms = 1;
	eptr->cont = cont;
	eptr->castle[cont] = 1;
	eptr->movepts = 30;
	eptr->city = plyptr->city;
	eptr->first_play[cont] = conq_info.today;
	eptr->spy[cont] = -1;
	default_formation(cont, eptr);
	size = (eptr->level[cont] + 1) * 25;
	for(carry = 0, count = 0; count < MAXTRP; ++count)
	{
		if(count == CATAPULTNUM)
			continue;
		if(units[eptr->troop_type][count].attack == 0 && units[eptr->troop_type][count].defend == 0)
		{
			carry = plyptr->army[enemy_army][cont].total[count] * size / 100;
			continue;
		}
		eptr->army[horde_army][cont].total[count] = plyptr->army[enemy_army][cont].total[count] * size / 100 + carry;
		carry = 0;
	}
	eptr->army[horde_army][cont].total[SOLDIERNUM] += carry;
	if(ally == 1 && plyptr->ally > 0 && players[plyptr->ally].level[plyptr->cont] >= 0)
	{
		for(carry = 0, count = 0; count < MAXTRP; ++count)
		{
			if(count == CATAPULTNUM)
				continue;
			if(units[eptr->troop_type][count].attack == 0 && units[eptr->troop_type][count].defend == 0)
			{
				carry = players[plyptr->ally].army[enemy_army][cont].total[count] * size / 100;
				continue;
			}
			eptr->army[horde_army][cont].total[count] += players[plyptr->ally].army[enemy_army][cont].total[count] * size / 100;
			carry = 0;
		}
		eptr->army[horde_army][cont].total[SOLDIERNUM] += carry;
	}
	if(units[eptr->troop_type][CATAPULTNUM].attack > 0)
		eptr->army[horde_army][cont].total[CATAPULTNUM] = randomnum(8, 2);
	strcpy(eptr->kname[cont], "Gargontia");
	if(army_size(cont, horde_army, eptr) < 1)
		eptr->army[horde_army][cont].total[SOLDIERNUM] = randomnum(100, 25);
	pct = compute_ratio(army_size(cont, horde_army, eptr));
	amount = (int)(army_size(cont, horde_army, eptr) * 150 * pct);
	eptr->copper[cont] = randomnum((int)(amount * .75), amount / 10);
	if(plyptr->peasant[cont] > MAXPEASANT)
		amount = 100;
	else
		amount = promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[cont] + 1][2];
	eptr->peasant[cont] = randomnum((int)(amount * .75), amount / 10);
	amount = army_size(cont, horde_army, eptr);
	eptr->food[cont] = randomnum((int)(amount * .75), amount / 10);
	eptr->isprotected = 1;
	eptr->surrender[cont] = (float)(plyptr->surrender[cont] + .05);
	if(eptr->surrender[cont] > 1)
		eptr->surrender[cont] = 1;
}

void horde_lost(player *horptr, player *plyptr)
{
	sprintf(message, "15041%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->nick, 1);
	sprintf(message, "15000%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, horptr->nick, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "2042%c%d%c%d%c%s%c", MSGDEL, horptr->land[horptr->cont], MSGDEL, horptr->copper[horptr->cont], MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "2043%c%d%c%s%c%s%c%d%c%s%c", MSGDEL, horptr->peasant[horptr->cont], MSGDEL, peasant_name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, horptr->food[horptr->cont], MSGDEL, food_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	plyptr->copper[plyptr->cont] += horptr->copper[horptr->cont];
	plyptr->food[plyptr->cont] += horptr->food[horptr->cont];
	plyptr->peasant[plyptr->cont] += horptr->peasant[horptr->cont];
	plyptr->honor[plyptr->cont] += plyptr->level[horptr->cont] / 2 + 1;
}

void horde_won(player *horptr, player *plyptr)
{
	float pct;
	int copper;
	int food;
	int peasant;

	sprintf(message, "15042%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->nick, 1);
	sprintf(message, "15001%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, horptr->nick, MSGDEL);
	send_channel(message, 0);
	if(horptr->army[ARMY_DEFENSE][horptr->cont].total[CATAPULTNUM] > 0 && plyptr->castle[horptr->cont] > 1)
	{
		plyptr->castle[plyptr->cont] -= 1;
		sprintf(message, "15002%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(horptr->classnum), MSGDEL, select_title(horptr->classnum, horptr->level[horptr->cont], horptr->gender), MSGDEL, horptr->name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15003%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(horptr->classnum), MSGDEL, select_title(horptr->classnum, horptr->level[horptr->cont], horptr->gender), MSGDEL, 
		horptr->name, MSGDEL, plyptr->nick, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_channel(message, 0);
	}
	send_channel(horptr->winmsg, 0);
	pct = compute_ratio(army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr));
	copper = (int)(plyptr->copper[plyptr->cont] * pct);
	food = (int)(plyptr->food[plyptr->cont] * pct);
	peasant = (int)(plyptr->peasant[plyptr->cont] * pct);
	plyptr->copper[plyptr->cont] -= copper;
	plyptr->food[plyptr->cont] -= food;
	plyptr->peasant[plyptr->cont] -= peasant;
	sprintf(message, "15004%c%d%c%s%c", MSGDEL, copper, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "15005%c%d%c%s%c%d%c%s%c", MSGDEL, peasant, MSGDEL, peasant_name, MSGDEL, food, MSGDEL, food_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	plyptr->honor[plyptr->cont] -= plyptr->level[plyptr->cont] + 1;
}

void quest(char *arg1, char *arg2, char *arg3, player *plyptr)
{
	int qtype = 0;
	int questok = 1;
	char quests[][16] = 
	{
		"", "horde", "joust", "item", "component", "hero", "riddle", ""
	};

	strcpy(quests[0], units[plyptr->troop_type][BLOODGUARDNUM].name);
	while(strlen(quests[qtype]) > 0)
	{
		if((STRCASECMP(arg1, quests[qtype])) == 0)
			break;
		++qtype;
	}
	if(strlen(arg1) < 1 || qtype > 6)
	{
		if(quest_riddle == 0)
			--qtype;
		sprintf(message, "15006%c%d%c", MSGDEL, qtype, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15007%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, QUEST_0, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15008%cVar.%c%s%c", MSGDEL, MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15009%c%d%c", MSGDEL, QUEST_5, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15010%c%d%c", MSGDEL, QUEST_1, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15011%c%d%c", MSGDEL, (plyptr->level[plyptr->cont] + 1) * 2, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "15012%c%d%c", MSGDEL, QUEST_2, MSGDEL);
		send_message(message, plyptr->name, 1);
		if(quest_riddle == 1)
		{
			sprintf(message, "15013%c%d%c", MSGDEL, QUEST_6, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		return;
	}
	switch(qtype)
	{
		case 0:
		case 5:
			break;
		case 4:
			if(!mage(plyptr) && !check_hero(HERO_DRAGON, plyptr))
				questok = 0;
			break;
		default:
			if(barbarian(plyptr) || vampire(plyptr))
				questok = 0;
			break;
	}
	if(!questok)
	{
		sprintf(message, "15014%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(qtype)
	{
		case 0:
			quest_0(plyptr);
			break;
		case 1:
			quest_1(plyptr);
			break;
		case 2:
			quest_2(plyptr);
			break;
		case 3:
			quest_3(plyptr);
			break;
		case 4:
			quest_4(arg2, 1, plyptr);
			break;
		case 5:
			quest_5(arg2, plyptr);
			break;
		case 6:
			quest_6(arg2, arg3, plyptr);
			break;
	}
}
		
void quest_0(player *plyptr)
{
	int add = 20;
	int amount;
	int cost = 3000;
	int honor;
	int msgnum = 32;
	int success = 0;

	if(plyptr->movepts < QUEST_0)
	{
		sprintf(message, "1%c%d%cto seek out the %ss%c", MSGDEL, QUEST_0, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < cost * (plyptr->level[plyptr->cont] + 1))
	{
		sprintf(message, "15016%c%s%c%d%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, cost * (plyptr->level[plyptr->cont] + 1), MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(plyptr->troop_type)
	{
		case BARBARIAN:
		case VAMPIRE:
			if(randomnum(100, 1) > 30 + ((plyptr->level[plyptr->cont] + 1) * 2))
				sprintf(message, "15043%c%s%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
			else
				success = 1;
			break;
		default:
			honor = (plyptr->level[plyptr->cont] + 1) * (plyptr->level[plyptr->cont] + 1);
			if(plyptr->honor[plyptr->cont] < honor)
			{
				switch(plyptr->troop_type)
				{
					case CLERIC:
						msgnum = 3061;
						break;
					case MAGE:
						msgnum = 11116;
						break;
					case RANGER:
						msgnum = 16020;
						break;
				}
				sprintf(message, "%d%c%s%c%s%c", msgnum, MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)], 0), MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL);
			}
			else
				success = 1;
			break;
	}
	if(!success)
	{
		if(barbarian(plyptr) || plyptr->troop_type == VAMPIRE)
		{
			plyptr->movepts -= QUEST_0;
			plyptr->copper[plyptr->cont] -= cost * (plyptr->level[plyptr->cont] + 1);
		}
		send_message(message, plyptr->name, 1);
		return;
	}
	amount = (plyptr->level[plyptr->cont] + 1) * add;
	if(check_buy(amount, BLOODGUARDNUM, plyptr))
	{
		plyptr->movepts -= QUEST_0;
		plyptr->copper[plyptr->cont] -= cost * (plyptr->level[plyptr->cont] + 1);
		if(!barbarian(plyptr) && plyptr->troop_type != VAMPIRE)
			plyptr->honor[plyptr->cont] -= (int)(plyptr->honor[plyptr->cont] * .30 + 1);
		add_bg(plyptr->cont, add, plyptr);
	}
}

void quest_1(player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int diff;
	int hours;
	int minutes;
	int num_castles = 0;
	int round = 1;
	int seconds;
	player *horptr;

	if((diff = (int)difftime(time(NULL), plyptr->can_attack)) < attack_time)
	{
		timediff(&hours, &minutes, &seconds, plyptr->can_attack);
		sprintf(message, "2000%c%s%c%d%c%d%c%d%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, hours, MSGDEL, minutes, MSGDEL, seconds, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < QUEST_1)
	{
		sprintf(message, "1%c%d%cto stop the advancing Horde!%c", MSGDEL, QUEST_1, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr) < 1)
	{
		sprintf(message, "2071%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_artifact(AEGIS, plyptr))
	{
		sprintf(message, "2002%cthe Horde%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
        }
	if(has_item(ITEM_CHAOS, plyptr))
	{
		sprintf(message, "2003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	++plyptr->attacks;
	plyptr->isprotected = 1;
	plyptr->lastattack = time(NULL);
	plyptr->movepts -= QUEST_1;
	horptr = (void *)plybuf[DEFENDER];
	combatants[ATTACKER] = plyptr;
	combatants[DEFENDER] = horptr;
	enemy(1, HORDE, ARMY_CAMPAIGN, ARMY_DEFENSE, -1, combatants[DEFENDER], combatants[ATTACKER]);
	if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
		horde_lost(horptr, plyptr);
	else
		horde_won(horptr, plyptr);
	if(count_casualties(ATTACKER))
		show_casualties(ATTACKER, ARMY_CAMPAIGN, combatants[ATTACKER]);
	if(combatants[ATTACKER_ALLY] && count_casualties(ATTACKER_ALLY))
		show_casualties(ATTACKER_ALLY, ARMY_CAMPAIGN, combatants[ATTACKER_ALLY]);
	reset_combatants();
	sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
}

void quest_2(player *plyptr)
{
	int count;
	int enemynum;
	int found = 0;
	int honor1, honor2;
	int land;
	int total;
	player *loser;
	player *winner;
	time_t current;

	if(mage(plyptr))
	{
		sprintf(message, "15017%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(ranger(plyptr))
	{
		sprintf(message, "15018%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 3)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "36%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < QUEST_2)
	{
		sprintf(message, "1%c%d%cto assemble a tournament%c", MSGDEL, QUEST_2, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < 5000 * (plyptr->level[plyptr->cont] + 1))
	{
		sprintf(message, "15019%c%s%c%d%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, 5000 * (plyptr->level[plyptr->cont] + 1), MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(forecast[plyptr->cont].joust == 0)
	{
		sprintf(message, "15020%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
        time(&current);
        if((difftime(current, plyptr->lastattack)) < ATTACKTIME)
	{
		sprintf(message, "15021%c%s%c%d%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, ATTACKTIME - (int)difftime(current, plyptr->lastattack), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(count = 0, total = 0; count < 9; ++count)
		total += totlvl[plyptr->cont][count];
	if(total < 2)
	{
		sprintf(message, "15039%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	count = 0;
	do
	{
		enemynum = find_randomplayer(plyptr->cont, 3);
		if(enemynum > 0 && players[enemynum].level[players[enemynum].cont] < maxlevel[BASE_CLASSNUM(players[enemynum].classnum)] - 1 && enemynum != playernum(plyptr))
		{
			if(players[enemynum].special_misc & SPC_VACATION || barbarian(&players[enemynum]) || mage(&players[enemynum]) || vampire(&players[enemynum]) || ranger(&players[enemynum]))
				continue;
			else
			{
				found = 1;
				break;
			}
		}
	} while(count++ < 1000);
	if(!found)
	{
		sprintf(message, "15040%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->lastattack = current;
	plyptr->movepts -= QUEST_2;
	plyptr->copper[plyptr->cont] -= 5000 * (plyptr->level[plyptr->cont] + 1);
	honor1 = randomnum(plyptr->honor[plyptr->cont], 1) + plyptr->honor[plyptr->cont] / 2;
	honor2 = randomnum(players[enemynum].honor[plyptr->cont], 1) + players[enemynum].honor[plyptr->cont] / 2;
	if(fighter(plyptr))
	{
		if(plyptr->level[plyptr->cont] >= 5)
			honor1 += (int)(honor1 * .15);
		else if(plyptr->level[plyptr->cont] >= 4)
			honor1 += (int)(honor1 * .10);
		else 
			honor1 += (int)(honor1 * .05);
	}
	if(fighter(&players[enemynum]))
	{
		if(players[enemynum].level[plyptr->cont] >= 5)
			honor2 += (int)(honor2 * .15);
		else if(players[enemynum].level[plyptr->cont] >= 4)
			honor2 += (int)(honor2 * .10);
		else
			honor2 += (int)(honor2 * .05);
	}
	if(honor1 >= honor2)
	{
		winner = plyptr;
		loser = &players[enemynum];
	}
	else
	{
		winner = &players[enemynum];
		loser = plyptr;
	}
	land = (int)(loser->land[loser->cont] * .05);
	sprintf(message, "15022%c    ~@   ^##          %cLand: %-6d%c     ##^   @~%c", MSGDEL, MSGDEL, land, MSGDEL, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "999%c**==)-----------                   -----------(--**", MSGDEL);
	send_channel(message, 0);
	sprintf(message, "999%c  **  * *                                  * *  * *", MSGDEL);
	send_channel(message, 0);
	sprintf(message, "999%c%s %s %s        %s %s %s", MSGDEL, 
	select_class(winner->classnum), select_title(winner->classnum, winner->level[winner->cont], winner->gender), winner->nick,
	select_class(loser->classnum), select_title(loser->classnum, loser->level[loser->cont], loser->gender), loser->nick);
	send_channel(message, 0);
	switch(randomnum(4, 0))
	{
		case 0:
			sprintf(message, "15023%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL);
			break;
		case 1:
			sprintf(message, "15024%c%s%c%s%c%s%c%d%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL, adjectives[loser->gender][0], MSGDEL, randomnum(9, 2), MSGDEL);
			break;
		case 2:
			sprintf(message, "15025%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL);
			break;
		default:
			sprintf(message, "15026%c%s%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL, adjectives[loser->gender][0], MSGDEL);
			break;
	}
	send_channel(message, 0);
	sprintf(message, "15027%c%d%c", MSGDEL, land, MSGDEL);
	send_message(message, winner->name, 1);
	sprintf(message, "15028%c%c", MSGDEL, MSGDEL);
	send_message(message, loser->name, 1);
	sprintf(message, "15029%c%s%c%d%c", MSGDEL, winner->name, MSGDEL, land, MSGDEL);
	send_message(message, loser->name, 1);
	log_event("", message, loser->name);
	winner->land[winner->cont] += land;
	loser->land[loser->cont] -= land;
	winner->honor[winner->cont] += 2;
	loser->honor[loser->cont] -= 2;
	list_change(loser->nick, winner->nick);
	save_player(enemynum);
}

void quest_6(char *cmd, char *solution, player *plyptr)
{
	int land;
	int peasants;

	if(quest_riddle == 0)
	{
		sprintf(message, "15030%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 3)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((STRCASECMP(cmd, "answer")) == 0)
	{
		if(conq_info.riddlenum == 0)
			sprintf(message, "15031%c%c", MSGDEL, MSGDEL);
		else
			sprintf(message, "15032%c%s%c", MSGDEL, riddles[conq_info.riddlenum - 1].solution, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((STRCASECMP(cmd, "solution")) == 0)
	{
		if(titan(plyptr))
		{
			sprintf(message, "15033%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(plyptr->movepts < QUEST_6)
		{
			sprintf(message, "1%c%d%ctp solve the Oracle's riddle%c", MSGDEL, QUEST_6, MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		plyptr->movepts -= QUEST_6;
		if((STRCASECMP(solution, riddles[conq_info.riddlenum].solution)) == 0)
		{
			land = promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont] + 1][1] / 2;
			peasants = promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont] + 1][2] / 2;
			plyptr->land[plyptr->cont] += land;
			plyptr->peasant[plyptr->cont] += peasants;
			sprintf(message, "15034%c%d%c%d%c%s%c", MSGDEL, land, MSGDEL, peasants, MSGDEL, peasant_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "15035%c%s%c%s%c%s%c%s%c%d%c", 
			MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, riddles[conq_info.riddlenum].solution, MSGDEL, num_riddles - (conq_info.riddlenum + 1), MSGDEL);
			send_channel(message, 0);
			if(++conq_info.riddlenum >= num_riddles)
				quest_riddle = 0;
			save_info();
		}
		else
		{
			sprintf(message, "15036%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		return;
	}
	sprintf(message, "15037%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "15038%c%s%c", MSGDEL, riddles[conq_info.riddlenum].clue, MSGDEL);
	send_message(message, plyptr->name, 1);
}

int summoned(player *plyptr)
{
	if(!plyptr)
		return(0);
	if(plyptr->classnum >= 70 && plyptr->classnum < 80)
		return(1);
	else
		return(0);
}
