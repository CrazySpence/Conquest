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

void charm(char *args, player *plyptr)
{
	char arg1[32] = {0};
	char arg2[32] = {0};
	char cmd[32] = {0};
	char name[32] = {0};
	int plynum;
	SOCKET sock;

	sock = INVALID_SOCKET;
	if(!vampire(plyptr) && !demigod(plyptr))
	{
		sprintf(message, "17000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire != playernum(plyptr) && plyptr->remorts < 1 && !demigod(plyptr))
	{
		sprintf(message, "17011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 6)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 6, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < CHARM)
	{
		sprintf(message, "1%c%d%cto use Charm%c", MSGDEL, CHARM, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sscanf(args, "%31s %31s %31s %31s", name, cmd, arg1, arg2);
	if(strlen(name) < 1)
	{
		help(plyptr->name, "charm", "", "");
		return;
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
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
	if(titan(&players[plynum]) || players[plynum].levelmax == 0)
	{
		sprintf(message, "17001%c%s%c%s%c", MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(vampire(&players[plynum]) || check_hero(HERO_VAMPIRE, &players[plynum]))
	{
		sprintf(message, "17002%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((STRCASECMP(cmd, "password")) == 0 || (STRCASECMP(cmd, "corpse")) == 0 || (STRCASECMP(cmd, "steward")) == 0 || (STRCASECMP(cmd, "vacation")) == 0 || 
	((STRCASECMP(cmd, "buy")) == 0 && (STRCASECMP(arg1, "item")) == 0 && (STRCASECMP(arg2, itemd[ITEM_SCEPTRE].keyword)) == 0))
	{
		sprintf(message, "17003%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, args + strlen(name) + 1, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= CHARM;
	if(titan(plyptr))
	{
		sprintf(message, "%s %s %s charmed %s: %s", NOWSTR, select_class(plyptr->classnum), plyptr->name, players[plynum].name, cmd);
		add_log(EMPLOG, message);
	}
	if(save_throw(&players[plynum]))
	{
		sprintf(message, "17004%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, adjectives[players[plynum].gender][2], MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "17005%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		return;
	}
	++plyptr->attacks;
	sprintf(message, "17006%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, adjectives[players[plynum].gender][2], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17007%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, args + strlen(name) + 1, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	if(players[plynum].validate == NOT_VAL)
		players[plynum].validate = VAL_VAMPIRE;
	sock = players[plynum].sock;
	players[plynum].sock = sockets[sockout].sock;
	parser(cmd, players[plynum].host, args + strlen(name) + strlen(cmd) + 2, XMLTYPE, players[plynum].name);
	players[plynum].sock = sock;
	if(players[plynum].validate == VAL_VAMPIRE)
		players[plynum].validate = NOT_VAL;
}

int create_vampire(int cont, player *loser, player *winner)
{
	int chance;
	int count;
	int hero;
	int type;

	if(mult_vampires == 0 || conq_info.vampire != playernum(winner) || winner->vassals.total >= MAXVAS)
		return(0);
	chance = 30 + loser->level[cont] + 1;
	if(winner->level[cont] >= 8)
		chance += 25;
	else if(winner->level[cont] >= 5)
		chance += 15;
	else if(winner->level[cont] >= 2)
		chance += 5;
	if(randomnum(100, 1) > chance)
		return(0);
	sprintf(message, "17008%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
	send_message(message, loser->name, 1);
	remove_vassal(loser);
	if(loser->vassals.total > 0)
		remove_all_vassal(loser);
	--totply[loser->classnum];
	loser->classnum = winner->classnum;
	loser->troop_type = troop_type(loser->classnum);
	++totply[loser->classnum];
	if(loser->special_misc & SPC_GUISE)
		loser->special_misc -= SPC_GUISE;
	for(count = 0; count < num_conts; ++count)
	{
		if(loser->level[count] < 0)
			continue;
		change_level(count, -(loser->level[count]), loser);
		loser->attacks_lost[count] = 0;
		loser->honor[count] = 0;
		loser->promotion[count] = 0;
		loser->copper[count] = 10000;
		loser->land[count] = promotions[loser->troop_type][0][1];
		loser->peasant[count] = promotions[loser->troop_type][0][2];
		for(type = 0; type < MAXTRP; ++type)
		{
			loser->army[ARMY_DEFENSE][count].total[type] = 0;
			loser->army[ARMY_CAMPAIGN][count].total[type] = 0;
		}
		loser->army[ARMY_DEFENSE][count].total[SOLDIERNUM] = promotions[loser->troop_type][0][0];
		if(units[loser->troop_type][CATAPULTNUM].attack > 0)
			loser->army[ARMY_DEFENSE][count].total[CATAPULTNUM] = 1;
		loser->castle[count] = combined_army(count, loser) / castle_max(loser->classnum);
		if(combined_army(cont, loser) % castle_max(loser->classnum) > 0)
			++loser->castle[count];
		loser->food[count] = combined_army(count, loser);
	}
	loser->lord = conq_info.vampire;
	add_vassal(conq_info.vampire, loser);
	if(loser->ally > 0)
		break_alliance(loser);
	if((hero = find_hero(loser) >= 0) && !hero_serve(hero, loser))
		dismiss(loser);
	return(1);
}

void destroy_vampires(player *vampptr)
{
	int count;
	player *vassal;

	if(vampptr->vassals.total < 1)
		return;
	for(count = 0; count < MAXVAS; ++count)
	{
		if(vampptr->vassals.plynum[count] < 1)
			continue;
		vassal = &players[vampptr->vassals.plynum[count]];
		sprintf(message, "17009%c%s%c%s%c%s%c%s%c", MSGDEL, adjectives[vampptr->gender][4], MSGDEL, select_class(vampptr->classnum), MSGDEL, select_title(vampptr->classnum, vampptr->level[vampptr->cont], vampptr->gender), MSGDEL, vampptr->name, MSGDEL);
		send_message(message, vassal->name, 1);
		sprintf(message, "17010%c%s%c%s%c%s%c%s%c", MSGDEL, conts[vassal->cont].name, MSGDEL, select_class(vassal->classnum), MSGDEL, select_title(vassal->classnum, vassal->level[vassal->cont], vassal->gender), MSGDEL, vassal->name, MSGDEL);
		send_channel(message, 0);
		delete_player(vampptr->vassals.plynum[count]);
	}
}

int find_vamp(player *vampptr)
{
	int count;
	int minlevel = 10;
	int vampnum = conq_info.vampire;

	if(vampptr->vassals.total < 1)
		return(vampnum);
	for(count = 0; count < MAXVAS; ++count)
	{
		if(vampptr->vassals.plynum[count] < 1)
			continue;
		if(players[vampptr->vassals.plynum[count]].levelmax < minlevel)
		{
			minlevel = players[vampptr->vassals.plynum[count]].levelmax;
			vampnum = vampptr->vassals.plynum[count];
		}
	}
	return(vampnum);
}

void guise(char *name, player *plyptr)
{
	char new[64] = {0};
	char old[64] = {0};
	int plynum;

	if(!vampire(plyptr))
	{
		sprintf(message, "17000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire != playernum(plyptr))
	{
		sprintf(message, "17011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "guise", "", "");
		return;
	}
	if(!check_word(plyptr->name, name, 31, 1))
		return;
	if((plynum = find_player(name)) > 0)
	{
		sprintf(message, "17012%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, adjectives[players[plynum].gender][1], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->special_misc & SPC_GUISE)
	{
		sprintf(message, "17013%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->special_misc += SPC_GUISE;
	sprintf(message, "17014%c%s%c", MSGDEL, name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(old, "logs/%s", plyptr->name);
	sprintf(new, "logs/%s", name);
	rename(old, new);
	sprintf(old, "logs/%s.msg", plyptr->name);
	sprintf(new, "logs/%s.msg", name);
	rename(old, new);
	strcpy(plyptr->name, name);
	reset_seek();
	reset_spy(0, playernum(plyptr));
}


int lifedrain(int cont, player *loser, player *winner)
{
	if(!vampire(winner) && !check_hero(HERO_VAMPIRE, winner))
		return(0);
	if(randomnum(100, 1) >= 11 - (loser->level[cont] + 1) || titan(loser) || vampire(loser))
		return(0);
	if(loser->level[cont] - 1 < 0)
	{
		sprintf(message, "17015%c%s%c%s%c%s%c", MSGDEL, select_class(loser->classnum), MSGDEL, loser->name, MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, winner->name, 1);
		sprintf(message, "17019%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, loser->name, 1);
		log_event("", message, loser->name);
		if(loser->kingdoms == 1)
		{
			sprintf(message, "17016%c%s%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, select_class(loser->classnum), MSGDEL, loser->name, MSGDEL);
			send_channel(message, 0);
		}
		return(loser->castle[cont]);
	}
	change_level(cont, -1, loser);
	check_assets(cont, loser->level[cont] + 2, loser);
	sprintf(message, "17017%c%s%c%s%c%s%c", MSGDEL, select_class(loser->classnum), MSGDEL, loser->name, MSGDEL, select_title(loser->classnum, loser->level[cont], loser->gender), MSGDEL);
	send_message(message, winner->name, 1);
	sprintf(message, "17018%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_classname(VAMPIRE), MSGDEL, select_class(loser->classnum), MSGDEL, loser->name, MSGDEL, select_title(loser->classnum, loser->level[cont], loser->gender), MSGDEL);
	send_channel(message, 0);
	sprintf(message, "17019%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
	send_message(message, loser->name, 1);
	log_event("", message, loser->name);
	return(0);
}

void make_vampire(player *plyptr)
{
	int count;
	int hero;

	if(allow_vampire == 0)
	{
		sprintf(message, "6%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire != 0)
	{
		sprintf(message, "17020%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2 && !check_hero(HERO_VAMPIRE, plyptr))
	{
		sprintf(message, "17021%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_artifact(ROD, plyptr))
	{
		sprintf(message, "17022%c%s%c", MSGDEL, artifacts[ROD].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < class_change[VAMPIRE])
	{
		sprintf(message, "17060%c%d%c%s%c", MSGDEL, class_change[VAMPIRE], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(randomnum(100, 1) >= 50 + (plyptr->levelmax * 2) && !check_hero(HERO_DESTROYER, plyptr))
	{
		sprintf(message, "17023%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "%s %s %s %s was killed trying to go %s.", NOWSTR, 
		select_class(plyptr->classnum), select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, select_classname(VAMPIRE));
		add_log(EMPLOG, message);
		delete_player(playernum(plyptr));
		return;
	}
	plyptr->copper[plyptr->cont] -= class_change[VAMPIRE];
	sprintf(message, "%s %s %s %s successfully went %s.", NOWSTR, 
	select_class(plyptr->classnum), select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, select_classname(VAMPIRE));
	add_log(EMPLOG, message);
	remove_vassal(plyptr);
	remove_all_vassal(plyptr);
	--totply[plyptr->classnum];
	plyptr->classnum = randomnum(30 + total_races[VAMPIRE] - 1, 30);
	++totply[plyptr->classnum];
	if(plyptr->special_misc & SPC_GUISE)
		plyptr->special_misc -= SPC_GUISE;
	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0)
			continue;
		if(plyptr->level[count] > 2)
			change_level(count, -(plyptr->level[count] - 2), plyptr);
		plyptr->attacks_lost[count] = 0;
		plyptr->honor[count] = 0;
		plyptr->promotion[count] = 0;
		check_assets(count, CLASSCHANGE, plyptr);
		if(plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] > 0 || plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] > 0)
		{
			sprintf(message, "17024%c%s%c%d%c%s%c%s%c", MSGDEL, 
			units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, plyptr->army[0][count].total[BLOODGUARDNUM] + plyptr->army[1][count].total[BLOODGUARDNUM], MSGDEL, 
			units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[count].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] = 0;
			plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] = 0;
		}
	}
	conq_info.vampire = playernum(plyptr);
	save_info();
	sprintf(message, "17025%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->ally > 0)
		break_alliance(plyptr);
	if((hero = find_hero(plyptr) >= 0) && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	plyptr->troop_type = troop_type(plyptr->classnum);
	check_troops(plyptr);
	sprintf(message, "17026%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void pestilence(char *name, player *plyptr)
{
	int amount;
	int max = 20;
	int min = 15;
	int plynum;

	if(!vampire(plyptr))
	{
		sprintf(message, "17000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 4)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 4, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < PESTILENCE)
	{
		sprintf(message, "1%c%d%cto use Pestilence%c", MSGDEL, PESTILENCE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "pestilence", "", "");
		return;
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
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
	if(check_hero(HERO_VAMPIRE, &players[plynum]))
	{
		sprintf(message, "17002%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= PESTILENCE;
	if(save_throw(&players[plynum]))
	{
		sprintf(message, "17027%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "17028%c%s%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, players[plynum].kname[plyptr->cont], MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		return;
	}
	if(plyptr->level[plyptr->cont] >= 6)
	{
		max = 30;
		min = 25;
		players[plynum].event[players[plynum].cont] = 15;
	}
	else if(plyptr->level[plyptr->cont] >= 5)
	{
		max = 25;
		min = 20;
	}
	amount = randomnum(max, min);
	players[plynum].food[players[plynum].cont] -= players[plynum].food[players[plynum].cont] * amount / 100;
	sprintf(message, "17029%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17030%c%s%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, players[plynum].kname[plyptr->cont], MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	save_player(plynum);
}

void reset_seek()
{
	int count;

	for(count = 1; count < current_player; ++count)
		if(players[count].special_misc & SPC_SEEK)
		{
			players[count].special_misc -= SPC_SEEK;
			save_player(count);
		}
}

void ritual(player *plyptr)
{
	char types[][16] =
	{
		"", "", "army"
	};
	int amount;
	int chance = 10;
	int cont;
	int count;
	int level = 0;
	int move = RITUAL;
	int troopnum;

	if(!vampire(plyptr))
	{
		sprintf(message, "17000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire != playernum(plyptr) && plyptr->remorts < 1)
	{
		sprintf(message, "17011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->special_misc & SPC_RITUAL)
	{
		sprintf(message, "17013%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	cont = plyptr->cont;
	if(plyptr->level[cont] >= 8)
		level = 2;
	else if(plyptr->level[cont] >= 5)
		level = 1;
	move += (plyptr->level[cont] + 1) * 2;
	if(plyptr->movepts < move)
	{
		sprintf(message, "1%c%d%cto use Ritual%c", MSGDEL, move, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= move;
	chance += (plyptr->level[cont] + 1) + plyptr->vassals.total;
	if(randomnum(100, 1) > chance)
	{
		sprintf(message, "17052%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	forecast[cont].violent = 1;
	plyptr->special_misc += SPC_RITUAL;
	sprintf(message, "17053%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17054%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][0], MSGDEL, select_classname(VAMPIRE), MSGDEL, adjectives[plyptr->gender][5], MSGDEL, adjectives[plyptr->gender][0], MSGDEL, conts[cont].name, MSGDEL);
	send_channel(message, 0);
	strcpy(types[0], food_name);
	sprintf(types[1], "%ss", peasant_name);
	for(count = 1; count < current_player; ++count)
	{
		if((strlen(players[count].name)) < 1 || players[count].level[cont] < 0)
			continue;
		if(titan(&players[count]) || vampire(&players[count]) || heroes[HERO_VAMPIRE].owner == count || players[count].special_misc & SPC_VACATION)
			continue;
		amount = randomnum(10, 5);
		switch(level)
		{
			case 1:
				players[count].peasant[cont] -= players[count].peasant[cont] * amount / 100;
				break;
			case 2:
				for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
				{
					players[count].army[ARMY_DEFENSE][cont].total[troopnum] -= players[count].army[ARMY_DEFENSE][cont].total[troopnum] * amount / 100;
					players[count].army[ARMY_CAMPAIGN][cont].total[troopnum] -= players[count].army[ARMY_CAMPAIGN][cont].total[troopnum] * amount / 100;
				}
				break;
			default:
				players[count].food[cont] -= players[count].food[cont] * amount / 100;
				break;
		}
		sprintf(message, "17058%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, adjectives[plyptr->gender][5], MSGDEL, amount, MSGDEL, types[level], MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, players[count].name, 1);
		log_event("", message, players[count].name);
		save_player(count);
	}
	switch(level)
	{
		case 1:
			sprintf(message, "17056%c%s%c%s%c", MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case 2:
			sprintf(message, "17057%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
			break;
		default:
			sprintf(message, "17055%c%s%c%s%c", MSGDEL, food_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
	}
	send_channel(message, 0);
}

void seek(player *plyptr)
{
	int chance;
	player *vampptr;

	if(!cleric(plyptr) && !ranger(plyptr) && !check_hero(HERO_HIGHLORD, plyptr) && !check_hero(HERO_BISHOP, plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire == 0)
	{
		sprintf(message, "3058%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 1 && !check_hero(HERO_HIGHLORD, plyptr) && !check_hero(HERO_BISHOP, plyptr))
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1); 
		return;
        }
	if(plyptr->movepts < SEEK)
	{
		sprintf(message, "1%c%d%cto seek out the elusive %s%c", MSGDEL, SEEK, MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	vampptr = &players[conq_info.vampire];
	if((ranger(plyptr) || check_hero(HERO_HIGHLORD, plyptr) || check_hero(HERO_BISHOP, plyptr)) && plyptr->cont != vampptr->cont && !cleric(plyptr))
	{
		sprintf(message, "17031%c%s%c%s%c", MSGDEL, select_classname(CLERIC), MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= SEEK;
	chance = 19 - vampptr->level[vampptr->cont] + (plyptr->level[plyptr->cont] - vampptr->level[vampptr->cont]);
	if(chance < 1)
		chance = 1;
	if((check_hero(HERO_PRIESTESS, plyptr) || check_hero(HERO_BISHOP, plyptr)) && check_artifact(GRAIL, plyptr))
		chance *= 2;
	if(randomnum(100, 1) > chance)
	{
		sprintf(message, "17050%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "17032%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		send_message(message, vampptr->name, 1);
		sprintf(message, "17033%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		log_event("", message, vampptr->name);
		sprintf(message, "17034%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, adjectives[vampptr->gender][5], MSGDEL);
		log_vassalmsg(message, vampptr);
		return;
	}
	if(!(plyptr->special_misc & SPC_SEEK))
		plyptr->special_misc += SPC_SEEK;
	sprintf(message, "17035%c%s%c%s%c%s%c", MSGDEL, 
	select_class(vampptr->classnum), MSGDEL, select_title(vampptr->classnum, vampptr->level[vampptr->cont], vampptr->gender), MSGDEL, vampptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17036%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, vampptr->name, 1);
	log_event("", message, vampptr->name);
	sprintf(message, "17037%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, adjectives[vampptr->gender][5], MSGDEL);
	log_vassalmsg(message, vampptr);
}

void slay(char *name, player *plyptr)
{
	int chance;
	int plynum;
	player *vampptr;
	time_t current;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire == 0)
	{
		sprintf(message, "3058%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1); 
		return;
        }
	if(plyptr->movepts < SLAY)
	{
		sprintf(message, "1%c%d%cto destroy the elusive %s%c", MSGDEL, SLAY, MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "slay", "", "");
		return;
	}
	if(!(plyptr->special_misc & SPC_SEEK))
	{
		sprintf(message, "17038%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, adjectives[players[conq_info.vampire].gender][1], MSGDEL);
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
		sprintf(message, "3006%c%s%c", MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plynum != conq_info.vampire)
	{
		sprintf(message, "3046%c%s%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_hero(HERO_VAMPIRE, plyptr))
	{
		sprintf(message, "17039%c%s%c%s%c", MSGDEL, heroes[HERO_VAMPIRE].name, MSGDEL, heroes[HERO_VAMPIRE].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	time(&current);
	if((difftime(current, plyptr->lastattack)) < ATTACKTIME)
	{
		sprintf(message, "2007%c%s%c%d%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, ATTACKTIME - (int)difftime(current, plyptr->lastattack), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(players[conq_info.vampire].vassals.total < 1 && check_vacation(plyptr->name, &players[conq_info.vampire]))
		return;
	++plyptr->attacks;
	plyptr->isprotected = 1;
	plyptr->lastattack = time(NULL);
	plyptr->movepts -= SLAY;
	plynum = find_vamp(&players[plynum]);
	vampptr = &players[plynum];
	chance = 14 - vampptr->level[plyptr->cont] + (plyptr->level[plyptr->cont] - vampptr->level[plyptr->cont]);
	if(chance < 1)
		chance = 1;
	if(randomnum(100, 1) > chance)
	{
		sprintf(message, "17040%c%s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, vampptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "17041%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
		send_message(message, vampptr->name, 1);
		log_event("", message, vampptr->name);
		sprintf(message, "17042%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL, 
		select_classname(VAMPIRE), MSGDEL, vampptr->name, MSGDEL);
		if(plynum != conq_info.vampire)
		{
			send_message(message, players[conq_info.vampire].name, 1);
			log_event("", message, players[conq_info.vampire].name);
		}
		log_vassalmsg(message, &players[conq_info.vampire]);
		return;
	}
	sprintf(message, "17043%c%s%c%s%c", MSGDEL, vampptr->name, MSGDEL, select_classname(VAMPIRE), MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17044%c%c", MSGDEL, MSGDEL);
	send_message(message, vampptr->name, 1);
	if(plynum == conq_info.vampire)
	{
		reset_seek();
		sprintf(message, "17045%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, 
		MSGDEL,  select_class(vampptr->classnum), MSGDEL, select_title(vampptr->classnum, vampptr->level[vampptr->cont], vampptr->gender), MSGDEL, vampptr->name, MSGDEL);
	}
	else
	{
		sprintf(message, "17059%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL, 
		select_classname(VAMPIRE), MSGDEL, vampptr->name, MSGDEL);
		send_message(message, players[conq_info.vampire].name, 1);
		log_event("", message, players[conq_info.vampire].name);
		log_vassalmsg(message, &players[conq_info.vampire]);
		sprintf(message, "17010%c%s%c%s%c%s%c%s%c", MSGDEL, conts[vampptr->cont].name, MSGDEL, select_class(vampptr->classnum), MSGDEL, select_title(vampptr->classnum, vampptr->level[vampptr->cont], vampptr->gender), MSGDEL, vampptr->name, MSGDEL);
	}
	send_channel(message, 0);
	take_vampassets(plyptr, vampptr);
	kia_change(vampptr->cont, vampptr->name, plyptr->name);
	delete_player(plynum);
	save_info();
}

void take_vampassets(player *plyptr, player *vampptr)
{
	int copper;
	int count;
	int land;
	int peasant;

	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0 || vampptr->level[count] < 0)
			continue;
		land = vampptr->land[count] / 2;
		peasant = vampptr->peasant[count] / 2;
		copper = vampptr->copper[count] / 2;
		plyptr->land[count] += land;
		plyptr->peasant[count] += peasant;
		plyptr->copper[count] += copper;
		plyptr->honor[count] += (vampptr->level[count] + 1) * 2;
		sprintf(message, "17046%c%d%c%d%c%s%c%d%c%s%c%s%c", MSGDEL, land, MSGDEL, peasant, MSGDEL, peasant_name, MSGDEL, copper, MSGDEL, money_name, MSGDEL, conts[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

int vampire(player *plyptr)
{
	if(!plyptr)
		return(0);
	if(plyptr->classnum >= 30 && plyptr->classnum < 40)
		return(1);
	else
		return(0);
}

void vampire_death(int cont, player *plyptr, player *vampptr)
{
	int count;
	int level;

	if(conq_info.vampire == playernum(vampptr))
		level = 2;
	else
		level = 0;
	sprintf(message, "17047%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(vampptr->classnum), MSGDEL, select_title(vampptr->classnum, vampptr->level[cont], vampptr->gender), MSGDEL, vampptr->name, MSGDEL, vampptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "17048%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, select_classname(VAMPIRE), MSGDEL, select_classname(VAMPIRE), MSGDEL);
	send_channel(message, 0);
	sprintf(message, "17049%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_message(message, vampptr->name, 1);
	log_event("", message, vampptr->name);
	vampptr->land[cont] = promotions[BASE_CLASSNUM(vampptr->classnum)][level][1];
	vampptr->peasant[cont] = promotions[BASE_CLASSNUM(vampptr->classnum)][level][2];
	vampptr->food[cont] = promotions[BASE_CLASSNUM(vampptr->classnum)][level][0];
	vampptr->copper[cont] = 10000;
	for(count = 0; count < MAXTRP; ++count)
	{
		vampptr->army[ARMY_DEFENSE][cont].total[count] = 0;
		vampptr->army[ARMY_CAMPAIGN][cont].total[count] = 0;
	}
	vampptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] = promotions[BASE_CLASSNUM(vampptr->classnum)][level][0];
	if(units[vampptr->troop_type][CATAPULTNUM].attack > 0)
		vampptr->army[ARMY_DEFENSE][cont].total[CATAPULTNUM] = 2;
	vampptr->castle[cont] = combined_army(cont, vampptr) / castle_max(vampptr->classnum);
	if(combined_army(cont, vampptr) % castle_max(vampptr->classnum) > 0)
		++vampptr->castle[count];
	vampptr->isprotected = 0;
	change_level(cont, -(vampptr->level[cont] - level), vampptr);
}
