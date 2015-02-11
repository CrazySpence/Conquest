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

void bless(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BLESS)
	{
		sprintf(message, "1%c%d%cto bless your army%c", MSGDEL, BLESS, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		if(plyptr->special_batmod & spcbless[plyptr->cont])
		{
			sprintf(message, "3002%cyour%c%s%c", MSGDEL, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		plyptr->movepts -= BLESS;
		plyptr->special_batmod += spcbless[plyptr->cont];
		sprintf(message, "3001%cyour%c%s%cyour%c", MSGDEL, MSGDEL, conts[plyptr->cont].name, MSGDEL, MSGDEL);
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
		bless("", plyptr);
		return;
	}
	if(!equal_class(plyptr->classnum, players[plynum].classnum))
	{
		sprintf(message, "3022%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
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
	if(players[plynum].special_batmod & spcbless[plyptr->cont])
	{
		sprintf(message, "3002%c%s's%c%s%c", MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= BLESS;
	players[plynum].special_batmod += spcbless[plyptr->cont];
	sprintf(message, "3001%c%s's%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL, adjectives[players[plynum].gender][0], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3060%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
}

int cleric(player *plyptr)
{
	if(!plyptr)
		return(0);
	if((plyptr->classnum >= 40 && plyptr->classnum < 50) || titan(plyptr))
		return(1);
	else
		return(0);
}

void crusade(char *name, player *plyptr)
{
	int clcnum;
	int crunum;
	int crusaders;
	int maxlvl;
	int maxnum;
	int maxtries = 0;
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 5)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 6, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < CRUSADE)
	{
		sprintf(message, "1%c%d%cto call a crusade%c", MSGDEL, CRUSADE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "crusade", "", "");
		return;
	}
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return;
	plyptr->movepts -= CRUSADE;
	if(plyptr->level[plyptr->cont] >= 7)
	{
		maxlvl = maxlevel[FIGHTER] - 1;
		maxnum = 4;
	}
	else if(plyptr->level[plyptr->cont] >= 6)
	{
		maxlvl = maxlevel[FIGHTER] - 3;
		maxnum = 3;
	}
	else 
	{
		maxlvl = maxlevel[FIGHTER] - 5;
		maxnum = 2;
	}
	crusaders = randomnum(maxnum, 1);
	sprintf(message, "3004%c%s%c%d%c", MSGDEL, players[plynum].name, MSGDEL, crusaders, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3005%c%s%c%s%c%s%c%d%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, 
	crusaders, MSGDEL, select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[plyptr->cont], players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL);
	send_channel(message, 0);
	clcnum = playernum(plyptr);
	while(crusaders > 0 && maxtries++ < 1000)
	{
		if((crunum = find_randomplayer(plyptr->cont, 2)) < 1 || crunum == plynum || clcnum == crunum)
			continue;
		if(barbarian(&players[crunum]) || ranger(&players[crunum]) || vampire(&players[crunum]) || titan(&players[crunum]))
			continue;
		if(players[crunum].special_misc & SPC_VACATION)
			continue;
		if((allow_attack(players[plynum].name, 0, &players[crunum])) == 0)
			continue;
		if((army_size(plyptr->cont, ARMY_CAMPAIGN, &players[crunum])) == 0)
			continue;
		--crusaders;
		players[crunum].isprotected = 1;
		combatants[ATTACKER] = &players[crunum];
		combatants[DEFENDER] = &players[plynum];
		sprintf(message, "3062%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL, 
		select_class(players[plynum].classnum), MSGDEL, 
		select_title(players[plynum].classnum, players[plynum].level[plyptr->cont], players[plynum].gender), MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, players[crunum].name, 1);
		log_event("", message, players[crunum].name);
		start_battle();
	}
	save_player(plynum);
}

void divination(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < DIVINATION)
	{
		sprintf(message, "1%c%d%cto use Divination%c", MSGDEL, DIVINATION, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "divination", "", "");
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
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "3007%c%s%c%s%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL, adjectives[players[plynum].gender][2], MSGDEL, conts[players[plynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= DIVINATION;
	sprintf(message, "3008%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	person("", plyptr, &players[plynum]);
	if(plyptr->level[plyptr->cont]  >= 1)
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

void divine_intervention(player *plyptr)
{
	int count;
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < DIVINEINTERVENTION)
	{
		sprintf(message, "1%c%d%cto request Divine Intervention%c", MSGDEL, DIVINEINTERVENTION, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= DIVINEINTERVENTION;
	if(randomnum(100, 1) <= 70)
	{
		
		sprintf(message, "3009%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "3010%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3011%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_channel(message, 0);
	sprintf(message, "3012%c%s%c%s%c%s%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, select_class(plyptr->classnum), MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
	plynum = playernum(plyptr);
	for(count = 1; count < current_player; ++count)
	{
		if(strlen(players[count].name) < 1 || players[count].level[players[count].cont] < 0 || count == plynum)
			continue;
		if(!equal_class(plyptr->classnum, players[count].classnum) || players[count].special_misc & SPC_VACATION)
			continue;
		players[count].attacks = 0;
		add_move(&players[count]);
		sprintf(message, "3013%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
		send_message(message, players[count].name, 1);
		log_event("", message, players[count].name);
		save_player(count);
	}
}

void excommunicate(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 7)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 7, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < EXCOMMUNICATE)
	{
		sprintf(message, "1%c%d%cto excommunicate%c", MSGDEL, EXCOMMUNICATE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "excommunicate", "", "");
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
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((!fighter(&players[plynum]) && !cleric(&players[plynum])) || titan(&players[plynum]) || check_hero(HERO_BISHOP, &players[plynum]))
	{
		sprintf(message, "3014%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!equal_class(plyptr->classnum, players[plynum].classnum))
	{
		sprintf(message, "3022%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= EXCOMMUNICATE;
	if(randomnum(100, 1) < (players[plynum].levelmax + 1) * 9 + (players[plynum].honor[players[plynum].cont] / 5))
	{
		sprintf(message, "3015%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "3016%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		log_event("", message, players[plynum].name);
		return;
	}
	sprintf(message, "3017%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3018%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3019%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "3020%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	log_event("", message, players[plynum].name);
	sprintf(message, "3021%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, players[plynum].name, MSGDEL, players[plynum].name, MSGDEL);
	send_channel(message, 0);
	make_barb(CLASSCHANGE + 2, &players[plynum]);
	save_player(plynum);
}

void grace(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 1)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < GRACE)
	{
		sprintf(message, "1%c%d%cto grace anyone%c", MSGDEL, GRACE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "grace", "", "");
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
	if(!equal_class(plyptr->classnum, players[plynum].classnum))
	{
		sprintf(message, "3022%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
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
	grace_player(plynum, plyptr);
}

void grace_player(int plynum, player *plyptr)
{
	int honor;
	int max = 2;
	int min = 1;

	if(plyptr->level[plyptr->cont] >= 5)
	{
		max = 10;
		min = 8;
	}
	else if(plyptr->level[plyptr->cont] >= 3)
	{
		max = 6;
		min = 4;
	}
	honor = randomnum(max, min);
	plyptr->movepts -= GRACE;
	players[plynum].honor[players[plynum].cont] += honor;
	sprintf(message, "3023%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3024%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	save_player(plynum);
}

void make_fighter(int cont, player *cleric, player *plyptr)
{
	int copper;
	int land;
	int peasant;

	land = (int)(plyptr->land[cont] * .20);
	peasant = (int)(plyptr->peasant[cont] * .20);
	copper = (int)(plyptr->copper[cont] * .20);
	plyptr->land[cont] -= land;
	plyptr->peasant[cont] -= peasant;
	plyptr->copper[cont] -= copper;
	default_formation(cont, plyptr);
	cleric->land[cleric->cont] += land;
	cleric->peasant[cleric->cont] += peasant;
	cleric->copper[cleric->cont] += copper;
	if(plyptr->level[cont] > 2)
		change_level(cont, -(plyptr->level[cont] - 2), plyptr);
}

void penance(char *name, player *plyptr)
{
	int count;
	int hero;
	int oldclass;
	int plynum;

	if(!barbarian(plyptr))
	{
		sprintf(message, "3025%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "penance", "", "");
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
	if(!cleric(&players[plynum]) || titan(&players[plynum]))
	{
		sprintf(message, "3027%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(players[plynum].level[plyptr->cont] < 3)
	{
		sprintf(message, "3028%c%s%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, select_title(players[plynum].classnum, 3, players[plynum].gender), MSGDEL, adjectives[players[plynum].gender][1], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < PENANCE)
	{
		sprintf(message, "1%c%d%cto do penance for your crimes%c", MSGDEL, PENANCE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < PENANCECOPPER)
	{
		sprintf(message, "3029%c%d%c%s%c", MSGDEL, PENANCECOPPER, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= PENANCE;
	plyptr->copper[plyptr->cont] -= PENANCECOPPER;
	remove_vassal(plyptr);
	--totply[plyptr->classnum];
	oldclass = plyptr->classnum;
	if(associate_races == 1)
		plyptr->classnum = players[plynum].classnum - 40;
	else
		plyptr->classnum = randomnum(total_races[FIGHTER] - 1, 0);
	++totply[plyptr->classnum];
	if(players[plynum].vassals.total < MAXVAS)
	{
		add_vassal(plynum, plyptr);
		plyptr->lord = plynum;
	}
	if((hero = find_hero(plyptr)) >= 0 && !hero_serve(hero, plyptr))
		dismiss(plyptr);
	if(!check_hero(HERO_NECROMANCER, plyptr))
		plyptr->troop_type = troop_type(plyptr->classnum);
	for(count = 0; count < num_conts; ++count)
		if(plyptr->level[count] >= 0)
			make_fighter(count, &players[plynum], plyptr);
	sprintf(message, "3030%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, players[plynum].name, MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL);
	sprintf(message, "3031%c%s%c%s%c", MSGDEL, select_class(oldclass), MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "3032%c%s%c%s%c", MSGDEL, select_class(oldclass), MSGDEL, plyptr->name, MSGDEL);
	log_event("", message, players[plynum].name);
	sprintf(message, "3033%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL, select_class(oldclass), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	save_player(plynum);
	check_troops(plyptr);
	check_vassals(plyptr);
}

void resurrect(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 5)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 5, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < RESURRECT)
	{
		sprintf(message, "1%c%d%cto use Resurrect%c", MSGDEL, RESURRECT, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "resurrect", "", "");
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
	if(!equal_class(plyptr->classnum, players[plynum].classnum))
	{
		sprintf(message, "3022%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
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
	resurrect_player(plynum, plyptr);
}

void resurrect_player(int plynum, player *plyptr)
{
	int amount;
	int cont;
	int count;
	int current;
	int max = 3;
	int min = 1;
	int pct;
	int total;

	if(players[plynum].troop_type == VAMPIRE)
	{
		sprintf(message, "3034%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(players[plynum].troops_lost < 1)
	{
		sprintf(message, "3035%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] >= 7)
	{
		max = 5;
		min = 3;
	}
	else if(plyptr->level[plyptr->cont] >= 6)
	{
		max = 4;
		min = 2;
	}
	pct = randomnum(max, min);
	if((amount = players[plynum].troops_lost * pct / (players[plynum].troops_lost < 100?10:100)) < 1)
		amount = 1;
	if((amount = fill_castles(amount, players[plynum].cont, &players[plynum])) <= 0)
	{
		sprintf(message, "3036%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= RESURRECT;
	cont = players[plynum].cont;
	plyptr->honor[plyptr->cont] += players[plynum].level[cont] / 2;
	for(count = RENNUM, total = amount; count < BLOODGUARDNUM; ++count)
	{
		if(count == RENNUM && total % BLOODGUARDNUM != 0)
			current = (total / BLOODGUARDNUM) + (total % BLOODGUARDNUM);
		else
			current = total / BLOODGUARDNUM;
		if(units[players[plynum].troop_type][count].attack > 0 && units[players[plynum].troop_type][count].defend > 0)
			players[plynum].army[ARMY_DEFENSE][cont].total[count] += current;
	}
	players[plynum].troops_lost -= amount;
	sprintf(message, "3037%c%d%c%s%c", MSGDEL, amount, MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3038%c%s%c%d%c", MSGDEL, plyptr->name, MSGDEL, amount, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	save_player(plynum);
}

void sanctuary(char *name, player *plyptr)
{
	int plynum;

	if(!cleric(plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 4)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 4, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < SANCTUARY)
	{
		sprintf(message, "1%c%d%cto provide Sanctuary%c", MSGDEL, SANCTUARY, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "sanctuary", "", "");
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
	if(!equal_class(plyptr->classnum, players[plynum].classnum))
	{
		sprintf(message, "3022%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
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
	if(players[plynum].isprotected == 0)
	{
		sprintf(message, "3039%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sanctuary_player(plynum, plyptr);
}

void sanctuary_player(int plynum, player *plyptr)
{
	plyptr->movepts -= SANCTUARY;
	players[plynum].isprotected = 0;
	sprintf(message, "3040%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3041%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	sprintf(message, "3042%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	log_event("", message, players[plynum].name);
	sprintf(message, "3043%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick,MSGDEL, 
	select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL);
	send_channel(message, 0);
	save_player(plynum);
}

void tithe(int cont, player *plyptr)
{
	int amount;
	int base;
	int classtype = -1;
	int level;

	if(titan(plyptr))
		return;
	if(cleric(plyptr) || check_hero(HERO_BISHOP, plyptr))
		classtype = CLERIC;
	else if(ranger(plyptr))
		classtype = RANGER;
	level = plyptr->level[cont];
	if(check_hero(HERO_BISHOP, plyptr) && level < 6)
		level = 6;
	switch(classtype)
	{
		case CLERIC:
			base = 50000 + (plyptr->level[cont] * 20000);
			if(plyptr->copper[cont] > base)
			{
				amount = plyptr->copper[cont] - base;
				plyptr->copper[cont] -= amount;
				sprintf(message, "3044%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
				send_message(message, plyptr->name, 1);
				log_event("", message, plyptr->name);
			}
			break;
		case RANGER:
			base = 60000 + (25000 * plyptr->level[cont]);
			if(plyptr->copper[cont] > base)
			{
				amount = plyptr->copper[cont] - base;
				plyptr->copper[cont] -= amount;
				sprintf(message, "3045%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
				send_message(message, plyptr->name, 1);
				log_event("", message, plyptr->name);
			}
		default:
			break;
	}
}

void turn_undead(char *name, player *plyptr)
{
	int cont;
	int count;
	int max = 10;
	int maxlvl = ARCHERNUM;
	int min = 5;
	int num;
	int plynum;

	if(!cleric(plyptr) && !check_hero(HERO_BISHOP, plyptr))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 3)
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < TURNUNDEAD)
	{
		sprintf(message, "1%c%d%cto turn Undead%c", MSGDEL, TURNUNDEAD, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "turn", "", "");
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
	if(players[plynum].troop_type != VAMPIRE)
	{
		sprintf(message, "3046%c%s%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, players[plynum].name, MSGDEL);
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
	plyptr->movepts -= TURNUNDEAD;
	if(save_throw(&players[plynum]))
	{
		sprintf(message, "3047%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "3048%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		send_message(message, players[plynum].name, 1);
		sprintf(message, "3049%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		log_event("", message, players[plynum].name);
		return;
	}
	cont = players[plynum].cont;
	if(plyptr->level[plyptr->cont] >= 7)
	{
		maxlvl = MERC2;
		max = 20;
		min = 15;
	}
	else if(plyptr->level[plyptr->cont] >= 5)
	{
		maxlvl = BLOODGUARDNUM;
		max = 15;
		min = 10;
	}
	num = randomnum(max, min);
	for(count = 0; count <= maxlvl; ++count)
	{
		if(count == CATAPULTNUM)
			continue;
		players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] * num / 100;
	}
	sprintf(message, "3050%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3051%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	sprintf(message, "3052%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, 
	MSGDEL, vampire(&players[plynum])?"Vampire":heroes[HERO_NECROMANCER].type, MSGDEL, conts[players[plynum].cont].name, MSGDEL);
	send_channel(message, 0);
	save_player(plynum);
}

void vocation(player *plyptr)
{
	int count;
	int hero;
	int oldclass;

	if(allow_cleric == 0)
	{
		sprintf(message, "6%c%s%c", MSGDEL, select_classname(CLERIC), MSGDEL);
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
		sprintf(message, "3053%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < VOCATION)
	{
		sprintf(message, "1%c%d%cto become a %s%c", MSGDEL, VOCATION, MSGDEL, select_classname(CLERIC), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < class_change[CLERIC])
	{
		sprintf(message, "3054%c%d%c%s%c", MSGDEL, class_change[CLERIC], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!cleric(&players[plyptr->lord]))
		remove_vassal(plyptr);
	plyptr->movepts -= VOCATION;
	plyptr->copper[plyptr->cont] -= class_change[CLERIC];
	oldclass = plyptr->classnum;
	--totply[plyptr->classnum];
	plyptr->classnum = change_class(CLERIC, plyptr);
	++totply[plyptr->classnum];
	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0)
			continue;
		if(plyptr->level[count] > 2)
			change_level(count, -(plyptr->level[count] - 2), plyptr);
		check_assets(count, CLASSCHANGE, plyptr);
		tithe(count, plyptr);
	}	
	sprintf(message, "3055%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3056%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "3057%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->nick, MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
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
