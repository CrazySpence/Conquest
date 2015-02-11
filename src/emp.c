
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

void cosine(char *itemname, char *name, int amount, int *item, int type, player *plyptr)
{
	int max = 1000000;
	int min = -100;

	if(!demigod(plyptr))
	{
		sprintf(message, "13%ccosine%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type == 15)
	{
		max = num_remorts;
		min = 0;
	}
	if(!emperor(plyptr->name) && ((*item + amount > max && amount > 0) || (*item + amount < min && amount < 0)))
	{
		sprintf(message, "999%cInvalid value for cosine.", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
        *item += amount;
	if(amount > 0)
	{
		sprintf(message, "6001%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->name, MSGDEL);
		send_message(message, name, 1);
		log_event("", message, name);
		sprintf(message, "6002%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
	{
		sprintf(message, "6003%c%d%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL);
		send_message(message, name, 1);
		log_event("", message, name);
		sprintf(message, "6004%c%s%c%d%c%s%c", MSGDEL, name, MSGDEL, amount, MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	save_player(find_player(name));
	sprintf(message, "%s A gift of %d %s from %s %s to %s.", NOWSTR, amount, itemname, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, name);
	add_log(EMPLOG, message);
}

void create_titan(char *name, int type, player *plyptr)
{
	int plynum;

	if(!demigod(plyptr))
	{
		sprintf(message, "13%ctitan%c", MSGDEL, MSGDEL);
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
	if(emperor(players[plynum].name))
	{
		sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
		send_message(message, plyptr->name, 1);
		return;
	}
	make_titan(type, &players[plynum]);
	save_player(plynum);
	sprintf(message, "%s %s %s promoted %s to %s.", NOWSTR, 
	select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, players[plynum].name, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender));
	add_log(EMPLOG, message);
}

void erase(int plynum, player *plyptr)
{
	if(!demigod(plyptr))
	{
		sprintf(message, "13%cerase%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plynum < 1 || plynum > total_player)
	{
		sprintf(message, "999%cInvalid player to delete.", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(emperor(players[plynum].name))
	{
		sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "%s %s %s deleted player \"%s\".", NOWSTR, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, players[plynum].name);
	add_log(EMPLOG, message);
	delete_player(plynum);
	sprintf(message, "999%cPlayer #%d deleted!", MSGDEL, plynum);
	send_message(message, plyptr->name, 1);
}

void find_artifacts(player *plyptr)
{
	int count;

	if(!titan(plyptr))
	{
		sprintf(message, "13%cartifacts%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < TOTART; ++count)
	{
		if(artifacts[count].city < 0)
			continue;
		if(count == WINGS || count == BOOTS)
		{
			if(artifacts[count].owner > 0)
				sprintf(message, "6007%c%s%c%s%c", MSGDEL, artifacts[count].name, MSGDEL, players[artifacts[count].owner].name, MSGDEL);
			else
				sprintf(message, "6008%c%s%c", MSGDEL, artifacts[count].name, MSGDEL);
		}
		else
		{
			if(artifacts[count].owner > 0)
				sprintf(message, "6009%c%s%c%s%c", MSGDEL, artifacts[count].name, MSGDEL, players[artifacts[count].owner].name, MSGDEL);
			else
				sprintf(message, "6010%c%s%c", MSGDEL, artifacts[count].name, MSGDEL);
		}
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void hero_list(player *plyptr)
{
	int count;

	if(!titan(plyptr))
	{
		sprintf(message, "13%cheroes%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < TOTHRO; ++count)
	{
		if(heroes[count].alive == 1)
			sprintf(message, "6011%c%s%c%s%c%s%c", MSGDEL, heroes[count].name, MSGDEL, heroes[count].type, MSGDEL, heroes[count].owner == 0?"no one":players[heroes[count].owner].name, MSGDEL);
		else
			sprintf(message, "6012%c%s%c%s%c%d%c", MSGDEL, heroes[count].name, MSGDEL, heroes[count].type, MSGDEL, heroes[count].serve, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void hk(int type, player *plyptr)
{
	if(!titan(plyptr))
	{
		sprintf(message, "13%chk%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	strcpy(conq_info.hkby, plyptr->name);
	hk_text(plyptr);
	housekeeping(type);
}

void hklog(player *plyptr)
{
	char input[128] = {0};
	FILE *fp;

	if((fp = fopen(HKLOG, "r")) == NULL)
	{
		sprintf(message, "999%cError opening %s: %s", MSGDEL, HKLOG, strerror(errno));
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	while((fgets(input, 128, fp)) != NULL)
	{
		strip(input);
		sprintf(message, "58%c%s%c", MSGDEL, input, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	fclose(fp);
}

void list_locks(player *plyptr)
{
	int count;

	sprintf(message, "999%cPlayer locks:", MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 1; count < current_player; ++count)
	{
		if((strlen(players[count].name)) < 1)
			continue;
		if(!(players[count].special_misc & SPC_LOCK))
			continue;
		sprintf(message, "999%c%s %s", MSGDEL, select_class(players[count].classnum), players[count].name);
		send_message(message, plyptr->name, 1);
	}
}

void lock(char *name, player *plyptr)
{
	int count;
	int plynum;

	if(!demigod(plyptr))
	{
		sprintf(message, "13%clock%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		list_locks(plyptr);
		return;
	}
	if((plynum = find_player(name)) <= 0 && strcmp(name, "ALL") != 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plynum > 0 && emperor(players[plynum].name))
	{
		sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plynum > 0)
	{
		if(players[plynum].special_misc & SPC_LOCK)
			players[plynum].special_misc -= SPC_LOCK;
		else
			players[plynum].special_misc += SPC_LOCK;
		players[plynum].validate = NOT_VAL;
		sprintf(message, "%s %s.", players[plynum].name, players[plynum].special_misc & SPC_LOCK?"LOCKED":"UNLOCKED");
		send_message(message, plyptr->name, 1);
		save_player(plynum);
	}
	else
	{
		for(count = 1; count < current_player; ++count)
		{
			if((strlen(players[count].name)) < 1)
				continue;
			if(emperor(players[count].name))
				continue;
			if(!(players[count].special_misc & SPC_LOCK))
			{
				players[count].special_misc += SPC_LOCK;
				players[count].validate = NOT_VAL;
				save_player(count);
			}
		}
		sprintf(message, "ALL players LOCKED.");
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "%s %s %s locked %s.", NOWSTR, select_class(plyptr->classnum), plyptr->name, plynum < 1?"all players":players[plynum].name);
	add_log(EMPLOG, message);
}

void make_questchar(player *plyptr)
{
	if(!demigod(plyptr))
	{
		sprintf(message, "13%cquestchar%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "999%cCreating quest characters.", MSGDEL);
	send_message(message, plyptr->name, 1);
	create_questchar();
}

void make_titan(int type, player *plyptr)
{
	int count;
	int level;

	if(plyptr->ally > 0)
		break_alliance(plyptr);
	--totply[plyptr->classnum];
	plyptr->classnum = DEMIGOD;
	++totply[plyptr->classnum];
	if(type)
		level = maxlevel[BASE_CLASSNUM(plyptr->classnum)];
	else
		level = maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1;
	change_level(plyptr->cont, level - plyptr->level[plyptr->cont], plyptr);
	plyptr->movepts = 100;
	plyptr->land[plyptr->cont] = 15000;
	plyptr->peasant[plyptr->cont] = 15000;
	plyptr->food[plyptr->cont] = 15000;
	plyptr->copper[plyptr->cont] = 100000;
	for(count = 0; count < MAXTRP; ++count)
	{
		plyptr->army[ARMY_DEFENSE][plyptr->cont].total[count] = 0;
		plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[count] = 0;
	}
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[SOLDIERNUM] = 50000;
	if(units[plyptr->troop_type][ARCHERNUM].attack > 0 && units[plyptr->troop_type][ARCHERNUM].defend > 0)
		plyptr->army[ARMY_DEFENSE][plyptr->cont].total[ARCHERNUM] = 25000;
	if(units[plyptr->troop_type][KNIGHTNUM].attack > 0 && units[plyptr->troop_type][KNIGHTNUM].defend > 0)
		plyptr->army[ARMY_DEFENSE][plyptr->cont].total[KNIGHTNUM] = 25000;
	plyptr->castle[plyptr->cont] = combined_army(plyptr->cont, plyptr) / castle_max(plyptr->classnum);
	if(combined_army(plyptr->cont, plyptr) % castle_max(plyptr->classnum) > 0)
		++plyptr->castle[plyptr->cont];
	plyptr->kingdoms = 1;
	for(count = 0; count < num_conts; ++count)
		if(count != plyptr->cont && plyptr->level[count] >= 0)
		{
			--totlvl[count][plyptr->level[count]];
			plyptr->level[count] = -1;
		}
	plyptr->remorts = num_remorts;
	plyptr->troop_type = troop_type(plyptr->classnum);
	sprintf(message, "6013%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6014%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6015%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6016%c%s%c", MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6017%c%s%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, adjectives[plyptr->gender][1], MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6018%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
	send_channel(message, 0);
	if((find_artifact(plyptr)) >= 0)
		drop(plyptr);
	if((find_hero(plyptr)) >= 0)
		dismiss(plyptr);
	default_formation(plyptr->cont, plyptr);
	update_levelmax(plyptr);
	remove_all_ret(plyptr);
	remove_vassal(plyptr);
	remove_all_vassal(plyptr);
}

void motd(char *msg, player *plyptr)
{
	char newmsg[256];

	if(!titan(plyptr))
	{
		sprintf(message, "13%cmotd%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_word(plyptr->name, msg, 128, 1))
		return;
	memset(newmsg, 0, 256);
	strncpy(newmsg, msg, 192);
	sprintf(conq_info.motd, "Message from %s %s: %s", select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, newmsg);
	save_info();
	sprintf(message, "6019%c%s%c", MSGDEL, msg, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void newtopic(char *msg, int flag, SOCKET sock, player *plyptr)
{
	int socknum;

	if(!titan(plyptr) && !flag)
	{
		sprintf(message, "13%cnewtopic%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(sock == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Newtopic error: Invalid socket\n", NOWSTR);
		return;
	}
	socknum = find_sock(sock);
	if(socknum == INVALID_SOCKET)
		return;
 	if(STRCASECMP(sockets[socknum].type, "IRC") != 0)
		send_message("Invalid option for your client", plyptr->nick, 1);
	else
		add_msg("NEWTOPIC", server_ip, msg, "CONQUEST", socknum);
}

void nick(char *msg, SOCKET sock, player *plyptr)
{
	int socknum;

	if(!titan(plyptr))
	{
		sprintf(message, "13%cnick%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(sock == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Nick error: Invalid socket\n", NOWSTR);
		return;
	}
	if(!check_word(plyptr->name, msg, 31, 1))
		return;
	socknum = find_sock(sock);
	if(socknum == INVALID_SOCKET)
		return;
 	if(STRCASECMP(sockets[socknum].type, "IRC") != 0)
	{
		send_message("Invalid option for your client", plyptr->nick, 1);
		return;
	}
	sprintf(message, "999%cAttempting to change nick to \"%s\".", MSGDEL, msg);
	send_message(message, plyptr->name, 1);
	add_msg("CHANGE_NICK", server_ip, msg, "CONQUEST", socknum);
	sprintf(message, "%s %s %s attempted to change IRC bot nick to \"%s\".", NOWSTR, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, msg);
	add_log(EMPLOG, message);
}

void ninja(char *contname, char *name, player *plyptr)
{
	char pword[32] = {0};
	char str[TIMESIZ] = {0};
	int cont = 0;
	int plynum;

	if(!titan(plyptr))
	{
		sprintf(message, "13%cninja%c", MSGDEL, MSGDEL);
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
	if((cont = find_cont(contname)) < 0 || players[plynum].level[cont] < 0)
		cont = players[plynum].cont;
	plyptr->ninja = plynum;
	sprintf(message, "15%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	person(conts[cont].name, plyptr, &players[plynum]);
	sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
	send_message(message, plyptr->name, 1);
	review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_DEFENSE, &players[plynum], &players[plynum]);
	show_formation(plyptr->name, cont, ARMY_DEFENSE, &players[plynum]);
	if(advanced_armies == 1)
	{
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_CAMPAIGN, &players[plynum], &players[plynum]);
		show_formation(plyptr->name, cont, ARMY_CAMPAIGN, &players[plynum]);
	}
	if(!emperor(plyptr->name) || emperor(players[plynum].name))
		strcpy(pword, "??????");
	else
		strncpy(pword, players[plynum].password, 31);
	sprintf(message, "6020%c%s%c", MSGDEL, players[plynum].host, MSGDEL);
	send_message(message, plyptr->name, 1);
	strftime(str, TIMESIZ, TIMEFORMAT, localtime(&players[plynum].vacation));
	sprintf(message, "6032%c%s%c", MSGDEL, players[plynum].special_misc & SPC_VACATION?str:"No", MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "6021%c%d%c%d%c%d%c", MSGDEL, plynum, MSGDEL, players[plynum].honor[cont], MSGDEL, players[plynum].event[cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "6022%c%s%c%d%c%d%c%d%c%s%c", MSGDEL, pword, MSGDEL, players[plynum].special_batmod, MSGDEL, players[plynum].special_kingdom, MSGDEL,  players[plynum].special_misc, MSGDEL, players[plynum].special_misc & SPC_OPS?"ON":"OFF", MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "6023%c%s%c%s%c%d%c%d%c", MSGDEL, players[plynum].promotion[cont] == 1?"Yes":"No", MSGDEL, players[plynum].validate == VAL_NORMAL?"Yes":"No", MSGDEL, players[plynum].validate, MSGDEL, players[plynum].kingdoms, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "6024%c%d%c%d%c%d%c", MSGDEL, (int)(players[plynum].surrender[cont] * 100.0), MSGDEL, players[plynum].attacks, MSGDEL, players[plynum].vassals.total, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "6031%c%d%c%d%c%s%c", MSGDEL, players[plynum].items[0], MSGDEL, players[plynum].remorts, MSGDEL, players[plynum].special_misc & SPC_LOCK?"Yes":"No", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void rainbow(player *plyptr)
{
	if(!titan(plyptr))
	{
		sprintf(message, "13%crainbow%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "6025%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6026%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
	send_channel(message, 0);
}

void reset(char *name, char *cmd, char *arg, char *arg2, player *plyptr)
{
	char new[64] = {0};
	char old[64] = {0};
	char str[16] = {0};
	char value[][16] = 
	{
		"attack", "login", "password", "ally", "tavern", "class",
		"protection", "lord", "name", "vampire", "promotion",
		"hero", "ops", "socket", "trooptype", "attacktime", "vassals",
		"cont", "tax", "items", "market", "fleet", "formation", 
		"specials", "bounty", "rank", "all_items", "vacation", 
		"kingdoms", "today", "contspc", "agewon", ""
	};
	int count;
	int ctynum;
	int hero;
	int mrknum;
	int num;
	int plynum = 0;
	int rnum = 0;
	int troop_num;
	int trooptype;

	if(!demigod(plyptr))
	{
		sprintf(message, "13%creset%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	while((strlen(value[rnum])) > 0)
	{
		if((STRCASECMP(cmd, value[rnum])) == 0)
			break;
		++rnum;
	}
	if(strlen(cmd) < 1 || strlen(value[rnum]) < 1)
	{
		sprintf(message, "999%cYou may reset one of the following:", MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "999%c", MSGDEL);
		count = 0;
		while((strlen(value[count])) > 0)
			sprintf(message, "%s%s ", message,  value[count++]);
		send_message(message, plyptr->name, 1);
		sprintf(message, "999%cUsage: reset <ITEM> <NICK> [ARGUMENT]", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(rnum)
	{
		case 9:
		case 11:
		case 20:
		case 23:
		case 25:
		case 26:
		case 29:
		case 30:
		case 31:
			break;
		default:
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
	}
	switch(rnum)
	{
		case 0:
			players[plynum].attacks = 0;
			sprintf(message, "999%cAttacks set to 0.", MSGDEL);
			break;
		case 1:
			players[plynum].trynum = 0;
			sprintf(message, "999%cValidation attempts reset to 0.", MSGDEL);
			break;
		case 2:
			if(demigod(&players[plynum]))
			{
				sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
				send_message(message, plyptr->name, 1);
				return;
			}
			if(strlen(arg) < 1 || strlen(arg) > 32)
				strcpy(players[plynum].password, "None");
			else
				strcpy(players[plynum].password, arg);
			sprintf(message, "999%cPassword set to \"%s\".", MSGDEL, players[plynum].password);
			break;
		case 3:
			break_alliance(&players[plynum]);
			sprintf(message, "999%cAlliance broken.", MSGDEL);
			break;
		case 4:
			players[plynum].tavern[players[plynum].cont] = 0;
			sprintf(message, "999%cTavern visits reset to 0.", MSGDEL);
			break;
		case 5:
			if(emperor(players[plynum].name))
			{
				sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
				send_message(message, plyptr->name, 1);
				return;
			}
			num = atoi(arg);
			if(strlen(arg) < 1 || num < 0 || num > 60 || strlen(classnames[num]) < 1)
			{
				sprintf(message, "999%cInvalid assignment for class.", MSGDEL);
				send_message(message, plyptr->name, 1);
				for(count = 0; count < 70; ++count)
				{
					if((strlen(classnames[count])) < 1)
						continue;
					sprintf(message, "999%c%d = %s %s", MSGDEL, count, select_class(count), select_classname(BASE_CLASSNUM(count)));
					send_message(message, plyptr->name, 1);
				}
				return;
			}
			if(vampire(&players[plynum]))
				strcpy(players[plynum].nick, players[plynum].name);
			--totply[players[plynum].classnum];
			players[plynum].classnum = num;
			++totply[players[plynum].classnum];
			if((hero = find_hero(&players[plynum])) >= 0 && !hero_serve(hero, &players[plynum]))
				dismiss(&players[plynum]);
			if(!check_hero(HERO_NECROMANCER, &players[plynum]))
				players[plynum].troop_type = troop_type(num);
			for(num = 0; num < num_conts; ++num)
				default_formation(num, &players[plynum]);
			sprintf(message, "999%cClass set to %s %s.", MSGDEL, select_class(players[plynum].classnum), select_classname(BASE_CLASSNUM(players[plynum].classnum)));
			break;
		case 6:
			players[plynum].isprotected = players[plynum].isprotected * -1 + 1;
			sprintf(message, "999%cProtection toggled.", MSGDEL);
			break;
		case 7:
			num = atoi(arg);
			if(players[num].vassals.total < MAXVAS)
			{
				remove_vassal(&players[plynum]);
				players[plynum].lord = num;
				add_vassal(num, &players[plynum]);
				sprintf(message, "999%cLord set to %s.", MSGDEL, players[num].name);
			}
			else
				sprintf(message, "999%c%s already has %d Vassals.", MSGDEL, players[num].name, MAXVAS);
			break;
		case 8:
			if(emperor(players[plynum].name))
			{
				sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
				send_message(message, plyptr->name, 1);
				return;
			}
			if((find_player(arg)) > 0)
			{
				sprintf(message, "999%cThe name \"%s\" is already taken.", MSGDEL, arg);
				send_message(message, plyptr->name, 1);
				return;
			}
			sprintf(old, "logs/%s", players[plynum].name);
			sprintf(new, "logs/%s", arg);
			rename(old, new);
			sprintf(old, "logs/%s.msg", players[plynum].name);
			sprintf(new, "logs/%s.msg", arg);
			rename(old, new);
			strcpy(players[plynum].name, arg);
			strcpy(players[plynum].nick, arg);
			sprintf(message, "999%cName changed to \"%s\".", MSGDEL, arg);
			break;
		case 9:
			num = atoi(name);
			if(num < 1)
				strcpy(str, "???");
			else
				strcpy(str, players[num].name);
			conq_info.vampire = num;
			sprintf(message, "999%c%s set to %s.", MSGDEL, select_classname(VAMPIRE), str);
			break;
		case 10:
			if(players[plynum].promotion[players[plynum].cont] == 0)
				players[plynum].promotion[players[plynum].cont] = 1;
			else
				players[plynum].promotion[players[plynum].cont] = 0;
			sprintf(message, "999%cPromotion toggled.", MSGDEL);
			break;
		case 11:
			if((num = find_heroname(name)) >= 0)
			{
				heroes[num].owner = 0;
				save_hero();
				sprintf(message, "999%c%s reset.", MSGDEL, heroes[num].name);
			}
			else
			{
				sprintf(message, "999%cNo such hero \"%s\".", MSGDEL, name);
				send_message(message, plyptr->name, 1);
				return;
			}
			break;
		case 12:
			if(players[plynum].special_misc & SPC_OPS)
				players[plynum].special_misc -= SPC_OPS;
			else
				players[plynum].special_misc += SPC_OPS;
			sprintf(message, "999%c%s's ops set to %s.", MSGDEL, players[plynum].name, players[plynum].special_misc & SPC_OPS?"ON":"OFF");
			break;
		case 13:
			players[plynum].sock = INVALID_SOCKET;
			sprintf(message, "999%c%s's socket reset.", MSGDEL, players[plynum].name);
			break;
		case 14:
			players[plynum].troop_type = troop_type(players[plynum].classnum);
			sprintf(message, "999%c%s's troop type reset.", MSGDEL, players[plynum].name);
			break;
		case 15:
			players[plynum].can_attack = 0;
			sprintf(message, "999%c%s's attack time reset.", MSGDEL, players[plynum].name);
			break;
		case 16:
			remove_all_vassal(&players[plynum]);
			for(count = 0; count < MAXVAS; ++count)
				players[plynum].vassals.plynum[count] = 0;
			players[plynum].vassals.total = 0;
			sprintf(message, "999%c%s's vassals reset.", MSGDEL, players[plynum].name);
			break;
		case 17:
			num = atoi(arg);
			if(num < 0 || num >= num_conts)
			{
				sprintf(message, "999%cInvalid assignment for continent.", MSGDEL);
				send_message(message, plyptr->name, 1);
				for(count = 0; count < num_conts; ++count)
				{
					sprintf(message, "999%c%d = %s", MSGDEL, count, conts[count].name);
					send_message(message, plyptr->name, 1);
				}
				return;
			}
			if(players[plynum].level[num] >= 0)
			{
				players[plynum].cont = num;
				players[plynum].city = conts[num].port;
				sprintf(message, "999%c%s's continent set to %s.", MSGDEL, players[plynum].name, conts[num].name);
			}
			else
				sprintf(message, "999%cInvalid continent for %s.", MSGDEL, players[plynum].name);
			break;
		case 18:
			if(players[plynum].special_kingdom & spctax[players[plynum].cont])
				players[plynum].special_kingdom -= spctax[players[plynum].cont];
			sprintf(message, "999%c%s tax adjustment reset.", MSGDEL, players[plynum].name);
			break;
		case 19:
			remove_all_items(&players[plynum]);
			sprintf(message, "999%c%s's items reset.", MSGDEL, players[plynum].name);
			break;
		case 20:
			if((ctynum = find_city(name)) < 0)
			{
				sprintf(message, "2%c%s%c", MSGDEL, name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return;
			}
			for(mrknum = 0; mrknum < MAXMRK; ++mrknum)
			{
				trooptype = cities[ctynum].troop_type[mrknum];
				troop_num = cities[ctynum].troop_num[mrknum];
				if((STRCASECMP(arg, units[trooptype][troop_num].name)) == 0)
					break;
			}
			if(mrknum == MAXMRK)
			{
				sprintf(message, "999%c\"%s\" market not found in %s.", MSGDEL, arg, cities[ctynum].name);
				send_message(message, plyptr->name, 1);
				return;
			}
			for(trooptype = FIGHTER; trooptype < TITAN; ++trooptype)
			{
				for(troop_num = 0; troop_num < MAXTRP; ++troop_num)
					if((STRCASECMP(arg2, units[trooptype][troop_num].name)) == 0)
						break;
				if(troop_num < MAXTRP)
					break;
			}
			if(trooptype == TITAN)
			{
				sprintf(message, "999%cUnknown market type \"%s\".", MSGDEL, arg2);
				send_message(message, plyptr->name, 1);
				return;
			}
			cities[ctynum].troop_type[mrknum] = trooptype;
			cities[ctynum].troop_num[mrknum] = troop_num;
			sprintf(message, "999%cMarket changed to %s.", MSGDEL, units[trooptype][troop_num].name);
			save_city(ctynum);
			break;
		case 21:
			players[plynum].fleet = 0;
			for(count = 0; count < MAXFLT; ++count)
				if(players[plynum].ships[count].type > 0)
					++players[plynum].fleet;
			sprintf(message, "999%c%s's fleet set to %d.", MSGDEL, players[plynum].name, players[plynum].fleet);
			break;
		case 22:
			default_formation(players[plynum].cont, &players[plynum]);
			sprintf(message, "999%c%s's formation restored to default.", MSGDEL, players[plynum].name);
			break;
		case 23:
			for(count = 1; count < current_player; ++count)
			{
				if(strlen(players[count].name) < 1)
					continue;
				players[count].special_misc = 0;
				players[count].special_misc += SPC_TIP;
				players[count].special_misc += SPC_OPS;
				players[count].special_misc += SPC_MESSAGES;
			}
			sprintf(message, "999%cALL specials reset!", MSGDEL);
			break;
		case 24:
			check_bounty(players[plynum].cont, plynum, plyptr);
			sprintf(message, "999%cAll bounties on %s for %s cleared.", MSGDEL, players[plynum].name, conts[players[plynum].cont].name);
			break;
		case 25:
			num = atoi(name);
			if(num < 1 || num > MAXRNK)
			{
				sprintf(message, "999%cPlease enter a rank between 1 and %d.", MSGDEL, MAXRNK);
				send_message(message, plyptr->name, 1);
				return;
			}
			conq_info.ranks[num - 1] = 0;
			sort_rank();
			save_info();
			sprintf(message, "999%cRank #%d reset.", MSGDEL, num);
			break;
		case 26:
			for(count = 1; count < current_player; ++count)
			{
				if(strlen(players[count].name) < 1)
					continue;
				players[count].items[0] = 0;
				for(num = 1; num < TOTITM; ++num)
					if(has_item(num, &players[count]))
						++players[count].items[0];
			}
			sprintf(message, "999%cALL items checked.", MSGDEL);
			break;
		case 27:
			if(!emperor(plyptr->name))
			{
				sprintf(message, "999%cOnly Emperors may reset vacations.", MSGDEL);
				send_message(message, plyptr->name, 1);
				return;
			}
			if(!(players[plynum].special_misc & SPC_VACATION))
			{
				sprintf(message, "999%c%s is not on vacation.", MSGDEL, players[plynum].name);
				send_message(message, plyptr->name, 1);
				return;
			}
			players[plynum].special_misc -= SPC_VACATION;
			players[plynum].vacation = 0;
			sprintf(message, "999%cVacation for %s removed.", MSGDEL, players[plynum].name);
			break;
		case 28:
			players[plynum].kingdoms = 0;
			for(count = 0; count < num_conts; ++count)
				if(players[plynum].level[count] >= 0)
					++players[plynum].kingdoms;
			sprintf(message, "999%cKingdoms set to %d.", MSGDEL, players[plynum].kingdoms);
			break;
		case 29:
			if((num = atoi(name)) < 0)
			{
				sprintf(message, "999%cInvalid date", MSGDEL);
				send_message(message, plyptr->name, 1);
				return;
			}
			conq_info.today = num;
			sprintf(message, "999%cDate set to %d.", MSGDEL, conq_info.today);
			save_info();
			break;
		case 30:
			conq_info.cont_special[plyptr->cont] = 0;
			save_info();
			sprintf(message, "999%cAll specials for %s removed.", MSGDEL, conts[plyptr->cont].name);
			break;
		case 31:
			conq_info.agewon = 0;
			save_info();
			sprintf(message, "999%cAge won reset to 0.", MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	sprintf(message, "%s %s %s reset %s: %s %s %s", NOWSTR, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, cmd, name, arg, arg2);
	add_log(EMPLOG, message);
	switch(rnum)
	{
		case 9:
		case 11:
		case 20:
		case 23:
		case 25:
		case 26:
		case 29:
		case 30:
		case 31:
			break;
		default:
			save_player(plynum);
			break;
	}
}

void set_event(char *name, char *arg, player *plyptr)
{
	char events[][32] = 
	{
		"None", "Wealthy Merchant", "Wandering Soldiers", "Recover Food",
		"Repair Catapult", "Nomadic Peasants", "Explorers",
		"Find Barge", "Honor", "Find Castle", "Black Death", 
		"Locusts", "Peasant Uprising", "Arson", "Thieves", "Sickness"
	};
	int count;
	int plynum;
	int type;

	if(!demigod(plyptr))
	{
		sprintf(message, "13%cevent%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1 || strlen(arg) < 1)
	{
		sprintf(message, "999%cUsage: EVENT <NICK> <EVENT NUMBER>", MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 0; count < 16; ++count)
		{
			sprintf(message, "999%c%d: %s", MSGDEL, count, events[count]);
			send_message(message, plyptr->name, 1);
		}
		return;
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	type = atoi(arg);
	if(type < 0 || type > 15)
	{
		sprintf(message, "999%cInvalid event number.", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	players[plynum].event[players[plynum].cont] = type;
	save_player(plynum);
	sprintf(message, "999%cDuring the next HK, the event \"%s\" will affect %s.", MSGDEL, events[type], players[plynum].name);
	send_message(message, plyptr->name, 1);
	sprintf(message, "%s %s %s set %s's event to \"%s\".", NOWSTR, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, players[plynum].name, events[type]);
	add_log(EMPLOG, message);
}

void show_player(player *plyptr)
{
	int count;

	if(!titan(plyptr))
	{
		sprintf(message, "13%cshowplayer%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "6033%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1); 
	for(count = 1; count < current_player; ++count)
	{
		if(strlen(players[count].name) < 1)
			continue;
		sprintf(message, "6034%c%03d%c%16s%c%02d%c%s%c", MSGDEL, 
		count, MSGDEL, players[count].name, MSGDEL, players[count].levelmax + 1, MSGDEL, players[count].host, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void show_vampire(player *plyptr)
{
	if(!titan(plyptr))
	{
		sprintf(message, "13%cshowvamp%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(conq_info.vampire == 0)
	{
		sprintf(message, "3058%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	} 
	sprintf(message, "999%c%s Guise: %s", MSGDEL, select_classname(VAMPIRE), players[conq_info.vampire].name);
	send_message(message, plyptr->name, 1);
}

void shut_down(player *plyptr)
{
	if(!emperor(plyptr->name))
	{
		sprintf(message, "13%cshutdown%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	fprintf(stderr, "%s SHUTDOWN Initiated by %s.\n", NOWSTR, plyptr->name);
	sprintf(message, "999%cShutdown initiated by %s...", MSGDEL, plyptr->name);
	send_channel(message, 1);
	exit_conquest();
}

void tangent(char *name, char *str_offset, player *plyptr)
{
	int cont;
	int offset;
	int plynum;

	if(!titan(plyptr))
	{
		sprintf(message, "13%ctangent%c", MSGDEL, MSGDEL);
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
	offset = atoi(str_offset);
	cont = players[plynum].cont;
	if(titan(&players[plynum]))
	{
		sprintf(message, "999%cYou may not use this command on %s.", MSGDEL, players[plynum].name);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(players[plynum].level[cont] + offset < 0 || players[plynum].level[cont] + offset >= maxlevel[BASE_CLASSNUM(players[plynum].classnum)])
	{
		sprintf(message, "999%cInvalid value for promotion/demotion.", MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	change_level(cont, offset, &players[plynum]);
	if(offset > 0)
	{
		sprintf(message, "6000%c%s%c%s%c", MSGDEL, select_title(players[plynum].classnum, players[plynum].level[cont], players[plynum].gender), MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		sprintf(message, "6005%c%s%c%s%c%s%c", MSGDEL, select_class(players[plynum].classnum), MSGDEL, players[plynum].nick, MSGDEL, select_title(players[plynum].classnum, players[plynum].level[cont], players[plynum].gender), MSGDEL);
		send_channel(message, 0);
	}
	else
	{
		sprintf(message, "6027%c%s%c%s%c", MSGDEL, players[plynum].nick, MSGDEL, 
		select_title(players[plynum].classnum, players[plynum].level[cont], players[plynum].gender), MSGDEL);
		send_channel(message, 0);
	}
	promote_max(cont, &players[plynum]);
	save_player(plynum);
	if(plyptr->level[plyptr->cont] >= VASRANK)
		remove_vassal(plyptr);
	sprintf(message, "999%c%s %s to %s.", MSGDEL, players[plynum].name, offset < 0?"demoted":"promoted", select_title(players[plynum].classnum, players[plynum].level[cont], players[plynum].gender));
	send_message(message, plyptr->name, 1);
	sprintf(message, "%s %s %s %s %s to %s.", NOWSTR, 
	select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, players[plynum].name,
	offset < 0?"demoted":"promoted", select_title(players[plynum].classnum, players[plynum].level[cont], players[plynum].gender));
	add_log(EMPLOG, message);
}

void thunder(player *plyptr)
{
	if(!titan(plyptr))
	{
		sprintf(message, "13%cthunder%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "6028%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6029%c%s%c%s%c%s%c%s%c", MSGDEL, 
	structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, plyptr->nick, MSGDEL, plyptr->gender == 0?"god":"goddess", MSGDEL, select_classname(BASE_CLASSNUM(plyptr->classnum)), MSGDEL);
	send_channel(message, 0);
	sprintf(message, "6030%c%s%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
}
