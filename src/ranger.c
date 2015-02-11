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

void ceasefire(player *plyptr)
{
	struct tm *pnt;
	time_t current;

	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.cont_special[plyptr->cont] & SPC_CEASEFIRE)
	{
		sprintf(message, "16022%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < CEASEFIRE)
	{
		sprintf(message, "1%c%d%cto use Ceasefire%c", MSGDEL, CEASEFIRE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->food[plyptr->cont] < promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][2])
	{
		sprintf(message, "16%c%s%c%d%cstart a ceasefire on %s%c", MSGDEL, food_name, MSGDEL, promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][2], MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= CEASEFIRE;
	if((randomnum(100, 1)) <= 70)
	{
		sprintf(message, "16023%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->food[plyptr->cont] -= plyptr->food[plyptr->cont] * 15 / 100;
	time(&current);
	pnt = localtime(&current);
	conq_info.cont_special[plyptr->cont] += SPC_CEASEFIRE;
	save_info();
	sprintf(message, "16024%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "16025%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL,select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "4394%cceasefire%c%d%c", MSGDEL, MSGDEL, 60 - pnt->tm_min, MSGDEL);
	send_channel(message, 0);
}

void commune(player *plyptr)
{
	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 4)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, 5, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < COMMUNE)
	{
		sprintf(message, "1%c%d%cto use Commune%c", MSGDEL, COMMUNE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= COMMUNE;
	if(randomnum(100, 1) <= 50 + ((plyptr->level[plyptr->cont] + 1) * 5))
	{
		sprintf(message, "11050%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	change_weather(plyptr->cont, -1, -1, -1);
	sprintf(message, "16002%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "16003%c%s%c%s%c%s%c%d%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, randomnum(8, 2), MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
}

int ranger(player *plyptr)
{
	if(!plyptr)
		return(0);
	if((plyptr->classnum >= 50 && plyptr->classnum < 60) || titan(plyptr))
		return(1);
	else
		return(0);
}

void serve(player *plyptr)
{
	int count;
	int hero;
	int oldclass;

	if(allow_ranger == 0)
	{
		sprintf(message, "6%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 1)
	{
		sprintf(message, "7%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
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
	if(plyptr->movepts < SERVE)
	{
		sprintf(message, "1%c%d%cto become a %s%c", MSGDEL, SERVE, MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < class_change[RANGER])
	{
		sprintf(message, "16004%c%s%c%d%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL, class_change[RANGER], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!ranger(&players[plyptr->lord]))
		remove_vassal(plyptr);
	plyptr->movepts -= SERVE;
	plyptr->copper[plyptr->cont] -= class_change[RANGER];
	oldclass = plyptr->classnum;
	--totply[oldclass];
	plyptr->classnum = change_class(RANGER, plyptr);
	++totply[plyptr->classnum];
	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0)
			continue;
		if(plyptr->level[count] > 0)
			change_level(count, -(plyptr->level[count]), plyptr);
		plyptr->honor[count] = 0;
		check_assets(count, CLASSCHANGE - 1, plyptr);
		tithe(count, plyptr);
	}
	sprintf(message, "16005%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "16006%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(oldclass), MSGDEL, plyptr->name, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL);
	send_channel(message, 0);
	if(plyptr->ally > 0)
		break_alliance(plyptr);
	if(plyptr->items[0] > 0)
	{
		sprintf(message, "10007%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL); 
		send_message(message, plyptr->name, 1);
		remove_all_items(plyptr);
	}
	if((hero = find_hero(plyptr)) >= 0 && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	if(heroes[HERO_NECROMANCER].owner != playernum(plyptr))
		plyptr->troop_type = troop_type(plyptr->classnum);
	check_troops(plyptr);
	check_vassals(plyptr);
}

void shield(player *plyptr)
{
	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < SHIELD)
	{
		sprintf(message, "1%c%d%cto use Shield%c", MSGDEL, SHIELD, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->special_batmod & spcshd[plyptr->cont])
	{
		sprintf(message, "16008%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= SHIELD;
	plyptr->special_batmod += spcshd[plyptr->cont];
	sprintf(message, "16010%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
}

void summon(char *name, player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int num_castles = 0;
	int plynum;
	int round = 1;
	int strength = MEDIOCRE;

	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < SUMMON)
	{
		sprintf(message, "1%c%d%cto use Summon%c", MSGDEL, SUMMON, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "summon", "", "");
		return;
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return;
	plyptr->movepts -= SUMMON;
	if(plyptr->level[plyptr->cont] >= 4)
		strength = MIGHTY;
	else if(plyptr->level[plyptr->cont] >= 3)
		strength = STRONG;
	if(plyptr->ally == 0)
	{
		++plyptr->attacks;
		plyptr->isprotected = 1;
		plyptr->lastattack = time(NULL);
		combatants[ATTACKER] = plyptr;
		combatants[DEFENDER] = &players[plynum];
		combatants[ATTACKER_HORDE1] = (void *)plybuf[ATTACKER_HORDE1];
		summon_creature(0, randomnum(LION, WOLF), ARMY_CAMPAIGN, ARMY_CAMPAIGN, strength, combatants[ATTACKER_HORDE1], plyptr);
		sprintf(message, "10013%c%s%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, players[plynum].nick, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "10014%c%s%c", MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		start_battle();
		save_player(plynum);
		return;
	}
	if(randomnum(100, 1) <= 30 + (players[plynum].level[players[plynum].cont] + 1))
	{
		sprintf(message, "16019%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	++plyptr->attacks;
	plyptr->isprotected = 1;
	plyptr->lastattack = time(NULL);
	sprintf(message, "10015%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	log_event("", message, players[plynum].name);
	combatants[ATTACKER] = (void *)plybuf[ATTACKER];
	summon_creature(1, randomnum(LION, WOLF), ARMY_DEFENSE, ARMY_CAMPAIGN, strength, combatants[ATTACKER], &players[plynum]);
	combatants[DEFENDER] = &players[plynum];
	if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
		beast_won(&players[plynum], "Pack", PACK);
	else
		beast_lost(&players[plynum], "Pack", PACK);
	if(count_casualties(DEFENDER))
		show_casualties(DEFENDER, ARMY_DEFENSE, combatants[DEFENDER]);
	if(combatants[DEFENDER_ALLY] && count_casualties(DEFENDER_ALLY))
		show_casualties(DEFENDER_ALLY, ARMY_DEFENSE, combatants[DEFENDER_ALLY]);
	check_ret(combatants[DEFENDER]->cont, combatants[ATTACKER], combatants[DEFENDER]);
	update_ret(combatants[DEFENDER]->cont, combatants[ATTACKER], combatants[DEFENDER]);
	save_player(plynum);
	reset_combatants();
	sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
}

void tend(char *name, player *plyptr)
{
	int amount;
	int max = 10;
	int min = 5;
	int plynum;

	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 1)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < TEND)
	{
		sprintf(message, "1%c%d%cto Tend any fields%c", MSGDEL, TEND, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "tend", "", "");
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
		sprintf(message, "16011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= TEND;
	if(plyptr->level[plyptr->cont] >= 2)
	{
		max = 20;
		min = 15;
	}
	else if(plyptr->level[plyptr->cont] >= 1)
	{
		max = 15;
		min = 10;
	}
	if(players[plynum].food[players[plynum].cont] > MAXFOOD)
		amount = 2;
	else
		amount = players[plynum].food[players[plynum].cont] * randomnum(max, min) / 100;
	players[plynum].food[players[plynum].cont] += amount;
	++plyptr->honor[plyptr->cont];
	sprintf(message, "16012%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, adjectives[players[plynum].gender][0], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "16013%c%s%c%d%c%s%c", MSGDEL, players[plynum].name, MSGDEL, amount, MSGDEL, food_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "16014%c%s%c%s%c%s%c%s%c", MSGDEL, 
	adjectives[players[plynum].gender][3], MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "16015%c%d%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "16016%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL, plyptr->name, MSGDEL);
	log_event("", message, players[plynum].name);
	if(titan(plyptr))
	{
		sprintf(message, "%s %s %s %s tended %s (+%d %s).", NOWSTR, select_class(plyptr->classnum), select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, players[plynum].name, amount, food_name);
		add_log(EMPLOG, message);
	}
}

void vision(char *name, player *plyptr)
{
	int plynum;

	if(!ranger(plyptr))
	{
		sprintf(message, "16000%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 0)
	{
		sprintf(message, "16001%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < VISION)
	{
		sprintf(message, "1%c%d%cto use Vision%c", MSGDEL, VISION, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "vision", "", "");
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
		sprintf(message, "16011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(check_artifact(MIRROR, &players[plynum]))
	{
		sprintf(message, "10023%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "16017%c%s%c%s%cVision%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL, MSGDEL, adjectives[players[plynum].gender][2], MSGDEL, conts[players[plynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= VISION;
	sprintf(message, "16018%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	person("", plyptr, &players[plynum]);
	if(plyptr->level[plyptr->cont] >= 1)
	{
		show_surrender(plyptr->name, &players[plynum]);
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, players[plynum].cont, plyptr->level[plyptr->cont] >= 2?REVIEW_ALL:REVIEW_NOBG, ARMY_DEFENSE, &players[plynum], &players[plynum]);
		show_formation(plyptr->name, players[plynum].cont, ARMY_DEFENSE, &players[plynum]);
		if(advanced_armies == 1)
		{
			sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
			send_message(message, plyptr->name, 1);
			review(plyptr->name, plyptr->special_misc & SPC_COLOR, players[plynum].cont, plyptr->level[plyptr->cont] >= 2?REVIEW_ALL:REVIEW_NOBG, ARMY_CAMPAIGN, &players[plynum], &players[plynum]);
			show_formation(plyptr->name, players[plynum].cont, ARMY_CAMPAIGN, &players[plynum]);
		}
	}
}
