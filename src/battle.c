/*****************************************************************************

	Conquest! - The original IRC war game
	Copyright (C) 1998 James D. Bennett

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

void add_balance(int adj, player *plyptr)
{
	int classnum;

	if(fighter(plyptr) || cleric(plyptr) || mage(plyptr))
	{
		classnum = plyptr->classnum % 10;
		if(conq_info.balance[classnum] + adj >= 0 && conq_info.balance[classnum] + adj <= 100)
		{
			conq_info.balance[classnum] += adj;
			save_info();
		}
	}
}

void add_battles(player *winner)
{
	int attacker = 0;
	int count;
	int defender = 0;

	if(winner == combatants[ATTACKER])
	{
		attacker = 1;
		defender = 0;
	}
	else
	{
		attacker = 0;
		defender = 1;
	}
	for(count = ATTACKER; count <= ATTACKER_ALLY; ++count)
	{
		if(combatants[count] && !summoned(combatants[count]))
		{
			combatants[count]->battles_won += attacker;
			combatants[count]->battles_lost += defender;
		}
		if(combatants[count + DEFENDER] && !summoned(combatants[count + DEFENDER]))
		{
			combatants[count + DEFENDER]->battles_won += defender;
			combatants[count + DEFENDER]->battles_lost += attacker;
		}
		if(!summoned(combatants[count]))
			check_rank(playernum(combatants[count]));
	}
	sort_rank();
	save_info();
}

void add_casualties(int enemynum, int num, player *plyptr)
{
	plyptr->troops_lost += count_casualties(num);
	plyptr->troops_killed += count_side_casualties(enemynum);
}

void adjust_honor(int cont, player *loser, player *winner)
{
	if(!barbarian(winner) && !vampire(winner))
	{
		winner->honor[cont] += loser->level[cont] + 1;
		if(winner->ally > 0)
			if(!barbarian(&players[winner->ally]) && players[winner->ally].level[cont] >= 0)
				players[winner->ally].honor[cont] += (loser->level[cont] + 1) / 2 + 1;
	}
	if(!barbarian(loser) && !vampire(loser))
	{
		loser->honor[cont] -= loser->level[cont] + 1;
		if(loser->ally > 0)
			if(!barbarian(&players[loser->ally]) && players[loser->ally].level[cont] >= 0)
				players[loser->ally].honor[cont] -= (loser->level[cont] + 1) / 2 + 1;
	}
}

void adjust_unit(int ambush, int attack, int defend, int num_attacks, int range, 
int regen, int shield, int swarm, int troopnum, player *plyptr, unit *mod)
{
	if(units[plyptr->troop_type][troopnum].attack == 0 && units[plyptr->troop_type][troopnum].defend == 0)
		return;
	mod[troopnum].ambush += ambush;
	mod[troopnum].attack += attack;
	mod[troopnum].defend += defend;
	mod[troopnum].num_attacks += num_attacks;
	mod[troopnum].range += range;
	mod[troopnum].regenerate += regen;
	mod[troopnum].shield += shield;
	mod[troopnum].swarm += swarm;
}

int allow_attack(char *name, int allow_invade, player *attacker)
{
	int a_level;
	int cont;
	int diff;
	int hours;
	int minutes;
	int plynum;
	int seconds;
	player *defender;
	struct tm *pnt;
	time_t current;

	time(&current);
	if((diff = (int)difftime(current, attacker->can_attack)) < attack_time)
	{
		timediff(&hours, &minutes, &seconds, attacker->can_attack);
		sprintf(message, "2000%c%s%c%d%c%d%c%d%c", MSGDEL, adjectives[attacker->gender][3], MSGDEL, hours, MSGDEL, minutes, MSGDEL, seconds, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	defender = &players[plynum];
	if(attacker == defender)
	{
		sprintf(message, "11%c%c", MSGDEL, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(!attack_vassal(attacker, defender))
		return(0);
	if(enforce_peace == 1 && equal_class(attacker->classnum, defender->classnum))
	{
		sprintf(message, "2001%c%s%c", MSGDEL, select_class(defender->classnum), MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(attacker->attacks >= attacks_allowed(attacker))
	{
		sprintf(message, "4%c%d%c", MSGDEL, attacker->attacks, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	cont = defender->cont;
	if(attacker->level[cont] == -1)
		a_level = 0;
	else
		a_level = attacker->level[cont];
	if(cont != attacker->cont && allow_invade == 0)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[attacker->gender][3], MSGDEL, defender->name, MSGDEL, conts[attacker->cont].name, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(defender->isprotected == 0)
	{
		sprintf(message, "2004%c%c", MSGDEL, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(restrict_combat == 1 && defender->validate != VAL_NORMAL && defender->validate != VAL_QUESTCHAR)
	{
		sprintf(message, "2089%c%s%c", MSGDEL, defender->name, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(attack_reprieve == 1 && defender->attacks_lost[cont] >= attacks_allowed(defender))
	{
		sprintf(message, "2078%c%s%c%d%c", MSGDEL, defender->name, MSGDEL, defender->attacks_lost[cont], MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(check_artifact(AEGIS, attacker) || check_artifact(AEGIS, defender))
	{
		sprintf(message, "2002%c%s%c", MSGDEL, defender->name, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(has_item(ITEM_CHAOS, attacker) || has_item(ITEM_CHAOS, defender))
	{
		sprintf(message, "2003%c%c", MSGDEL, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(a_level < maxlevel[BASE_CLASSNUM(attacker->classnum)] - 1 && titan(defender))
	{
		sprintf(message, "2005%c%c", MSGDEL, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(a_level - (attacker->level[cont] == -1?0:attack_level(cont, attacker)) > defender->level[cont] && !can_ret(attacker, defender) && !titan(attacker))
	{
		sprintf(message, "2006%c%c", MSGDEL, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if((difftime(current, attacker->lastattack)) < ATTACKTIME)
	{
		sprintf(message, "2007%c%s%c%d%c", MSGDEL, adjectives[attacker->gender][3], MSGDEL, ATTACKTIME - (int)difftime(current, attacker->lastattack), MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	if(check_vacation(attacker->name, defender))
		return(0);
	if(conq_info.cont_special[cont] & SPC_CEASEFIRE)
	{
		pnt = localtime(&current);
		sprintf(message, "16026%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, attacker->name, 1);
		sprintf(message, "4394%cceasefire%c%d%c", MSGDEL, MSGDEL, 60 - pnt->tm_min, MSGDEL);
		send_message(message, attacker->name, 1);
		return(0);
	}
	return(plynum);
}

int army_size(int cont, int type, player *plyptr)
{
	int count;
	int total = 0;

	if(plyptr == NULL)
		return(0);
	for(count = 0; count < MAXTRP; ++count)
		if(count != CATAPULTNUM)
			total += plyptr->army[type][cont].total[count];
	return(total);
}

void attack(char *name, player *plyptr)
{
	int plynum;

	if(strlen(name) < 1)
	{
		help(plyptr->name, "attack", "", "");
		return;
	}
	if(plyptr->movepts < ATTACK)
	{
		sprintf(message, "1%c%d%cto attack%c", MSGDEL, ATTACK, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return;
	if(army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr) < 1)
	{
		sprintf(message, "2071%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= ATTACK;
	plyptr->isprotected = 1;
	if((check_hero(HERO_GENERAL1, plyptr) && check_hero(HERO_GENERAL2, &players[plynum])) || (check_hero(HERO_GENERAL2, plyptr) && check_hero(HERO_GENERAL1, &players[plynum])))
	{
		sprintf(message, "2008%c%s%c%s%c", MSGDEL, heroes[HERO_GENERAL1].type, MSGDEL, heroes[HERO_GENERAL2].type, MSGDEL);
		send_channel(message, 0);
		do {
			combatants[ATTACKER] = plyptr;
			combatants[DEFENDER] = &players[plynum];
			start_battle();
			combatants[ATTACKER] = &players[plynum];
			combatants[DEFENDER] = plyptr;
			start_battle();
		} while(plyptr->cont == players[plynum].cont && heroes[HERO_GENERAL1].owner > 0 && heroes[HERO_GENERAL2].owner > 0 && ++plyptr->attacks < attacks_allowed(plyptr));
	}
	else
	{
		++plyptr->attacks;
		if((hero_prebattle(&players[plynum], plyptr)) != 2)
		{
			combatants[ATTACKER] = plyptr;
			combatants[DEFENDER] = &players[plynum];
			if(plyptr->ally == plynum)
			{
				break_alliance(combatants[ATTACKER]);
				sprintf(message, "2073%c%c", MSGDEL, MSGDEL);
				send_channel(message, 0);
			}
			if(combatants[DEFENDER]->ally > 0)
			{
				if(check_hero(HERO_NINJA, plyptr) && randomnum(100, 1) <= 25)
				{
					if(fog_of_war == 0)
					{
						sprintf(message, "2009%c%s%c%s%c%s%c", MSGDEL, heroes[HERO_NINJA].type, MSGDEL, combatants[DEFENDER]->nick, MSGDEL, players[combatants[DEFENDER]->ally].nick, MSGDEL);
						send_channel(message, 0);
					}
					break_alliance(combatants[DEFENDER]);
				}
			}
			start_battle();
		}
	}
	plyptr->lastattack = time(NULL);
	hero_postbattle(plyptr->cont, ATTACKER, &players[plynum], plyptr);
	if(plyptr->ally > 0 && players[plyptr->ally].cont == plyptr->cont)
		hero_postbattle(plyptr->cont, ATTACKER_ALLY, &players[plynum], &players[plyptr->ally]);
	save_player(plynum);
}

int attack_level(int cont, player *plyptr)
{
	switch(BASE_CLASSNUM(plyptr->classnum))
	{
		case BARBARIAN:
			if(plyptr->level[cont] >= 6)
				return(4);
			if(plyptr->level[cont] >= 3)
				return(3);
			return(2);
		case MAGE:
			return(2);
		case VAMPIRE:
			return(3);
		case CLERIC:
			return(2);
		case RANGER:
			return(1);
		default:
			return(2);
	}
}

int attacks_allowed(player *plyptr)
{
	int attacks = 0;

	switch(BASE_CLASSNUM(plyptr->classnum))
	{
		case BARBARIAN:
			if(plyptr->levelmax <= 2)
				attacks = 2;
			else if(plyptr->levelmax <= 5)
				attacks = 7;
			else
				attacks = 12;
			break;
		case MAGE:
			if(plyptr->levelmax >= 3)
			{
				attacks += 1;
				if(plyptr->levelmax >= 5 && plyptr->levelmax < 7)
					attacks += 2;
				if(plyptr->levelmax >= 7)
					attacks += 4;
			}
			break;
		case CLERIC:
		case RANGER:
			if(plyptr->levelmax >= 3)
				attacks += plyptr->levelmax - 2;
			break;
		default:
			if(plyptr->levelmax >= 3)
			{
				attacks += 2;
				if(plyptr->levelmax >= 5 && plyptr->levelmax < 7)
					attacks += 2;
				if(plyptr->levelmax >= 7)
					attacks += 4;
			}
			break;
	}
	attacks += 3;
	if(check_artifact(BOOTS, plyptr))
		attacks += 4;
	return(attacks);
}

void auto_transfer(int cont, int from, int to, player *plyptr)
{
	int troopnum;

	if(advanced_armies == 0)
		return;
	for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
	{
		plyptr->army[to][cont].total[troopnum] += plyptr->army[from][cont].total[troopnum];
		plyptr->army[from][cont].total[troopnum] = 0;
	}
}

int battle(int *num_castles, int *round, int *total_attack, int *total_defend)
{
	char lord[2][128] = {{0}, {0}};
	int attack_damage[MAXADV];
	int chaos = 0;
	int cnt;
	int cont;
	int count;
	int defend_damage[MAXADV];
	int division;
	int max_casualties[MAXCBT];
	int misfires[MAXCBT];
	int total[MAXCBT];
	int won = -1;

	reset_casualties();
	for(count = 0; count < MAXCBT; ++count)
	{
		max_casualties[count] = 0;
		total[count] = 0;
	}
	cont = combatants[ATTACKER]->cont;
	if(!summoned(combatants[ATTACKER]) && !summoned(combatants[DEFENDER]))
	{
		sprintf(message, "2010%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(combatants[ATTACKER]->classnum), MSGDEL, select_title(combatants[ATTACKER]->classnum, combatants[ATTACKER]->level[cont], combatants[ATTACKER]->gender), MSGDEL, combatants[ATTACKER]->nick, MSGDEL, 
		structures[BASE_CLASSNUM(combatants[DEFENDER]->classnum)].name, MSGDEL);
		send_message(message, combatants[DEFENDER]->name, 0);
	}
	for(count = ATTACKER; count <= ATTACKER_ALLY; ++count)
	{
		if(count == ATTACKER)
		{
			check_ally(cont, count, combatants[count]);
			check_ally(cont, count + (MAXCBT / 2), combatants[count + (MAXCBT / 2)]);
		}
		check_horde(cont, MAXCBT / 2, count, ARMY_CAMPAIGN, combatants[count]);
		check_horde(cont, MAXCBT, count + (MAXCBT / 2), ARMY_DEFENSE, combatants[count + (MAXCBT / 2)]);
	}
	vassal_aid(cont, ARMY_CAMPAIGN, combatants[DEFENDER], combatants[ATTACKER]);
	vassal_aid(cont, ARMY_DEFENSE, combatants[ATTACKER], combatants[DEFENDER]);
	if(!summoned(combatants[ATTACKER]) && !summoned(combatants[DEFENDER]) && !barbarian(combatants[DEFENDER]) && combatants[DEFENDER]->troop_type != VAMPIRE)
	{
		if(combatants[DEFENDER]->level[cont] == 0 && combatants[ATTACKER]->level[cont] >= 2)
		{
			combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] += (combatants[ATTACKER]->level[cont] + 1) * 300;
			sprintf(message, "2016%c%s%c%s%c%s%c", MSGDEL, select_title(combatants[DEFENDER]->classnum, maxlevel[BASE_CLASSNUM(combatants[DEFENDER]->classnum)], 0), MSGDEL, 
			select_title(combatants[DEFENDER]->classnum, combatants[DEFENDER]->level[cont], combatants[DEFENDER]->gender), MSGDEL, units[combatants[DEFENDER]->troop_type][BLOODGUARDNUM].name, MSGDEL);
			send_message(message, combatants[ATTACKER]->name, 1);
		}
		if(check_artifact(DSHIELD, combatants[DEFENDER]) && !ranger(combatants[DEFENDER]))
		{
			combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] += (combatants[ATTACKER]->level[cont] + 1) * 150;
			sprintf(message, "2017%c%s%c%s%c%s%c", MSGDEL, artifacts[DSHIELD].name, MSGDEL, units[combatants[DEFENDER]->troop_type][BLOODGUARDNUM].name, MSGDEL, combatants[DEFENDER]->name, MSGDEL);
			send_message(message, combatants[ATTACKER]->name, 1);
		}
	}
	*total_attack = 0;
	*total_defend = 0;
	for(count = 0; count <= ATTACKER_HORDE2; ++count)
	{
		*total_attack += army_size(cont, ARMY_CAMPAIGN, combatants[ATTACKER + count]);
		*total_defend += army_size(cont, ARMY_DEFENSE, combatants[DEFENDER + count]);
	}
	sprintf(lord[0], ", Vassal of %s %s", select_class(players[combatants[ATTACKER]->lord].classnum), players[combatants[ATTACKER]->lord].nick);
	sprintf(lord[1], ", Vassal of %s %s", select_class(players[combatants[DEFENDER]->lord].classnum), players[combatants[DEFENDER]->lord].nick);
	sprintf(message, "2018%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", 
	MSGDEL, select_class(combatants[ATTACKER]->classnum), MSGDEL, select_title(combatants[ATTACKER]->classnum, combatants[ATTACKER]->level[cont], combatants[ATTACKER]->gender), MSGDEL, combatants[ATTACKER]->nick, MSGDEL, 
	combatants[ATTACKER]->kname[cont], MSGDEL, combatants[ATTACKER]->lord > 0?lord[0]:"", MSGDEL, 
	select_class(combatants[DEFENDER]->classnum), MSGDEL, select_title(combatants[DEFENDER]->classnum, combatants[DEFENDER]->level[cont], combatants[DEFENDER]->gender), MSGDEL, combatants[DEFENDER]->nick, MSGDEL, 
	combatants[DEFENDER]->kname[cont], MSGDEL, combatants[DEFENDER]->lord > 0?lord[1]:"", MSGDEL, conts[cont].name, MSGDEL);
	send_channel(message, 0);
	if(check_hero(HERO_BARBLEADER, combatants[ATTACKER]))
	{
		sprintf(message, "2020%c%s%c%s%c", MSGDEL, heroes[HERO_BARBLEADER].type, MSGDEL, combatants[ATTACKER]->nick, MSGDEL);
		send_channel(message, 0);
	}
	for(count = 0; count < MAXCBT; ++count)
		if(combatants[count])
		{
			check_catapult(cont, count, count <= ATTACKER_HORDE2?ARMY_CAMPAIGN:ARMY_DEFENSE, combatants[count]);
			total[count] = army_size(cont, count <= ATTACKER_HORDE2?ARMY_CAMPAIGN:ARMY_DEFENSE, combatants[count]);
			max_casualties[count] = (int)(total[count] * combatants[count]->surrender[cont]);
			if(max_casualties[count] < 1 && total[count] > 0)
				max_casualties[count] = 1;
		}
	if((*num_castles = castles_captured(cont, combatants[ATTACKER])) >= combatants[DEFENDER]->castle[cont])
	{
		auto_transfer(cont, ARMY_CAMPAIGN, ARMY_DEFENSE, combatants[DEFENDER]);
		total[DEFENDER] = army_size(cont, ARMY_DEFENSE, combatants[DEFENDER]);
		max_casualties[DEFENDER] = total[DEFENDER];
	}
	else
	{
		if(*total_attack >= *total_defend * 3 && !summoned(combatants[DEFENDER]) && combatants[DEFENDER]->surrender[cont] != 1)
		{
			if(fog_of_war == 0)
			{
				sprintf(message, "2021%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
				send_channel(message, 0);
			}
			won = ATTACKER;
		}
	}
	sprintf(message, "2019%c%c", MSGDEL, MSGDEL);
	send_message(message, combatants[ATTACKER]->name, 1);
	review(combatants[ATTACKER]->name, combatants[ATTACKER]->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_DEFENSE, combatants[DEFENDER], combatants[DEFENDER]);
	sprintf(message, "2019%c%c", MSGDEL, MSGDEL);
	send_message(message, combatants[DEFENDER]->name, 1);
	review(combatants[DEFENDER]->name, combatants[DEFENDER]->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_CAMPAIGN, combatants[ATTACKER], combatants[ATTACKER]);
	if(!summoned(combatants[ATTACKER]) && !summoned(combatants[DEFENDER]))
	{
		show_surrender(combatants[ATTACKER]->name, combatants[DEFENDER]);
		show_surrender(combatants[DEFENDER]->name, combatants[ATTACKER]);
		show_formation(combatants[ATTACKER]->name, cont, ARMY_DEFENSE, combatants[DEFENDER]);
		show_formation(combatants[DEFENDER]->name, cont, ARMY_CAMPAIGN, combatants[ATTACKER]);
	}
	if(won == -1)
	{
		show_allies(ATTACKER, ARMY_CAMPAIGN, combatants[ATTACKER], combatants[DEFENDER]);
		show_allies(DEFENDER, ARMY_DEFENSE, combatants[DEFENDER], combatants[ATTACKER]);
		if(check_hero(HERO_LORDCHAOS, combatants[ATTACKER]) && !summoned(combatants[DEFENDER]))
		{
			sprintf(message, "2011%c%s%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL, heroes[HERO_LORDCHAOS].type, MSGDEL);
			send_channel(message, 0);
			if(combatants[DEFENDER]->ally > 0)
				count = randomnum(3, 0);
			else
				count = randomnum(2, 0);
			switch(count)
			{
				case 0:
					attack_damage[MODE_WALK] = army_size(cont, ARMY_DEFENSE, combatants[DEFENDER]) * randomnum(5, 1) / 100;
					subtract_casualties(cont, attack_damage, DEFENDER, *round, ARMY_DEFENSE, combatants[DEFENDER]);
					sprintf(message, "2012%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
					break;
				case 1:
					sprintf(message, "2013%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
					won = ATTACKER;
					break;
				case 2:
					sprintf(message, "2014%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
					chaos = 1;
					break;
				default:
					sprintf(message, "2015%c%s%c%s%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL, adjectives[combatants[DEFENDER]->gender][0], MSGDEL, players[combatants[DEFENDER]->ally].nick, MSGDEL);
					break_alliance(combatants[DEFENDER]);
					break;
			}
			if(fog_of_war == 0)
				send_channel(message, 0);
		}
	}
	if(max_casualties[DEFENDER] == army_size(cont, ARMY_DEFENSE, combatants[DEFENDER]) && !summoned(combatants[DEFENDER]) && won == -1 && fog_of_war == 0)
	{
		sprintf(message, "2022%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
		send_channel(message, 0);
	}
	check_batmod(combatants[ATTACKER]->cont, combatants[ATTACKER], batmod[ATTACKER]);
	check_batmod(combatants[ATTACKER]->cont, combatants[DEFENDER], batmod[DEFENDER]);
	if(combatants[ATTACKER_ALLY])
		check_batmod(combatants[ATTACKER]->cont, combatants[ATTACKER_ALLY], batmod[ATTACKER_ALLY]);
	if(combatants[DEFENDER_ALLY])
		check_batmod(combatants[ATTACKER]->cont, combatants[DEFENDER_ALLY], batmod[DEFENDER_ALLY]);
	while(won == -1)
	{
		if(army_size(cont, ARMY_CAMPAIGN, combatants[ATTACKER]) < 1)
		{
			won = DEFENDER;
			break;
		}
		if(army_size(cont, ARMY_DEFENSE, combatants[DEFENDER]) < 1)
		{
			won = ATTACKER;
			break;
		}
		for(count = 0; count < MAXADV; ++count)
		{
			attack_damage[count] = 0;
			defend_damage[count] = 0;
		}
		for(count = ATTACKER; count <= ATTACKER_HORDE2; ++count)
		{
			misfires[count] = 0;
			if(combatants[count])
			{
				damage_catapult(cont, count, ARMY_CAMPAIGN, combatants[count]);
				compute_casualties(cont, attack_damage, misfires, count, *round, ARMY_CAMPAIGN, combatants[count]);
			}
		}
		for(count = DEFENDER; count <= DEFENDER_HORDE2; ++count)
		{
			misfires[count] = 0;
			if(combatants[count] && (count != DEFENDER_ALLY || *round > 1))
			{
				damage_catapult(cont, count, ARMY_DEFENSE, combatants[count]);
				compute_casualties(cont, defend_damage, misfires, count, *round, ARMY_DEFENSE, combatants[count]);
			}
		}
		for(count = ATTACKER; count <= ATTACKER_HORDE2; ++count)
			defend_damage[MODE_RNG] += misfires[count];
		for(count = DEFENDER; count <= DEFENDER_HORDE2; ++count)
			attack_damage[MODE_RNG] += misfires[count];
		if(chaos == 1)
		{
			chaos = 0;
			for(count = 0; count < MAXADV; ++count)
				defend_damage[count] = 0;
		}
		for(count = ATTACKER, division = 0; count <= ATTACKER_HORDE2; ++count)
			if(combatants[count])
				++division;
		if(division > 0)
			for(count = 0; count < MAXADV; ++count)
				defend_damage[count] /= division;
		for(count = DEFENDER, division = 0; count <= DEFENDER_HORDE2; ++count)
			if(combatants[count])
				++division;
		if(division > 0)
			for(count = 0; count < MAXADV; ++count)
				attack_damage[count] /= division;
		for(count = ATTACKER; count <= ATTACKER_HORDE2; ++count)
			if(combatants[count])
				subtract_casualties(cont, defend_damage, count, *round, ARMY_CAMPAIGN, combatants[count]);
		for(count = DEFENDER; count <= DEFENDER_HORDE2; ++count)
			if(combatants[count])
				subtract_casualties(cont, attack_damage, count, *round, ARMY_DEFENSE, combatants[count]);
		if(randomnum(100, 1) <= 25 && fog_of_war == 0)
		{
			sprintf(message, "2080%c%d%c", MSGDEL, *round, MSGDEL);
			send_channel(message, 0);
			if(!summoned(combatants[ATTACKER]) && total[ATTACKER] > 0)
			{
				sprintf(message, "2023%c%s%c%d%c", MSGDEL, combatants[ATTACKER]->nick, MSGDEL, (int)((float)count_casualties(ATTACKER) / (float)total[ATTACKER] * 100.0), MSGDEL);
				send_channel(message, 0);
			}
			if(!summoned(combatants[DEFENDER]) && total[DEFENDER] > 0)
			{
				sprintf(message, "2023%c%s%c%d%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL, (int)((float)count_casualties(DEFENDER) / (float)total[DEFENDER] * 100.0), MSGDEL);
				send_channel(message, 0);
			}
		}
		for(count = 0; count < MAXCBT; ++count)
		{
			switch(count)
			{
				case ATTACKER:
				case ATTACKER_ALLY:
				case DEFENDER:
				case DEFENDER_ALLY:
					if(misfires[count] > 0 && !summoned(combatants[count]) && fog_of_war == 0)
					{
						sprintf(message, "2077%c%s%c", MSGDEL, combatants[count]->nick, MSGDEL);
						send_channel(message, 0);
					}
					break;
				default:
					break;
			}
			if(combatants[count] && count_casualties(count) >= max_casualties[count])
			{
				switch(count)
				{
					case ATTACKER:
						if(army_size(cont, ARMY_DEFENSE, combatants[DEFENDER]) >= 1)
							won = DEFENDER;
						break;
					case DEFENDER:
						if(army_size(cont, ARMY_CAMPAIGN, combatants[ATTACKER]) >= 1)
							won = ATTACKER;
						break;
					case ATTACKER_ALLY:
						if(fog_of_war == 0)
						{
							sprintf(message, "2024%c%s%c", MSGDEL, players[combatants[count]->ally].nick, MSGDEL);
							send_channel(message, 0);
						}
						++combatants[count]->battles_lost;
						add_casualties(DEFENDER, count, combatants[count]);
						combatants[count] = NULL;
						break;
					case DEFENDER_ALLY:
						if(fog_of_war == 0)
						{
							sprintf(message, "2024%c%s%c", MSGDEL, players[combatants[count]->ally].nick, MSGDEL);
							send_channel(message, 0);
						}
						++combatants[count]->battles_lost;
						add_casualties(ATTACKER, count, combatants[count]);
						combatants[count] = NULL;
						break;
					case ATTACKER_HORDE1:
					case ATTACKER_HORDE2:
						if(fog_of_war == 0)
						{
							sprintf(message, "2025%c%s%c%s%c", MSGDEL, combatants[ATTACKER]->nick, MSGDEL, combatants[count]->nick, MSGDEL);
							send_channel(message, 0);
						}
						combatants[count] = NULL;
						break;
					case DEFENDER_HORDE1:
					case DEFENDER_HORDE2:
						if(fog_of_war == 0)
						{
							sprintf(message, "2025%c%s%c%s%c", MSGDEL, combatants[DEFENDER]->nick, MSGDEL, combatants[count]->nick, MSGDEL);
							send_channel(message, 0);
						}
						combatants[count] = NULL;
						break;
				}
				if(won >= ATTACKER)
					break;
			}
		}
		if(won >= ATTACKER)
			break;
		if((++(*round)) > 30)
		{
			sprintf(message, "2079%c%c", MSGDEL, MSGDEL);
			send_channel(message, 0);
			won = DEFENDER;
		}
	}
	for(count = 0; count < MAXCBT; ++count)
		for(cnt = 0; cnt < MAXTRP; ++cnt)
			memset(&batmod[count][cnt], 0, UNTSIZ);
	if(!summoned(combatants[ATTACKER]) && !summoned(combatants[DEFENDER]) && !barbarian(combatants[DEFENDER]) && combatants[DEFENDER]->troop_type != VAMPIRE)
	{
		if(combatants[DEFENDER]->level[cont] == 0 && combatants[ATTACKER]->level[cont] >= 2)
			combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] -= (combatants[ATTACKER]->level[cont] + 1) * 300;
		if(check_artifact(DSHIELD, combatants[DEFENDER]) && !ranger(combatants[DEFENDER]))
			combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] -= (combatants[ATTACKER]->level[cont] + 1) * 150;
		if(combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] < 0)
			combatants[DEFENDER]->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] = 0;
	}
	if(won == ATTACKER)
	{
		add_battles(combatants[ATTACKER]);
		auto_transfer(cont, ARMY_CAMPAIGN, ARMY_DEFENSE, combatants[DEFENDER]);
		if(attack_reprieve == 1)
		{
			if(!summoned(combatants[ATTACKER]) && combatants[ATTACKER]->attacks_lost[cont] > 0)
				--combatants[ATTACKER]->attacks_lost[cont];
			if(!summoned(combatants[DEFENDER]) && !vampire(combatants[DEFENDER]) && combatants[DEFENDER]->level[cont] <= 2)
				++combatants[DEFENDER]->attacks_lost[cont];
		}
	}
	else
		add_battles(combatants[DEFENDER]);
	for(count = ATTACKER; count <= ATTACKER_ALLY; ++count)
		if(combatants[count])
		{
			add_balance(won == ATTACKER?1:-1, combatants[count]);
			add_casualties(DEFENDER, count, combatants[count]);
		}
	for(count = DEFENDER; count <= DEFENDER_ALLY; ++count)
		if(combatants[count])
		{
			add_balance(won == DEFENDER?1:-1, combatants[count]);
			add_casualties(ATTACKER, count, combatants[count]);
		}
	return(won);
}

void battle_end(player *loser, player *winner, int num_castles, int round, int troops)
{
	int casualties[MAXCBT];
	int cont;
	int count;

	cont = combatants[ATTACKER]->cont;
	for(count = 0; count < MAXCBT; ++count)
		casualties[count] = count_casualties(count);
	sprintf(message, "2026%c%c", MSGDEL, MSGDEL);
	send_message(message, winner->name, 1);
	sprintf(message, "2027%c%s%c%s%c%s%c%s%c", MSGDEL, conts[winner->cont].name, MSGDEL, select_class(winner->classnum), MSGDEL, select_title(winner->classnum, winner->level[cont], winner->gender), MSGDEL, winner->nick, MSGDEL);
	send_message(message, loser->name, 1);
	if(winner == combatants[ATTACKER])
	{
		log_event("", message, loser->name);
		strcpy(message, "");
		if(casualties[ATTACKER] > casualties[DEFENDER])
			sprintf(message, "2028%c%s%c", MSGDEL, winner->nick, MSGDEL);
		else
			if(casualties[DEFENDER] > casualties[ATTACKER] * 2)
				sprintf(message, "2029%c%s%c", MSGDEL, loser->nick, MSGDEL);
		if(strlen(message) && fog_of_war == 0)
			send_channel(message, 0);
		sprintf(message, "2030%c%d%c%s%c%s%c", MSGDEL, round, MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL);
	}
	else
	{
		sprintf(message, "2031%c%s%c%s%c%s%c%s%c", MSGDEL, winner->kname[cont], MSGDEL, select_class(loser->classnum), MSGDEL, select_title(loser->classnum, loser->level[cont], loser->gender), MSGDEL, loser->nick, MSGDEL);
		log_event("", message, winner->name);
		strcpy(message, "");
		if(casualties[DEFENDER] > casualties[ATTACKER])
			sprintf(message, "2032%c%s%c", MSGDEL, winner->nick, MSGDEL);
		else
			if(casualties[ATTACKER] > casualties[DEFENDER] * 2)
				sprintf(message, "2033%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL);
		if(strlen(message) && fog_of_war == 0)
			send_channel(message, 0);
		sprintf(message, "2034%c%d%c%s%c%s%c%s%c", MSGDEL, round, MSGDEL, winner->nick, MSGDEL, winner->kname[cont], MSGDEL, loser->nick, MSGDEL);
	}
	if(fog_of_war == 0)
		send_channel(message, 0);
	for(count = 0; count < MAXCBT; ++count)
		if(combatants[count] && (casualties[count] || totalcasualties[count][CATAPULTNUM] > 0))
			show_casualties(count, count <= ATTACKER_HORDE2?ARMY_CAMPAIGN:ARMY_DEFENSE, combatants[count]);
	take_assets(cont, num_castles, troops, loser, winner);
	adjust_honor(cont, loser, winner);
	enlist_ren(cont, loser);
	check_nightblade(cont, loser, winner);
	reset_combatants();
	sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
	if(!summoned(winner))
		check_steward(winner->cont, STEWARD_WIN, loser, winner);
	if(!summoned(loser))
		check_steward(loser->cont, STEWARD_LOSE, winner, loser);
	if(winner->special_misc & SPC_QUESTCHAR)
		refresh_army(winner->cont, winner);
	if(loser->special_misc & SPC_QUESTCHAR)
		refresh_army(loser->cont, loser);
}

void capture_castle(int cont, int land, int num_castles, player *loser, player *winner)
{
	char winmsg[512] = {0};
	int capture = 0;
	int count;
	int ctotal = 0;
	int scopper = 0;

	if(num_castles < 1)
		return;
	loser->castle[cont] -= num_castles;
	if(capture_foreign == 0 || BASE_CLASSNUM(winner->classnum) == BASE_CLASSNUM(loser->classnum))
	{
		for(count = 0; count < num_castles; ++count)
			if(winner->land[cont] + land >= (winner->castle[cont] + 1) * structures[BASE_CLASSNUM(winner->classnum)].land)
			{
				++ctotal;
				++winner->castle[cont];
				capture = 1;
			}
	}
	else
	{
		ctotal = num_castles;
		scopper = num_castles * structures[BASE_CLASSNUM(loser->classnum)].cost / 2;
		winner->copper[cont] += scopper;
		capture = 2;
	}
	if(loser->castle[cont] == 0)
	{
		if(capture == 1)
			sprintf(message, "2048%c%s%c%s%c%s%c%s%c", MSGDEL, loser->nick, MSGDEL, structures[BASE_CLASSNUM(loser->classnum)].name, MSGDEL, conts[cont].name, MSGDEL, winner->nick, MSGDEL);
		else
			sprintf(message, "2049%c%s%c%s%c%s%c%s%c", MSGDEL, loser->nick, MSGDEL, structures[BASE_CLASSNUM(loser->classnum)].name, MSGDEL, conts[cont].name, MSGDEL, winner->nick, MSGDEL);
		send_channel(message, 0);
		parse_winmsg(winmsg, loser, winner);
		sprintf(message, "2050%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, winmsg, MSGDEL);
		send_channel(message, 0);
		if(strlen(winner->winsound) > 0)
		{
			sprintf(message, "999%c%cSOUND :%s%c", MSGDEL, 1, winner->winsound, 1);
			send_channel(message, 0);
		}
		if(loser->kingdoms == 1 && fog_of_war == 0)
		{
			sprintf(message, "2051%c%s%c%s%c", MSGDEL, loser->nick, MSGDEL, winner->nick, MSGDEL);
			send_channel(message, 0);
		}
		kia_change(cont, loser->nick, winner->nick);
		save_info();
		return;
	}
	if(capture)
		sprintf(message, "2052%c%d%c%s%c", MSGDEL, ctotal, MSGDEL, structures[BASE_CLASSNUM(loser->classnum)].name, MSGDEL);
	else
		sprintf(message, "2053%c%d%c%s%c", MSGDEL, num_castles, MSGDEL, structures[BASE_CLASSNUM(loser->classnum)].name, MSGDEL);
	send_message(message, winner->name, 1);
	if(capture == 2)
	{
		sprintf(message, "2088%c%d%c%s%c%d%c%s%c", MSGDEL, num_castles, MSGDEL, structures[BASE_CLASSNUM(loser->classnum)].name, MSGDEL, scopper, MSGDEL, money_name, MSGDEL);
		send_message(message, winner->name, 1);
	}
}

int castles_captured(int cont, player *plyptr)
{
	int total = 0;

	if(plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] > 0)
		++total;
	if(plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] >= 2 && plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] < 5 && randomnum(100, 1) <= 20)
		++total;
	if(plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] >= 5 && plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] < 11 && randomnum(100, 1) <= 35)
		++total;
	if(plyptr->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] >= 11 && randomnum(100, 1) <= 50)
		++total;
	if(check_hero(HERO_GIANT, plyptr))
		++total;
	return(total);
}

void check_ally(int cont, int num, player *plyptr)
{
	int checknum;

	if(!plyptr || summoned(plyptr))
		return;
	if(plyptr->ally < 1 || players[plyptr->ally].level[cont] < 0)
		return;
	if(num == ATTACKER)
		checknum = DEFENDER;
	else
		checknum = ATTACKER;
	if(is_vassal(combatants[checknum], &players[plyptr->ally]))
		return;
	combatants[num + 1] = &players[plyptr->ally];
	if(summoned(combatants[num + 1]))
		return;
	if(plyptr == combatants[ATTACKER])
		sprintf(message, "2054%c%s%c%s%c", MSGDEL, combatants[checknum]->nick, MSGDEL, conts[cont].name, MSGDEL);
	else
		sprintf(message, "2055%c%s%c%s%c", MSGDEL, combatants[checknum]->nick, MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, combatants[num + 1]->name, 0);
}

void check_batmod(int cont, player *plyptr, unit *mod)
{
	int count;
	int cumattack = 0;
	int cumdefend = 0;

	if(plyptr->troop_type == VAMPIRE || has_item(ITEM_WART, plyptr))
	{
		check_weathermod(cont, plyptr, mod);
		return;
	}
	if(fighter(plyptr))
		for(count = 0; count < CATAPULTNUM; ++count)
		{
			if(plyptr->level[cont] >= 1)
				adjust_unit(0, 0, 1, 0, 0, 0, 0, 0, count, plyptr, mod);
			if(plyptr->level[cont] >= 3)
				adjust_unit(0, 1, 0, 0, 0, 0, 0, 0, count, plyptr, mod);
			if(plyptr->level[cont] >= 5)
				adjust_unit(0, 0, 0, 0, 0, 0, 0, 1, count, plyptr, mod);
		}
	if(!summoned(plyptr) && (plyptr == combatants[DEFENDER] || plyptr == combatants[DEFENDER_ALLY]))
		++cumdefend;
	if(plyptr->special_batmod & spcbless[cont])
	{
		++cumattack;
		++cumdefend;
	}
	if(plyptr->special_batmod & spcshd[cont])
	{
		if(plyptr == combatants[ATTACKER] || plyptr == combatants[ATTACKER_ALLY])
			cumattack -= 2;
		else
		{
			cumattack += 2;
			cumdefend += 3;
		}
	}
	if(plyptr->special_batmod & spcstr[cont])
		cumattack += 2;
	if(plyptr->cont == cont)
	{
		if(check_hero(HERO_ALCHEMIST, plyptr))
			for(count = 0; count < MAXTRP; ++count)
				if(randomnum(100, 1) <= 50)
					adjust_unit(0, 0, 0, 1, 0, 0, 0, 0, count, plyptr, mod);
		if(check_hero(HERO_FARMER, plyptr))
			adjust_unit(0, 1, 1, 0, 0, 0, 0, 0, RENNUM, plyptr, mod);
		if(check_hero(HERO_GENERAL1, plyptr))
			adjust_unit(0, 0, 2, 0, 0, 0, 0, 0, BLOODGUARDNUM, plyptr, mod);
		if(check_hero(HERO_GENERAL2, plyptr))
			adjust_unit(0, 2, 0, 0, 0, 0, 0, 0, BLOODGUARDNUM, plyptr, mod);
		if(check_hero(HERO_GIANT, plyptr))
			adjust_unit(0, 1, 0, 1, 0, 0, 0, 0, CATAPULTNUM, plyptr, mod);
		if(check_hero(HERO_WKNIGHT, plyptr))
			adjust_unit(0, 1, 1, 0, 0, 0, 0, 0, KNIGHTNUM, plyptr, mod);
		if(check_hero(HERO_TITAN, plyptr))
			for(count = 0; count < MAXTRP; ++count)
				adjust_unit(0, 5, 5, 0, 0, 0, 1, 0, count, plyptr, mod);
	}
	if(check_artifact(SWORD, plyptr))
		cumattack += 2;
	if(check_artifact(BOOTS, plyptr))
		for(count = 0; count <= CATAPULTNUM; ++count)
			adjust_unit(0, 0, 0, 1, 0, 0, 0, 0, count, plyptr, mod);
	if(check_artifact(BOW, plyptr))
		for(count = 0; count < CATAPULTNUM; ++count)
			if(units[plyptr->troop_type][count].range == 1)
				adjust_unit(0, 2, 0, 0, 0, 0, 0, 0, count, plyptr, mod);
	if(check_artifact(WINGS, plyptr))
		for(count = 0; count < CATAPULTNUM; ++count)
			adjust_unit(1, 0, 0, 0, 0, 0, 0, 0, count, plyptr, mod);
	if(check_artifact(CROWN, plyptr))
		for(count = 0; count < CATAPULTNUM; ++count)
			if(units[plyptr->troop_type][count].num_attacks > 1)
				adjust_unit(0, 2, 1, 0, 0, 0, 0, 0, count, plyptr, mod);
	if(check_artifact(DSHIELD, plyptr))
		cumdefend += 2;
	if(check_artifact(GRAIL, plyptr) && !barbarian(plyptr))
		for(count = 0; count < CATAPULTNUM; ++count)
			adjust_unit(0, 0, 0, 0, 0, 1, 0, 0, count, plyptr, mod);
	if(check_artifact(LANCE, plyptr) && !barbarian(plyptr))
		++cumattack;
	if(fighter(plyptr))
		if(check_artifact(SWORD, plyptr))
		{
			cumattack += 2;
			if(plyptr->cont == cont && check_hero(HERO_WKNIGHT, plyptr))
			{
				++cumattack;
				++cumdefend;
			}
		}
	if(barbarian(plyptr))
		if(check_artifact(HAMMER, plyptr))
		{
			cumattack += 2;
			if(plyptr->cont == cont && check_hero(HERO_BARBLEADER, plyptr))
			{
				++cumattack;
				++cumdefend;
			}
		}
	if(mage(plyptr))
		if(check_artifact(STAFF, plyptr))
		{
			cumattack += 2;
			if(plyptr->cont == cont && check_hero(HERO_WARLOCK, plyptr))
			{
				++cumattack;
				++cumdefend;
			}
		}
	if(cleric(plyptr))
		if(check_artifact(GRAIL, plyptr))
		{
			cumdefend += 2;
			if(plyptr->cont == cont && check_hero(HERO_PRIESTESS, plyptr))
			{
				++cumattack;
				++cumdefend;
			}
		}
	if(ranger(plyptr))
		if(check_artifact(BOW, plyptr))
		{
			cumattack += 2;
			if(plyptr->cont == cont && check_hero(HERO_HIGHLORD, plyptr))
			{
				++cumattack;
				++cumdefend;
			}
		}
	if(has_item(ITEM_BANNER, plyptr))
		for(count = MERC; count < MAXTRP; ++count)
			adjust_unit(0, 1, 1, 0, 0, 0, 0, 0, count, plyptr, mod);
	if(has_item(ITEM_BLADE, plyptr))
		adjust_unit(0, 1, 1, 0, 0, 0, 0, 0, SOLDIERNUM, plyptr, mod);
	if(has_item(ITEM_GAUNTLETS, plyptr))
	{
		--cumattack;
		--cumdefend;
	}
	if(!barbarian(plyptr))
	{
		if(plyptr->honor[cont] < 0)
		{
			--cumattack;
			--cumdefend;
		}
		else
		{
			if(plyptr->honor[cont] > (plyptr->level[cont] + 1) * 5)
			{
				++cumattack;
				++cumdefend;
			}
			if(plyptr->honor[cont] > (plyptr->level[cont] + 1) * 10)
			{
				++cumattack;
				++cumdefend;
			}
		}
	}
	for(count = 0; count < CATAPULTNUM; ++count)
		adjust_unit(0, cumattack, cumdefend, 0, 0, 0, 0, 0, count, plyptr, mod);
	check_weathermod(cont, plyptr, mod);
}

void check_catapult(int cont, int num, int type, player *plyptr)
{
	plyptr->army[type][cont].total[SOLDIERNUM] -= plyptr->army[type][cont].total[CATAPULTNUM] * 5;
	while(plyptr->army[type][cont].total[SOLDIERNUM] < 0 && plyptr->army[type][cont].total[CATAPULTNUM] > 0)
	{
		plyptr->army[type][cont].total[SOLDIERNUM] += 5;
		++totalcasualties[num][CATAPULTNUM];
		--plyptr->army[type][cont].total[CATAPULTNUM];
	}
	plyptr->army[type][cont].total[SOLDIERNUM] += plyptr->army[type][cont].total[CATAPULTNUM] * 5;
}

void check_horde(int cont, int max, int num, int type, player *plyptr)
{
	int horde;
	player dummy;

	if(!plyptr || summoned(plyptr))
		return;
	for(horde = num + 2; horde < max; ++horde)
		if(!combatants[horde])
			break;
	if(horde == max)
		return;
	if(num == ATTACKER)
	{
		if(conq_info.cont_special[cont] & SPC_BLITZ)
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, HORDE, type, type, -1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
		if((barbarian(plyptr) && plyptr->level[plyptr->cont] > 2))
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, HORDE, type, type, check_artifact(HAMMER, plyptr)?MIGHTY:-1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
		if(check_artifact(LANCE, plyptr) && (cleric(plyptr) || mage(plyptr)))
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, CRUSADERS, type, type, -1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
	}
	else if(num == DEFENDER)
	{
		if(barbarian(plyptr))
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, HORDE, type, type, check_artifact(HAMMER, plyptr)?MIGHTY:-1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
		if(check_artifact(LANCE, plyptr) && (cleric(plyptr) || mage(plyptr)))
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, CRUSADERS, type, type, cleric(plyptr)?MIGHTY:-1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
		if((cleric(plyptr) && plyptr->level[combatants[ATTACKER]->cont] <= 2) && (barbarian(combatants[ATTACKER]) || vampire(combatants[ATTACKER])))
		{
			combatants[horde] = (void *)plybuf[horde];
			enemy(0, CRUSADERS, type, type, -1, combatants[horde++], plyptr);
			if(horde >= max)
				return;
		}
	}
	if(emperor(plyptr->name))
	{
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, IMMORTAL, type, type, MIGHTY, combatants[horde++], plyptr);
		if(horde >= max)
			return;
	}
	if(!barbarian(plyptr) && check_artifact(HAMMER, plyptr))
	{
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, HORDE, type, type, -1, combatants[horde++], plyptr);
		if(horde >= max)
			return;
	}
	if((vampire(plyptr)))
	{
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, UNDEAD, type, type, -1, combatants[horde++], plyptr);
		if(horde >= max)
			return;
	}
	if(plyptr->cont == cont && check_hero(HERO_BISHOP, plyptr))
	{
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, CRUSADERS, type, type, WEAK, combatants[horde++], plyptr);
		if(horde >= max)
			return;
	}
	if(plyptr->cont == cont && check_hero(HERO_HIGHLORD, plyptr))
	{
		combatants[horde] = (void *)plybuf[horde];
		summon_creature(0, randomnum(LION, WOLF), type, type, -1, combatants[horde++], plyptr);
		if(horde >= max)
			return;
	}
	if(plyptr->cont == cont && check_hero(HERO_MERCGENERAL, plyptr))
	{
		memset(&dummy, 0, PLYSIZ);
		dummy.army[type][plyptr->cont].total[MERC] = army_size(cont, type, plyptr);
		dummy.cont = cont;
		dummy.troop_type = plyptr->troop_type;
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, MERCENARY, type, type, -1, combatants[horde++], &dummy);
		if(horde >= max)
			return;
	}
	if(plyptr->cont == cont && check_hero(HERO_BARBLEADER, plyptr))
	{
		combatants[horde] = (void *)plybuf[horde];
		enemy(0, HORDE, type, type, check_artifact(HAMMER, plyptr)?MIGHTY:-1, combatants[horde++], plyptr);
	}
}

void check_nightblade(int cont, player *loser, player *winner)
{
	int amount;

	if(!check_artifact(NIGHTBLADE, winner) || loser->movepts < 1)
		return;
	amount = (int)(loser->movepts * .20);
	if(amount < 1)
		amount = 1;
	winner->movepts += amount;
	loser->movepts -= amount;
	sprintf(message, "2046%c%s%c%d%c", MSGDEL, artifacts[NIGHTBLADE].name, MSGDEL, amount, MSGDEL);
	send_message(message, winner->name, 1);
	sprintf(message, "2047%c%s%c%s%c%d%c", MSGDEL, artifacts[NIGHTBLADE].name, MSGDEL, winner->nick, MSGDEL, amount, MSGDEL);
	send_message(message, loser->name, 1);
	log_event("", message, loser->name);
}

void check_weathermod(int cont, player *plyptr, unit *mod)
{
	int count;
	unit modifiers[MAXTRP];

	find_weather(cont, plyptr->troop_type, modifiers);
	for(count = 0; count < MAXTRP; ++count)
	{
		adjust_unit(0, modifiers[count].attack, modifiers[count].defend, modifiers[count].num_attacks, modifiers[count].range, 0, 0, 0, count, plyptr, mod);
		if(units[plyptr->troop_type][count].attack + mod[count].attack < 0)
			mod[count].attack = -units[plyptr->troop_type][count].attack;
		if(units[plyptr->troop_type][count].defend + mod[count].defend < 1)
			mod[count].defend = -(units[plyptr->troop_type][count].defend - 1);
		if(units[plyptr->troop_type][count].num_attacks + mod[count].num_attacks < 1)
			mod[count].num_attacks = -(units[plyptr->troop_type][count].num_attacks - 1);
	}
	mod[CATAPULTNUM].defend = 0;
}

int combined_army(int cont, player *plyptr)
{
	return(army_size(cont, 0, plyptr) + army_size(cont, 1, plyptr));
}

void compute_casualties(int cont, int damage[], int misfires[], int num, int round, int type, player *plyptr)
{
	int attack;
	int attack_total;
	int frontline = -1;
	int min;
	int mod_attack;
	int nonfs = -1;
	int num_attacks;
	int ranknum;
	int rng;
	int troopnum;
	int unit;

	for(ranknum = 0; ranknum < MAXTRP; ++ranknum)
		for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
		{
			if((unit = plyptr->army[type][cont].formation[ranknum][troopnum]) < 0)
				continue;
			if(frontline == -1)
				frontline = ranknum;
			rng = units[plyptr->troop_type][unit].range + batmod[num][unit].range;
			if(units[plyptr->troop_type][unit].ambush + batmod[num][unit].ambush > 0 && round == 1)
				rng = 1;
			if(plyptr->army[type][cont].total[unit] > 0 && (rng > 0 || (round > 1 && (nonfs == -1 || nonfs == ranknum))))
			{
				mod_attack = units[plyptr->troop_type][unit].attack + batmod[num][unit].attack;
				num_attacks = units[plyptr->troop_type][unit].num_attacks + batmod[num][unit].num_attacks;
				if(units[plyptr->troop_type][unit].swarm + batmod[num][unit].swarm > 0 && plyptr->army[type][cont].total[unit] >= 1000 && round % 2 == 0)
				{
					++num_attacks;
					if(plyptr->army[type][cont].total[unit] >= 2000)
						++num_attacks;
					if(plyptr->army[type][cont].total[unit] >= 4000)
						++num_attacks;
				}
				if(rng == 0)
				{
					nonfs = ranknum;
					if(mod_attack < 1)
						min = 0;
					else
						min = 1;
				}
				else
					min = 0;
				for(attack = 0, attack_total = 0; attack < num_attacks; ++attack)
					attack_total += randomnum(mod_attack, min);
				if(rng == 0)
				{
					damage[units[plyptr->troop_type][unit].mode] += attack_total * plyptr->army[type][cont].total[unit];
					continue;
				}
				damage[MODE_RNG] += attack_total * plyptr->army[type][cont].total[unit];
				if(round < 2 || check_artifact(BOW, plyptr))
					continue;
				if((randomnum(100, 1)) <= (ranknum - (frontline >= 0?frontline:ranknum)) * 4)
				{
					misfires[num] += ((units[plyptr->troop_type][unit].attack + batmod[num][unit].attack) * num_attacks) * (randomnum(10, 1) * plyptr->army[type][cont].total[unit] / 100);
					damage[MODE_RNG] -= misfires[num];
				}
			}
		}
}

float compute_ratio(int troops)
{
        
	if(troops <= 500)
		return((float).05);
	if(troops <= 2000)
		return((float).10);
	if(troops <= 4000)
		return((float).12);
	if(troops <= 8000)
		return((float).14);
	if(troops <= 12000)
		return((float).16);
	return((float).20);
}

int count_casualties(int num)
{
	int count;
	int total = 0;

	for(count = 0; count < MAXTRP; ++count)
		if(count != CATAPULTNUM)
			total += totalcasualties[num][count];
	return(total);
}

int count_side_casualties(int start)
{
	int count;
	int total = 0;

	for(count = start; count < start + (MAXCBT / 2); ++count)
		if(combatants[count])
			total += count_casualties(count);
	return(total);
}

void damage_catapult(int cont, int num, int type, player *plyptr)
{
	if(randomnum(100, 1) <= 10 && plyptr->army[type][cont].total[CATAPULTNUM] > 0)
	{
		++totalcasualties[num][CATAPULTNUM];
		--plyptr->army[type][cont].total[CATAPULTNUM];
	}
}

void enlist_ren(int cont, player *loser)
{
	int ren;

	if(loser != combatants[DEFENDER] || !(loser->special_kingdom & spcenlist[cont]))
		return;
	ren = (int)(loser->peasant[cont] * .20);
	if((ren = fill_castles(ren, cont, loser)) <= 0)
		return;
	loser->army[ARMY_DEFENSE][cont].total[RENNUM] += ren;
	loser->peasant[cont] -= ren;
	sprintf(message, "2045%c%d%c%s%c%s%c", MSGDEL, ren, MSGDEL, units[loser->troop_type][RENNUM].name, MSGDEL, loser->kname[cont], MSGDEL);
	send_message(message, loser->name, 1);
	log_event("", message, loser->name);
}

void lose_kingdom(int cont, player *loser, player *winner)
{
	int count;
	int dest = 0;
	int exper = 0;
	int minlevel = 10;

	check_bounty(winner->cont, playernum(loser), winner);
	take_ship(loser, winner);
	if(loser->kingdoms == 1)
	{
		take_artifact(loser, winner);
		take_items(loser, winner);
		if((vampire(loser) && !cleric(winner) && !check_artifact(GRAIL, winner)) || conq_info.vampire == playernum(loser))
			vampire_death(cont, winner, loser);
		else
		{
			if(!create_vampire(cont, loser, winner))
			{
				if((cleric(winner) || check_artifact(GRAIL, winner)) && vampire(loser))
				{
					sprintf(message, "17043%c%s%c%s%c", MSGDEL, loser->name, MSGDEL, select_classname(VAMPIRE), MSGDEL);
					send_message(message, winner->name, 1);
					sprintf(message, "17044%c%c", MSGDEL, MSGDEL);
					send_message(message, loser->name, 1);
				}
				exper = loser->remorts * 50000;
				delete_player(playernum(loser));
			}
		}
		if(exper > 0)
		{
			winner->copper[cont] += exper;
			sprintf(message, "2044%c%s%c%d%c%s%c", MSGDEL, select_title(winner->classnum, maxlevel[BASE_CLASSNUM(winner->classnum)], 0), MSGDEL, exper, MSGDEL, money_name, MSGDEL);
			send_message(message, winner->name, 1);
		}
		return;
	}
	--totlvl[cont][loser->level[cont]];
	loser->level[cont] = -1;
	if(loser->special_kingdom & spctax[cont])
		loser->special_kingdom -= spctax[cont];
	for(count = STEWARD_HK; count <= STEWARD_WIN; ++count)
		memset(loser->steward[cont][count], 0, 128);
	--loser->kingdoms;
	update_levelmax(loser);
	for(count = 0; count < num_conts; ++count)
		if(loser->level[count] >= 0 && loser->level[count] < minlevel)
		{
			dest = count;
			minlevel = loser->level[count];
		}
	loser->cont = dest;
	loser->city = conts[loser->cont].port;
	if(fog_of_war == 0)
	{
		sprintf(message, "2040%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(winner->classnum), MSGDEL, winner->nick, MSGDEL, loser->kname[cont], MSGDEL, conts[cont].name, MSGDEL, loser->nick, MSGDEL, winner->nick, MSGDEL);
		send_channel(message, 0);
	}
	sprintf(message, "2041%c%s%c%s%c%s%c", MSGDEL, loser->kname[cont], MSGDEL, conts[cont].name, MSGDEL, winner->nick, MSGDEL);
	send_message(message, loser->name, 1);
	log_event("", message, loser->name);
	if(loser->validate == VAL_NORMAL)
		if((demote(dest, loser)) == 0)
			promote(dest, loser);
	check_vassals(loser);
}

void parse_winmsg(char *winmsg, player *loser, player *winner)
{
	char list[][5] = 
	{
		"%DN", "%DC", "%DG1", "%DG2", "%DT", "%DS", ""
	};
	char newmsg[512] = {0};
	char *ptr;
	int count = 0;
	int len;

	strncpy(newmsg, winner->winmsg, 256);
	strncpy(winmsg, winner->winmsg, 256);
	while(strlen(list[count]) > 0)
	{
		while((ptr = strstr(winmsg, list[count])) != NULL)
		{
			len = ptr - winmsg;
			strncpy(newmsg, winmsg, len);
			newmsg[len] = 0;
			switch(count)
			{
				case 0:
					strcat(newmsg, loser->nick);
					break;
				case 1:
					strcat(newmsg, select_classname(BASE_CLASSNUM(loser->classnum)));
					break;
				case 2:
					strcat(newmsg, loser->gender == 0?"he":"she");
					break;
				case 3:
					strcat(newmsg, loser->gender == 0?"his":"her");
					break;
				case 4:
					strcat(newmsg, select_title(loser->classnum, loser->level[loser->cont], loser->gender));
					break;
				case 5:
					strcat(newmsg, select_class(loser->classnum));
					break;
				default:
					break;
			}
			strcat(newmsg, winmsg + len + strlen(list[count]));
			strcpy(winmsg, newmsg);
			if(strlen(winmsg) > 479)
				return;
		}
		++count;
	}
}

void refresh_army(int cont, player *plyptr)
{
	int total;

	total = plyptr->castle[cont] * castle_max(plyptr->classnum);
	if((total = fill_castles(total, cont, plyptr)) <= 0)
		return;
	plyptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] += (int)(total * .20);
	if(units[plyptr->troop_type][ARCHERNUM].attack > 0 && units[plyptr->troop_type][ARCHERNUM].defend > 0)
		plyptr->army[ARMY_DEFENSE][cont].total[ARCHERNUM] += (int)(total * .40);
	else
		plyptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] += (int)(total * .40);
	if(units[plyptr->troop_type][KNIGHTNUM].attack > 0 && units[plyptr->troop_type][KNIGHTNUM].defend > 0)
		plyptr->army[ARMY_DEFENSE][cont].total[KNIGHTNUM] += (int)(total * .40);
	else
		plyptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] += (int)(total * .40);
}

void reset_casualties()
{
	int cnt;
	int count;

	for(count = 0; count < MAXCBT; ++count)
		for(cnt = 0; cnt < MAXTRP; ++cnt)
			totalcasualties[count][cnt] = 0;
}

void reset_combatants()
{
	int count;

	for(count = 0; count < MAXCBT; ++count)
		combatants[count] = NULL;
}

void show_allies(int start, int type, player *attacker, player *defender)
{
	int count;

	if(combatants[start + 1])
	{
		sprintf(message, "2056%c%s%c%s%c", MSGDEL, attacker->nick, MSGDEL, combatants[start + 1]->nick, MSGDEL);
		send_message(message, defender->name, 1);
		review(defender->name, defender->special_misc & SPC_COLOR, attacker->cont, REVIEW_ALL, type, combatants[start + 1], combatants[start + 1]);
	}
	for(count = start + 2; count < start + (MAXCBT / 2); ++count)
		if(combatants[count])
		{
			sprintf(message, "2057%c%s%c%s%c%s%c", MSGDEL, select_title(combatants[count]->classnum, combatants[count]->level[attacker->cont], combatants[count]->gender), MSGDEL, combatants[count]->nick, MSGDEL, attacker->nick, MSGDEL);
			send_channel(message, 0);
			send_message(message, defender->name, 1);
			review(defender->name, defender->special_misc & SPC_COLOR, attacker->cont, REVIEW_ALL, type, combatants[count], combatants[count]);
		}
}

void show_casualties(int num, int type, player *plyptr)
{
	int count;
	player dummy;

	if(plyptr->validate != VAL_NORMAL)
		return;
	dummy.troop_type = plyptr->troop_type;
	sprintf(message, "2074%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXTRP; ++count)
		dummy.army[type][plyptr->cont].total[count] = totalcasualties[num][count];
	review(plyptr->name, plyptr->special_misc & SPC_COLOR, plyptr->cont, REVIEW_ALL, type, &dummy, plyptr);
}

void show_formation(char *name, int cont, int type, player *plyptr)
{
	char str[256] = {0};
	int found;
	int ranknum;
	int troopnum;

	if(allow_formation == 0)
		return;
	if((strcmp(name, plyptr->name)) == 0)
		sprintf(message, "4317%c%s%c%s%c", MSGDEL, army_names[type], MSGDEL, conts[cont].name, MSGDEL);
	else
		sprintf(message, "2076%c", MSGDEL);
	for(ranknum = 0; ranknum < MAXTRP; ++ranknum)
	{
		for(found = 0, troopnum = 0; troopnum < MAXTRP; ++troopnum)
			if(plyptr->army[type][cont].formation[ranknum][troopnum] >= 0)
			{
				if(!found)
				{
					found = 1;
					sprintf(str, "%s[", str);
				}
				sprintf(str, "%s %s", str, units[plyptr->troop_type][plyptr->army[type][cont].formation[ranknum][troopnum]].name);
			}
		if(found)
			sprintf(str, "%s ]", str);
	}
	sprintf(message, "%s%s%c", message, str, MSGDEL);
	send_message(message, name, 1);
}

void show_surrender(char *name, player *plyptr)
{
	int surr;

	if(summoned(plyptr))
		return;
	surr = (int)(plyptr->surrender[plyptr->cont] * 100);
	sprintf(message, "2058%c%s%c%d%c", MSGDEL, plyptr->nick, MSGDEL, surr < 1?1:surr, MSGDEL);
	send_message(message, name, 1);
}

void start_battle()
{
	int num_castles = 0;
	int outcome;
	int round = 1;
	int total_attack;
	int total_defend;

	if((outcome = battle(&num_castles, &round, &total_attack, &total_defend)) == ATTACKER)
		battle_end(combatants[DEFENDER], combatants[ATTACKER], num_castles, round, total_defend);
	else
		battle_end(combatants[ATTACKER], combatants[DEFENDER], num_castles, round, total_attack);
}

void subtract_casualties(int cont, int *damage, int num, int round, int type, player *plyptr)
{
	int count;
	int division;
	int rankdmg[MAXADV];
	int ranknum;
	int trample[MAXADV] = {0, 0, 0, 0};
	int troopnum;
	int unit;

	if(army_size(cont, type, plyptr) < 1)
		return;
	if(damage[MODE_WALK] < 1 && damage[MODE_RIDE] < 1 && damage[MODE_FLY] < 1 && damage[MODE_RNG] < 1)
		return;
	memcpy(rankdmg, damage, MAXADV * sizeof(int));
	for(ranknum = 0; ranknum < MAXTRP; ++ranknum)
	{
		for(division = 0, troopnum = 0; troopnum < MAXTRP; ++troopnum)
			if(((unit = plyptr->army[type][cont].formation[ranknum][troopnum]) >= 0) && unit != CATAPULTNUM && plyptr->army[type][cont].total[unit] > 0)
				++division;
		if(division < 1)
			continue;
		for(count = 0; count < MAXADV; ++count)
			rankdmg[count] /= division;
		for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
		{
			if(((unit = plyptr->army[type][cont].formation[ranknum][troopnum]) < 0) || unit == CATAPULTNUM || plyptr->army[type][cont].total[unit] < 1)
				continue;
			for(count = 0; count < MAXADV; ++count)
				trample[count] += subtract_troop(cont, rankdmg[count], count, num, round, type, unit, plyptr);
			if(unit == SOLDIERNUM)
				check_catapult(cont, num, type, plyptr);
		}
		if(trample[MODE_WALK] < 1 && trample[MODE_RIDE] < 1 && trample[MODE_FLY] < 1 && trample[MODE_RNG] < 1)
			break;
		for(count = 0; count < MAXADV; ++count)
		{
			rankdmg[count] = trample[count];
			trample[count] = 0;
		}
	}
}

int subtract_troop(int cont, int damage, int mode, int num, int round, int type, int unit, player *plyptr)
{
	int adjd;
	int losses;
	int totald;
	int trample = 0;

	adjd = units[plyptr->troop_type][unit].defend + batmod[num][unit].defend;
	if(advanced_combat == 1)
		switch(mode)
		{
			case MODE_WALK:
				if(units[plyptr->troop_type][unit].mode == MODE_RIDE)
					damage -= damage * 20 / 100;
				if(units[plyptr->troop_type][unit].mode == MODE_FLY)
					damage -= damage * 40 / 100;
				break;
			case MODE_RIDE:
				if(units[plyptr->troop_type][unit].mode == MODE_WALK)
					damage += damage * 20 / 100;
				if(units[plyptr->troop_type][unit].mode == MODE_FLY)
					damage -= damage * 40 / 100;
				break;
			case MODE_FLY:
				if(units[plyptr->troop_type][unit].mode == MODE_WALK)
					damage += damage * 10 / 100;
				if(units[plyptr->troop_type][unit].mode == MODE_RIDE)
					damage += damage * 30 / 100;
				break;
			case MODE_RNG:
				if(units[plyptr->troop_type][unit].mode == MODE_RIDE)
					damage -= damage * 10 / 100;
				if(units[plyptr->troop_type][unit].mode == MODE_FLY)
					damage += damage * 50 / 100;
				break;
		}
	if(units[plyptr->troop_type][unit].regenerate + batmod[num][unit].regenerate > 0)
		damage -= damage * randomnum(25, 10) / 100;
	if(units[plyptr->troop_type][unit].shield + batmod[num][unit].shield > 0 && mode == MODE_RNG)
		damage -= damage * randomnum(40, 20) / 100;
	if(damage < adjd)
		return(0);
	totald = plyptr->army[type][cont].total[unit] * adjd - damage;
	losses = totald / adjd;
	if(totald % adjd != 0)
		++losses;
	if(losses >= 0)
	{
		totalcasualties[num][unit] += plyptr->army[type][cont].total[unit] - losses;
		plyptr->army[type][cont].total[unit] = losses;
	}
	else
	{
		if(damage > plyptr->army[type][cont].total[unit] * adjd * 4)
			trample = abs(totald);
		totalcasualties[num][unit] += plyptr->army[type][cont].total[unit];
		plyptr->army[type][cont].total[unit] = 0;
	}
	return(trample);
}

void take_artifact(player *loser, player *winner)
{
	int anum;
	int dnum;

	if((dnum = find_artifact(loser)) >= 0)
	{
		anum = find_artifact(winner);
		if(summoned(winner) || (anum >= 0 && dnum != ROD) || (vampire(winner) && dnum == GRAIL) || ((winner->levelmax >= maxlevel[BASE_CLASSNUM(winner->classnum)] - 1) && dnum != ROD 
		&& (!mage(winner) || dnum != STAFF)))
		{
			artifacts[dnum].owner = 0;
			sprintf(message, "2059%c%s%c", MSGDEL, artifacts[dnum].name, MSGDEL);
		}
		else
		{
			if(anum >= 0)
			{
				artifacts[anum].owner = 0;
				save_art();
			}
			artifacts[dnum].owner = playernum(winner);
			sprintf(message, "2060%c%s%c", MSGDEL, artifacts[dnum].name, MSGDEL);
		}
		send_message(message, winner->name, 1);
		save_art();
	}
}

void take_assets(int cont, int num_castles, int troops, player *loser, player *winner)
{
	int amount;
	int copper = 0;
	int food = 0;
	int land = 0;
	int lcopper = 0;
	int lfood = 0;
	int lland = 0;
	int lpeasant = 0;
	int peasant = 0;
	float pct = 0;

	if(winner == combatants[ATTACKER])
	{
		if(winner->army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] < 1)
		{
			if(check_hero(HERO_GIANT, winner))
				num_castles = 1;
			else
				num_castles = 0;
		}
		num_castles += lifedrain(cont, loser, winner);
		if(num_castles > loser->castle[cont])
			num_castles = loser->castle[cont];
		if(num_castles > 0)
			land = loser->land[cont] / loser->castle[cont] * num_castles;
		else
			land = loser->land[cont] / loser->castle[cont] - structures[BASE_CLASSNUM(loser->classnum)].land;
	}
	else
		land = loser->land[cont] / loser->castle[cont] - structures[BASE_CLASSNUM(loser->classnum)].land;
	if(land < 0)
		land = 0;
	lland = land;
	if(winner == combatants[ATTACKER])
		capture_castle(cont, land, num_castles, loser, winner);
	if(loser->land[cont] > 0)
		pct = (float)((float)land / (float)loser->land[cont]);
	copper = lcopper = (int)(loser->copper[cont] * (pct + (check_hero(HERO_NINJA, winner)?(float).05:(float)0)));
	food = lfood = (int)(loser->food[cont] * pct);
	peasant = lpeasant = (int)(loser->peasant[cont] * pct);
	if(check_hero(HERO_LORDCHAOS, winner) && peasant > 0)
	{
		amount = (int)(peasant * .20);
		peasant -= amount;
		sprintf(message, "2038%c%s%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_LORDCHAOS].name, MSGDEL, heroes[HERO_LORDCHAOS].type, MSGDEL, amount, MSGDEL, peasant_name, MSGDEL);
		send_message(message, winner->name, 1);
	}
	if(check_hero(HERO_MERCGENERAL, winner) && copper > 0)
	{
		amount = (int)(copper * .25);
		copper -= amount;
		sprintf(message, "2039%c%s%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_MERCGENERAL].name, MSGDEL, heroes[HERO_MERCGENERAL].type, MSGDEL, amount, MSGDEL, money_name, MSGDEL);
		send_message(message, winner->name, 0);
	}
	if(check_hero(HERO_DRAGON, winner) && copper > 0)
	{
		amount = (int)(copper * .05);
		copper -= amount;
		sprintf(message, "2039%c%s%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_DRAGON].name, MSGDEL, heroes[HERO_DRAGON].type, MSGDEL, amount, MSGDEL, money_name, MSGDEL);
		send_message(message, winner->name, 0);
	}
	if(check_hero(HERO_MERCHANTKING, winner) && troops > 1)
	{
		amount = randomnum(troops, troops / 2) * 10;
		winner->copper[cont] += amount;
		sprintf(message, "2087%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_MERCHANTKING].name, MSGDEL, amount, MSGDEL, money_name, MSGDEL);
		send_message(message, winner->name, 0);
	}
	vassal_postbattle(&copper, &food, &land, &peasant, loser, winner);
	winner->copper[cont] += copper;
	winner->food[cont] += food;
	winner->land[cont] += land;
	winner->peasant[cont] += peasant;
	loser->copper[cont] -= lcopper;
	loser->food[cont] -= lfood;
	loser->land[cont] -= lland;
	loser->peasant[cont] -= lpeasant;
	sprintf(message, "2042%c%d%c%d%c%s%c", MSGDEL, land, MSGDEL, copper, MSGDEL, money_name, MSGDEL);
	send_message(message, winner->name, 1);
	sprintf(message, "2043%c%d%c%s%c%s%c%d%c%s%c", MSGDEL, peasant, MSGDEL, peasant_name, MSGDEL, winner->kname[cont], MSGDEL, food, MSGDEL, food_name, MSGDEL);
	send_message(message, winner->name, 1);
	if(loser->castle[cont] <= 0)
		lose_kingdom(cont, loser, winner);
	else
	{
		check_ret(cont, combatants[ATTACKER], combatants[DEFENDER]);
		update_ret(cont, combatants[ATTACKER], combatants[DEFENDER]);
	}
}

void take_items(player *loser, player *winner)
{
	int count;
	int total;

	if(loser->items[0] < 1)
		return;
	for(count = 1, total = 0; count < TOTITM; ++count)
	{
		if(has_item(count, loser) && !has_item(count, winner))
		{
			if(!check_artifact(ORB, winner) && winner->city != antimagic_city)
			{
				++winner->items[0];
				winner->items[count] = loser->items[count];
			}
			remove_item(count, loser);
			++total;
		}
	}
	if(total == 0)
		return;
	sprintf(message, "2061%c%s%c%d%c", MSGDEL, loser->nick, MSGDEL, total, MSGDEL);
	send_message(message, winner->name, 1);
	if(check_artifact(ORB, winner) || winner->city == antimagic_city)
	{
		sprintf(message, "2062%c%s%c", MSGDEL, check_artifact(ORB, winner)?artifacts[ORB].name:cities[winner->city].name, MSGDEL);
		send_message(message, winner->name, 1);
	}
}

void take_ship(player *loser, player *winner)
{
	int newshp;
	int shpnum;

	if((shpnum = find_randomshpnum(ANYSHIP, loser)) < 0)
		return;
	if(winner->fleet == MAXFLT && fog_of_war == 0)
	{
		sprintf(message, "2064%c%s%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL, shiptypes[loser->ships[shpnum].type].name, MSGDEL);
		send_channel(message, 0);
	}
	else
		if((newshp = find_shpnum(winner)) >= 0)
		{
			++winner->fleet;
			strcpy(winner->ships[newshp].name, loser->ships[shpnum].name);
			winner->ships[newshp].type = loser->ships[shpnum].type;
			winner->ships[newshp].hull = loser->ships[shpnum].hull;
			winner->ships[newshp].cargo = loser->ships[shpnum].cargo;
			winner->ships[newshp].cargotype = loser->ships[shpnum].cargotype;
			if(fog_of_war == 0)
			{
				sprintf(message, "2063%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL, shiptypes[loser->ships[shpnum].type].name, MSGDEL, winner->nick, MSGDEL, loser->nick, MSGDEL, loser->ships[shpnum].name, MSGDEL);
				send_channel(message, 0);
			}
		}
	remove_ship(shpnum, loser);
}

void vassal_aid(int cont, int type, player *enemy, player *plyptr)
{
	int amount;
	int count;
	int plynum;
	int total;
	int vtotal;
	player *ptr;

	if(plyptr->lord > 0 && players[plyptr->lord].special_kingdom & spcaid[cont])
	{
		ptr = &players[plyptr->lord];
		for(count = 0, total = 0; count < MAXTRP; ++count)
		{
			if((amount = ptr->army[type][cont].total[count] * 2 / 100) < 1)
				continue;
			if((amount = fill_castles(amount, cont, plyptr)) <= 0)
				break;
			total += amount;
			ptr->army[type][cont].total[count] -= amount;
			plyptr->army[type][cont].total[count] += amount;
		}
		if(total > 0)
		{
			sprintf(message, "2081%c%s%c%s%c%d%c%s%c", MSGDEL, adjectives[ptr->gender][4], MSGDEL, ptr->nick, MSGDEL, total, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			log_event("", message, plyptr->name);
			sprintf(message, "2082%c%d%c%s%c%s%c", MSGDEL, total, MSGDEL, plyptr->name, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, ptr->name, 1);
			log_event("", message, ptr->name);
			sprintf(message, "2083%c%s%c%s%c%s%c%d%c%s%c", MSGDEL, select_class(ptr->classnum), MSGDEL, select_title(ptr->classnum, ptr->level[cont], ptr->gender), MSGDEL, ptr->nick, MSGDEL, total, MSGDEL, adjectives[ptr->gender][0], MSGDEL);
			send_message(message, enemy->name, 1);
		}
	}
	if(plyptr->vassals.total > 0 && plyptr->special_kingdom & spcaid[cont])
	{
		total = 0;
		for(plynum = 0; plynum < MAXVAS; ++plynum)
		{
			if(plyptr->vassals.plynum[plynum] < 1)
				continue;
			ptr = &players[plyptr->vassals.plynum[plynum]];
			for(count = 0, vtotal = 0; count < MAXTRP; ++count)
			{
				if((amount = ptr->army[type][cont].total[count] * 5 / 100) < 1)
					continue;
				if((amount = fill_castles(amount, cont, plyptr)) <= 0)
					break;
				total += amount;
				vtotal += amount;
				ptr->army[type][cont].total[count] -= amount;
				plyptr->army[type][cont].total[count] += amount;
			}
			if(vtotal > 0)
			{
				sprintf(message, "2084%c%d%c%s%c%s%c%s%c", MSGDEL, total, MSGDEL, adjectives[ptr->gender][4], MSGDEL, plyptr->nick, MSGDEL, conts[cont].name, MSGDEL);
				send_message(message, ptr->name, 1);
				log_event("", message, ptr->name);
			}
		}
		if(total > 0)
		{
			sprintf(message, "2085%c%d%c%s%c", MSGDEL, total, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			log_event("", message, plyptr->name);
			sprintf(message, "2086%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, total, MSGDEL, adjectives[plyptr->gender][1], MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, enemy->name, 1);
		}
	}
}

void vassal_postbattle(int *copper, int *food, int *land, int *peasant, player *loser, player *winner)
{
	int count;
	int vcopper = 0;
	int vfood = 0;
	int vland = 0;
	int vpeasant = 0;

	if(winner->lord > 0)
	{
		vcopper = (int)(*copper * .05);
		vfood = (int)(*food * .05);
		vland = (int)(*land * .05);
		vpeasant = (int)(*peasant * .05);
		if(!barbarian(&players[winner->lord]) && !vampire(&players[winner->lord]))
			++players[winner->lord].honor[winner->cont];
		sprintf(message, "2065%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_title(winner->classnum, winner->level[winner->cont], winner->gender), MSGDEL, winner->name, MSGDEL, 
		select_class(loser->classnum), MSGDEL, select_title(loser->classnum, loser->level[winner->cont], loser->gender), MSGDEL, loser->nick, MSGDEL, conts[winner->cont].name, MSGDEL);
		send_message(message, players[winner->lord].name, 1);
		log_event("", message, players[winner->lord].name);
		if(players[winner->lord].level[winner->cont] >= 0)
		{
			players[winner->lord].copper[winner->cont] += vcopper;
			*copper -= vcopper;
			players[winner->lord].food[winner->cont] += vfood;
			*food -= vfood;
			players[winner->lord].land[winner->cont] += vland;
			*land -= vland;
			players[winner->lord].peasant[winner->cont] += vpeasant;
			*peasant -= vpeasant;
			sprintf(message, "2066%c%d%c%s%c%d%c%s%c%d%c%d%c%s%c%s%c%s%c", MSGDEL, 
			vcopper, MSGDEL, money_name, MSGDEL, vfood, MSGDEL, food_name, MSGDEL, vland, MSGDEL, vpeasant, MSGDEL, peasant_name, MSGDEL, conts[winner->cont].name, MSGDEL, winner->nick, MSGDEL);
			send_message(message, players[winner->lord].name, 1);
			log_event("", message, players[winner->lord].name);
		}
		save_player(winner->lord);
	}
	if(winner->vassals.total > 0)
	{
		vcopper = (int)(*copper * .02);
		if(vcopper * winner->vassals.total > *copper)
			vcopper = 0;
		vfood = (int)(*food * .02);
		if(vfood * winner->vassals.total > *food)
			vfood = 0;
		vland = (int)(*land * .02);
		if(vland * winner->vassals.total > *land)
			vland = 0;
		vpeasant = (int)(*peasant * .02);
		if(vpeasant * winner->vassals.total > *peasant)
			vpeasant = 0;
		sprintf(message, "2067%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		adjectives[winner->gender][4], MSGDEL, select_title(winner->classnum, winner->level[winner->cont], winner->gender), MSGDEL, winner->nick, MSGDEL, 
		select_class(loser->classnum), MSGDEL, select_title(loser->classnum, loser->level[winner->cont], loser->gender), MSGDEL, loser->nick, MSGDEL, conts[winner->cont].name, MSGDEL);
		log_vassalmsg(message, winner);
		for(count = 0; count < MAXVAS; ++count)
		{
			if(winner->vassals.plynum[count] < 1)
				continue;
			if(!barbarian(&players[winner->vassals.plynum[count]]) && !vampire(&players[winner->vassals.plynum[count]]))
				++players[winner->vassals.plynum[count]].honor[winner->cont];
			if(players[winner->vassals.plynum[count]].level[winner->cont] >= 0)
			{
				players[winner->vassals.plynum[count]].copper[winner->cont] += vcopper;
				*copper -= vcopper;
				players[winner->vassals.plynum[count]].food[winner->cont] += vfood;
				*food -= vfood;
				players[winner->vassals.plynum[count]].land[winner->cont] += vland;
				*land -= vland;
				players[winner->vassals.plynum[count]].peasant[winner->cont] += vpeasant;
				*peasant -= vpeasant;
				sprintf(message, "2066%c%d%c%s%c%d%c%s%c%d%c%d%c%s%c%s%c%s%c", MSGDEL, 
				vcopper, MSGDEL, money_name, MSGDEL, vfood, MSGDEL, food_name, MSGDEL, vland, MSGDEL, vpeasant, MSGDEL, peasant_name, MSGDEL, conts[winner->cont].name, MSGDEL, winner->nick, MSGDEL);
				log_event("", message, players[winner->vassals.plynum[count]].name);
			}
			save_player(winner->vassals.plynum[count]);
		}
	}
	if(loser->lord > 0)
	{
		sprintf(message, "2068%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_title(loser->classnum, loser->level[winner->cont], loser->gender), MSGDEL, loser->name, MSGDEL, 
		select_class(winner->classnum), MSGDEL, select_title(winner->classnum, winner->level[winner->cont], winner->gender), MSGDEL, winner->nick, MSGDEL, conts[winner->cont].name, MSGDEL);
		send_message(message, players[loser->lord].name, 1);
		log_event("", message, players[loser->lord].name);
	}
	if(loser->vassals.total > 0)
	{
		sprintf(message, "2069%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		adjectives[loser->gender][4], MSGDEL, select_title(loser->classnum, loser->level[winner->cont], loser->gender), MSGDEL, loser->nick, MSGDEL, 
		select_class(winner->classnum), MSGDEL, select_title(winner->classnum, winner->level[winner->cont], winner->gender), MSGDEL, winner->nick, MSGDEL, conts[winner->cont].name, MSGDEL);
		log_vassalmsg(message, loser);
	}
}
