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

void add_bg(int cont, int mult, player *plyptr)
{
	int amount;

	amount = (plyptr->level[cont] + 1) * mult;
	if((amount = fill_castles(amount, cont, plyptr)) <= 0)
		return;
	plyptr->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] += amount;
	switch(plyptr->troop_type)
	{
		case BARBARIAN:
			sprintf(message, "15044%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case CLERIC:
			sprintf(message, "13000%c%s%c%d%c%s%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case FIGHTER:
		case TITAN:
			sprintf(message, "13000%c%s%c%d%c%s%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case MAGE:
			sprintf(message, "13000%c%s%c%d%c%s%c%s%c", MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case RANGER:
			sprintf(message, "16021%c%d%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL);
			break;
		case VAMPIRE:
			sprintf(message, "15045%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	send_message(units[plyptr->troop_type][BLOODGUARDNUM].desc, plyptr->name, 1);
}

void add_host(char *host)
{
	char *ptr;
	hosts *next;
	hosts *new;
	hosts *prev;

	if((new = (hosts *)malloc(HSTSIZ)) == NULL)
	{
		fprintf(stderr, "%s %s: %s\n", NOWSTR, ERRMSG1, strerror(errno));
		return;
	}
	new->host = strdup(host);
        if((ptr = strrchr(new->host, '.')) != NULL)
                *(ptr + 1) = 0;
	new->next = NULL;
	if(!hostshead)
	{
		hostshead = new;
		return;
	}
	next = hostshead->next;
	prev = hostshead;
	while(next != NULL)
	{
		prev = next;
		next = next->next;
	}
	prev->next = new;
}

void add_log(char *file, char *msg)
{
	FILE *fp;

	if((fp = fopen(file, "a")) == NULL)
		return;
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}

int add_player(char *name, char *host, int classnum, int cont, int lord)
{
	int count;
	int plynum;

	plynum = find_slot();
	if(plynum == current_player)
		++current_player;
	if(plynum == total_player)
	{
		total_player += INCREMENT;
		if((players = (player *)realloc(players, total_player * PLYSIZ)) == NULL)
		{
			fprintf(stderr, "%s %s:%s\n", NOWSTR, ERRMSG1, strerror(errno));
			exit_conquest();
		}
		memset(&players[plynum], 0, INCREMENT * PLYSIZ);
	}
	else
		memset(&players[plynum], 0, PLYSIZ);
	if(sockout == INVALID_SOCKET)
		players[plynum].sock = INVALID_SOCKET;
	else
		players[plynum].sock = sockets[sockout].sock;
	strcpy(players[plynum].name, name);
	strcpy(players[plynum].nick, name);
	sprintf(players[plynum].password, "%s%d", DFTPWD, randomnum(9999, 1000));
	strncpy(players[plynum].host, host, 127);
	add_host(players[plynum].host);
	strcpy(players[plynum].winmsg, dft_winmsg);
	players[plynum].can_attack = time(NULL);
	if(classnum < 0)
		players[plynum].classnum = randomnum(total_races[FIGHTER] - 1, 0);
	else
		players[plynum].classnum = classnum;
	++totply[players[plynum].classnum];
	if(cont < 0)
	{
		if(force_cont[players[plynum].classnum] >= 0)
			cont = force_cont[players[plynum].classnum];
		else
		{
			if(associate_races == 1)
				cont = players[plynum].classnum;
			else
				cont = randomnum(num_conts - 1, 0);
		}
	}
	players[plynum].cont = cont;
	players[plynum].kingdoms = 1;
	for(count = 0; count < num_conts; ++count)
		players[plynum].level[count] = -1;
	if(classnum == DEMIGOD)
		players[plynum].level[cont] = maxlevel[BASE_CLASSNUM(classnum)];
	else
		players[plynum].level[cont] = 0;
	++totlvl[cont][players[plynum].level[cont]];
	players[plynum].land[cont] = 100;
	players[plynum].copper[cont] = 15000;
	players[plynum].tax[cont] = 2;
	players[plynum].peasant[cont] = 100;
	players[plynum].food[cont] = 100;
	players[plynum].movepts = 30;
	do {
		players[plynum].city = conts[cont].cities[randomnum(conts[cont].totalcty - 1, 0)];
	} while(players[plynum].city == secret_city && conts[cont].totalcty > 1);
	players[plynum].troop_type = troop_type(classnum);
	players[plynum].first_play[cont] = conq_info.today;
	players[plynum].lastplay = time(NULL);
	players[plynum].special_misc += SPC_OPS;
	players[plynum].special_misc += SPC_TIP;
	players[plynum].special_misc += SPC_MESSAGES;
	players[plynum].spy[cont] = -1;
	default_formation(cont, &players[plynum]);
	players[plynum].army[ARMY_DEFENSE][cont].total[SOLDIERNUM] += 50;
	players[plynum].army[ARMY_CAMPAIGN][cont].total[SOLDIERNUM] += 50;
	if(units[players[plynum].troop_type][CATAPULTNUM].attack > 0)
		players[plynum].army[ARMY_CAMPAIGN][cont].total[CATAPULTNUM] = 1;
	players[plynum].castle[cont] = combined_army(cont, &players[plynum]) /castle_max(players[plynum].classnum);
	if(combined_army(cont, &players[plynum]) % castle_max(players[plynum].classnum) > 0)
		++players[plynum].castle[cont];
	players[plynum].validate = VAL_NORMAL;
	if(lord >= 0)
	{
		players[plynum].lord = lord;
		add_vassal(lord, &players[plynum]);
		switch(BASE_CLASSNUM(classnum))
		{
			case FIGHTER:
				players[plynum].copper[cont] += 10000;
				sprintf(message, "13001%c%s%c10000%c%s%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, money_name, MSGDEL);
				break;
			case MAGE:
				players[plynum].food[cont] += 100;
				sprintf(message, "13001%c%s%c100%c%s%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, food_name, MSGDEL);
				break;
			case VAMPIRE:
				++players[plynum].castle[cont];
				sprintf(message, "13001%c%s%c1%c%s%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL);
				break;
			case CLERIC:
				players[plynum].peasant[cont] += 25;
				sprintf(message, "13001%c%s%c25%c%ss%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, peasant_name, MSGDEL);
				break;
			case RANGER:
				players[plynum].army[ARMY_DEFENSE][cont].total[ARCHERNUM] += 25;
				sprintf(message, "13001%c%s%c25%c%ss%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, units[players[plynum].troop_type][ARCHERNUM].name, MSGDEL);
				break;
			default:
				players[plynum].army[ARMY_CAMPAIGN][cont].total[SOLDIERNUM] += 25;
				sprintf(message, "13001%c%s%c25%c%ss%c", MSGDEL, adjectives[players[lord].gender][4], MSGDEL, MSGDEL, units[players[plynum].troop_type][SOLDIERNUM].name, MSGDEL);
				break;
		}
		send_message(message, players[plynum].name, 0);
	}
	players[plynum].surrender[cont] = (float)dft_surrender;
	strcpy(players[plynum].kname[cont], dft_kname);
	save_player(plynum);
	return(plynum);
}

void add_ret(int plynum, player *plyptr)
{
	int pos;

	if((pos = find_ret(plynum, plyptr)) < 0)
	{
		for(pos = 0; pos < MAXRET; ++pos)
			if(plyptr->ret[pos].plynum == 0)
				break;
		if(pos == MAXRET)
			return;
		plyptr->ret[pos].plynum = plynum;
	}
	++plyptr->ret[pos].attacks;
}

int can_ret(player *attacker, player *defender)
{
	int pos;

	if((pos = find_ret(playernum(defender), attacker)) < 0)
		return(0);
	if(attacker->ret[pos].attacks >= 1)
		return(1);
	else
		return(0);
}

int castle_max(int classnum)
{
	return(structures[BASE_CLASSNUM(classnum)].troops);
}

int change_class(int newclass, player *plyptr)
{
	int offset;

	offset = newclass * 10;
	if((strlen(classnames[plyptr->classnum + offset])) > 0)
		return(offset + plyptr->classnum);
	else
		return(randomnum(offset + total_races[newclass] - 1, offset));
}

void change_level(int cont, int offset, player *plyptr)
{
	--totlvl[cont][plyptr->level[cont]];
	plyptr->level[cont] += offset;
	update_levelmax(plyptr);
	++totlvl[cont][plyptr->level[cont]];
}

int check_artifact(int artnum, player *plyptr)
{
	if(!summoned(plyptr) && artifacts[artnum].owner == playernum(plyptr))
		return(1);
	else
		return(0);
}

void check_assets(int cont, int level, player *plyptr)
{
	if(plyptr->land[cont] > promotions[BASE_CLASSNUM(plyptr->classnum)][level][1])
		plyptr->land[cont] = promotions[BASE_CLASSNUM(plyptr->classnum)][level][1];
	if(plyptr->peasant[cont] > promotions[BASE_CLASSNUM(plyptr->classnum)][level][2])
                plyptr->peasant[cont] = promotions[BASE_CLASSNUM(plyptr->classnum)][level][2];
}

int check_buy(int amount, int type, player *plyptr)
{
	int castles;
	int ok = 1;

	switch(type)
	{
		case RENNUM:
		case SOLDIERNUM:
		case ARCHERNUM:
		case KNIGHTNUM:
		case BLOODGUARDNUM:
			if(combined_army(plyptr->cont, plyptr) + amount > plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum))
			{
				sprintf(message, "31%c%s%c%d%c%d%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum), MSGDEL, combined_army(plyptr->cont, plyptr), MSGDEL);
				send_message(message, plyptr->name, 1);
				ok = 0;
			}
			break;
		case CATAPULTNUM:
			break;
		case 9:		// Castles
			if((plyptr->castle[plyptr->cont] + amount) * structures[BASE_CLASSNUM(plyptr->classnum)].land > plyptr->land[plyptr->cont])
			{
				castles = plyptr->land[plyptr->cont] / structures[BASE_CLASSNUM(plyptr->classnum)].land - plyptr->castle[plyptr->cont];
				if(castles < 0)
					castles = 0;
				sprintf(message, "34%c%s%c%d%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, castles, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				ok = 0;
			}
			break;
		default:
			break;
	}
	return(ok);
}

int check_file(char *ext, char *name)
{
	char fname[64] = {0};
	struct stat finfo;

	sprintf(fname, "logs/%s%s", name, ext);
	if((stat(fname, &finfo)) == -1)
		return(0);
	if(finfo.st_size < LOGSIZ)
		return(0);
	else
		return(finfo.st_size / LOGSIZ);
}

int check_password(char *name, char *word)
{
	int count;
	int has_digit = 0;
	int has_lower = 0;
	int has_upper = 0;

	for(count = 0; count < (int)strlen(word); ++count)
	{
		if(isdigit(word[count]))
			has_digit = 1;
		if(isalpha(word[count]))
		{
			if(isupper(word[count]))
				has_upper = 1;
			else
				has_lower = 1;
		}
	}
	if(has_digit == 0 || has_lower == 0 || has_upper == 0)
	{
		sprintf(message, "4353%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 1);
		return(0);
	}
	return(1);
}

void check_rank(int plynum)
{
	int cnt, count;

	if(titan(&players[plynum]))
		return;
	for(cnt = 0; cnt < MAXRNK; ++cnt)
		if(conq_info.ranks[cnt] == plynum)
			return;
	for(cnt = 0; cnt < MAXRNK; ++cnt)
	{
		if(players[plynum].battles_won > 0 && (players[plynum].battles_won > players[conq_info.ranks[cnt]].battles_won || titan(&players[conq_info.ranks[cnt]])))
		{
			if(!titan(&players[conq_info.ranks[cnt]]))
				for(count = MAXRNK - 1; count > cnt; --count)
					conq_info.ranks[count] = conq_info.ranks[count - 1];
			conq_info.ranks[cnt] = plynum;
			break;
		}
	}
}

void check_ret(int cont, player *attacker, player *defender)
{
	if(defender->level[cont] - attack_level(cont, defender) > attacker->level[cont])
		add_ret(playernum(attacker), defender);
}

int check_sell(int amount, int type, player *plyptr)
{
	int total;

	switch(type)
	{
		case 1:
			if(plyptr->castle[plyptr->cont] <= amount)
			{
				sprintf(message, "13005%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			}
			if(plyptr->land[plyptr->cont] < amount * structures[BASE_CLASSNUM(plyptr->classnum)].land)
			{
				sprintf(message, "13006%c%d%c%s%c", MSGDEL, amount, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			}
			total = amount;
			while(combined_army(plyptr->cont, plyptr) > castle_max(plyptr->classnum) * (plyptr->castle[plyptr->cont] - amount) && amount > 0)
				--amount;
			if(total != amount)
			{
				sprintf(message, "13007%c%d%c%s%c", MSGDEL, amount, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			}
		default:
			break;

	}
	return(1);
}

void check_troops(player *plyptr)
{
	int cont;
	int num;

	for(cont = 0; cont < num_conts; ++cont)
	{
		default_formation(cont, plyptr);
		for(num = 0; num < MAXTRP; ++num)
			if(units[plyptr->troop_type][num].attack == 0 && units[plyptr->troop_type][num].defend == 0)
			{
				plyptr->army[ARMY_DEFENSE][cont].total[num] = 0;
				plyptr->army[ARMY_CAMPAIGN][cont].total[num] = 0;
			}
	}
}

int check_vacation(char *name, player *plyptr)
{
	char str[TIMESIZ] = {0};

	if(plyptr->special_misc & SPC_VACATION)
	{
		strftime(str, TIMESIZ, TIMEFORMAT, localtime(&(plyptr->vacation)));
		sprintf(message, "4360%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, str, MSGDEL);
		send_message(message, name, 1);
		return(1);
	}
	return(0);
}

int check_word(char *name, char *word, int max, int min)
{
	char *cptr = NULL;
	char input[16] = {0};
	char notallowed[][16] = 
	{
		"ass", "balls", "cunt", "fag", "dick", "penis", 
		"pussy", "tits", "fuck", "whore", "slut", "bitch", 
		"shit", "cock", "prick", ""
	};
	char str[256] = {0};
	int count;
	int pos;
	int slen;

	strncpy(str, word, 255);
	strip(str);
	if((slen = strlen(str)) < min || slen > max)
	{
		sprintf(message, "13008%c%d%c%d%c", MSGDEL, min, MSGDEL, max, MSGDEL);
		send_message(message, name, 0);
		return(0);
	}
	for(count = 0, pos = 0; count < slen; ++count)
		if(isprint(word[count]))
			str[pos++] = tolower(word[count]);
		else
			if(word[count] == CNTLC)
				count += 2;
	str[pos] = '\0';
	count = 0;
	while((strlen(notallowed[count])) > 0)
	{
		if((cptr = strstr(str, notallowed[count])) != NULL)
		{
			strncpy(input, cptr, strlen(notallowed[count]));
			sprintf(message, "13009%c%s%c", MSGDEL, input, MSGDEL);
			send_message(message, name, 0);
			return(0);
		}
		++count;
	}
	return(1);
}

void closed(char *str, int num)
{
	memset(str, 0, 16);
	if(num > 0)
		sprintf(str, "%d", num);
	else
		sprintf(str, "[-closed-]");
}

int cmp_command(const void *ptr1, const void *ptr2)
{
	return(STRCASECMP(((commandlist *)ptr1)->cmdname, ((commandlist *)ptr2)->cmdname));
}

void default_formation(int cont, player *plyptr)
{
	int rank0 = 0;
	int rank1 = 0;
	int ranknum;
	int troopnum;

	for(ranknum = 0; ranknum < MAXTRP; ++ranknum)
		for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
		{
			plyptr->army[ARMY_DEFENSE][cont].formation[ranknum][troopnum] = -1;
			plyptr->army[ARMY_CAMPAIGN][cont].formation[ranknum][troopnum] = -1;
		}
	for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
	{
		if(units[plyptr->troop_type][troopnum].attack < 1 && units[plyptr->troop_type][troopnum].defend < 1)
			continue;
		if(units[plyptr->troop_type][troopnum].range == 0 && troopnum != SOLDIERNUM && troopnum != CATAPULTNUM)
		{
			plyptr->army[ARMY_DEFENSE][cont].formation[0][rank0] = troopnum;
			plyptr->army[ARMY_CAMPAIGN][cont].formation[0][rank0++] = troopnum;
		}
		else
		{
			plyptr->army[ARMY_DEFENSE][cont].formation[1][rank1] = troopnum;
			plyptr->army[ARMY_CAMPAIGN][cont].formation[1][rank1++] = troopnum;
		}
	}
}

void delete_player(int plynum)
{
	char fname[64];
	int artnum;
	int count;

	if((count = find_hero(&players[plynum])) >= 0)
	{
		heroes[count].owner = 0;
		save_hero();
		sprintf(message, "13010%c%s%c%s%c%s%c", MSGDEL, heroes[count].type, MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, players[plynum].name, MSGDEL);
		send_channel(message, 0);
	}
	if(conq_info.vampire == plynum)
	{
		if(mult_vampires == 1)
			destroy_vampires(&players[plynum]);
		conq_info.vampire = 0;
		if(heroes[HERO_DESTROYER].owner != 0)
		{
			make_vampire(&players[heroes[HERO_DESTROYER].owner]);
			save_player(heroes[HERO_DESTROYER].owner);
		}
	}
	--totply[players[plynum].classnum];
	if(players[plynum].ally > 0)
	{
		sprintf(message, "13024%c%c", MSGDEL, MSGDEL);
		log_event("", message, players[players[plynum].ally].name);
		players[players[plynum].ally].ally = 0;
		save_player(players[plynum].ally);
	}
	if((artnum = find_artifact(&players[plynum])) >= 0)
	{
		artifacts[artnum].owner = 0;
		save_art();
	}
	remove_bounty(plynum);
	remove_host(players[plynum].host);
	remove_vassal(&players[plynum]);
	if(players[plynum].vassals.total > 0)
		remove_all_vassal(&players[plynum]);
	reset_ally(plynum);
	reset_ret(plynum);
	reset_spy(0, plynum);
	for(count = 0; count < num_conts; ++count)
		if(players[plynum].level[count] >= 0)
			--totlvl[count][players[plynum].level[count]];
	for(count = 0; count < MAXRNK; ++count)
		if(conq_info.ranks[count] == plynum)
		{
			conq_info.ranks[count] = 0;
			break;
		}
	if(players[plynum].remorts > 1)
		lose_exp(&players[plynum]);
	else
	{
		sprintf(fname, "logs/%s", players[plynum].name);
		remove(fname);
		sprintf(fname, "logs/%s.msg", players[plynum].name);
		remove(fname);
	}
	sort_rank();
	save_info();
	memset(&players[plynum], 0, PLYSIZ);
	for(count = 0; count < num_conts; ++count)
		players[plynum].level[count] = -1;
	save_player(plynum);
}

int demigod(player *plyptr)
{
	if(plyptr->levelmax == maxlevel[BASE_CLASSNUM(plyptr->classnum)])
		return(1);
	return(0);
}

int demote(int cont, player *plyptr)
{
	if(plyptr->honor[cont] > -2 || plyptr->level[cont] == 0 || plyptr->level[cont] >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
		return(0);
	if(check_artifact(GRAIL, plyptr) || check_artifact(ROD, plyptr))
		return(0);
	plyptr->honor[cont] = 0;
	plyptr->land[cont] -= (int)(plyptr->land[cont] * .20);
	plyptr->peasant[cont] -= (int)(plyptr->peasant[cont] * .20);
	plyptr->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] = 0;
	plyptr->army[ARMY_CAMPAIGN][cont].total[BLOODGUARDNUM] = 0;
	change_level(cont, -1, plyptr);
	sprintf(message, "13011%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, 
	plyptr->nick, MSGDEL, conts[cont].name, MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL);
	send_channel(message, 0);
	sprintf(message, "13012%c%s%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "13013%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	check_vassals(plyptr);
	return(1);
}

int emperor(char *name)
{
	int count;

	for(count = 0; count < num_conts; ++count)
	{
		if((strlen(emp[count])) < 1)
			continue;
		if((STRCASECMP(name, emp[count])) == 0)
			break;
	}
	if(count == num_conts)
		return(0);
	else
		return(1);
}

int equal_class(int class1, int class2)
{
	int count;
	int exceptions[4] = {10, 30, 50, 60};
	int side1;
	int side2;

	if(class1 == class2)
		return(1);
	for(count = 0; count < 4; ++count)
	{
		if(class1 >= exceptions[count] && class1 < (exceptions[count] + 10))
		{
			if(class2 >= exceptions[count] && class2 < (exceptions[count] + 10))
				return(1);
			else
				return(0);
		}
		if(class2 >= exceptions[count] && class2 < (exceptions[count] + 10))
		{
			if(class1 >= exceptions[count] && class1 < (exceptions[count] + 10))
				return(1);
			else
				return(0);
		}
	}
	if(class1 < 10)
		side1 = class1;
	else
		side1 = class1 - (class1 / 10 * 10);
	if(class2 < 10)
		side2 = class2;
	else
		side2 = class2 - (class2 / 10 * 10);
	if(side1 == side2)
		return(1);
	return(0);
}

int fill_castles(int amount, int cont, player *plyptr)
{
	int net = amount;

	if(combined_army(cont, plyptr) + amount > plyptr->castle[cont] * castle_max(plyptr->classnum))
		net = plyptr->castle[cont] * castle_max(plyptr->classnum) - combined_army(cont, plyptr);
	return(net);
}

int find_artifact(player *plyptr)
{
	int count;
	int plynum;

	plynum = playernum(plyptr);
	for(count = 0; count < TOTART; ++count)
		if(artifacts[count].owner == plynum)
			return(count);
	return(-1);
}

int find_city(char *cityname)
{
	int count;
	int found = -1;
	int len;
	int testlen;

	len = strlen(cityname);
	for(count = 0; count < num_cities; ++count)
	{
		if(count == secret_city)
			testlen = strlen(cities[count].name);
		else
			testlen = len;
		if((STRNCASECMP(cityname, cities[count].name, testlen)) == 0)
		{
			found = count;
			break;
		}
	}
	return(found);
}

int find_class(char *classname)
{
	int troop_num = 0;
	int len;
	
	if((strlen(classname)) < 1)
		return(-1);
	len = strlen(classname);
	while((strlen(basenames[troop_num])) > 0)
	{
		if((STRNCASECMP(classname, basenames[troop_num], len)) == 0)
			return(troop_num);
		++troop_num;
	}
	return(-1);
}

int find_cont(char *contname)
{
	int cont;
	int len;

	if((strlen(contname)) < 1)
		return(-1);
	len = strlen(contname);
	for(cont = 0; cont < num_conts; ++cont)
		if((STRNCASECMP(contname, conts[cont].name, len)) == 0)
			return(cont);
	return(-1);
}

int find_distance(int from, int to, player *plyptr)
{
	int d;

	d = (int)sqrt(pow(cities[to].locx - cities[from].locx, 2) + pow(cities[to].locy - cities[from].locy, 2));
	if(ranger(plyptr) || vampire(plyptr) || check_artifact(WINGS, plyptr))
		d /= 2;
	if(d < 1)
		d = 1;
	return(d);
}

int find_host(char *host)
{
	hosts *next;
	int found = 0;

	next = hostshead;
	while(next != NULL)
	{
		if((STRNCASECMP(next->host, host, strlen(next->host))) == 0)
		{
			found = 1;
			break;
		}
		next = next->next;
	}
	return(found);
}

int find_openmarket(int ctynum)
{
	int count;

	for(count = 0; count < MAXMRK; ++count)
		if(cities[ctynum].troop_pr[count] < 1)
			return(count);
	return(-1);
}

int find_player(char *name)
{
	int count;
	int found;

	for(count = 1, found = 0; count < current_player; ++count)
		if((STRCASECMP(name, players[count].name)) == 0)
		{
			found = count;
			break;
		}
	return(found);
}

int find_randomartifact(int cont)
{
	int artnums[TOTART];
	int count;
	int pos;

	for(count = 0; count < TOTART; ++count)
		artnums[count] = -1;
	for(count = 0, pos = 0; count < TOTART; ++count)
		if(artifacts[count].city >= 0 && cities[artifacts[count].city].cont == cont)
			artnums[pos++] = count;
	return(artnums[randomnum(pos - 1, 0)]);
}

int find_randomplayer(int cont, int level)
{
	int found = -1;
	int maxtries = 0;
	int plynum;

	while(found == -1 && maxtries++ < 1000)
	{
		plynum = randomnum(current_player, 1);
		if((strlen(players[plynum].name)) > 0 && (cont == -1 || players[plynum].cont == cont) && (level == -1 || players[plynum].level[cont] >= level))
		{
			found = plynum;
			break;
		}
	}
	return(found);
}

int find_ret(int plynum, player *plyptr)
{
	int count;

	for(count = 0; count < MAXRET; ++count)
		if(plyptr->ret[count].plynum == plynum)
			break;
	if(count == MAXRET)
		return(-1);
	else
		return(count);
}

int find_slot()
{
	int count;

	for(count = 1; count < current_player; ++count)
		if(strlen(players[count].name) < 1)
			break;
	return(count);
}

void highway_robbers(char *cityname, player *plyptr)
{
	char item[16] = {0};
	int amount = 0;
	int type = 0;

	if(check_artifact(WINGS, plyptr) || ranger(plyptr) || vampire(plyptr) || has_item(ITEM_HOLDING, plyptr))
		return;
	if(plyptr->food[plyptr->cont] > MAXFOOD && plyptr->food[plyptr->cont] > plyptr->peasant[plyptr->cont])
		type = 1;
	else if(plyptr->copper[plyptr->cont] > MAXCOPPER)
		type = 2;
	else if(randomnum(100, 1) <= (plyptr->level[plyptr->cont] + 1) * 2)
		type = randomnum(2, 1);
	if(type == 0)
		return;
	if(randomnum(100, 1) <= 1)
	{
		sprintf(message, "13014%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "13015%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "13016%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type == 1)
	{
		if(plyptr->food[plyptr->cont] < 1)
			return;
		if((amount = plyptr->food[plyptr->cont] * randomnum(20, 10) / 100) < 1)
			amount = 1;
		plyptr->food[plyptr->cont] -= amount;
		strcpy(item, food_name);
	}
	else
	{
		if(plyptr->copper[plyptr->cont] < 1)
			return;
		if((amount = plyptr->copper[plyptr->cont] * randomnum(20, 10) / 100) < 1)
			amount = 1;
		plyptr->copper[plyptr->cont] -= amount;
		strcpy(item, money_name);
	}
	sprintf(message, "13017%c%s%c%d%c%s%c", MSGDEL, cityname, MSGDEL, amount, MSGDEL, item, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void hk_text(player *plyptr)
{
	sprintf(message, "13018%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "13019%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "13020%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "13021%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
}

void init()
{
        char cmds[][32] = {
		"person", "review", "done", "password", "market", "buy",
		"build", "petition", "alliance","break", "spy", "ally", "sell",
		"enlist", "attack", "max", "tavern", "travel", "log", "give",
		"quest","view", "church", "roll", "ninja", "tangent",
		"corpse", "cosine", "titan", "hk", "reset", "erase", "newtopic",
		"message", "shutdown", "bestow", "mystic", "search", "drop",
		"motd", "artifacts", "items", "use", "destroy", "abdicate",
		"rebel", "loot", "nick", "devote", "book", "cast",
		"components", "event", "join", "fleet", "loadship", "engage",
		"population", "repair", "sail", "scuttle", "unload",
		"vampire", "guise", "pestilence", "charm", "seek", "slay",
		"bless", "crusade", "divination", "divineintervention",
		"excommunicate", "grace", "resurrect", "sanctuary",
		"turn", "vocation", "serve", "shield", "summon", "tend",
		"vision", "train", "heroinfo", "dismiss", "revive",
		"heroes", "weather", "thunder", "commune", "map",
		"berserk", "rainbow", "escape", "penance", "showplayer",
		"espionage", "release", "vassals", "hklog", "showvamp","hire",
		"disband", "besiege", "formation", "reprieve", "bounty","steward",
		"invade", "ritual", "post", "lock", "vacation", "retaliation",
		"blitz", "blockade", "ceasefire", "transfer", "set", "balance", 
		"questchar", ""
	};
	int count;
	int skip = 0;

	srand((unsigned)time(NULL));
	time(&uptime);
	for(count = 1; count < MAX_SOCKETS; ++count)
	{
		msghead[count] = NULL;
		sockets[count].sock = INVALID_SOCKET;
		sockets[count].ip = NULL;
	}
	for(count = 0; count < MAXCMD; ++count)
	{
		memset(&commands[count], 0, CMDSIZ);
		if(!skip)
			if(strlen(cmds[count]) < 1)
				skip = 1;
		if(skip)
			strcpy(commands[count].cmdname, "ZZZZZZZZ");
		else
			strcpy(commands[count].cmdname, cmds[count]);
		commands[count].num = count;
	}
	qsort(&commands[0], MAXCMD, CMDSIZ, cmp_command);
	read_art();
	read_bounty();
	read_heroes();
	read_info();
	read_items();
	read_messages();
	read_promotions();
	read_riddles();
	read_spells();
	read_ships();
	sort_rank();
	for(count = 0; count < num_conts; ++count)
		change_weather(count, randomnum(W_HEAVY, W_DRY), randomnum(W_HOT, W_FREEZE), randomnum(W_GUSTY, W_STILL));
	if((strlen(log_file)) > 0)
	{
		fprintf(stderr, "%s Opening log file %s\n", NOWSTR, log_file);
		if((logfile = fopen(log_file, "a")) == NULL)
			fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, log_file, strerror(errno));
	}
}

void kia_change(int cont, char *killed, char *killer)
{
	int count;

	for(count = 9; count > 0; --count)
	{
		memset(conq_info.killed[count], 0, 32);
		memset(conq_info.killer[count], 0, 32);
		strncpy(conq_info.killed[count], conq_info.killed[count - 1], 31);
		strncpy(conq_info.killer[count], conq_info.killer[count - 1], 31);
	}
	memset(conq_info.killed[0], 0, 32);
	memset(conq_info.killer[0], 0, 32);
	strncpy(conq_info.killed[0], killed, 31);
	strncpy(conq_info.killer[0], killer, 31);
	sprintf(message, "%s %s's last %s on %s was captured by %s.", NOWSTR, killed, structures[FIGHTER].name, conts[cont].name, killer);
	add_log("logs/kia.log", message);
}

void king_pic(int classnum)
{
	int end_msg = 0;
	int start_msg = 0;

	switch(BASE_CLASSNUM(classnum))
	{
		case FIGHTER:
			start_msg = 22000;
			end_msg = 22011;
			break;
		case BARBARIAN:
			start_msg = 22012;
			end_msg = 22020;
			break;
		case MAGE:
			start_msg = 22020;
			end_msg = 22032;
			break;
		case VAMPIRE:
			start_msg = 22033;
			end_msg = 22045;
			break;
		case CLERIC:
			start_msg = 22046;
			end_msg = 22060;
			break;
		case RANGER:
			start_msg = 22061;
			end_msg = 22074;
			break;
		default:
			break;
	}
	for(; start_msg <= end_msg; ++start_msg)
	{
		sprintf(message, "%d%c%c", start_msg, MSGDEL, MSGDEL);
		send_channel(message, 0);
	}
}

void list_change(char *loser, char *winner)
{
	int count;

	for(count = 14; count > 0; --count)
	{
		memset(joustlosers[count], 0, 32);
		memset(joustwinners[count], 0, 32);
		strncpy(joustlosers[count], joustlosers[count - 1], 31);
		strncpy(joustwinners[count], joustwinners[count - 1], 31);
	}
	memset(joustlosers[0], 0, 32);
	memset(joustwinners[0], 0, 32);
	strncpy(joustlosers[0], loser, 31);
	strncpy(joustwinners[0], winner, 31);
}

void lose_exp(player *plyptr)
{
	int classnum;
	int plynum;

	if(vampire(plyptr) || titan(plyptr))
		classnum = randomnum(total_races[FIGHTER] - 1, 0);
	else
		classnum = plyptr->classnum;
	plynum = add_player(plyptr->name, plyptr->host, classnum, plyptr->cont, -1);
	players[plynum].battles_lost = plyptr->battles_lost;
	players[plynum].battles_won = plyptr->battles_won;
	players[plynum].gender = plyptr->gender;
	players[plynum].remorts = plyptr->remorts - 1;
	players[plynum].troops_killed = plyptr->troops_killed;
	players[plynum].troops_lost = plyptr->troops_lost;
	strcpy(players[plynum].password, plyptr->password);
	strcpy(players[plynum].winmsg, plyptr->winmsg);
	save_player(plynum);
	check_rank(plynum);
	sprintf(message, "13022%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, 0, players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL, adjectives[players[plynum].gender][0], MSGDEL);
	send_channel(message, 0);
}

void log_event(char *from, char *msg, char *name)
{
	char fname[64];
	char newmsg[2048] = {0};
	int fp;
	loginfo newlog;

	parse_msg(msg, newmsg);
	memset(&newlog, 0, LOGSIZ);
	strncpy(newlog.time, NOWSTR, 31);
	if((strlen(from)) > 0)
	{
		sprintf(fname, "logs/%s.msg", name);
		sprintf(newlog.message, "%s sends you a message: ", from);
		strncat(newlog.message, newmsg, 255 - strlen(newlog.message));
	}
	else
	{
		sprintf(fname, "logs/%s", name);
		strncpy(newlog.message, newmsg, 255);
	}
#ifdef WIN32
	if((fp = open(fname, O_RDWR | O_CREAT | _O_BINARY, DFTPERM)) == -1)
#else
	if((fp = open(fname, O_RDWR | O_CREAT, DFTPERM)) == -1)
#endif
		return;
	if((lseek(fp, 0, SEEK_END)) < 0)
	{
		fprintf(stderr, "%s Error positioning %s: %s\n", NOWSTR, fname, strerror(errno));
		return;
	}
	if((write(fp, &newlog, LOGSIZ)) != LOGSIZ)
		fprintf(stderr, "%s Error writing %s: %s\n", NOWSTR, fname, strerror(errno));
	close(fp);
}

int playernum(player *plyptr)
{
	if(plyptr == NULL || summoned(plyptr))
		return(0);
	else
		return(((long)plyptr - (long)players) / PLYSIZ);
}

void pre_init()
{
	int cont;
	int count;
	struct stat finfo;

	if((stat("logs", &finfo)) == -1)
#ifdef WIN32
		mkdir("logs");
#else
		mkdir("logs", S_IRWXU);
#endif
#ifdef WIN32
	if((city_fp = open(CTYFIL, O_RDWR | O_CREAT | _O_BINARY, DFTPERM)) == -1)
#else
	if((city_fp = open(CTYFIL, O_RDWR | O_CREAT, DFTPERM)) == -1)
#endif
	{
		fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, CTYFIL, strerror(errno));
		exit_conquest();
	}
#ifdef WIN32
	if((info_fp = open(INFOFIL, O_RDWR | O_CREAT | _O_BINARY, DFTPERM)) == -1)
#else
	if((info_fp = open(INFOFIL, O_RDWR | O_CREAT, DFTPERM)) == -1)
#endif
	{
		fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, INFOFIL, strerror(errno));
		exit_conquest();
	}
#ifdef WIN32
	if((player_fp = open(FSAVE, O_RDWR | O_CREAT | _O_BINARY, DFTPERM)) == -1)
#else
	if((player_fp = open(FSAVE, O_RDWR | O_CREAT, DFTPERM)) == -1)
#endif
	{
		fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, FSAVE, strerror(errno));
		exit_conquest();
	}
	if((players = (player *)calloc(INCREMENT, PLYSIZ)) == NULL)
	{
		fprintf(stderr, "%s %s:%s\n", NOWSTR, ERRMSG1, strerror(errno));
		exit_conquest();
	}
	current_player = 0;
	total_player = INCREMENT;
	strcpy(players[current_player].name, select_title(0, maxlevel[FIGHTER], 0));
	players[current_player].classnum = 0;
	for(count = 0; count < MAXCNT; ++count)
		players[current_player].level[count] = maxlevel[FIGHTER];
	update_levelmax(&players[current_player]);
	++current_player;
	for(count = 0; count < 15; ++count)
	{
		strcpy(joustlosers[count], "??????");
		strcpy(joustwinners[count], "??????");
	}
	for(cont = 0; cont < num_conts; ++cont)
		for(count = 0; count < 10; ++count)
			totlvl[cont][count] = 0;
	if(num_conts < 1)
	{
		num_conts = 1;
		conts[0].cities[0] = 0;
		conts[0].port = 0;
		conts[0].totalcty = 1;
		strcpy(conts[0].name, "Normandy");
	}
	if(num_cities < 1)
		num_cities = 1;
}

void post_init()
{
	int cont;
	int count = 0;

	while((read(city_fp, &cities[count], CTYSIZ)) == CTYSIZ && count++ < MAXCTY);
	for(count = 0; count < 80; ++count)
		totply[count] = 0;
	if((lseek(player_fp, PLYSIZ, SEEK_SET)) < 0)
	{
		fprintf(stderr, "%s Error positioning %s to %d: %s\n", NOWSTR, FSAVE, PLYSIZ, strerror(errno));
		return;
	}
	while((read(player_fp, &players[current_player], PLYSIZ)) == PLYSIZ)
	{
		if(strlen(players[current_player].name) > 0)
		{
			++totply[players[current_player].classnum];
			if(players[current_player].validate == VAL_NORMAL)
				players[current_player].validate = NOT_VAL;
			for(cont = 0; cont < num_conts; ++cont)
				if(players[current_player].level[cont] >= 0)
					++totlvl[cont][players[current_player].level[cont]];
			players[current_player].sock = INVALID_SOCKET;
		}
		if(++current_player == total_player)
		{
			total_player += INCREMENT;
			if((players = (player *)realloc(players, total_player * PLYSIZ)) == NULL)
			{
				fprintf(stderr, "%s %s:%s\n", NOWSTR, ERRMSG1, strerror(errno));
				exit_conquest();
			}
		}
	}
}

void promote(int cont, player *plyptr)
{
	int autopromote;
	int classnum;
	int honor;
	int nextlevel;

	if(conq_info.agewon == 1 || plyptr->promotion[cont] == 1 || vampire(plyptr))
		return;
	nextlevel = plyptr->level[cont] + 1;
	if(nextlevel >= maxlevel[BASE_CLASSNUM(plyptr->classnum)])
		return;
	if(promote_knights == 1 && plyptr->level[cont] == 0 && conq_info.today == plyptr->first_play[cont])
		return;
	if(barbarian(plyptr) || check_artifact(GRAIL, plyptr))
		honor = 0;
	else
	{
		honor = nextlevel * nextlevel;
		if(cleric(plyptr) || ranger(plyptr))
			honor += nextlevel;
	}
	autopromote = nextlevel + 1;
	if(autopromote > maxlevel[BASE_CLASSNUM(plyptr->classnum)])
		autopromote = maxlevel[BASE_CLASSNUM(plyptr->classnum)];
	classnum = BASE_CLASSNUM(plyptr->classnum);
	if(plyptr->land[cont] >= promotions[classnum][nextlevel][1] && plyptr->peasant[cont] >= promotions[classnum][nextlevel][2] && plyptr->honor[cont] >= honor)
	{
		if(!barbarian(plyptr))
			plyptr->honor[cont] = plyptr->level[cont] * plyptr->level[cont];
		plyptr->movepts += 10;
		if(combined_army(cont, plyptr) >= promotions[classnum][nextlevel][0] && plyptr->troop_type != VAMPIRE)
			add_bg(cont, 50, plyptr);
		if(promote_protection == 1)
                        plyptr->isprotected = 0;
	}
	else if(plyptr->land[cont] >= promotions[classnum][autopromote][1])
	{
		plyptr->honor[cont] = 0;
		plyptr->army[ARMY_DEFENSE][cont].total[BLOODGUARDNUM] = 0;
		plyptr->army[ARMY_CAMPAIGN][cont].total[BLOODGUARDNUM] = 0;
		sprintf(message, "13004%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][0], MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)], 0), MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
		return;
	change_level(cont, 1, plyptr);
	plyptr->promotion[cont] = 1;
	sprintf(message, "13002%c%s%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "13003%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->name, MSGDEL, conts[cont].name, MSGDEL, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), MSGDEL);
	send_channel(message, 0);
	promote_max(cont, plyptr);
	if(plyptr->lord > 0)
		if(plyptr->levelmax > players[plyptr->lord].levelmax || plyptr->level[cont] >= VASRANK)
			remove_vassal(plyptr);
}

void promote_max(int cont, player *plyptr)
{
	int artnum;

	if(plyptr->level[cont] != maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
		return;
	sprintf(message, "%s %s achieved the rank of %s on %s in %d.", 
	select_class(plyptr->classnum), vampire(plyptr)?"??????":plyptr->nick, select_title(plyptr->classnum, plyptr->level[cont], plyptr->gender), conts[cont].name, conq_info.today);
	add_log(KNGFIL, message);
	king_pic(plyptr->classnum);
	plyptr->king[cont] = conq_info.today;
	if((artnum = find_artifact(plyptr)) >= 0 && artnum != STAFF)
	{
		sprintf(message, "35%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		drop(plyptr);
	}
	if(plyptr->ally > 0)
		break_alliance(plyptr);
}

int randomnum(int max, int min)
{
	int num;

	if(max == 0 || min >= max)
		return(min);
	do {
		num = (rand() % (max + 1)) + min;
	} while(num > max || num < min);
	return(num);
}

/*
#ifdef WIN32
int randomnum(int max, int min)
{
	int num;

	if(max == 0 || min >= max)
		return(min);
	do {
		num = (int)((max + 1) * rand() / RAND_MAX) + min;
	} while(num > max || num < min);
	return(num);
}
#else
int randomnum(int max, int min)
{
	char str[32];
	int digits;
	int num;
	int shift;

	if(max == 0 || min >= max)
		return(min);
	sprintf(str, "%d", max);
	digits = strlen(str) + 1;
	shift = (sizeof(int) * 8) - (4 + (digits - 1) * 3);
	do {

		num = random() >> shift;
	} while(num > max || num < min);
	return(num);
}
#endif
*/

void read_army()
{
	char input[1024] = {0};
	char *ptr;
	FILE *fp;
	int classnum;
	int troop;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, ARMFIL);
	if((fp = fopen(ARMFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, ARMFIL, strerror(errno));
		exit_conquest();
	}
	for(classnum = 0; classnum < 8; ++classnum)
		for(troop = 0; troop < MAXTRP; ++troop)
		{
			memset(&batmod[classnum][troop], 0, UNTSIZ);
			memset(&units[classnum][troop], 0, UNTSIZ);
		}
	classnum = troop = 0;
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#' || strlen(input) < 1)
			continue;
		strip(input);
		if(classnum > 6)
			break;
		if((strncmp(input, "class", 5)) == 0 || troop >= MAXTRP)
		{
			++classnum;
			troop = 0;
			continue;
		}
		sscanf(input, "%31s %d %d %d %d %d %d %d %d %d %d %d %d",
		units[classnum][troop].name, &units[classnum][troop].ambush, &units[classnum][troop].attack, &units[classnum][troop].copper, &units[classnum][troop].defend, 
		&units[classnum][troop].range, &units[classnum][troop].food, &units[classnum][troop].mode, &units[classnum][troop].num_attacks, 
		&units[classnum][troop].regenerate, &units[classnum][troop].shield, &units[classnum][troop].swarm, &units[classnum][troop].train);
		if((ptr = strchr(input, ':')) != NULL)
			strncpy(units[classnum][troop].desc, ptr + 1, 255);
		if(units[classnum][troop].ambush < 0 || units[classnum][troop].ambush > 1)
			units[classnum][troop].ambush = 0;
		if(units[classnum][troop].attack < 0)
			units[classnum][troop].attack = 0;
		if(units[classnum][troop].copper < 0)
			units[classnum][troop].copper = 0;
		if(troop == CATAPULTNUM)
		{
			if(units[classnum][troop].defend < 0)
				units[classnum][troop].defend = 0;
		}
		else
			if(units[classnum][troop].attack > 0 && units[classnum][troop].defend < 1)
				units[classnum][troop].defend = 1;
		if(units[classnum][troop].range < 0 || units[classnum][troop].range > 1)
			units[classnum][troop].range = 0;
		if(units[classnum][troop].food < 0)
			units[classnum][troop].food = 0;
		if(units[classnum][troop].mode < MODE_WALK || units[classnum][troop].mode > MODE_FLY)
			units[classnum][troop].mode = MODE_WALK;
		if(units[classnum][troop].num_attacks < 1)
			units[classnum][troop].num_attacks = 1;
		if(units[classnum][troop].regenerate < 0 || units[classnum][troop].regenerate > 1)
			units[classnum][troop].regenerate = 0;
		if(units[classnum][troop].shield < 0 || units[classnum][troop].shield > 1)
			units[classnum][troop].shield = 0;
		if(units[classnum][troop].swarm < 0 || units[classnum][troop].swarm > 1)
			units[classnum][troop].swarm = 0;
		if(units[classnum][troop].train < 0 || units[classnum][troop].train > 1)
			units[classnum][troop].train = 0;
		++troop;
	}
	fclose(fp);
	// Summoned units
	units[7][WOLF].attack = 4;
	units[7][WOLF].defend = 2;
	units[7][WOLF].num_attacks = 1;
	units[7][WOLF].swarm = 1;
	strcpy(units[7][WOLF].name, "Wolf");
	units[7][BEAR].attack = 3;
	units[7][BEAR].defend = 6;
	units[7][BEAR].num_attacks = 2;
	strcpy(units[7][BEAR].name, "Bear");
	units[7][LION].attack = 8;
	units[7][LION].defend = 5;
	units[7][LION].num_attacks = 1;
	strcpy(units[7][LION].name, "Lion");
	units[7][GIANT].attack = 100;
	units[7][GIANT].defend = 100;
	units[7][GIANT].num_attacks = 1;
	strcpy(units[7][GIANT].name, "Giant");
	units[7][DRAGON].attack = 200;
	units[7][DRAGON].defend = 200;
	units[7][DRAGON].mode = 2;
	units[7][DRAGON].num_attacks = 1;
	units[7][DRAGON].range = 1;
	strcpy(units[7][DRAGON].name, "Dragon");
	units[7][BEHEMOTH].attack = 250;
	units[7][BEHEMOTH].defend = 250;
	units[7][BEHEMOTH].num_attacks = 2;
	strcpy(units[7][BEHEMOTH].name, "Behemoth");
}

void read_art()
{
	char input[1024] = {0};
	char *mid;
	char *start;
	char str[32] = {0};
	FILE *fp;
	int artnum = 0;
	int city;
	int owner;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, ARTFIL);
	if((fp = fopen(ARTFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, ARTFIL, strerror(errno));
		exit_conquest();
	}
	for(city = 0; city < MAXCTY; ++city)
		memset(&artifacts[city], 0, CTYSIZ);
	while((fgets(input, 1024, fp)) != NULL && artnum < TOTART)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%d %d %31s", &city, &owner, str);
		if(city < 0 || city >= num_cities)
			continue;
		if((start = strstr(input, str)) == NULL)
			continue;
		if((mid = strchr(start, '@')) == NULL)
			continue;
		artifacts[artnum].city = city;
		artifacts[artnum].owner = owner;
		strncpy(artifacts[artnum].name, start, (mid - start) > 31?31:(mid - start));
		strncpy(artifacts[artnum++].location, mid + 1, 127);
	}
	fclose(fp);
	if(artnum < TOTART)
	{
		fprintf(stderr, "%s Error reading %s: count should be %d not %d\n", NOWSTR, ARTFIL, TOTART, artnum);
		exit_conquest();
	}
}

void read_classes()
{
	char input[1024] = {0};
	int basenum = 0;
	int classnum = -1;
	int cnt;
	int count;
	int level = 0;
	int num;
	int race = -10;
	int setok = 1;
	int symbol = 0;
	FILE *fp;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, CLAFIL);
	if((fp = fopen(CLAFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s %s file missing...proceeding with defaults\n", NOWSTR, CLAFIL);
		return;
	}
	total_races[FIGHTER] = 0;
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#' || strlen(input) < 1)
			continue;
		strip(input);
		if((strncmp(input, "associate_races", 15)) == 0 && setok == 1)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				associate_races = num;
			continue;
		}
		if((strncmp(input, "class_change", 12)) == 0)
		{
			if((num = atoi(input + 12)) >= 0 && basenum > 0 && basenum <= MAXCLS)
				class_change[basenum - 1] = num;
			continue;
		}
		if((strncmp(input, "race", 4)) == 0 && strlen(input + 5) > 0)
		{
			if(race >= 0 && race < 70 && ++classnum < 10)
			{
				memset(classnames[race + classnum], 0, 16);
				strncpy(classnames[race + classnum], input + 5, 16);
				++total_races[BASE_CLASSNUM(race)];
			}
			if(classnum >= num_conts && BASE_CLASSNUM(race) != BARBARIAN && BASE_CLASSNUM(race) != VAMPIRE && BASE_CLASSNUM(race) != TITAN)
			{
				associate_races = 0;
				setok = 0;
			}
			symbol = 0;
			continue;
		}
		if((strncmp(input, "continent", 9)) == 0)
		{
			num = atoi(input + 9) - 1;
			if(num >= -1 && num < num_conts)
				force_cont[race + classnum] = num;
			continue;
		}
		if((strncmp(input, "symbol", 6)) == 0 && strlen(input + 7) > 0)
		{
			if(symbol > 3)
				continue;
			strncpy(symbols[symbol++][race + classnum], input + 7, 7);
			continue;
		}
		if((strncmp(input, "title", 5)) == 0 && strlen(input + 6) > 0)
		{
			if(level > 9)
				continue;
			switch(BASE_CLASSNUM(race))
			{
				case FIGHTER:	/* Fighter */
					memset(f_titles[0][level], 0, 16);
					memset(f_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", f_titles[0][level], f_titles[1][level]);
					break;
				case BARBARIAN:	/* Barbarian */
					memset(b_titles[0][level], 0, 16);
					memset(b_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", b_titles[0][level], b_titles[1][level]);
					break;
				case MAGE:	/* Mage */
					memset(m_titles[0][level], 0, 16);
					memset(m_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", m_titles[0][level], m_titles[1][level]);
					break;
				case VAMPIRE:	/* Vampire */
					memset(v_titles[0][level], 0, 16);
					memset(v_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", v_titles[0][level], v_titles[1][level]);
					break;
				case CLERIC:	/* Cleric */
					memset(c_titles[0][level], 0, 16);
					memset(c_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", c_titles[0][level], c_titles[1][level]);
					break;
				case RANGER:	/* Ranger */
					memset(r_titles[0][level], 0, 16);
					memset(r_titles[1][level], 0, 16);
					sscanf(input + 6, "%15s %15s", r_titles[0][level], r_titles[1][level]);
					break;
				default:
					break;
			}
			maxlevel[BASE_CLASSNUM(race)] = level++;
			continue;
		}
		if((strncmp(input, "class", 5)) == 0)
		{
			if((strlen(input + 6)) > 0 && basenum < MAXCLS)
			{
				memset(basenames[basenum], 0, 16);
				strncpy(basenames[basenum], input + 6, 15);
			}
			++basenum;
			race += 10;
			classnum = -1;
			level = 0;
			total_races[BASE_CLASSNUM(race)] = 0;
			for(cnt = 0; cnt < 4; ++cnt)
				for(count = 0; count < 10; ++count)
					memset(symbols[cnt][race + count], 0, 7);
			continue;
		}
	}
	fclose(fp);
}

void read_dfts(int type)
{
	char input[1024] = {0};
	char keyword[16] = {0};
	int comp = 0;
	int count;
	int ctynum = 0;
	int emps = 0;
	int emps_pass = 0;
	int expnum = 1;
	int hks = 0;
	int num = 0;
	int nyhks = 0;
	int piratenum = 0;
	int tvnnum = 0;
	int totalhks = 0;

	FILE *fp;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, DFTFIL);
	if(type == READ_NEWAGE)
	{
		for(count = 0; count < MAXCTY; ++count)
		{
			memset(&cities[count], 0, CTYSIZ);
			cities[count].artifact = -1;
			cities[count].cont = -1;
		}
	}
	if((fp = fopen(DFTFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s %s file missing...proceeding with defaults\n", NOWSTR, DFTFIL);
		return;
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#' || strlen(input) < 1)
			continue;
		strip(input);
		if((strncmp(input, "server_ip", 9)) == 0 && strlen(input + 10) > 0)
		{
			memset(stralt_ip, 0, 16);
			strncpy(stralt_ip, input + 10, 15);
			continue;
		}
		if((strncmp(input, "port_num", 8)) == 0)
		{
			if(type == READ_SIGNAL)
				continue;
			if((num = atoi(input + 8)) > 1024)
				port_num = num;
			else
				fprintf(stderr, "%s conqd can only connect to non-privileged ports.", NOWSTR);
			continue;
		}
		if((strncmp(input, "master_server", 13)) == 0 && strlen(input + 14) > 0)
		{
			memset(master_server, 0, 128);
			strncpy(master_server, input + 14, 127); 
			continue;
		}
		if((strncmp(input, "log_file", 8)) == 0 && strlen(input + 9) > 0)
		{
			if(type == READ_SIGNAL)
				continue;
			memset(log_file, 0, 128);
			strncpy(log_file, input + 9, 127); 
			continue;
		}
		if((strncmp(input, "max_player", 10)) == 0)
		{
			if((num = atoi(input + 10)) > 0)
				max_player = num;
			continue;
		}
		if((strncmp(input, "use_dns", 7)) == 0)
		{
			if((num = atoi(input + 7)) == 0 || num == 1)
				use_dns = num;
			continue;
		}
		if((strncmp(input, "continent_name", 14)) == 0 && num_conts < MAXCNT && strlen(input + 15) > 0)
		{
			if(type == READ_SIGNAL)
				continue;
			memset(&conts[num_conts], 0, CNTSIZ);
			for(count = 0, ctynum = 0; count < MAXCTY; ++count)
				conts[num_conts].cities[count] = -1;
			conts[num_conts].port = num_cities;
			conts[num_conts].totalcty = 0;
			strncpy(conts[num_conts++].name, input + 15, 15);
			continue;
		}
		if((strncmp(input, "city_name", 9)) == 0 && num_cities < MAXCTY && strlen(input + 10) > 0)
		{
			if(type == READ_SIGNAL)
				continue;
			if(num_conts < 1)
			{
				fprintf(stderr, "%s City definition ignored...no continent defined: %s\n", NOWSTR, input);
				continue;
			}
			cities[num_cities].cont = num_conts - 1;
			conts[num_conts - 1].cities[ctynum++] = num_cities;
			++conts[num_conts - 1].totalcty;
			memset(city_names[num_cities], 0, 16);
			sscanf(input + 10, "%15s %15s", city_names[num_cities], keyword);
			if((strlen(keyword)) > 1)
			{
				if((STRCASECMP(keyword, "magic")) == 0)
					magic_city = num_cities;
				else if((STRCASECMP(keyword, "secret")) == 0)
					secret_city = num_cities;
				else if((STRCASECMP(keyword, "antimagic")) == 0)
					antimagic_city = num_cities;
				memset(keyword, 0, 16);
			}
			++num_cities;
			continue;
		}
		if((strncmp(input, "tavern_name", 11)) == 0 && tvnnum < MAXCTY && strlen(input + 12) > 0)
		{
			memset(tavern_names[tvnnum], 0, 32);
			strncpy(tavern_names[tvnnum++], input + 12, 31);
			continue;
		}
		if((strncmp(input, "money_name", 10)) == 0 && strlen(input + 11) > 0)
		{
			memset(money_name, 0, 16);
			strncpy(money_name, input + 11, 15);
			continue;
		}
		if((strncmp(input, "restrict_markets", 16)) == 0)
		{
			if((num = atoi(input + 16)) == 0 || num == 1)
				restrict_markets = num;
			continue;
		}
		if((strncmp(input, "emperor", 7)) == 0 && emps < MAXCNT && strlen(input + 8) > 0)
		{
			if(type == READ_SIGNAL)
				continue;
			memset(emp[emps], 0, 32);
			strncpy(emp[emps++], input + 8, 31); 
			continue;
		}
		if((strncmp(input, "emp_password", 12)) == 0 && emps_pass < MAXCNT && strlen(input + 13) > 0)
		{
			if(type == READ_SIGNAL)
				continue;
			memset(emp_passwd[emps_pass], 0, 32);
			strncpy(emp_passwd[emps_pass++], input + 13, 31); 
			continue;
		}
		if((strncmp(input, "barbarian_class", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				allow_barb = num;
			continue;
		}
		if((strncmp(input, "cleric_class", 12)) == 0)
		{
			if((num = atoi(input + 12)) == 0 || num == 1)
				allow_cleric = num;
			continue;
		}
		if((strncmp(input, "mage_class", 10)) == 0)
		{
			if((num = atoi(input + 10)) == 0 || num == 1)
				allow_mage = num;
			continue;
		}
		if((strncmp(input, "ranger_class", 12)) == 0)
		{
			if((num = atoi(input + 12)) == 0 || num == 1)
				allow_ranger = num;
			continue;
		}
		if((strncmp(input, "vampire_class", 13)) == 0)
		{
			if((num = atoi(input + 13)) == 0 || num == 1)
				allow_vampire = num;
			continue;
		}
		if((strncmp(input, "mult_vampires", 13)) == 0)
		{
			if((num = atoi(input + 13)) == 0 || num == 1)
				mult_vampires = num;
			continue;
		}
		if((strncmp(input, "advanced_armies", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				advanced_armies = num;
			ARMY_CAMPAIGN = advanced_armies;
			continue;
		}
		if((strncmp(input, "advanced_combat", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				advanced_combat = num;
			continue;
		}
		if((strncmp(input, "allow_formation", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				allow_formation = num;
			continue;
		}
		if((strncmp(input, "allow_surrender", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				allow_surrender = num;
			continue;
		}
		if((strncmp(input, "attack_reprieve", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				attack_reprieve = num;
			continue;
		}
		if((strncmp(input, "attack_time", 11)) == 0)
		{
			if((num = atoi(input + 11)) >= 0 && num <= 720)
				attack_time = num * 60;
			continue;
		}
		if((strncmp(input, "capture_foreign", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				capture_foreign = num;
			continue;
		}
		if((strncmp(input, "dft_surrender", 13)) == 0)
		{
			if((num = atoi(input + 13)) >= 1 && num <= 100)
				dft_surrender = (float)(num / 100.0);
			continue;
		}
		if((strncmp(input, "enforce_peace", 13)) == 0)
		{
			if((num = atoi(input + 13)) == 0 || num == 1)
				enforce_peace = num;
			continue;
		}
		if((strncmp(input, "fog_of_war", 10)) == 0)
		{
			if((num = atoi(input + 10)) == 0 || num == 1)
				fog_of_war = num;
			continue;
		}
		if((strncmp(input, "promote_protection", 18)) == 0)
		{
			if((num = atoi(input + 18)) == 0 || num == 1)
				promote_protection = num;
			continue;
		}
		if((strncmp(input, "restrict_combat", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				restrict_combat = num;
			continue;
		}
		if((strncmp(input, "sail_protection", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				sail_protection = num;
			continue;
		}
		if((strncmp(input, "auto_ar", 7)) == 0)
		{
			if((num = atoi(input + 7)) >= 0)
				auto_ar = num;
			continue;
		}
		if((strncmp(input, "num_remorts", 11)) == 0)
		{
			if((num = atoi(input + 11)) >= 1 && num <= 20)
				num_remorts = num;
			continue;
		}
		if((strncmp(input, "experience_name", 15)) == 0 && expnum <= 20 && strlen(input + 16) > 0)
		{
			memset(elitenames[expnum], 0, 16);
			strncpy(elitenames[expnum++], input + 16, 15); 
			continue;
		}
		if((strncmp(input, "current_age", 11)) == 0 && strlen(input + 12) > 0)
		{
			memset(age, 0, 8);
			strncpy(age, input + 12, 7);
			continue;
		}
		if((strncmp(input, "start_date", 10)) == 0)
		{
			if((num = atoi(input + 10)) >= 0)
				start_date = num;
			continue;
		}
		if((strncmp(input, "end_date", 8)) == 0)
		{
			if((num = atoi(input + 8)) >= 0)
				end_date = num;
			continue;
		}
		if((strncmp(input, "normal_hk", 9)) == 0)
		{
			if(type == READ_SIGNAL)
				continue;
			if((num = atoi(input + 9)) >= 0 && num <= 24 && totalhks < MAXHKS)
			{
				if(num == 24)
					num = 0;
				normal_hks[hks++] = num;
				++totalhks;
			}
			continue;
		}
		if((strncmp(input, "ny_hk", 5)) == 0)
		{
			if(type == READ_SIGNAL)
				continue;
			if((num = atoi(input + 5)) >= 0 && num <= 24 && totalhks < MAXHKS)
			{
				if(num == 24)
					num = 0;
				ny_hks[nyhks++] = num;
				++totalhks;
			}
			continue;
		}
		if((strncmp(input, "quest_chars", 11)) == 0)
		{
			if((num = atoi(input + 11)) >= 0 && num <= 32)
				quest_chars = num;
			continue;
		}
		if((strncmp(input, "quest_riddle", 12)) == 0)
		{
			if((num = atoi(input + 12)) == 0 || num == 1)
				quest_riddle = num;
			continue;
		}
		if((strncmp(input, "promote_knights", 15)) == 0)
		{
			if((num = atoi(input + 15)) == 0 || num == 1)
				promote_knights = num;
			continue;
		}
		if((strncmp(input, "remove_items", 12)) == 0)
		{
			if((num = atoi(input + 12)) == 0 || num == 1)
				remove_items = num;
			continue;
		}
		if((strncmp(input, "mps_15", 6)) == 0)
		{
			if((num = atoi(input + 6)) >= 0 && num <= 10)
				mps_15 = num;
			continue;
		}
		if((strncmp(input, "component_name", 14)) == 0 && comp < MAXCMP && strlen(input + 15) > 0)
		{
			memset(components[comp], 0, 16);
			strncpy(components[comp++], input + 15, 15);
			continue;
		}
		if((strncmp(input, "food_name", 9)) == 0 && strlen(input + 10) > 0)
		{
			memset(food_name, 0, 16);
			strncpy(food_name, input + 10, 15);
			continue;
		}
		if((strncmp(input, "peasant_name", 12)) == 0 && strlen(input + 13) > 0)
		{
			memset(peasant_name, 0, 16);
			strncpy(peasant_name, input + 13, 15);
			continue;
		}
		if((strncmp(input, "pirate_name", 11)) == 0 && piratenum < 4 && strlen(input + 12) > 0)
		{
			memset(pirate_names[piratenum], 0, 16);
			strncpy(pirate_names[piratenum++], input + 12, 15);
			continue;
		}
		if((strncmp(input, "dft_kname", 9)) == 0 && strlen(input + 10) > 0)
		{
			memset(dft_kname, 0, 32);
			strncpy(dft_kname, input + 10, 31);
			continue;
		}
		if((strncmp(input, "dft_sname", 9)) == 0 && strlen(input + 10) > 0)
		{
			memset(dft_sname, 0, 32);
			strncpy(dft_sname, input + 10, 31);
			continue;
		}
		if((strncmp(input, "dft_winmsg", 10)) == 0 && strlen(input + 11) > 0)
		{
			memset(dft_winmsg, 0, 256);
			strncpy(dft_winmsg, input + 11, 255);
			continue;
		}
		if((strlen(input)) > 0)
			fprintf(stderr, "%s Unknown option: %s\n", NOWSTR, input);
	}
	fclose(fp);
	if(advanced_armies == 0)
		strcpy(army_names[ARMY_DEFENSE], "Enlisted");
	if(allow_cleric == 0)
		allow_vampire = 0;
	if(allow_vampire == 0)
		mult_vampires = 0;
	if(start_date >= end_date)
		end_date = 0;
}

void read_info()
{
	if((read(info_fp, &conq_info, INFOSIZ)) != INFOSIZ)
		fprintf(stderr, "%s Error reading %s: %s\n", NOWSTR, INFOFIL, strerror(errno));
}

void read_legend(char *name)
{
	char input[1024] = {0};
	FILE *fp;
	int count = 0;
	int msgnum = 0;
	int total = 0;

	if((fp = fopen(LEGFIL, "r")) == NULL)
	{
		sprintf(message, "Error opening %s: %s", LEGFIL, strerror(errno));
		fprintf(stderr, "%s %s\n", NOWSTR, message);
		send_message(message, name, 1);
		return;
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		if(isdigit(input[0]))
		{
			total = atoi(input);
			msgnum = randomnum(total, 1);
			continue;
		}
		if(++count == msgnum)
		{
			send_message(input, name, 1);
			break;
		}
	}
	fclose(fp);
}

void read_promotions()
{
	char input[1024] = {0};
	FILE *fp;
	int classnum = 0;
	int level = 0;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, PROFIL);
	if((fp = fopen(PROFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, PROFIL, strerror(errno));
		exit_conquest();
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%d %d %d", &promotions[classnum][level][0], &promotions[classnum][level][1], &promotions[classnum][level][2]);
		if(++level > maxlevel[classnum])
		{
			if(++classnum > TITAN)
				break;
			level = 0;
		}
	}
	fclose(fp);
}

void read_riddles()
{
	char input[1024] = {0};
	FILE *fp;

	if(quest_riddle == 0)
		return;
	fprintf(stderr, "%s Reading %s\n", NOWSTR, RIDFIL);
	if((fp = fopen(RIDFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, RIDFIL, strerror(errno));
		quest_riddle = 0;
		return;
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		if((strncmp(input, "riddle", 6)) == 0 && strlen(input + 7) > 0)
		{
			memset(riddles[num_riddles].clue, 0, 512);
			strncpy(riddles[num_riddles].clue, input + 7, 511);
			continue;
		}
		if((strncmp(input, "solution", 8)) == 0 && strlen(input + 9) > 0)
		{
			memset(riddles[num_riddles].solution, 0, 16);
			strncpy(riddles[num_riddles].solution, input + 9, 15);
			if(++num_riddles >= MAXRID)
				break;
			continue;
		}
	}
	fclose(fp);
	if(conq_info.riddlenum >= num_riddles)
		quest_riddle = 0;
}

void read_structures()
{
	char input[1024] = {0};
	char *ptr;
	FILE *fp;
	int classnum = 0;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, STRFIL);
	if((fp = fopen(STRFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, STRFIL, strerror(errno));
		exit_conquest();
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		memset(&structures[classnum], 0, STRSIZ);
		sscanf(input, "%15s %d %d %d %d %d", structures[classnum].name, &structures[classnum].cost, &structures[classnum].land, &structures[classnum].mpcost, &structures[classnum].tax, &structures[classnum].troops);
		if((ptr = strchr(input, ':')) != NULL)
			strncpy(structures[classnum].desc, ptr + 1, 127);
		if(++classnum > TITAN)
			break;
	}
	fclose(fp);
}

char *read_tip(void)
{
	char input[128];
	FILE *fp;
	int count = 0;
	int total = 0;
	int tipnum = 0;

	strcpy(message, "No Tips available.");
	if((fp = fopen(TIPFIL, "r")) != NULL)
	{
		while((fgets(input, 128, fp)) != NULL)
		{
			if(input[0] == '#')
				continue;
			strip(input);
			if(isdigit(input[0]))
			{
				total = atoi(input);
				tipnum = randomnum(total - 1, 0);
			}
			else
			{
				if(count++ == tipnum)
				{
					strcpy(message, input);
					break;
				}
			}
		}
		fclose(fp);
	}
	return(message);
}

void remove_all_hosts()
{
	hosts *current;
	hosts *next;

	current = hostshead;
	while(current != NULL)
	{
		next = current->next;
		if(current->host)
			free(current->host);
		free(current);
		current = next;
	}
	hostshead = NULL;
}

void remove_all_ret(player *plyptr)
{
	int count;

	for(count = 0; count < MAXRET; ++count)
	{
		plyptr->ret[count].attacks = 0;
		plyptr->ret[count].plynum = 0;
	}
}

void remove_host(char *host)
{
	hosts *next;
	hosts *prev = NULL;

	next = hostshead;
	while(next != NULL)
	{
		if((STRNCASECMP(next->host, host, strlen(next->host))) == 0)
		{
			if(next == hostshead)
				hostshead = next->next;
			if(prev)
				prev->next = next->next;
			if(next->host)
				free(next->host);
			free(next);
			break;
		}
		prev = next;
		next = next->next;
	}
}

void remove_ret(int plynum, player *plyptr)
{
	int pos;

	if((pos = find_ret(plynum, plyptr)) < 0)
		return;
	plyptr->ret[pos].attacks = 0;
	plyptr->ret[pos].plynum = 0;
	save_player(playernum(plyptr));
}

void reset_ally(int plynum)
{
	int count;

	for(count = 1; count < current_player; ++count)
	{
		if(players[count].ally_request == plynum)
		{
			players[count].ally_request = 0;
			save_player(count);
			break;
		}
	}
}

void reset_done()
{
	int count;
	player *plyptr;
	time_t current;

	time(&current);
	for(count = 0; count < current_player; ++count)
		if((strlen(players[count].name)) > 0)
			if(players[count].done > 0 && difftime(current, players[count].done) > DONETIME)
			{
				plyptr = &players[count];
				sprintf(message, "4063%c%s%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
				send_message(message, plyptr->name, 1);
				plyptr->done = 0;
				plyptr->sock = INVALID_SOCKET;
				plyptr->validate = NOT_VAL;
			}

}

void reset_ret(int plynum)
{
	int count;

	for(count = 1; count < current_player; ++count)
		if((strlen(players[count].name)) > 0)
			remove_ret(plynum, &players[count]);
}

void reset_special(player *plyptr)
{
	int count;

	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->special_batmod & spcbless[count])
			plyptr->special_batmod -= spcbless[count];
		if(plyptr->special_batmod & spcshd[count])
			plyptr->special_batmod -= spcshd[count];
		if(plyptr->special_batmod & spcstr[count])
			plyptr->special_batmod -= spcstr[count];
		if(plyptr->special_kingdom & spctax[count])
			plyptr->special_kingdom -= spctax[count];
	}
	if(vampire(plyptr))
	{
		if(plyptr->special_misc & SPC_GUISE)
			plyptr->special_misc -= SPC_GUISE;
		if(plyptr->special_misc & SPC_RITUAL)
			plyptr->special_misc -= SPC_RITUAL;
	}
}

void reset_spy(int newspy, int plynum)
{
	int found;
	int cont;
	int count;

	for(count = 1; count < current_player; ++count)
	{
		found = 0;
		for(cont = 0; cont < num_conts; ++cont)
			if(players[count].level[cont] >= 0 && players[count].spy[cont] == plynum)
				{
					players[count].spy[cont] = newspy;
					found = 1;
				}
		if(found)
			save_player(count);
	}
}

void reset_val()
{
	int count;

	for(count = 0; count < current_player; ++count)
		if(players[count].trynum > 2)
		{
			players[count].trynum = 0;
			save_player(count);
		}
}

void save_art()
{
	char fname[128] = {0};
	char input[1024] = {0};
	FILE *in;
	FILE *out;
	int count;

	strcpy(fname, tempnam("./", NULL));
	if((in = fopen(ARTFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, ARTFIL, strerror(errno));
		return;
	}
	if((out = fopen(fname, "w")) == NULL)
	{
		fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, fname, strerror(errno));
		return;
	}
	while((fgets(input, 1024, in)) != NULL)
	{
		if(input[0] == '#')
			fputs(input, out);
		else
			break;
	}
	fclose(in);
	for(count = 0; count < TOTART; ++count)
	{
		if((strlen(artifacts[count].name)) < 1)
			break;
		sprintf(input, "%d %d %s@%s\n", artifacts[count].city, artifacts[count].owner, artifacts[count].name, artifacts[count].location);
		fputs(input, out);
	}
	fclose(out);
	remove(ARTFIL);
	rename(fname, ARTFIL);
}

void save_city(int ctynum)
{
	if((lseek(city_fp, ctynum * CTYSIZ, SEEK_SET)) < 0)
	{
		fprintf(stderr, "%s Error positioning %s to %d: %s\n", NOWSTR, CTYFIL, ctynum * CTYSIZ, strerror(errno));
		return;
	}
	if((write(city_fp, &cities[ctynum], CTYSIZ)) != CTYSIZ)
		fprintf(stderr, "%s Error writing %s: %s\n", NOWSTR, CTYFIL, strerror(errno));
}

void save_info()
{
	if((lseek(info_fp, 0, SEEK_SET)) < 0)
	{
		fprintf(stderr, "%s Error positioning %s: %s\n", NOWSTR, INFOFIL, strerror(errno));
		return;
	}
	if((write(info_fp, &conq_info, INFOSIZ)) != INFOSIZ)
		fprintf(stderr, "%s Error writing %s: %s\n", NOWSTR, INFOFIL, strerror(errno));
}

void save_player(int plynum)
{
	if((lseek(player_fp, plynum * PLYSIZ, SEEK_SET)) < 0)
	{
		fprintf(stderr, "%s Error positioning %s to %d: %s\n", NOWSTR, FSAVE, plynum * PLYSIZ, strerror(errno));
		return;
	}
	if((write(player_fp, &players[plynum], PLYSIZ)) != PLYSIZ)
		fprintf(stderr, "%s Error writing %s: %s\n", NOWSTR, FSAVE, strerror(errno));
}

char *select_class(int classnum)
{
	switch(classnum)
	{
		default:
			return(classnames[classnum]);
	}
}

char *select_classname(int classnum)
{
	switch(classnum)
	{
		default:
			return(basenames[classnum]);
	}
}

char *select_title(int classnum, int level, int gender)
{
	if(classnum >= 10 && classnum < 20)
		return(b_titles[gender][level]);
	if(classnum >= 20 && classnum < 30)
		return(m_titles[gender][level]);
	if(classnum >= 30 && classnum < 40)
		return(v_titles[gender][level]);
	if(classnum >= 40 && classnum < 50)
		return(c_titles[gender][level]);
	if(classnum >= 50 && classnum < 60)
		return(r_titles[gender][level]);
	if(classnum >= 60 && classnum < 70)
		return(t_titles[gender][level - (maxlevel[BASE_CLASSNUM(classnum)] - 1)]);
	if(classnum >= 70 && classnum < 80)
		return(s_titles[level]);
	return(f_titles[gender][level]);
}

void signal_fatal(int signum)
{
	fprintf(stderr, "%s FATAL ERROR EXCEPTION: %s\n", NOWSTR, function_name);
	exit_conquest();
}

void signal_reset(int signum)
{
	recv_sig = signum;
	signal(signum, signal_reset);
}

void signal_shutdown(int signum)
{
	fprintf(stderr, "%s Shutting down\n", NOWSTR);
	exit_conquest();
}

void sort_rank()
{
	int i, j, hold;
	player *p1, *p2;

	for(i = 0; i < MAXRNK; ++i)
		for(j = 0; j < MAXRNK - i - 1; ++j)
		{
			p1 = &players[conq_info.ranks[j]];
			p2 = &players[conq_info.ranks[j + 1]];
			if(p1->battles_won < p2->battles_won || (p1->battles_won == p2->battles_won && p1->troops_killed < p2->troops_killed))
			{
				hold = conq_info.ranks[j];
				conq_info.ranks[j] = conq_info.ranks[j + 1];
				conq_info.ranks[j + 1] = hold;
			}
		}
}

void strip(char *string)
{
	int len;

        len = strlen(string) - 1;
        while(len >= 0 && isspace(string[len]))
                string[len--] = 0;
}

char *strtolower(char *instr)
{
	char *ptr;
	int count;

	if((ptr = strdup(instr)) == NULL)
		return(NULL);
	for(count = 0; count < (int)strlen(instr); ++count)
		ptr[count] = tolower(ptr[count]);
	return(ptr);
}

int tax(int amount, int price, int taxrate)
{
        float total;

        total = (float)(price * taxrate / 100 * amount);
        return((int)(total + .5));
}

void timediff(int *hours, int *minutes, int *seconds, time_t from)
{
	int diff;
	time_t current;

	time(&current);
	diff = (int)difftime(current, from);
	*minutes = (attack_time / 60) - (diff / 60);
	*hours = *minutes / 60;
	*minutes = *minutes % 60;
	*seconds = 60 - (diff % 60);
	if(*seconds == 60)
	{
		++*minutes;
		*seconds = 0;
	}
	if(*minutes == 60)
	{
		++*hours;
		*minutes = 0;
	}
}

int titan(player *plyptr)
{
	if(plyptr->levelmax == maxlevel[BASE_CLASSNUM(plyptr->classnum)])
		return(1);
	if(plyptr->classnum == DEMIGOD)
		return(1);
	return(0);
}

int troop_type(int classnum)
{
	if(classnum < 10)
		return(FIGHTER);
	if(classnum >= 10 && classnum < 20)
		return(BARBARIAN);
	if(classnum >= 20 && classnum < 30)
		return(MAGE);
	if(classnum >= 30 && classnum < 40)
		return(VAMPIRE);
	if(classnum >= 40 && classnum < 50)
		return(CLERIC);
	if(classnum >= 50 && classnum < 60)
		return(RANGER);
	if(classnum >= 60 && classnum < 70)
		return(TITAN);
	if(classnum >= 70 && classnum < 80)
		return(SUMMONED);
	return(FIGHTER);
}

void update_levelmax(player *plyptr)
{
	int count;

	for(count = 0, plyptr->levelmax = 0; count < num_conts; ++count)
		if(plyptr->level[count] >= 0 && plyptr->level[count] > plyptr->levelmax)
			plyptr->levelmax = plyptr->level[count];
}

void update_ret(int cont, player *attacker, player *defender)
{
	int pos;

	if(attacker->level[cont] - attack_level(cont, attacker) <= defender->level[cont])
		return;
	if((pos = find_ret(playernum(defender), attacker)) < 0)
		return;
	if((--attacker->ret[pos].attacks) < 1)
		remove_ret(playernum(defender), attacker);
}
