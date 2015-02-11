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

int allow_spell(char *name, player *plyptr)
{
	int plynum;

	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr == &players[plynum])
	{
		sprintf(message, "11038%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(check_artifact(ORB, &players[plynum]) || players[plynum].city == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return(0);
	return(plynum);
}

void beast_lost(player *plyptr, char *desc, int type)
{
	if(count_casualties(DEFENDER))
		show_casualties(DEFENDER, ARMY_DEFENSE, plyptr);
	switch(type)
	{
		case GIANT:
		case DRAGON:
		case BEHEMOTH:
			sprintf(message, "11000%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, desc, MSGDEL, desc, MSGDEL);
			send_channel(message, 0);
			if(!barbarian(plyptr) && !vampire(plyptr))
				plyptr->honor[plyptr->cont] += type;
			break;
		case PACK:
			sprintf(message, "11001%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, desc, MSGDEL, desc, MSGDEL);
			send_channel(message, 0);
			if(!barbarian(plyptr) && !vampire(plyptr))
				++plyptr->honor[plyptr->cont];
			break;
	}
}

void beast_won(player *plyptr, char *desc, int type)
{
	if(count_casualties(DEFENDER))
		show_casualties(DEFENDER, ARMY_DEFENSE, plyptr);
	switch(type)
	{
		case GIANT:
		case DRAGON:
		case BEHEMOTH:
			sprintf(message, "11002%c%s%c%s%c", MSGDEL, desc, MSGDEL, plyptr->nick, MSGDEL);
			send_channel(message, 0);
			if(plyptr->castle[plyptr->cont] >= 2)
			{
				plyptr->castle[plyptr->cont] -= 1;
				sprintf(message, "11003%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, desc, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				send_channel(message, 0);
				sprintf(message, "11004%c%s%c%s%c", MSGDEL, desc, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
				log_event("", message, plyptr->name);
			}
			if(!barbarian(plyptr) && !vampire(plyptr))
				plyptr->honor[plyptr->cont] -= type;
			break;
		case PACK:
			sprintf(message, "11005%c%s%c%s%c", MSGDEL, desc, MSGDEL, plyptr->nick, MSGDEL);
			send_channel(message, 0);
			if(!barbarian(plyptr) && !vampire(plyptr))
				--plyptr->honor[plyptr->cont];
			break;
	}
}

void book(player *plyptr)
{
	char cmp_list[16] = {0};
	char *ptr = NULL;
	char spell_list[2048] = {0};
	int bin;
	int cmp;
	int count;
	int level;
	int splnum;

	if(!mage(plyptr) && !check_hero(HERO_DRAGON, plyptr))
	{
		sprintf(message, "10029%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(titan(plyptr))
		level = maxlevel[MAGE];
	else
	{
		if(check_hero(HERO_DRAGON, plyptr) && !mage(plyptr)) 
		{
			if((level = (plyptr->level[plyptr->cont] / 2) - 1) < 0)
				level = 0;
		}
		else
			level = plyptr->level[plyptr->cont];
		if(mage(plyptr) && check_artifact(STAFF, plyptr))
		{
			++level;
			if(check_hero(HERO_WARLOCK, plyptr) && level + 1 <= maxlevel[BASE_CLASSNUM(plyptr->classnum)])
				++level;
		}
	}
	sprintf(message, "11093%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count <= maxlevel[MAGE]; ++count)
	{
		memset(spell_list, 0, 2048);
		for(splnum = 0; splnum < TOTSPL; ++splnum)
		{
			if(spells[splnum].level != count)
				continue;
			memset(cmp_list, 0, 16);
			for(cmp = GENSING; cmp <= ETHER; ++cmp)
			{
				bin = (int)pow((double)2, (double)cmp);
				if(spells[splnum].components & bin)
				{
					if(cmp == ETHER)
						sprintf(cmp_list, "%s?,", cmp_list);
					else
						sprintf(cmp_list, "%s%c, ", cmp_list, components[cmp][0]);
				}
			}
			if((ptr = strrchr(cmp_list, ',')) != NULL)
				*ptr = 0;
			sprintf(spell_list, "%s%s (%s) ", spell_list, spells[splnum].name, cmp_list);
		}
		sprintf(message, "%d%c%s%c", 11094 + count, MSGDEL, (level >= count?spell_list:"[-None-]"), MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "11104%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void cast(char *spellname, char *arg1, char *arg2, char *arg3, player *plyptr)
{
	int bin;
	int count;
	int level;
	int movepts;
	int plynum;
	int spellnum;
	int spellok = 0;

	if(strlen(spellname) < 1)
	{
		help(plyptr->name, "cast", "", "");
		return;
	}
	if(!mage(plyptr) && !check_hero(HERO_DRAGON, plyptr))
	{
		sprintf(message, "10029%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_artifact(ORB, plyptr) || plyptr->city == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(count = 0; count < TOTSPL; ++count)
		if((STRCASECMP(spellname, spells[count].name)) == 0)
			break;
	if(count == TOTSPL)
	{
		sprintf(message, "11007%c%s%c", MSGDEL, spellname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	spellnum = count;
	if(check_hero(HERO_DRAGON, plyptr) && !mage(plyptr)) 
	{
		if((level = (plyptr->level[plyptr->cont] / 2) - 1) < 0)
			level = 0;
	}
	else
		level = plyptr->level[plyptr->cont];
	if(mage(plyptr) && check_artifact(STAFF, plyptr))
	{
		++level;
		if(check_hero(HERO_WARLOCK, plyptr) && level + 1 <= maxlevel[BASE_CLASSNUM(plyptr->classnum)])
			++level;
	}
	if(level < spells[spellnum].level && !titan(plyptr))
	{
		sprintf(message, "11008%c%s%c", MSGDEL, select_title(plyptr->classnum, spells[spellnum].level, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	movepts = spells[spellnum].move;
	if(mage(plyptr))
	{
		if(plyptr->level[plyptr->cont] >= 7 && spells[spellnum].level <= 4)
			movepts /= 2;
		else if(plyptr->level[plyptr->cont] >= 6 && spells[spellnum].level <= 3)
			movepts /= 2;
		else if(plyptr->level[plyptr->cont] >= 5 && spells[spellnum].level <= 2)
			movepts /= 2;
	}
	if(plyptr->movepts < movepts)
	{
		sprintf(message, "1%c%d%cto cast %s%c", MSGDEL, movepts, MSGDEL, spells[spellnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(titan(plyptr))
		for(count = 0; count < MAXCMP; ++count)
			if(plyptr->components[count] < 1)
				plyptr->components[count] = 1;
	for(count = 0; count < MAXCMP; ++count)
	{
		bin = (int)pow((double)2, (double)count);
		if(spells[spellnum].components & bin)
			if(plyptr->components[count] < 1)
			{
				need_components(spells[spellnum].name, spells[spellnum].components, plyptr);
				return;
			}
	}
	if(plyptr->attacks >= attacks_allowed(plyptr) && spells[spellnum].attack)
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(emperor(arg1))
	{
		if((plynum = find_player(arg1)) > 0)
		{
			strcpy(arg1, plyptr->name);
			plyptr = &players[plynum];
			sprintf(message, "11009%c%s%c%s%c%s%c%s%c", MSGDEL, 
			adjectives[plyptr->gender][0], MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
			send_message(message, arg1, 1);
		}
	}
	switch(spellnum)
	{
		case 0:
			spellok = spell_gender(arg1, plyptr);
			break;
		case 1:
			spellok = spell_createfood(plyptr);
			break;
		case 2:
			spellok = spell_fireball(arg1, plyptr);
			break;
		case 3:
			spellok = spell_truesight(arg1, plyptr);
			break;
		case 4:
			spellok = spell_chaos(arg1, plyptr);
			break;
		case 5:
			spellok = spell_forget(arg1, plyptr);
			break;
		case 6:
			spellok = spell_giant(arg1, plyptr);
			break;
		case 7:
			spellok = spell_polymorph(arg1, plyptr);
			break;
		case 8:
			spellok = spell_strength(plyptr);
			break;
		case 9:
			spellok = spell_famine(arg1, plyptr);
			break;
		case 10:
			spellok = spell_shadowwarriors(arg1, plyptr);
			break;
		case 11:
			spellok = spell_teleport(arg1, plyptr);
			break;
		case 12:
			spellok = spell_weather(arg1, arg2, arg3, plyptr);
			break;
		case 13:
			spellok = spell_enchant(plyptr);
			break;
		case 14:
			spellok = spell_flamestrike(arg1, plyptr);
			break;
		case 15:
			spellok = spell_move(arg1, plyptr);
			break;
		case 16:
			spellok = spell_sleep(arg1, plyptr);
			break;
		case 17:
			spellok = spell_vanish(arg1, plyptr);
			break;
		case 18:
			spellok = spell_charm(arg1, plyptr);
			break;
		case 19:
			spellok = spell_dragon(arg1, plyptr);
			break;
		case 20:
			spellok = spell_maelstorm(arg1, plyptr);
			break;
		case 21:
			spellok = spell_meteorstorm(arg1, plyptr);
			break;
		case 22:
			spellok = spell_portal(arg1, plyptr);
			break;
		case 23:
			spellok = spell_behemoth(arg1, plyptr);
			break;
		case 24:
			spellok = spell_rampage(arg1, plyptr);
			break;
		case 25:
			spellok = spell_wizardlock(arg1, plyptr);
			break;
		case 26:
			spellok = spell_death(arg1, plyptr);
			break;
		case 27:
			spellok = spell_juggernaut(arg1, plyptr);
			break;
		case 28:
			spellok = spell_plague(arg1, plyptr);
			break;
		case 29:
			spellok = spell_earthquake(arg1, plyptr);
			break;
		case 30:
			spellok = spell_kraken(plyptr);
			break;
		case 31:
			spellok = spell_wish(arg1, arg2, plyptr);
			break;
		case 32:
			spellok = spell_alterreality(0, plyptr);
			break;
		case 33:
			spellok = spell_season(plyptr);
			break;
	}
	if(spellok)
	{
		plyptr->movepts -= movepts;
		for(count = 0; count < MAXCMP; ++count)
		{
			bin = (int)pow((double)2, (double)count);
			if(spells[spellnum].components & bin)
				--plyptr->components[count];
		}
		if(spellok & 2)
		{
			plyptr->lastattack = time(NULL);
			plyptr->attacks += spells[spellnum].attack;
			plyptr->isprotected = 1;
		}
		if(spells[spellnum].level > plyptr->level[plyptr->cont] && check_artifact(STAFF, plyptr))
		{
			if(spells[spellnum].level > plyptr->level[plyptr->cont] + 1)
				count = 20;
			else
				count = 10;
			if(randomnum(100, 1) <= count)
				drop(plyptr);
		}
	}
}

void change_player(int classnum, int cont, player *plyptr)
{
	int count;

	--totlvl[cont][plyptr->level[cont]];
	plyptr->level[cont] = randomnum(maxlevel[BASE_CLASSNUM(classnum)] - 2, 0);
	++totlvl[cont][plyptr->level[cont]];
	plyptr->land[cont] = promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][1];
	plyptr->peasant[cont] = promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][2];
	plyptr->food[cont] = promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][0];
	for(count = 0; count < MAXTRP; ++count)
		plyptr->army[ARMY_CAMPAIGN][cont].total[count] = 0;
	for(count = 0; count < MAXTRP; ++count)
		switch(count)
		{
			case SOLDIERNUM:
				plyptr->army[ARMY_DEFENSE][cont].total[count] = (int)(promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][0] * .40);
				break;
			case ARCHERNUM:
			case KNIGHTNUM:
			case BLOODGUARDNUM:
				if(units[plyptr->troop_type][count].attack > 0 && units[plyptr->troop_type][count].defend > 0)
					plyptr->army[ARMY_DEFENSE][cont].total[count] = (int)(promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][0] * .20);
				else
					plyptr->army[ARMY_DEFENSE][cont].total[SOLDIERNUM] += (int)(promotions[BASE_CLASSNUM(classnum)][plyptr->level[cont]][0] * .20);
				break;
			default:
				plyptr->army[ARMY_DEFENSE][cont].total[count] = 0;
				break;
		}
	if(units[plyptr->troop_type][CATAPULTNUM].attack > 0)
		plyptr->army[ARMY_DEFENSE][cont].total[CATAPULTNUM] = randomnum(8, 2);
	default_formation(cont, plyptr);
	plyptr->castle[cont] = combined_army(cont, plyptr) / castle_max(classnum);
	if(combined_army(cont, plyptr) % castle_max(classnum) > 0)
		++plyptr->castle[cont];
	if(plyptr->castle[cont] < 1)
		plyptr->castle[cont] = 1;
	if(barbarian(plyptr))
		plyptr->honor[cont] = 0;
	else
		plyptr->honor[cont] = (plyptr->level[cont] + 1) * 2;
}

void component_market(int ctynum, player *plyptr)
{
	int bin;

	sprintf(message, "11010%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11105%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11106%c%-16s%c[-Closed-]%c", MSGDEL, "??????", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	bin = (int)pow((double)2, (double)MANDRAKE) * 1000;
	sprintf(message, "11107%c%-16s%c%d%c", MSGDEL, components[MANDRAKE], MSGDEL, bin, MSGDEL);
	send_message(message, plyptr->name, 1);
	bin = (int)pow((double)2, (double)NIGHTSHADE) * 1000;
	sprintf(message, "11108%c%-16s%c%d%c", MSGDEL, components[NIGHTSHADE], MSGDEL, bin, MSGDEL);
	send_message(message, plyptr->name, 1);
	bin = (int)pow((double)2, (double)SAGE) * 1000;
	sprintf(message, "11109%c%-16s%c%d%c", MSGDEL, components[SAGE], MSGDEL, bin, MSGDEL);
	send_message(message, plyptr->name, 1);
	bin = (int)pow((double)2, (double)CARDAMOM) * 1000;
	sprintf(message, "11110%c%-16s%c%d%c", MSGDEL, components[CARDAMOM], MSGDEL, bin, MSGDEL);
	send_message(message, plyptr->name, 1);
	bin = (int)pow((double)2, (double)GENSING) * 1000;
	sprintf(message, "11111%c%-16s%c%d%c", MSGDEL, components[GENSING], MSGDEL, bin, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void create_guise(int plynum)
{
	int maxtries = 0;
	int num;

	do {
		if((num = find_randomplayer(-1, -1)) < 1 || num == plynum)
			continue; 
		strcpy(players[plynum].nick, players[num].nick);
	} while(maxtries++ < 1000);
}

void devote(player *plyptr)
{
	int count;
	int hero;
	int oldclass;

	if(allow_mage == 0)
	{
		sprintf(message, "6%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
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
	if(plyptr->movepts < DEVOTE)
	{
		sprintf(message, "1%c%d%cto devote your life to magic%c", MSGDEL, DEVOTE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < class_change[MAGE])
	{
		sprintf(message, "11013%c%d%c%s%c", MSGDEL, class_change[MAGE], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!mage(&players[plyptr->lord]))
		remove_vassal(plyptr);
	plyptr->movepts -= DEVOTE;
	plyptr->copper[plyptr->cont] -= class_change[MAGE];
	oldclass = plyptr->classnum;
	--totply[plyptr->classnum];
	plyptr->classnum = change_class(MAGE, plyptr);
	++totply[plyptr->classnum];
	for(count = 0; count < num_conts; ++count)
	{
		if(plyptr->level[count] < 0)
			continue;
		if(plyptr->level[count] > 2)
			change_level(count, -(plyptr->level[count] - 2), plyptr);
		check_assets(count, CLASSCHANGE,  plyptr);
	}
	plyptr->components[0] = 1;
	for(count = 1; count < 6; ++count)
		plyptr->components[count] = 0;
	sprintf(message, "11014%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11015%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_classname(MAGE), MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11016%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, plyptr->nick, MSGDEL);
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

void fail_save(char *desc, char *spellname, player *plyptr, player *target)
{
	if(target)
	{
		sprintf(message, "11017%c%s%c%s%c%s%c", MSGDEL, desc, MSGDEL, spellname, MSGDEL, target->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11018%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, spellname, MSGDEL, target->kname[target->cont], MSGDEL, conts[target->cont].name, MSGDEL);
		send_message(message, target->name, 1);
		sprintf(message, "11019%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, spellname, MSGDEL, target->kname[target->cont], MSGDEL, conts[target->cont].name, MSGDEL);
		log_event("", message, target->name);
	}
	else
	{
		sprintf(message, "11020%c%s%c%s%c", MSGDEL, desc, MSGDEL, spellname, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void get_components(int type, player *plyptr)
{
	int amount;
	int count;
	int min = 1;
	int max;
	int maxcmp;
	int total = 0;

	if(!mage(plyptr) || plyptr->levelmax < 2)
		return;
	if(plyptr->levelmax >= 6)
	{
		max = 4;
		maxcmp = MAXCMP - 2;
	} else if(plyptr->levelmax >= 4)
	{
		max = 3;
		maxcmp = MAXCMP - 3;
	}
	else
	{
		max = 2;
		maxcmp = MAXCMP - 4;
	}
	for(count = 0; count <= maxcmp; ++count)
	{
		amount = randomnum(max, min);
		if(plyptr->components[count] + amount  > MAXCOMPONENT)
			amount = MAXCOMPONENT - plyptr->components[count];
		plyptr->components[count] += amount;
		total += amount;
	}
	if(total > 0)
	{
		sprintf(message, "11115%c%d%c", MSGDEL, total, MSGDEL);
		log_event("", message, plyptr->name);
	}
}

int mage(player *plyptr)
{
	if(!plyptr)
		return(0);
	if((plyptr->classnum >= 20 && plyptr->classnum < 30) || titan(plyptr))
		return(1);
	else
		return(0);
}

void made_save(char *spellname, player *plyptr, player *target)
{
	sprintf(message, "11021%c%s%c%s%c%s%c", MSGDEL, target->name, MSGDEL, target->name, MSGDEL, spellname, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11022%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, spellname, MSGDEL, target->kname[target->cont], MSGDEL, conts[target->cont].name, MSGDEL);
	send_message(message, target->name, 1);
	sprintf(message, "11023%c%s%c%s%c%s%c", MSGDEL, spellname, MSGDEL, plyptr->nick, MSGDEL, conts[target->cont].name, MSGDEL);
	log_event("", message, target->name);
}

void need_components(char *spellname, int component, player *plyptr)
{
	int bin;
	int count;

	sprintf(message, "11024%c%s%c", MSGDEL, spellname, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11025%c", MSGDEL);
	for(count = GENSING; count <= ETHER; ++count)
	{
		bin = (int)pow((double)2, (double)count);
		if(component & bin)
		{
			if(count == ETHER)
				sprintf(message, "%s?????", message);
			else
				sprintf(message, "%s%s ", message, components[count]);
		}
	}
	sprintf(message, "%s%c", message, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void quest_4(char *component, int type, player *plyptr)
{
	int bin;
	int count;
	int move;

	if(!mage(plyptr) && !check_hero(HERO_DRAGON, plyptr))
	{
		sprintf(message, "10029%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(component) < 1)
	{
		if(type || plyptr->city != magic_city)
		{
			sprintf(message, "17%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		else
			component_market(plyptr->city, plyptr);
		return;
	}
	if((STRCASECMP(component, "all")) == 0)
	{
		for(count = 0; count < MAXCMP - 1; ++count)
			quest_4(components[count], type, plyptr);
		return;
	}
	for(count = 0; count < MAXCMP; ++count)
		if((STRCASECMP(component, components[count])) == 0)
			break;
	if(count == MAXCMP)
	{
		sprintf(message, "11026%c%s%c", MSGDEL, component, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->components[count] + 1 > MAXCOMPONENT)
	{
		sprintf(message, "11027%c%s%c", MSGDEL, components[count], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	bin = (int)pow((double)2, (double)count);
	if(type)
	{
		move = bin;
		if(magic_city >= 0)
			if(plyptr->cont != cities[magic_city].cont)
				move /= 2;
		if(plyptr->movepts < move)
		{
			sprintf(message, "1%c%d%cto seek out %s%c", MSGDEL, move, MSGDEL, components[count], MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		plyptr->movepts -= move;
	}
	else
	{
		if(plyptr->city != magic_city)
		{
			if(magic_city < 0)
				sprintf(message, "11028%c%c", MSGDEL, MSGDEL);
			else
				sprintf(message, "11029%c%s%c%s%c", MSGDEL, cities[magic_city].name, MSGDEL, conts[cities[magic_city].cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(plyptr->copper[plyptr->cont] < bin * 1000)
		{
			sprintf(message, "11030%c%s%c%s%c%d%c%s%c", MSGDEL, cities[magic_city].name, MSGDEL, components[count], MSGDEL, bin * 1000, MSGDEL, money_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		plyptr->copper[plyptr->cont] -= bin * 1000;
		cities[plyptr->city].copper += bin * 1000;
	}
	++plyptr->components[count];
	if(type)
		sprintf(message, "11031%c%s%c", MSGDEL, components[count], MSGDEL);
	else
		sprintf(message, "11032%c%s%c", MSGDEL, components[count], MSGDEL);
	send_message(message, plyptr->name, 1);
}

void read_spells()
{
	char input[1024] = {0};
	FILE *fp;
	int count = 0;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, SPLFIL);
	if((fp = fopen(SPLFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, SPLFIL, strerror(errno));
		exit_conquest();
	}
	while((fgets(input, 1024, fp)) != NULL && count < TOTSPL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%31s %d %d %d %d", spells[count].name, &spells[count].level, &spells[count].components, &spells[count].attack, &spells[count].move);
		if(spells[count].level < 0)
			spells[count].level = 0;
		if(spells[count].components < 0)
			spells[count].components = 0;
		if(spells[count].attack < 0 || spells[count].attack > 1)
			spells[count].attack = 0;
		if(spells[count].move < 0)
			spells[count].move = 0;
		++count;
	}
	fclose(fp);
	if(count < TOTSPL)
	{
		fprintf(stderr, "%s Error reading %s: count should be %d not %d\n", NOWSTR, SPLFIL, TOTSPL, count);
		exit_conquest();
	}
}

int save_throw(player *plyptr)
{
	int base = 15;

	base += ((plyptr->level[plyptr->cont] + 1) * 5);
	if(mage(plyptr))
		base += 10;
	if(check_artifact(STAFF, plyptr))
		base += 10;
	if(titan(plyptr))
		base += 10;
	if(randomnum(100, 1) >= base)
		return(0);
	else
		return(1);
}

void show_components(player *plyptr)
{
	if(!mage(plyptr) && !check_hero(HERO_DRAGON, plyptr))
	{
		sprintf(message, "10029%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "11112%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11106%c%-16s%c%d%c", MSGDEL, "?????", MSGDEL, plyptr->components[ETHER], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11107%c%-16s%c%d%c", MSGDEL, components[MANDRAKE], MSGDEL, plyptr->components[MANDRAKE], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11108%c%-16s%c%d%c", MSGDEL, components[NIGHTSHADE], MSGDEL, plyptr->components[NIGHTSHADE], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11109%c%-16s%c%d%c", MSGDEL, components[SAGE], MSGDEL, plyptr->components[SAGE], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11110%c%-16s%c%d%c", MSGDEL, components[CARDAMOM], MSGDEL, plyptr->components[CARDAMOM], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11111%c%-16s%c%d%c", MSGDEL, components[GENSING], MSGDEL, plyptr->components[GENSING], MSGDEL);
	send_message(message, plyptr->name, 1);
}

void spell_clue(player *plyptr)
{
	int cluenum;

	sprintf(message, "11035%c%s%c", MSGDEL, cities[magic_city].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->level[plyptr->cont] < 4)
	{
		sprintf(message, "11036%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	cluenum = randomnum(strlen(components[5]) - 1, 0);
	sprintf(message, "11037%c%c%c", MSGDEL, tolower(components[5][cluenum]), MSGDEL);
	send_message(message, plyptr->name, 1);
}

// Spells

// Level 0

int spell_gender(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		if(players[plynum].gender == 1)
			sprintf(message, "11039%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		else
			sprintf(message, "11040%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		players[plynum].gender = players[plynum].gender * -1 + 1;
		save_player(plynum);
	}
	else
		made_save("Gender", plyptr, &players[plynum]);
	return(1);
}

// Level 1

int spell_createfood(player *plyptr)
{
	int amount;

	amount = (plyptr->level[plyptr->cont] + 1) * 100;
	amount = randomnum(amount, 25);
	fail_save("controlling nature itself", "CreateFood", plyptr, NULL);
	sprintf(message, "11041%c%d%c%s%c", MSGDEL, amount, MSGDEL, food_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	plyptr->food[plyptr->cont] += amount;
	return(1);
}

int spell_fireball(char *name, player *plyptr)
{
	int cont;
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		cont = players[plynum].cont;
		plyptr->isprotected = 1;
		fail_save("summoning the elements of fire", "Fireball", plyptr, &players[plynum]);
		for(count = 0; count < MAXTRP; ++count)
			players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] / 10;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Fireball", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_truesight(char *name, player *plyptr)
{
	int cont;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(check_artifact(MIRROR, &players[plynum]))
	{
		sprintf(message, "11042%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	sprintf(message, "11043%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	cont = players[plynum].cont;
	sprintf(message, "11044%c%s%c%s%c", MSGDEL, players[plynum].kname[cont], MSGDEL, players[plynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	person("", plyptr, &players[plynum]);
	show_surrender(plyptr->name, &players[plynum]);
	sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
	send_message(message, plyptr->name, 1);
	review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_DEFENSE, &players[plynum], &players[plynum]);
	show_formation(plyptr->name, players[plynum].cont, ARMY_DEFENSE, &players[plynum]);
	if(advanced_armies == 1)
	{
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_CAMPAIGN, &players[plynum], &players[plynum]);
		show_formation(plyptr->name, players[plynum].cont, ARMY_CAMPAIGN, &players[plynum]);
	}
	return(1);
}

// Level 2

int spell_chaos(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(players[plynum].ally < 1)
	{
		sprintf(message, "11045%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(!save_throw(&players[plynum]))
	{
		sprintf(message, "11046%c%s%c%s%c%s%c%s%c", MSGDEL, 
		players[plynum].name, MSGDEL, players[plynum].name, MSGDEL, adjectives[players[plynum].gender][0], MSGDEL, players[players[plynum].ally].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		fail_save("invoking the power of hate", "Chaos", plyptr, &players[plynum]);
		break_alliance(&players[plynum]);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Chaos", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_forget(char *name, player *plyptr)
{
	char fname[64];
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		sprintf(fname, "logs/%s", players[plynum].name);
		remove(fname);
		fail_save("commanding the mind", "Forget", plyptr, &players[plynum]);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
	}
	else
		made_save("Forget", plyptr, &players[plynum]);
	return(1);
}

int spell_giant(char *name, player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int num_castles = 0;
	int plynum;
	int round = 1;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("wielding great power", "Giant", plyptr, &players[plynum]);
		combatants[ATTACKER] = (void *)plybuf[ATTACKER];
		summon_creature(1, GIANT, ARMY_DEFENSE, ARMY_CAMPAIGN, -1, combatants[ATTACKER], &players[plynum]);
		combatants[DEFENDER] = &players[plynum];
		if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
			beast_won(&players[plynum], "Giant", GIANT);
		else
			beast_lost(&players[plynum], "Giant", GIANT);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		reset_combatants();
		sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
		return(2);
	}
	else
	{
		made_save("Giant", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_polymorph(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		combatants[ATTACKER] = plyptr;
		combatants[DEFENDER] = &players[plynum];
		create_guise(playernum(plyptr));
		plyptr->isprotected = 1;
		start_battle();
		save_player(plynum);
		strcpy(plyptr->nick, plyptr->name);
		return(2);
	}
	else
	{
		made_save("Polymorph", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_strength(player *plyptr)
{
	if(plyptr->special_batmod & spcstr[plyptr->cont])
	{
		sprintf(message, "11113%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->special_batmod += spcstr[plyptr->cont];
	fail_save("endowing your troops with great strength", "Strength", plyptr, NULL);
	return(1);
}

// Level 3

int spell_famine(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("controlling the dark forces of nature", "Famine", plyptr, &players[plynum]);
		players[plynum].food[players[plynum].cont] /= 2;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Famine", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_shadowwarriors(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("calling upon the powers of the undead", "ShadowWarriors", plyptr, &players[plynum]);
		combatants[ATTACKER] = plyptr;
		combatants[DEFENDER] = &players[plynum];
		combatants[ATTACKER_HORDE1] = (void *)plybuf[ATTACKER_HORDE1];
		enemy(0, UNDEAD, ARMY_CAMPAIGN, ARMY_CAMPAIGN, -1, combatants[ATTACKER_HORDE1], plyptr);
		sprintf(message, "11047%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, players[plynum].nick, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL, plyptr->nick, MSGDEL, 
		select_title(combatants[ATTACKER_HORDE1]->classnum, combatants[ATTACKER_HORDE1]->level[plyptr->cont], combatants[ATTACKER_HORDE1]->gender), MSGDEL, combatants[ATTACKER_HORDE1]->nick, MSGDEL, adjectives[plyptr->gender][1], MSGDEL);
		send_channel(message, 0);
		start_battle();
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("ShadowWarriors", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_teleport(char *cityname, player *plyptr)
{
	int ctynum;

	if(strlen(cityname) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == plyptr->city)
	{
		sprintf(message, "19%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "3%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->city = ctynum;
	sprintf(message, "11048%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

int spell_weather(char *arg1, char *arg2, char *arg3, player *plyptr)
{
	char humidity_names[][8] =
	{
		"dry", "normal", "moist", "heavy"
	};
	char temp_names[][16] =
	{
		"freeze", "cold", "temperate", "hot"
	};
	char wind_names[][8] =
	{
		"still", "breezy", "windy", "gusty"
	};
	int count;
	int humidity = -1;
	int temp = -1;
	int wind = -1;

	for(count = W_DRY; count <= W_HEAVY; ++count)
		if((STRCASECMP(arg1, humidity_names[count])) == 0)
		{
			humidity = count;
			break;
		}
	for(count = W_FREEZE; count <= W_HOT; ++count)
		if((STRCASECMP(arg2, temp_names[count])) == 0)
		{
			temp = count;
			break;
		}
	for(count = W_STILL; count <= W_GUSTY; ++count)
		if((STRCASECMP(arg3, wind_names[count])) == 0)
		{
			wind = count;
			break;
		}
	sprintf(message, "11051%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	change_weather(plyptr->cont, humidity, temp, wind);
	switch(forecast[plyptr->cont].type)
	{
		case WT_FREEZE:
		case WT_RAIN:
		case WT_STORM:
			sprintf(message, "11053%c%c", MSGDEL, MSGDEL);
			send_channel(message, 0);
			sprintf(message, "11054%c%c", MSGDEL, MSGDEL);
			send_channel(message, 0);
			sprintf(message, "11055%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
			conts[plyptr->cont].name, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
			send_channel(message, 0);
			break;
		case WT_SNOW:
			sprintf(message, "11056%c%s%c", MSGDEL, conts[plyptr->cont].name, MSGDEL);
			send_channel(message, 0);
			sprintf(message, "11057%c%s%c%s%c%s%c%s%c", MSGDEL, 
			select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
			send_channel(message, 0);
			break;
		case WT_FOG:
			sprintf(message, "11052%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
			conts[plyptr->cont].name, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, adjectives[plyptr->gender][0], MSGDEL);
			send_channel(message, 0);
			break;
		default:
			sprintf(message, "11058%c%c", MSGDEL, MSGDEL);
			send_channel(message, 0);
			sprintf(message, "11059%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
			select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, conts[plyptr->cont].name, MSGDEL, adjectives[plyptr->gender][2], MSGDEL);
			send_channel(message, 0);
			break;

	}
	return(1);
}

// Level 4

int spell_enchant(player *plyptr)
{
	int chance;
	int inum;

	if((inum = find_randomitem()) < 1 || has_item(inum, plyptr))
	{
		sprintf(message, "11060%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	chance = randomnum(100, 1);
	switch(inum)
	{
		case ITEM_CHAOS:
		case ITEM_ORB:
		case ITEM_RING:
		case ITEM_SPEED:
		case ITEM_STONE:
			chance -= 25;
			break;
		default:
			break;
	}
	chance += (plyptr->level[plyptr->cont] + 1) * 2;
	sprintf(message, "11061%c%s%c", MSGDEL, itemd[inum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(chance >= 55)
		add_item(inum, plyptr);
	else
	{
		sprintf(message, "11062%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	return(1);
}

int spell_flamestrike(char *name, player *plyptr)
{
	int cont;
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		cont = players[plynum].cont;
		fail_save("commanding the powers of fire", "FlameStrike", plyptr, &players[plynum]);
		for(count = 0; count < MAXTRP; ++count)
			players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] / 5;
		players[plynum].food[cont] -= players[plynum].food[cont] / 20;
		players[plynum].peasant[cont] -= players[plynum].peasant[cont] / 20;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("FlameStrike", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_move(char *name, player *plyptr)
{
	int ctynum;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		if(conts[players[plynum].cont].totalcty > 1)
		{
			do {
				ctynum = conts[players[plynum].cont].cities[randomnum(conts[players[plynum].cont].totalcty - 1, 0)];
			} while (ctynum == players[plynum].city || ctynum == antimagic_city);
			players[plynum].city = ctynum;
		}
		sprintf(message, "11063%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, players[plynum].city == secret_city?"??????":cities[players[plynum].city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11064%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, cities[players[plynum].city].name, MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		save_player(plynum);
	}
	else
		made_save("Move", plyptr, &players[plynum]);
	return(1);
}

int spell_sleep(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("weaving powerful magics", "Sleep", plyptr, &players[plynum]);
		if(players[plynum].movepts <= 5)
			players[plynum].movepts = 0;
		else
			players[plynum].movepts /= 2;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Sleep", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_vanish(char *name, player *plyptr)
{
	int artnum;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((artnum = find_artifact(&players[plynum])) < 0)
	{
		sprintf(message, "11065%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(artnum == ROD)
	{
		sprintf(message, "11066%c%s%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(!save_throw(&players[plynum]))
	{
		fail_save("weaving powerful magics", "Vanish", plyptr, &players[plynum]);
		artifacts[artnum].owner = 0;
		save_art();
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
	}
	else
		made_save("Vanish", plyptr, &players[plynum]);
	return(1);
}

// Level 5

int spell_charm(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(players[plynum].troop_type == VAMPIRE)
	{
		sprintf(message, "11067%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(!save_throw(&players[plynum]))
	{
		fail_save("using the hypnotic power of magic", "Charm", plyptr, &players[plynum]);
		combatants[ATTACKER] = (void *)plybuf[ATTACKER];
		combatants[DEFENDER] = &players[plynum];
		memcpy((void *)combatants[ATTACKER], (void *)combatants[DEFENDER], PLYSIZ);
		combatants[ATTACKER]->classnum = 70;
		combatants[ATTACKER]->level[combatants[ATTACKER]->cont] = 7;
		strcpy(s_titles[7], select_title(combatants[DEFENDER]->classnum, combatants[DEFENDER]->level[combatants[DEFENDER]->cont], combatants[DEFENDER]->gender));
		if(combatants[ATTACKER]->ally > 0)
			combatants[ATTACKER]->ally = 0;
		if(combatants[ATTACKER]->vassals.total > 0 && combatants[ATTACKER]->special_kingdom & spcaid[combatants[ATTACKER]->cont])
			combatants[ATTACKER]->special_kingdom -= spcaid[combatants[ATTACKER]->cont];
		combatants[ATTACKER]->copper[combatants[ATTACKER]->cont] = 0;
		combatants[ATTACKER]->food[combatants[ATTACKER]->cont] = 0;
		combatants[ATTACKER]->land[combatants[ATTACKER]->cont] = 0;
		combatants[ATTACKER]->peasant[combatants[ATTACKER]->cont] = 0;
		combatants[ATTACKER]->surrender[combatants[ATTACKER]->cont] /= 2;
		combatants[ATTACKER]->army[ARMY_CAMPAIGN][combatants[ATTACKER]->cont].total[CATAPULTNUM] = 0;
		sprintf(message, "11068%c%s%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL, conts[players[plynum].cont].name, MSGDEL);
		send_channel(message, 0);
		start_battle();
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Charm", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_dragon(char *name, player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int num_castles = 0;
	int plynum;
	int round = 1;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("wielding great power", "Dragon", plyptr, &players[plynum]);
		combatants[ATTACKER] = (void *)plybuf[ATTACKER];
		summon_creature(1, DRAGON, ARMY_DEFENSE, ARMY_CAMPAIGN, -1, combatants[ATTACKER], &players[plynum]);
		combatants[DEFENDER] = &players[plynum];
		if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
			beast_won(&players[plynum], "Dragon", DRAGON);
		else
			beast_lost(&players[plynum], "Dragon", DRAGON);
		save_player(plynum);
		reset_combatants();
		sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		return(2);
	}
	else
	{
		made_save("Dragon", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_maelstorm(char *name, player *plyptr)
{
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!mage(&players[plynum]))
	{
		sprintf(message, "11069%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, select_classname(MAGE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(!save_throw(&players[plynum]))
	{
		fail_save("controlling the mind", "Maelstorm", plyptr, &players[plynum]);
		if(players[plynum].movepts < 5)
			players[plynum].movepts = 0;
		else
			players[plynum].movepts = 5;
		for(count = 0; count < MAXCMP; ++count)
		{
			--players[plynum].components[count]; 
			if(players[plynum].components[count] < 0)
				players[plynum].components[count] = 0;
		}
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Maelstorm", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_meteorstorm(char *name, player *plyptr)
{
	int cont;
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		cont = players[plynum].cont;
		fail_save("controlling the skies and heavens", "MeteorStorm", plyptr, &players[plynum]);
		for(count = 0; count < MAXTRP; ++count)
			players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] / 4;
		players[plynum].food[cont] -= players[plynum].food[cont] / 10;
		players[plynum].peasant[cont] -= players[plynum].peasant[cont] / 10;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("MeteorStorm", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_portal(char *contname, player *plyptr)
{
	int cont;

	if(strlen(contname) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((cont = find_cont(contname)) < 0)
	{
		sprintf(message, "29%c%s%c", MSGDEL, contname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cont == plyptr->cont)
	{
		sprintf(message, "30%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->level[cont] < 0)
	{
		sprintf(message, "11070%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	plyptr->cont = cont;
	plyptr->city = conts[cont].port;
	sprintf(message, "11071%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

// Level 6

int spell_behemoth(char *name, player *plyptr)
{
	int attack_damage;
	int defend_damage;
	int num_castles = 0;
	int plynum;
	int round = 1;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("wielding tremendous power", "Behemoth", plyptr, &players[plynum]);
		combatants[ATTACKER] = (void *)plybuf[ATTACKER];
		summon_creature(1, BEHEMOTH, ARMY_DEFENSE, ARMY_CAMPAIGN, -1, combatants[ATTACKER], &players[plynum]);
		combatants[DEFENDER] = &players[plynum];
		if(battle(&num_castles, &round, &attack_damage, &defend_damage) == ATTACKER)
			beast_won(&players[plynum], "Behemoth", BEHEMOTH);
		else
			beast_lost(&players[plynum], "Behemoth", BEHEMOTH);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		reset_combatants();
		sprintf(message, "2070%c%c", MSGDEL, MSGDEL);
		send_channel(message, 0);
		return(2);
	}
	else
	{
		made_save("Behemoth", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_rampage(char *name, player *plyptr)
{
	int amount = 0;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("creating a magical storm", "Rampage", plyptr, &players[plynum]);
		if((amount = randomnum(plyptr->level[plyptr->cont] + 1, 1)) >= players[plynum].castle[players[plynum].cont])
			amount = players[plynum].castle[players[plynum].cont] - 1;
		players[plynum].castle[players[plynum].cont] -= amount;
		sprintf(message, "11072%c%d%c%s%c", MSGDEL, amount, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Rampage", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_wizardlock(char *cityname, player *plyptr)
{
	int ctynum;

	if(strlen(cityname) < 1)
	{
		sprintf(message, "17%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "3%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(ctynum == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(cities[ctynum].lock == 0)
	{
		sprintf(message, "11073%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11074%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
		send_channel(message, 0);
		cities[ctynum].lock = 1;
	}
	else
	{
		sprintf(message, "11075%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11076%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
		send_channel(message, 0);
		cities[ctynum].lock = 0;
	}
	save_city(ctynum);
	return(1);
}

// Level 7

int spell_death(char *name, player *plyptr)
{
	int cont;
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		cont = players[plynum].cont;
		fail_save("controlling the power of death", "Death", plyptr, &players[plynum]);
		for(count = 0; count < MAXTRP; ++count)
			if(count != CATAPULTNUM)
				players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] / 3;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Death", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_juggernaut(char *name, player *plyptr)
{
	int num_castles;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("wielding powerful magic", "Juggernaut", plyptr, &players[plynum]);
		reset_casualties();
		combatants[ATTACKER] = plyptr;
		combatants[DEFENDER] = &players[plynum];
		sprintf(message, "11077%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11078%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "11079%c%s%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, 
		select_class(combatants[DEFENDER]->classnum), MSGDEL, select_title(combatants[DEFENDER]->classnum, combatants[DEFENDER]->level[plyptr->cont], combatants[DEFENDER]->gender), MSGDEL, combatants[DEFENDER]->nick, MSGDEL);
		send_channel(message, 0);
		add_battles(combatants[ATTACKER]);
		num_castles = castles_captured(plyptr->cont, plyptr);
		battle_end(combatants[DEFENDER], plyptr, num_castles, 1, army_size(plyptr->cont, ARMY_DEFENSE, combatants[DEFENDER]));
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Juggernaut", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_plague(char *name, player *plyptr)
{
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		fail_save("controlling the dark forces of nature", "Plague", plyptr, &players[plynum]);
		players[plynum].event[players[plynum].cont] = 10;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Plague", plyptr, &players[plynum]);
		return(1);
	}
}

// Level 8

int spell_earthquake(char *name, player *plyptr)
{
	int cont;
	int count;
	int plynum;

	if((plynum = allow_spell(name, plyptr)) <= 0)
		return(0);
	if((plynum = allow_attack(name, 0, plyptr)) == 0)
		return(0);
	if(!save_throw(&players[plynum]))
	{
		cont = players[plynum].cont;
		fail_save("controlling the elements of earth", "Earthquake", plyptr, &players[plynum]);
		for(count = 0; count < MAXTRP; ++count)
			players[plynum].army[ARMY_DEFENSE][cont].total[count] -= players[plynum].army[ARMY_DEFENSE][cont].total[count] / 3;
		players[plynum].food[cont] -= players[plynum].food[cont] / 5;
		players[plynum].peasant[cont] -= players[plynum].peasant[cont] / 5;
		players[plynum].castle[cont] -= players[plynum].castle[cont] / 10;
		check_ret(players[plynum].cont, plyptr, &players[plynum]);
		update_ret(players[plynum].cont, plyptr, &players[plynum]);
		save_player(plynum);
		return(2);
	}
	else
	{
		made_save("Earthquake", plyptr, &players[plynum]);
		return(1);
	}
}

int spell_kraken(player *plyptr)
{
	if(conq_info.kraken.alive == 1)
	{
		sprintf(message, "11080%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->copper[plyptr->cont] < KRAKENCOPPER)
	{
		sprintf(message, "11114%c%d%c%s%c", MSGDEL, KRAKENCOPPER, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(randomnum(100, 1) <= 75 || (!emperor(plyptr->name) && titan(plyptr)))
	{
		sprintf(message, "11081%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	plyptr->copper[plyptr->cont] -= KRAKENCOPPER;
	conq_info.kraken.alive = 1;
	conq_info.kraken.health = conq_info.kraken.maxhealth;
	sprintf(message, "11082%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "11083%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_channel(message, 0);
	save_info();
	return(1);
}

int spell_wish(char *str_amount, char *item, player *plyptr)
{
	char items[][16] = 
	{
		"land", "", "", "", "", "move"
	};
	int amount;
	int chance;
	int count;
	int factor = 0;

	amount = atoi(str_amount);
	if(amount < 1)
	{
		sprintf(message, "37%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	strcpy(items[1], peasant_name);
	strcpy(items[2], money_name);
	strcpy(items[3], food_name);
	strcpy(items[4], structures[BASE_CLASSNUM(plyptr->classnum)].name);
	for(count = 0; count < 6; ++count)
		if((STRCASECMP(item, items[count])) == 0)
			break;
	if(count == 6)
	{
		sprintf(message, "11084%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	switch(count)
	{
		case 0:
		case 1:
		case 3:
			if(amount > 1000)
				amount = 1000;
			factor = 10;
			break;
		case 2:
			if(amount > 100000)
				amount = 100000;
			factor = 1000;
			break;
		case 4:
		case 5:
			if(amount > 100)
				amount = 100;
			factor = 1;
			break;
	}
	chance = (int)(200 / (amount / factor) * 10);
	if(chance < 1)
		chance = 1;
	else if(chance > 100)
		chance = 99;
	if(randomnum(100, 1) > chance)
	{
		sprintf(message, "11085%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	switch(count)
	{
		case 0:
			plyptr->land[plyptr->cont] += amount;
			break;
		case 1:
			plyptr->peasant[plyptr->cont] += amount;
			break;
		case 2:
			plyptr->copper[plyptr->cont] += amount;
			break;
		case 3:
			plyptr->food[plyptr->cont] += amount;
			break;
		case 4:
			plyptr->castle[plyptr->cont] += amount;
			break;
		case 5:
			plyptr->movepts += amount;
			break;
	}
	sprintf(message, "11086%c%d%c%s%c", MSGDEL, amount, MSGDEL, items[count], MSGDEL);
	send_message(message, plyptr->name, 1);
	return(1);
}

// Level 9

int spell_season(player *plyptr)
{
	if(randomnum(100, 1) <= 75)
	{
		sprintf(message, "11050%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	strcpy(conq_info.hkby, plyptr->nick);
	hk_text(plyptr);
	housekeeping(0);
	return(1);
}

int spell_alterreality(int chance, player *plyptr)
{
	int classnum;
	int cont;
	int cnt;
	int count;
	int hero;
	int numtries;
	int plynum;

	if((randomnum(100, 1) <= 90 || titan(plyptr)) && !chance)
	{
		sprintf(message, "11087%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(1);
	}
	sprintf(message, "11088%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "11089%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 0);
	plynum = playernum(plyptr);
	for(count = 1; count < current_player; ++count)
	{
		cont = players[count].cont;
		if(strlen(players[count].name) < 1 || players[count].level[cont] < 0)
			continue;
		if(titan(&players[count]) || count == plynum)
			continue;
		remove_all_vassal(&players[count]);
		if(players[count].ally > 0)
			break_alliance(&players[count]);
		if(check_artifact(ORB, &players[count]) || players[count].city == antimagic_city || players[count].special_misc & SPC_VACATION)
			continue;
		if(vampire(&players[count]))
		{
			strcpy(players[count].nick, players[count].name);
			if(conq_info.vampire == count)
			{
				conq_info.vampire = 0;
				save_info();
			}
		}
		--totply[players[count].classnum];
		classnum = -1;
		numtries = 0;
		while(classnum < 0 && numtries++ < 1000)
		{
			switch(randomnum(4, 0))
			{
				case 0:
					classnum = randomnum(total_races[FIGHTER] - 1, 0);
					break;
				case 1:
					if(allow_barb == 1)
						classnum = randomnum(10 + total_races[BARBARIAN]  - 1, 10);
					break;
				case 2:
					if(allow_mage == 1)
						classnum = randomnum(20 + total_races[MAGE] - 1, 20);
					break;
				case 3:
					if(allow_cleric == 1)
						classnum = randomnum(40 + total_races[CLERIC] - 1, 40);
					break;
				default:
					if(allow_ranger == 1)
						classnum = randomnum(50 + total_races[RANGER] - 1, 50);
					break;
			}
		}
		if(classnum < 0)
			classnum = 0;
		players[count].classnum = classnum;
		++totply[players[count].classnum];
		if((hero = find_hero(&players[count])) >= 0 && !hero_serve(hero, &players[count]))
			dismiss(&players[count]);
		if(heroes[HERO_NECROMANCER].owner != count)
			players[count].troop_type = troop_type(classnum);
		for(cnt = 0; cnt < num_conts; ++cnt)
		{
			if(players[count].level[cnt] < 0)
				continue;
			change_player(classnum, cnt, &players[count]);
		}
		players[count].isprotected = 0;
		reset_special(&players[count]);
		update_levelmax(&players[count]);
		sprintf(message, "11091%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, 
		select_class(players[count].classnum), MSGDEL, select_title(players[count].classnum, players[count].level[cont], players[count].gender), MSGDEL);
		send_message(message, players[count].name, 1);
		log_event("", message, players[count].name);
		save_player(count);
	}
	sprintf(message, "11090%c%c", MSGDEL, MSGDEL);
	send_channel(message, 0);
	if(chance || randomnum(100, 1) > 1)
	{
		sprintf(message, "11092%c%s%c%s%c%s%c%s%c", MSGDEL, 
		select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		delete_player(plynum);
		return(0);
	}
	return(1);
}

void summon_creature(int ally, int creature, int enemy_army, int horde_army, int strength, player *eptr, player *plyptr)
{
	int cont;
	int size = 1;

	memset(eptr, 0, PLYSIZ);
	if((size = strength) < 0)
		size = randomnum(MIGHTY, WEAK);
	cont = plyptr->cont;
	switch(creature)
	{
		case WOLF:
			strcpy(eptr->name, "Wolf Pack");
			strcpy(eptr->nick, "Wolf Pack");
			eptr->classnum = 70;
			eptr->level[cont] = size;
			eptr->surrender[cont] = .50;
			eptr->troop_type = troop_type(eptr->classnum);
			size = (eptr->level[cont] + 1) * 25;
			break;
		case BEAR:
			strcpy(eptr->name, "Bear Pack");
			strcpy(eptr->nick, "Bear Pack");
			eptr->classnum = 70;
			eptr->level[cont] = size;
			eptr->surrender[cont] = .50;
			eptr->troop_type = troop_type(eptr->classnum);
			size = (eptr->level[cont] + 1) * 25;
			break;
		case LION:
			strcpy(eptr->name, "Lion Pack");
			strcpy(eptr->nick, "Lion Pack");
			eptr->classnum = 70;
			eptr->level[cont] = size;
			eptr->surrender[cont] = .50;
			eptr->troop_type = troop_type(eptr->classnum);
			size = (eptr->level[cont] + 1) * 25;
			break;
		case GIANT:
			strcpy(eptr->name, "Giant");
			strcpy(eptr->nick, "Giant");
			eptr->classnum = 70;
			eptr->level[cont] = 4;
			eptr->surrender[cont] = .50;
			eptr->troop_type = troop_type(eptr->classnum);
			size = 6;
			break;
		case DRAGON:
			strcpy(eptr->name, "Dragon");
			strcpy(eptr->nick, "Dragon");
			eptr->classnum = 70;
			eptr->level[cont] = 5;
			eptr->surrender[cont] = .75;
			eptr->troop_type = troop_type(eptr->classnum);
			size = 3;
			break;
		case BEHEMOTH:
			strcpy(eptr->name, "Behemoth");
			strcpy(eptr->nick, "Behemoth");
			eptr->classnum = 70;
			eptr->level[cont] = 6;
			eptr->surrender[cont] = .75;
			eptr->troop_type = troop_type(eptr->classnum);
			size = 1;
			break;
	}
	eptr->kingdoms = 1;
	eptr->cont = cont;
	eptr->castle[cont] = 1;
	eptr->movepts = 30;
	eptr->city = plyptr->city;
	eptr->first_play[cont] = conq_info.today;
	eptr->spy[cont] = -1;
	default_formation(cont, eptr);
	eptr->army[horde_army][cont].total[creature] = army_size(cont, enemy_army, plyptr) * size / 100;
	if(ally == 1 && plyptr->ally > 0)
		eptr->army[horde_army][cont].total[creature] += army_size(cont, enemy_army, &players[plyptr->ally]) * size / 100;
	if(eptr->army[horde_army][cont].total[creature] < 1)
		eptr->army[horde_army][cont].total[creature] = 1;
	strcpy(eptr->kname[cont], "Gargontia");
	eptr->isprotected = 1;
}
