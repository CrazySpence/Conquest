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

int castleqty = 100000;

void buy_spy(player *plyptr)
{
	int price = 0;

	if(plyptr->spy[plyptr->cont] >= 0)
	{
		sprintf(message, "14000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[plyptr->city].spy_pr < 1)
	{
		sprintf(message, "14001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[plyptr->city].spy_qty < 1)
	{
		sprintf(message, "14002%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < cities[plyptr->city].spy_pr / 100)
	{
		sprintf(message, "1%c%d%cto seek out an operative%c", MSGDEL,  cities[plyptr->city].spy_pr / 100, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	price += cities[plyptr->city].spy_pr + tax(1, cities[plyptr->city].spy_pr, cities[plyptr->city].tax);
	if(plyptr->copper[plyptr->cont] >= price)
	{
		plyptr->spy[plyptr->cont] = 0;
		--(cities[plyptr->city].spy_qty);
		plyptr->copper[plyptr->cont] -= price;
		plyptr->movepts -= cities[plyptr->city].spy_pr / 100;
		cities[plyptr->city].copper += price;
		sprintf(message, "14003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		save_city(plyptr->city);
	}
	else
	{
		sprintf(message, "14004%c%d%c%s%c", MSGDEL, price, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void dcc_cmd(char *cmd, char *host, char *type, char *user)
{
	char cmds[][8] = 
	{
		"chat", "close", ""
	};
	int count = 0;
	int plynum;

	if((plynum = find_player(user)) < 0)
	{
		sprintf(message, "14005%c%c", MSGDEL, MSGDEL);
		send_message(message, user, 0);
		return;
	}
	while((strlen(cmds[count])) > 0)
	{
		if((STRCASECMP(cmds[count], cmd)) == 0)
			break;
		++count;
	}
	switch(count)
	{
		case 0:
			if(players[plynum].validate == VAL_NORMAL)
				dcc_chat(host, plynum);
			else
			{
				sprintf(message, "14005%c%c", MSGDEL, MSGDEL);
				send_message(message, user, 0);
			}
			break;
		case 1:
			dcc_close(plynum);
			break;
		default:
			sprintf(message, "14006%c%c", MSGDEL, MSGDEL);
			send_message(message, user, 0);
			break;
	}
}

void parser(char *cmd, char *host, char *msg, char *type, char *user)
{
	char msccmds[][16] = 
	{
		"newplayer", "validate", "kia", "info", "help", "rank", "news",
		"titans", "kings", "dcc", "list", "email", "game"
	};
	char prms[][32] =
	{
		{0}, {0}, {0}, {0}
	};
	char str[32] = {0};
	commandlist *cmdptr = NULL;
	int count;
	int ctynum;
	int len;
	int plynum;

	sscanf(msg, "%31s %31s %31s %31s", prms[0], prms[1], prms[2], prms[3]);
	sprintf(function_name, "%s %s %s", user, cmd, prms[0]);
	cmdptr = (commandlist *)bsearch(cmd, &commands[0], MAXCMD, CMDSIZ, cmp_command);
	if(cmdptr)
	{
		if((plynum = find_player(user)) < 1)
		{
			sprintf(message, "41%c%c", MSGDEL, MSGDEL);
			send_message(message, user, 0);
			sprintf(message, "42%c%c", MSGDEL, MSGDEL);
			send_message(message, user, 0);
			return;
		}
		if(players[plynum].validate == NOT_VAL)
		{
			sprintf(message, "40%c%c", MSGDEL, MSGDEL);
			send_message(message, user, 0);
			return;
		}
		if(players[plynum].sock != sockets[sockout].sock)
		{
			sprintf(message, "43%c%c", MSGDEL, MSGDEL);
			send_message(message, user, 0);
			return;
		}
		if(players[plynum].done > 0)
		{
			count = (int)(difftime(players[plynum].done + DONETIME, time(NULL)));
			if(count % 60 >= 30)
				count += 60;
			count /= 60;
			if(count < 1)
				strcpy(str, "< 1");
			else
				sprintf(str, "%d", count);
			sprintf(message, "4041%c%s%c", MSGDEL, str, MSGDEL);
			send_message(message, user, 1);
			return;
		}
		if(cmdptr->num != 3 && strstr(players[plynum].password, "None"))
		{
			sprintf(message, "14008%c%s%c", MSGDEL, players[plynum].password, MSGDEL);
			send_message(message, players[plynum].name, 1);
			sprintf(message, "14009%c%c", MSGDEL, MSGDEL);
			send_message(message, players[plynum].name, 1);
			return;
		}
		if(vampire(&players[plynum]))
			create_guise(plynum);
		switch(cmdptr->num)
		{
			case 0:
				person(prms[0], &players[plynum], &players[plynum]);
				break;
			case 1:
				review_army(prms[0], &players[plynum]);
				break;
			case 2:
				done(&players[plynum]);
				break;
			case 3:
				password(prms[0], &players[plynum]);
				break;
			case 4:
				if(strlen(prms[0]))
				{
					if((ctynum = find_city(prms[0])) < 0)
					{
						sprintf(message, "2%c%s%c", MSGDEL, prms[0], MSGDEL);
						send_message(message, players[plynum].name, 1);
						break;
					}
				}
				else
					ctynum = players[plynum].city;
				market(ctynum, &players[plynum]);
				break;
			case 5:
			case 6:
				process_buy(prms[0], prms[1], &players[plynum]);
				break;
			case 7:
				petition(atoi(prms[0]), &players[plynum]);
				break;
			case 8:
				alliance(prms[0], &players[plynum]);
				break;
			case 9:
				break_alliance(&players[plynum]);
				break;
			case 10:
				spy(prms[0], prms[1], &players[plynum]);
				break;
			case 11:
				ally(prms[0], &players[plynum]);
				break;
			case 12:
				process_sell(prms[0], prms[1], &players[plynum]);
				break;
			case 13:
				enlist(prms[0], &players[plynum]);
				break;
			case 14:
				attack(prms[0], &players[plynum]);
				break;
			case 15:
				process_max(prms[0], &players[plynum]);
				break;
			case 16:
				tavern(&players[plynum]);
				break;
			case 17:
				travel(prms[0], &players[plynum]);
				break;
			case 18:
				read_log(players[plynum].name);
				break;
			case 19:
				process_transfer(NULL, prms[0], prms[1], 0, &players[plynum]);
				break;
			case 20:
				quest(prms[0], prms[1], prms[2], &players[plynum]);
				break;
			case 21:
				view(prms[0], &players[plynum]);
				break;
			case 22:
				church(prms[0], &players[plynum]);
				break;
			case 23:
				roll(prms[0], &players[plynum]);
				break;
			case 24:
				ninja(prms[1], prms[0], &players[plynum]);
				break;
			case 25:
				tangent(prms[0], prms[1], &players[plynum]);
				break;
			case 26:
				corpse(&players[plynum]);
				break;
			case 27:
				process_transfer(prms[0], prms[1], prms[2], 1, &players[plynum]);
				break;
			case 28:
				create_titan(prms[0], atoi(prms[1]), &players[plynum]);
				break;
			case 29:
				hk(atoi(prms[0]), &players[plynum]);
				break;
			case 30:
				reset(prms[1], prms[0], prms[2], prms[3], &players[plynum]);
				break;
			case 31:
				erase(atoi(prms[0]), &players[plynum]);
				break;
			case 32:
				newtopic(msg, 0, players[plynum].sock, &players[plynum]);
				break;
			case 33:
				courier(prms[0], msg + strlen(prms[0]), &players[plynum]);
				break;
			case 34:
				shut_down(&players[plynum]);
				break;
			case 35:
				process_transfer(prms[0], prms[1], prms[2], 2, &players[plynum]);
				break;
			case 36:
				mystic(&players[plynum]);
				break;
			case 37:
				search(msg, &players[plynum]);
				break;
			case 38:
				drop(&players[plynum]);
				break;
			case 39:
				motd(msg, &players[plynum]);
				break;
			case 40:
				find_artifacts(&players[plynum]);
				break;
			case 41:
				dsp_items(&players[plynum]);
				break;
			case 42:
				use(prms[0], prms[1], &players[plynum]);
				break;
			case 43:
				destroy(prms[0], &players[plynum]);
				break;
			case 44:
				abdicate(&players[plynum]);
				break;
			case 45:
				rebel(&players[plynum]);
				break;
			case 46:
				loot(prms[0], &players[plynum]);
				break;
			case 47:
				nick(prms[0], players[plynum].sock, &players[plynum]);
				break;
			case 48:
				devote(&players[plynum]);
				break;
			case 49:
				book(&players[plynum]);
				break;
			case 50:
				cast(prms[0], prms[1], prms[2], prms[3], &players[plynum]);
				break;
			case 51:
				show_components(&players[plynum]);
				break;
			case 52:
				set_event(prms[0], prms[1], &players[plynum]);
				break;
			case 53:
				break;
			case 54:
				fleet(&players[plynum]);
				break;
			case 55:
				if(isdigit(prms[1][0]))
				{
					count = atoi(prms[1]);
					strncpy(str, prms[2], 31);
				}
				else
				{
					count = atoi(prms[2]);
					strncpy(str, prms[1], 31);
				}
				str[31] = 0;
				strip(str);
				len = strlen(str) - 1;
				if(str[len] == 's' || str[len] == 'S')
					str[len] = 0;
				loadship(str, count, atoi(prms[0]), &players[plynum]);
				break;
			case 56:
				engage(prms[0], &players[plynum]);
				break;
			case 57:
				population(prms[0], &players[plynum]);
				break;
			case 58:
				repair(atoi(prms[0]), &players[plynum]);
				break;
			case 59:
				sail(prms[0], &players[plynum]);
				break;
			case 60:
				scuttle(atoi(prms[0]), &players[plynum]);
				break;
			case 61:
				unload(atoi(prms[1]), atoi(prms[0]), &players[plynum]);
				break;
			case 62:
				make_vampire(&players[plynum]);
				break;
			case 63:
				guise(prms[0], &players[plynum]);
				break;
			case 64:
				pestilence(prms[0], &players[plynum]);
				break;
			case 65:
				charm(msg, &players[plynum]);
				break;
			case 66:
				seek(&players[plynum]);
				break;
			case 67:
				slay(prms[0], &players[plynum]);
				break;
			case 68:
				bless(prms[0], &players[plynum]);
				break;
			case 69:
				crusade(prms[0], &players[plynum]);
				break;
			case 70:
				divination(prms[0], &players[plynum]);
				break;
			case 71:
				divine_intervention(&players[plynum]);
				break;
			case 72:
				excommunicate(prms[0], &players[plynum]);
				break;
			case 73:
				grace(prms[0], &players[plynum]);
				break;
			case 74:
				resurrect(prms[0], &players[plynum]);
				break;
			case 75:
				sanctuary(prms[0], &players[plynum]);
				break;
			case 76:
				turn_undead(prms[0], &players[plynum]);
				break;
			case 77:
				vocation(&players[plynum]);
				break;
			case 78:
				serve(&players[plynum]);
				break;
			case 79:
				shield(&players[plynum]);
				break;
			case 80:
				summon(prms[0], &players[plynum]);
				break;
			case 81:
				tend(prms[0], &players[plynum]);
				break;
			case 82:
				vision(prms[0], &players[plynum]);
				break;
			case 83:
				train(atoi(prms[0]), &players[plynum]);
				break;
			case 84:
				hero_info(&players[plynum]);
				break;
			case 85:
				dismiss(&players[plynum]);
				break;
			case 86:
				revive(msg, &players[plynum]);
				break;
			case 87:
				hero_list(&players[plynum]);
				break;
			case 88:
				len = strlen(prms[0]);
				for(count = 0; count < num_conts; ++count)
					if((STRNCASECMP(prms[0], conts[count].name, len)) == 0)
						break;
				if(count == num_conts)
					show_weather(prms[0], prms[1], &players[plynum]);
				else
					show_weather(prms[1], prms[0], &players[plynum]);
				break;
			case 89:
				thunder(&players[plynum]);
				break;
			case 90:
				commune(&players[plynum]);
				break;
			case 91:
				map(prms[0], &players[plynum]);
				break;
			case 92:
				berserk(prms[0], &players[plynum]);
				break;
			case 93:
				rainbow(&players[plynum]);
				break;
			case 94:
				escape(prms[0], &players[plynum]);
				break;
			case 95:
				penance(prms[0], &players[plynum]);
				break;
			case 96:
				show_player(&players[plynum]);
				break;
			case 97:
				espionage(prms[0], prms[1], &players[plynum]);
				break;
			case 98:
				release(prms[0], &players[plynum]);
				break;
			case 99:
				vassals(prms[0], &players[plynum]);
				break;
			case 100:
				hklog(&players[plynum]);
				break;
			case 101:
				show_vampire(&players[plynum]);
				break;
			case 102:
				if(isdigit(prms[0][0]))
				{
					count = atoi(prms[0]);
					strncpy(str, prms[1], 31);
				}
				else
				{
					count = atoi(prms[1]);
					strncpy(str, prms[0], 31);
				}
				str[31] = 0;
				strip(str);
				len = strlen(str) - 1;
				if(str[len] == 's' || str[len] == 'S')
					str[len] = 0;
				hire(str, count, &players[plynum]);
				break;
			case 103:
				if(isdigit(prms[1][0]))
				{
					count = atoi(prms[1]);
					strncpy(str, prms[2], 31);
				}
				else
				{
					count = atoi(prms[2]);
					strncpy(str, prms[1], 31);
				}
				str[31] = 0;
				strip(str);
				len = strlen(str) - 1;
				if(str[len] == 's' || str[len] == 'S')
					str[len] = 0;
				disband(prms[0], str, count, &players[plynum]);
				break;
			case 104:
				besiege(prms[0], &players[plynum]);
				break;
			case 105:
				formation(msg + strlen(prms[0]) + 1, prms[0], &players[plynum]);
				break;
			case 106:
				reprieve(prms[0], &players[plynum]);
				break;
			case 107:
				if(isdigit(prms[0][0]))
				{
					count = atoi(prms[0]);
					strncpy(str, prms[1], 31);
				}
				else
				{
					count = atoi(prms[1]);
					strncpy(str, prms[0], 31);
				}
				bounty(str, count, &players[plynum]);
				break;
			case 108:
				steward(prms[0], msg + strlen(prms[0]) + 1, &players[plynum]);
				break;
			case 109:
				invade(prms[0], &players[plynum]);
				break;
			case 110:
				ritual(&players[plynum]);
				break;
			case 111:
				post(msg, &players[plynum]);
				break;
			case 112:
				lock(prms[0], &players[plynum]);
				break;
			case 113:
				vacation(atoi(prms[0]), &players[plynum]);
				break;
			case 114:
				show_retaliation(&players[plynum]);
				break;
			case 115:
				blitz(&players[plynum]);
				break;
			case 116:
				blockade(&players[plynum]);
				break;
			case 117:
				ceasefire(&players[plynum]);
				break;
			case 118:
				if((STRCASECMP(prms[1], "ALL")) == 0)
				{
					transfer_army_all(prms[0], prms[2], &players[plynum]);
					break;
				}
				if(isdigit(prms[1][0]))
				{
					count = atoi(prms[1]);
					strncpy(str, prms[2], 31);
				}
				else
				{
					count = atoi(prms[2]);
					strncpy(str, prms[1], 31);
				}
				str[31] = 0;
				strip(str);
				len = strlen(str) - 1;
				if(str[len] == 's' || str[len] == 'S')
					str[len] = 0;
				transfer_army(prms[0], str, count, &players[plynum]);
				break;
			case 119:
				set(msg, &players[plynum]);
				break;
			case 120:
				balance(&players[plynum]);
				break;
			case 121:
				make_questchar(&players[plynum]);
				break;
		}
		save_player(plynum);
		return;
	}
	for(count = 0; count < 13; ++count)
		if((STRCASECMP(cmd, msccmds[count])) == 0)
			break;
	switch(count)
	{
		case 0:
			newplayer(user, host, prms[0]);
			break;
		case 1:
			validate(user, host, prms[0]);
			break;
		case 2:
			kia(user);
			break;
		case 3:
			information(user);
			break;
		case 4:
			help(user, prms[0], prms[1], prms[2]);
			break;
		case 5:
			rank(user);
			break;
		case 6:
			display_news(user);
			break;
		case 7:
			titans(user);
			break;
		case 8:
			kings(user);
			break;
		case 9:
			dcc_cmd(prms[0], host, type, user);
			break;
		case 10:
			list(user);
			break;
		case 11:
			email(prms[0], prms[1], user);
			break;
		case 12:
			game(user);
			break;
		default:
			sprintf(message, "13%c%s%c", MSGDEL, cmd, MSGDEL);
			send_message(message, user, 0);
	}
}

void process_buy(char *arg1, char *arg2, player *plyptr)
{
	char desc[256] = {0};
	char names[][32] = 
	{
		"", "", "", "", "", "", "", "", 
		"", "", "", "Spy", "Move", "Component", "Ship", "Item"
	};
	char type[32] = {0};
	int amount;
	int cprice = 0;
	int *cqty;
	int count;
	int honor = 0;
	int *item;
	int len;
	int marketnum;
	int move = 0;
	int special_troop = 0;
	int troop_num = 0;
	int troop_typenum = 0;

	if(strlen(arg1) < 1)
	{
		help(plyptr->name, "buy", "", "");
		return;
	}
	if((STRCASECMP(arg1, "max")) == 0 || (STRCASECMP(arg2, "max")) == 0)
	{
		if((STRCASECMP(arg1, "max")) == 0)
		{
			amount = process_max(arg2, plyptr);
			strcpy(type, arg2);
		}
		else
		{
			amount = process_max(arg1, plyptr);
			strcpy(type, arg1);
		}
		if(amount <= 0)
			return;
	}
	else
	{
		for(count = 0; count < (int)strlen(arg1); ++count)
			if(!isdigit(arg1[count]))
				break;
		if(count < (int)strlen(arg1))
		{
			amount = atoi(arg2);
			strcpy(type, arg1);
		}
		else
		{
			amount = atoi(arg1);
			strcpy(type, arg2);
		}
	}
	len = strlen(type) - 1;
	if(type[len] == 's' || type[len] == 'S')
		type[len] = 0;
	for(marketnum = 0; marketnum < 16; ++marketnum)
	{
		switch(marketnum)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				troop_num = cities[plyptr->city].troop_num[marketnum];
				if(restrict_markets == 0)
					troop_typenum = plyptr->troop_type;
				else
					troop_typenum = cities[plyptr->city].troop_type[marketnum];
				if(troop_num == -1 || troop_typenum == -1)
					strcpy(names[marketnum], "");
				else
					strcpy(names[marketnum], units[troop_typenum][troop_num].name);
				break;
			case 8:
				strcpy(names[marketnum], food_name);
				break;
			case 9:
				strcpy(names[marketnum], structures[BASE_CLASSNUM(plyptr->classnum)].name);
				break;
			case 10:
				strcpy(names[marketnum], peasant_name);
				break;
			default:
				break;
		}
		if((STRCASECMP(type, names[marketnum])) == 0)
			break;
	}
	if(marketnum == 16)
	{
		if(plyptr->troop_type == TITAN || plyptr->troop_type == VAMPIRE)
		{
			troop_typenum = plyptr->troop_type;
			for(marketnum = RENNUM; marketnum < BLOODGUARDNUM; ++marketnum)
			{
				troop_num = marketnum;
				strcpy(names[marketnum], units[troop_typenum][troop_num].name);
				if((STRCASECMP(type, names[marketnum])) == 0)
					break;
			}
			if(marketnum == BLOODGUARDNUM)
			{
				sprintf(message, "14011%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][5], MSGDEL, type, MSGDEL);
				send_message(message, plyptr->name, 1);
				return;
			}
			special_troop = 1;
		}
		else
		{
			sprintf(message, "14010%c%s%c", MSGDEL, type, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	}
	switch(marketnum)
	{
		case 11:
			buy_spy(plyptr);
			break;
		case 13:
			quest_4(arg2, 0, plyptr);
			break;
		case 14:
			buy_ship(arg2, plyptr);
			break;
		case 15:
			buy_item(arg2, plyptr);
			break;
		default:
			select_item(&cqty, &cprice, &honor, &item, marketnum, &move, special_troop, plyptr);
			if(marketnum < MAXMRK)
				sprintf(desc, "4376%c%s%c", MSGDEL, units[troop_typenum][troop_num].desc, MSGDEL);
			else if(marketnum == 8)
				sprintf(desc, "4112%c%c", MSGDEL, MSGDEL);
			else if(marketnum == 9)
				sprintf(desc, "14012%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].desc, MSGDEL);
			buy(desc, names[marketnum], amount, cprice, cqty, honor, item, marketnum, move, special_troop, plyptr);
	}
}

int process_max(char *arg1, player *plyptr)
{
	char names[][32] = 
	{
		"", "", "", "", "", "", "", "", 
		"", "", "", "Spy", "Move", "Component", "Ship", "Item"
	};
	int amount = -1;
	int cprice = 0;
	int *cqty;
	int honor = 0;
	int *item;
	int len;
	int marketnum;
	int move = 0;
	int special_troop = 0;
	int troop_num = 0;
	int troop_typenum = 0;

	if(strlen(arg1) < 1)
	{
		help(plyptr->name, "max", "", "");
		return(0);
	}
	len = strlen(arg1) - 1;
	if(arg1[len] == 's' || arg1[len] == 'S')
		arg1[len] = 0;
	for(marketnum = 0; marketnum < 16; ++marketnum)
	{
		switch(marketnum)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				troop_num = cities[plyptr->city].troop_num[marketnum];
				if(restrict_markets == 0)
					troop_typenum = plyptr->troop_type;
				else
					troop_typenum = cities[plyptr->city].troop_type[marketnum];
				strcpy(names[marketnum], units[troop_typenum][troop_num].name);
				break;
			case 8:
				strcpy(names[marketnum], food_name);
				break;
			case 9:
				strcpy(names[marketnum], structures[BASE_CLASSNUM(plyptr->classnum)].name);
				break;
			case 10:
				strcpy(names[marketnum], peasant_name);
				break;
			case 11:
			case 12:
				break;
			default:
				sprintf(message, "14010%c%s%c", MSGDEL, arg1, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
		}
		if((STRCASECMP(arg1, names[marketnum])) == 0)
			break;
	}
	if(marketnum == 16)
	{
		if(plyptr->troop_type == TITAN || plyptr->troop_type == VAMPIRE)
		{
			troop_typenum = plyptr->troop_type;
			for(marketnum = RENNUM; marketnum < CATAPULTNUM; ++marketnum)
			{
				troop_num = marketnum;
				strcpy(names[marketnum], units[troop_typenum][troop_num].name);
				if((STRCASECMP(arg1, names[marketnum])) == 0)
					break;
			}
			if(marketnum == CATAPULTNUM)
			{
				sprintf(message, "14011%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][5], MSGDEL, arg1, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			}
			special_troop = 1;
		}
		else
		{
			sprintf(message, "14010%c%s%c", MSGDEL, arg1, MSGDEL);
			send_message(message, plyptr->name, 1);
			return(0);
		}
	}
	select_item(&cqty, &cprice, &honor, &item, marketnum, &move, special_troop, plyptr);
	amount = find_max(names[marketnum], cprice, *cqty, plyptr);
	return(amount);
}

void process_sell(char *arg1, char *arg2, player *plyptr)
{
	char desc[][256] = 
	{
		"", "", ""
	};
	char names[][32] = 
	{
		"", "", "Ship", "Item"
	};
	char type[32];
	int amount = 0;
	int *ccopper = NULL;
	int cprice = 0;
	int *cqty = NULL;
	int *item = NULL;
	int len;
	int money;
	int qty = 0;
	int typenum = 0;

	if(strlen(arg1) < 1 && strlen(arg2) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(isdigit(arg1[0]))
	{
		amount = atoi(arg1);
		strncpy(type, arg2, 31);
	}
	else
	{
		amount = atoi(arg2);
		strncpy(type, arg1, 31);
	}
	type[31] = 0;
	strip(type);
	len = strlen(type) - 1;
	if(type[len] == 's' || type[len] == 'S')
		type[len] = 0;
	strcpy(names[0], food_name);
	strcpy(names[1], structures[BASE_CLASSNUM(plyptr->classnum)].name);
	for(typenum = 0; typenum < 4; ++typenum)
		if((STRCASECMP(type, names[typenum])) == 0)
			break;
	if(typenum == 4)
	{
		sprintf(message, "14013%c%s%c", MSGDEL, type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(typenum < 2 && amount < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(typenum)
	{
		case 0:
			ccopper = &cities[plyptr->city].copper;
			cqty = &cities[plyptr->city].food_qty;
			cprice = cities[plyptr->city].food_buy;
			item = &(plyptr->food[plyptr->cont]);
			sprintf(desc[typenum], "Hope your army is well fed...");
			break;
		case 1:
			money = structures[BASE_CLASSNUM(plyptr->classnum)].cost / 2 * amount;
			ccopper = &money;
			cqty = &qty;
			cprice = structures[BASE_CLASSNUM(plyptr->classnum)].cost / 2;
			item = &(plyptr->castle[plyptr->cont]);
			sprintf(desc[typenum], "It is a desperate move, but with a heavy sack of %s in hand, a new hope arises for your kingdom.", money_name);
			break;
		case 2:
			sell_ship(amount, plyptr);
			return;
		case 3:
			sell_item(arg2, plyptr);
			return;
	}
	sell(desc[typenum], names[typenum], amount, ccopper, cprice, cqty, item, typenum, plyptr);
}

void process_transfer(char *name, char *arg1, char *arg2, int ttype, player *plyptr)
{
	char names[][32] = 
	{
		"", "", "", "", "", "", "", "", 
		"", "", "", "", "move", "land", "honor", 
		"remort", "battles_won", "battles_lost", 
		"troops_killed", "troops_lost"
	};
	char type[32] = {0};
	int amount;
	int *gitem = NULL;
	int *item = NULL;
	int len;
	int plynum;
	int typenum;

	if(strlen(arg1) < 1 || strlen(arg2) < 1)
	{
		switch(ttype)
		{
			case 0:
				help(plyptr->name, "give", "", "");
				break;
			case 1:
				sprintf(message, "17%c%c", MSGDEL, MSGDEL);
				send_message(message, plyptr->name, 1);
				break;
			case 2:
				help(plyptr->name, "bestow", "", "");
				break;
		}
		return;
	}
	if(isdigit(arg2[0]))
	{
		amount = atoi(arg2);
		strncpy(type, arg1, 31);
	}
	else
	{
		amount = atoi(arg1);
		strncpy(type, arg2, 31);
	}
	type[31] = 0;
	strip(type);
	if(ttype == 0)
		plynum = plyptr->ally;
	else
		if((plynum = find_player(name)) <= 0)
		{
			sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	if(playernum(plyptr) == plynum && ttype != 1)
	{
		sprintf(message, "14014%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	len = strlen(type) - 1;
	if(type[len] == 's' || type[len] == 'S')
		type[len] = 0;
	for(typenum = 0; typenum < 20; ++typenum)
	{
		switch(typenum)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				if(units[players[plynum].troop_type][typenum].attack > 0 || units[players[plynum].troop_type][typenum].defend > 0)
					strcpy(names[typenum], units[players[plynum].troop_type][typenum].name);
				gitem = &plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[typenum];
				item = &players[plynum].army[ARMY_DEFENSE][players[plynum].cont].total[typenum];
				break;
			case 8:
				strcpy(names[typenum], food_name);
				gitem = &plyptr->food[plyptr->cont];
				item = &players[plynum].food[players[plynum].cont];
				break;
			case 9:
				strcpy(names[typenum], money_name);
				gitem = &plyptr->copper[plyptr->cont];
				item = &players[plynum].copper[players[plynum].cont];
				break;
			case 10:
				strcpy(names[typenum], peasant_name);
				gitem = &plyptr->peasant[plyptr->cont];
				item = &players[plynum].peasant[players[plynum].cont];
				break;
			case 11:
				strcpy(names[typenum], structures[BASE_CLASSNUM(players[plynum].classnum)].name);
				gitem = &plyptr->castle[plyptr->cont];
				item = &players[plynum].castle[players[plynum].cont];
				break;
			case 12:
				gitem = &plyptr->movepts;
				item = &players[plynum].movepts;
				break;
			case 13:
				gitem = &plyptr->land[plyptr->cont];
				item = &players[plynum].land[players[plynum].cont];
				break;
			case 14:
				gitem = &plyptr->honor[plyptr->cont];
				item = &players[plynum].honor[players[plynum].cont];
				break;
			case 15:
				gitem = &plyptr->remorts;
				item = &players[plynum].remorts;
				break;
			case 16:
				gitem = &plyptr->battles_won;
				item = &players[plynum].battles_won;
				break;
			case 17:
				gitem = &plyptr->battles_lost;
				item = &players[plynum].battles_lost;
				break;
			case 18:
				gitem = &plyptr->troops_killed;
				item = &players[plynum].troops_killed;
				break;
			case 19:
				gitem = &plyptr->troops_lost;
				item = &players[plynum].troops_lost;
				break;
			default:
				break;
		}
		if((STRCASECMP(type, names[typenum])) == 0)
			break;
	}
	if(typenum == 20)
	{
		sprintf(message, "14015%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(ttype)
	{
		case 0:
			give(names[typenum], amount, gitem, item, typenum, &players[plynum], plyptr);
			break;
		case 1:
			cosine(names[typenum], players[plynum].name, amount, item, typenum, plyptr);
			break;
		case 2:
			bestow(names[typenum], amount, gitem, item, typenum, &players[plynum], plyptr);
			break;
	}
}

void select_item(int **cqty, int *cprice, int *honor, int **item, int marketnum, int *move, int special_troop, player *plyptr)
{
	int troop_num = 0;

	switch(marketnum)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if(special_troop == 1)
			{
				troop_num = marketnum;
				ctyqty = 1250 - (250 * troop_num);
				*cqty = &ctyqty;
				*cprice = 50 + (25 * troop_num);
			}
			else
			{
				troop_num = cities[plyptr->city].troop_num[marketnum];
				*cqty = &cities[plyptr->city].troop_qty[marketnum];
				*cprice = cities[plyptr->city].troop_pr[marketnum];
			}
			*item = &(plyptr->army[ARMY_DEFENSE][plyptr->cont].total[troop_num]);
			switch(troop_num)
			{
				case BLOODGUARDNUM:
					*move = 0;
					if(plyptr->troop_type == BARBARIAN || plyptr->troop_type == VAMPIRE)
						*honor = 0;
					else
						*honor = (plyptr->level[plyptr->cont] + 1) * (plyptr->level[plyptr->cont] + 1);
					break;
				case CATAPULTNUM:
					*move = *cprice / 100;
					*honor = -1000;
					break;
				default:
					*move = 0;
					*honor = -1000;
					break;
			}
			break;	
		case 8:
			*cqty = &cities[plyptr->city].food_qty;
			*cprice = cities[plyptr->city].food_pr;
			*item = &(plyptr->food[plyptr->cont]);
			*move = 0;
			*honor = -1000;
			break;
		case 9:
			*cqty = &castleqty;
			*cprice = structures[BASE_CLASSNUM(plyptr->classnum)].cost;
			*item = &(plyptr->castle[plyptr->cont]);
			*move = structures[BASE_CLASSNUM(plyptr->classnum)].mpcost;
			*honor = -1000;
			break;
		case 10:
			*cqty = &cities[plyptr->city].peasant_qty;
			*cprice = cities[plyptr->city].peasant_pr;
			*item = &(plyptr->peasant[plyptr->cont]);
			*move = 0;
			*honor = -1000;
			break;
		case 11:
			*cqty = &cities[plyptr->city].spy_qty;
			*cprice = cities[plyptr->city].spy_pr;
			*move = 0;
			*honor = -1000;
			break;
		case 12:
			*cqty = &cities[plyptr->city].movepts_qty;
			*cprice = cities[plyptr->city].movepts_pr;
			*item = &(plyptr->movepts);
			*move = 0;
			*honor = -1000;
			break;
	}
}
