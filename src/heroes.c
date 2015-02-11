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

int check_hero(int heronum, player *plyptr)
{
	if(!summoned(plyptr) && heroes[heronum].owner == playernum(plyptr))
		return(1);
	else
		return(0);
}

void dismiss(player *plyptr)
{
	int num;

	if((num = find_hero(plyptr)) < 0)
	{
		sprintf(message, "8000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	sprintf(message, "8001%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	heroes[num].owner = 0;
	save_hero();
	lost_hero(num, plyptr);
}

int find_hero(player *plyptr)
{
	int count;
	int found = -1;
	int plynum;

	plynum = playernum(plyptr);
	for(count = 0; count < TOTHRO; ++count)
		if(heroes[count].owner == plynum)
		{
			found = count;
			break;
		}
	return(found);
}

int find_heroname(char *name)
{
	int count;
	int found = -1;

	for(count = 0; count < TOTHRO; ++count)
		if((STRCASECMP(name, heroes[count].name)) == 0)
		{
			found = count;
			break;
		}
	return(found);
}

void hero_hk(int type)
{
	int amount;
	int count;
	player *plyptr;

	for(count = 0; count < TOTHRO; ++count)
	{
		if(heroes[count].owner == 0)
			continue;
		plyptr = &players[heroes[count].owner];
		if(plyptr->level[plyptr->cont] > 2 && count != HERO_GENERAL1 && count != HERO_GENERAL2)
			amount = 5000 * (plyptr->level[plyptr->cont] - 2);
		else
			amount = 0;
		if(plyptr->copper[plyptr->cont] >= heroes[count].cost + amount)
		{
			if(heroes[count].cost > 0)
			{
				plyptr->copper[plyptr->cont] -= heroes[count].cost + amount;
				sprintf(message, "8002%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[count].name, MSGDEL, heroes[count].cost + amount, MSGDEL, money_name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				log_event("", message, plyptr->name);
			}
			hero_hkpower(count, type, plyptr);
		}
		else
		{
			sprintf(message, "8003%c%s%c%d%c%s%c", MSGDEL, heroes[count].name, MSGDEL, heroes[count].cost + amount, MSGDEL, money_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			log_event("", message, plyptr->name);
			heroes[count].owner = 0;
			save_hero();
			lost_hero(count, plyptr);
		}
	}
}

void hero_hkpower(int num, int type, player *plyptr)
{
	int amount;
	int count;
	int inum;
	int land;
	int max;
	int peasant;

	switch(num)
	{
		case HERO_WKNIGHT:
			if(units[plyptr->troop_type][KNIGHTNUM].attack == 0 && units[plyptr->troop_type][KNIGHTNUM].defend == 0)
				break;
			amount = (plyptr->level[plyptr->cont] + 1) * 25;
			if((amount = fill_castles(amount, plyptr->cont, plyptr)) <= 0)
				break;
			plyptr->army[ARMY_DEFENSE][plyptr->cont].total[KNIGHTNUM] += amount;
			sprintf(message, "8004%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][KNIGHTNUM].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			break;
		case HERO_FARMER:
			if(plyptr->land[plyptr->cont] > promotions[BASE_CLASSNUM(plyptr->classnum)][maxlevel[BASE_CLASSNUM(plyptr->classnum)]][1])
				land = 2;
			else
			{
				land = randomnum(10, 1);
				land = plyptr->land[plyptr->cont] * land / 100;
			}
			plyptr->land[plyptr->cont] += land;
			if(plyptr->peasant[plyptr->cont] > promotions[BASE_CLASSNUM(plyptr->classnum)][maxlevel[BASE_CLASSNUM(plyptr->classnum)]][2])
				peasant = 2;
			else
			{
				peasant = randomnum(15, 5);
				peasant = plyptr->peasant[plyptr->cont] * peasant / 100;
			}
			plyptr->peasant[plyptr->cont] += peasant;
			sprintf(message, "8005%c%s%c%s%c%d%c%d%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, land, MSGDEL, peasant, MSGDEL, peasant_name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			amount = randomnum(15, 5);
			amount = plyptr->food[plyptr->cont] * amount / 100;
			plyptr->food[plyptr->cont] += amount;
			sprintf(message, "9016%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL, food_name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			break;
		case HERO_GIANT:
			if(!vampire(plyptr) && randomnum(100, 1) <= 20)
			{
				amount = randomnum(10, 1);
				for(count = 0; count < MAXTRP; ++count)
					plyptr->army[ARMY_DEFENSE][plyptr->cont].total[count] -= plyptr->army[ARMY_DEFENSE][plyptr->cont].total[count] * amount / 100;
				sprintf(message, "8006%c%s%c%d%c%s%c", MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL, conts[plyptr->cont].name, MSGDEL);
				log_event("", message, plyptr->name);
			}
			break;
		case HERO_NINJA:
			count = 15;
			if(check_artifact(DAGGER, plyptr))
				count += 10;
			if(randomnum(100, 1) <= count)
			{
				amount = randomnum(15, 5);
				amount = cities[plyptr->city].copper * amount / 100;
				plyptr->copper[plyptr->cont] += amount;
				cities[plyptr->city].copper -= amount;
				sprintf(message, "8007%c%s%c%s%c%d%c%s%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL, money_name, MSGDEL, cities[plyptr->city].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
				log_event("", message, plyptr->name);
			}
			break;
		case HERO_GENERAL1:
		case HERO_GENERAL2:
			if(units[plyptr->troop_type][BLOODGUARDNUM].attack == 0 && units[plyptr->troop_type][BLOODGUARDNUM].defend == 0)
				break;
			amount = army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr) / 1000 * 100;
			if((amount = fill_castles(amount, plyptr->cont, plyptr)) <= 0)
				break;
			plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[BLOODGUARDNUM] += amount;
			sprintf(message, "8008%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			log_event("", message, plyptr->name);
			break;
		case HERO_ALCHEMIST:
			if(remove_items == 1 && type)
				remove_all_items(plyptr);
			if(check_artifact(CHEST, plyptr))
				max = randomnum(7, 2);
			else
				max = randomnum(6, 1);
			for(amount = 0, count = 0; count < max; ++count)
				if((inum = find_randomitem()) > 0 && !has_item(inum, plyptr))
				{
					add_item(inum, plyptr);
					++amount;
				}
			if(amount == 0)
				sprintf(message, "8043%c%s%c", MSGDEL, heroes[num].type, MSGDEL);
			else
				sprintf(message, "8009%c%s%c%s%c%d%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL, amount, MSGDEL);
			log_event("", message, plyptr->name);
			if(randomnum(100, 1) <= 25 && (count = find_randomartifact(plyptr->cont)) >= 0)
			{
				sprintf(message, "8010%c%s%c%s%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, artifacts[count].name, MSGDEL, artifacts[count].location, MSGDEL, cities[artifacts[count].city].name, MSGDEL);
				log_event("", message, plyptr->name);
			}
			if(randomnum(100, 1) <= 25)
			{
				while((count = randomnum(num_cities - 1, 0)) == secret_city);
				sprintf(message, "8011%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, cities[count].clue, MSGDEL);
				log_event("", message, plyptr->name);
			}
			break;
		default:
			break;
	}
}

void hero_info(player *plyptr)
{
	char desc[TOTHRO][512] =
	{
		"+1/+1 Knights, +25 * level Knights per HK",
		"Casts Fireball, FlameStrike, Giant, ShadowWarriors, or Strength before each battle", 
		"Casts Grace, Resurrect, or Sanctuary after each battle",
		"+1 Renegades, +1-10% land, +1-15% peasants, and +5-15% food per HK",
		"Barbarian horde every battle, no pirates, bonus in naval battles, WarShip when first hired", 
		"Animal horde every battle, seek the Vampire",
		"+1/+0 Catapults, sail costs 1/2 MPs, capture/destroy 2 Castles per attack", 
		"25% chance of breaking an alliance before a battle, 15% chance of robbing cities per HK, spies are always successful, can assassinate other heroes in battle",
		"+0/+2 BloodGuards, +100 BloodGuards per 1000 troops per HK",
		"+2/+0 BloodGuards, +100 BloodGuards per 1000 troops per HK",
		"Casts Fireball or FlameStrike before each battle, automatically makes you the Vampire, 25% chance of corrupting food in a battle, lifedrain", 
		"+1-6 items per HK, 25% chance finding artifacts and secret city clues",
		"Any 2 non-Vampire players may ally",
		"Protection against Vampire magic, guise every battle",
		"Cast spells as if 1/2 level Mage, 50% chance of casting FlameStrike before a battle",
		"Supplies troops for your army, army requires no food at HK",
		"50% chance of stealing an attack in a battle or enemy fights at -1/0, loot cities with 80% chance of success", 
		"Causes enemy army to attack each other, retreat immediately, freeze for 1 round, or break their alliance", 
		"Mercenary army every battle",
		"Immune to taxes, remote markets are free, 10% * level chance of getting purchase 1/2, collects taxes from cities", 
		"Can seek/turn, immune to excommunicate, weak crusader horde", 
		"+5/+5,shd to all troops, can cast meteorstorm before each battle"
	};
	int amount = 0;
	int num;

	if((num = find_hero(plyptr)) < 0)
	{
		sprintf(message, "8000%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] > 2 && num != HERO_GENERAL1 && num != HERO_GENERAL2)
		amount = 5000 * (plyptr->level[plyptr->cont] - 2);
	sprintf(message, "8012%c%s%c", MSGDEL, heroes[num].type, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "8013%c%s%c%d%c%d%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].serve, MSGDEL, heroes[num].cost + amount, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "8014%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, desc[num], MSGDEL);
	send_message(message, plyptr->name, 1);
	switch(opinion(num, plyptr))
	{
		case 1:
			sprintf(message, "8015%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, money_name, MSGDEL);
			break;
		case 2:
			sprintf(message, "8016%c%s%c", MSGDEL, heroes[num].name, MSGDEL);
			break;
		case 3:
			sprintf(message, "8017%c%s%c", MSGDEL, heroes[num].name, MSGDEL);
			break;
		default:
			sprintf(message, "8018%c%s%c", MSGDEL, heroes[num].name, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
}

int hero_prebattle(player *defend, player *plyptr)
{
	char spells[][32] = 
	{
		"Fireball", "Flamestrike", "Giant", 
		"Shadow Warriors", "Strength"
	};
	int count;
	int spellnum;
	int spellok = 0;

	if(check_hero(HERO_DRAGON, plyptr) && randomnum(100, 1) <= 50)
	{
		spell_flamestrike(defend->name, plyptr);
		sprintf(message, "8019%c%s%c%s%c", MSGDEL, defend->nick, MSGDEL, heroes[HERO_DRAGON].type, MSGDEL);
		send_channel(message, 0);
		return(0);
	}
	if(check_hero(HERO_VAMPIRE, plyptr))
	{
		create_guise(playernum(plyptr));
		return(0);
	}
	if(check_hero(HERO_QUEENSPIDER, plyptr) && randomnum(100, 1) <= 50)
	{
		sprintf(message, "8020%c%s%c%s%c%s%c", MSGDEL, heroes[HERO_QUEENSPIDER].name, MSGDEL, heroes[HERO_QUEENSPIDER].type, MSGDEL, defend->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "8021%c%s%c", MSGDEL, heroes[HERO_QUEENSPIDER].type, MSGDEL);
		send_message(message, defend->name, 1);
		if(defend->attacks < attacks_allowed(defend))
			++defend->attacks;
		else
			for(count = 0; count < CATAPULTNUM; ++count)
				--batmod[DEFENDER][count].attack;
		return(0);
	}
	if(check_hero(HERO_TITAN, plyptr) && randomnum(100, 1) <= 15)
	{
		spell_meteorstorm(defend->name, plyptr);
		sprintf(message, "8045%c%s%c%s%c", MSGDEL, defend->nick, MSGDEL, heroes[HERO_TITAN].type, MSGDEL);
		send_channel(message, 0);
		return(0);
	}
	if(!check_hero(HERO_WARLOCK, plyptr) && !check_hero(HERO_DESTROYER, plyptr))
		return(0);
	if(check_artifact(ORB, plyptr) || check_artifact(ORB, defend) || plyptr->city == antimagic_city || defend->city == antimagic_city)
	{
		sprintf(message, "8022%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(check_hero(HERO_DESTROYER, plyptr))
	{
		if(randomnum(100, 1) <= 50)
			return(0);
		spellnum = randomnum(1, 0);
		sprintf(message, "8023%c%s%c%s%c%s%c", MSGDEL, heroes[HERO_DESTROYER].type, MSGDEL, spells[spellnum], MSGDEL, defend->nick, MSGDEL);
	}
	else
	{
		if(plyptr->special_batmod & spcstr[plyptr->cont])
			spellnum = randomnum(3, 0);
		else
			spellnum = randomnum(4, 0);
		if(spellnum == 4)
			sprintf(message, "8024%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, heroes[HERO_WARLOCK].type, MSGDEL, spells[spellnum], MSGDEL, plyptr->nick, MSGDEL);
		else
			sprintf(message, "8024%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, heroes[HERO_WARLOCK].type, MSGDEL, spells[spellnum], MSGDEL, defend->nick, MSGDEL);
	}
	send_channel(message, 0);
	--plyptr->attacks;
	switch(spellnum)
	{
		case 0:
			spell_fireball(defend->name, plyptr);
			break;
		case 1:
			spell_flamestrike(defend->name, plyptr);
			break;
		case 2:
			spell_giant(defend->name, plyptr);
			break;
		case 3:
			spellok = spell_shadowwarriors(defend->name, plyptr);
			break;
		case 4:
			spellok = spell_strength(plyptr);
			break;
	}
	++plyptr->attacks;
	return(spellok);
}

void hero_postbattle(int cont, int type, player *loser, player *winner)
{
	char spells[][16] = 
	{
		"Grace", "Sanctuary", "Resurrect"
	};
	int heronum;
	int num;
	int spellnum;
	player *drkpr;

	if((num = find_hero(winner)) >= 0 && count_casualties(type) > 0 && randomnum(100, 1) <= 2)
	{
		sprintf(message, "8025%c%s%c", MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, winner->name, 1);
		log_event("", message, winner->name);
		send_channel(message, 0);
		heroes[num].alive = 0;
		heroes[num].owner = 0;
		heroes[num].serve = conq_info.today;
		save_hero();
		lost_hero(num, winner);
		return;
	}
	if(check_hero(HERO_DESTROYER, winner))
	{
		if(loser->level[cont] < 0 || randomnum(100, 1) > 25)
			return;
		num = loser->food[cont] * randomnum(10, 3) / 100;
		loser->food[cont] -= num;
		sprintf(message, "2035%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_DESTROYER].name, MSGDEL, num, MSGDEL, food_name, MSGDEL);
		send_message(message, winner->name, 1);
		sprintf(message, "2036%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_DESTROYER].type, MSGDEL, num, MSGDEL, food_name, MSGDEL);
		send_message(message, loser->name, 1);
		sprintf(message, "2037%c%d%c%s%c%s%c%s%c", MSGDEL, num, MSGDEL, food_name, MSGDEL, conts[loser->cont].name, MSGDEL, heroes[HERO_DESTROYER].type, MSGDEL);
		log_event("", message, loser->name);
		return;
	}
	if(check_hero(HERO_NINJA, winner))
	{
		if((heronum = find_hero(loser)) < 0)
			return;
		if((randomnum(100, 1)) <= 90)
			return;
		sprintf(message, "8048%c%s%c%s%c", MSGDEL, heroes[HERO_NINJA].type, MSGDEL, heroes[heronum].type, MSGDEL);
		send_message(message, winner->name, 1);
		send_message(message, loser->name, 1);
		send_channel(message, 0);
		log_event("", message, loser->name);
		heroes[heronum].alive = 0;
		heroes[heronum].owner = 0;
		heroes[heronum].serve = conq_info.today;
		save_hero();
		lost_hero(heronum, loser);
		return;
	}
	if(check_hero(HERO_VAMPIRE, winner))
	{
		strcpy(winner->nick, winner->name);
		return;
	}
	if(!check_hero(HERO_PRIESTESS, winner))
		return;
	drkpr = (void *)plybuf[ATTACKER];
	memset(drkpr, 0, PLYSIZ);
	if(fighter(winner))
		drkpr->classnum = winner->classnum + 40;
	if(mage(winner))
		drkpr->classnum = winner->classnum + 20;
	if(cleric(winner))
		drkpr->classnum = winner->classnum;
	if(ranger(winner))
		drkpr->classnum = winner->classnum - 10;
	drkpr->level[cont] = 6;
	drkpr->movepts = 25;
	drkpr->cont = cont;
	strcpy(drkpr->name, heroes[HERO_PRIESTESS].name);
	strcpy(drkpr->nick, "??????");
	spellnum = randomnum(2, 0);
	sprintf(message, "8026%c%s%c%s%c%s%c", MSGDEL, heroes[HERO_PRIESTESS].type, MSGDEL, spells[spellnum], MSGDEL, winner->nick, MSGDEL);
	send_channel(message, 0);
	switch(spellnum)
	{
		case 0:
			grace_player(playernum(winner), drkpr);
			break;
		case 1:
			sanctuary_player(playernum(winner), drkpr);
			break;
		default:
			resurrect_player(playernum(winner), drkpr);
			break;
	}
}

int hero_serve(int num, player *plyptr)
{
	switch(num)
	{
		case HERO_WKNIGHT:
		case HERO_PRIESTESS:
			if(barbarian(plyptr) || vampire(plyptr))
				return(0);
			break;
		case HERO_NINJA:
		case HERO_BARBLEADER:
		case HERO_NECROMANCER:
			if(cleric(plyptr))
				return(0);
			break;
		case HERO_HIGHLORD:
		case HERO_DIPLOMAT:
		case HERO_VAMPIRE:
			if(vampire(plyptr))
				return(0);
			break;
		case HERO_GENERAL1:
		case HERO_GENERAL2:
			if(!fighter(plyptr))
				return(0);
			break;
		case HERO_BISHOP:
		case HERO_TITAN:
			if(barbarian(plyptr) || vampire(plyptr) || ranger(plyptr))
				return(0);
		default:
			break;
	}
	return(1);
}

void lost_hero(int heronum, player *plyptr)
{
	switch(heronum)
	{
		case HERO_DIPLOMAT:
			if(plyptr->ally > 0 && (!equal_class(plyptr->classnum, players[plyptr->ally].classnum) || (!fighter(plyptr) && !ranger(plyptr))))
				break_alliance(plyptr);
			break;
		case HERO_VAMPIRE:
			strcpy(plyptr->nick, plyptr->name);
			break;
		case HERO_NECROMANCER:
			lost_necromancer(plyptr);
			break;
		case HERO_TITAN:
			lost_titan(plyptr);
			break;
		default:
			break;
	}
}

void lost_necromancer(player *plyptr)
{
	int cont;
	int count;

	if(vampire(plyptr))
		return;
	sprintf(message, "8027%c%s%c", MSGDEL, heroes[HERO_NECROMANCER].type, MSGDEL);
	send_message(message, plyptr->name, 1);
	log_event("", message, plyptr->name);
	plyptr->troop_type = troop_type(plyptr->classnum);
	for(cont = 0; cont < num_conts; ++cont)
	{
		default_formation(cont, plyptr);
		for(count = 0; count < MAXTRP; ++count)
		{
			plyptr->army[ARMY_DEFENSE][cont].total[count] = 0;
			plyptr->army[ARMY_CAMPAIGN][cont].total[count] = 0;
		}
	}
}

void lost_titan(player *plyptr)
{
	int amount;

	if(plyptr->castle[plyptr->cont] < 2)
		return;
	amount = plyptr->castle[plyptr->cont] * randomnum(50, 20) / 100;
	if(amount < 1)
		amount = 1;
	plyptr->castle[plyptr->cont] -= amount;
	sprintf(message, "8046%c%s%c%s%c%d%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, conts[plyptr->cont].name, MSGDEL, amount, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	log_event("", message, plyptr->name);
}

int opinion(int num, player *plyptr)
{
	int base = 0;
	int total;

	switch(num)
	{
		case HERO_WKNIGHT:
			base = 25;
			if(fighter(plyptr) || cleric(plyptr))
				base += randomnum(20, 10);
			if(mage(plyptr))
				base -= randomnum(30, 10);
			if(ranger(plyptr))
				base -= randomnum(20, 10);
			if(plyptr->honor[plyptr->cont] < (plyptr->level[plyptr->cont] + 1) * 2)
				base -= randomnum(20, 10);
			else
				base += randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 90)
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_WARLOCK:
			base = 40;
			if(fighter(plyptr) || barbarian(plyptr) || cleric(plyptr))
				base -= randomnum(20, 10);
			if(mage(plyptr))
				base += randomnum(40, 20);
			if(vampire(plyptr))
				base += randomnum(20, 10);
			if(ranger(plyptr))
				base -= randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 70)
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_PRIESTESS:
			base = 40;
			if(fighter(plyptr))
				base += randomnum(20, 10);
			if(mage(plyptr))
				base -= randomnum(40, 20);
			if(cleric(plyptr))
				base += randomnum(40, 20);
			if(vampire(plyptr))
				base += randomnum(20, 10);
			if(ranger(plyptr))
				base += randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 80)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_FARMER:
			base = 50;
			if(fighter(plyptr) || mage(plyptr))
				base += randomnum(30, 20);
			if(barbarian(plyptr))
				base -= randomnum(30, 20);
			if(cleric(plyptr))
				base += randomnum(40, 20);
			if(vampire(plyptr))
				base -= randomnum(30, 10);
			if(ranger(plyptr))
				base += randomnum(30, 10);
			if(plyptr->peasant[plyptr->cont] >= promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont] + 1][2])
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_BARBLEADER:
			base = 30;
			if(fighter(plyptr))
				base -= randomnum(20, 10);
			if(barbarian(plyptr))
				base += randomnum(40, 20);
			if(mage(plyptr) || cleric(plyptr))
				base -= randomnum(30, 20);
			if(vampire(plyptr))
				base -= randomnum(40, 20);
			if(ranger(plyptr))
				base += randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 90)
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_HIGHLORD:
			base = 40;
			if(fighter(plyptr) || cleric(plyptr))
				base -= randomnum(20, 10);
			if(barbarian(plyptr))
				base -= randomnum(30, 10);
			if(mage(plyptr))
				base += randomnum(20, 10);
			if(ranger(plyptr))
				base += randomnum(40, 20);
			total = army_size(plyptr->cont, ARMY_CAMPAIGN, plyptr);
			if(total < 1)
				total = 1;
			if(plyptr->army[ARMY_CAMPAIGN][plyptr->cont].total[ARCHERNUM] * 100 / total >= 50)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_GIANT:
			base = 40;
			if(fighter(plyptr))
				base -= randomnum(30, 20);
			if(barbarian(plyptr))
				base += randomnum(30, 20);
			if(mage(plyptr))
				base += randomnum(20, 10);
			if(cleric(plyptr))
				base -= randomnum(40, 20);
			if(vampire(plyptr))
				base += randomnum(40, 20);
			if(ranger(plyptr))
				base = randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 70)
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_NINJA:
			base = 35;
			if(fighter(plyptr) || mage(plyptr) || ranger(plyptr))
				base += randomnum(20, 10);
			if(barbarian(plyptr))
				base += randomnum(30, 20);
			if(cleric(plyptr))
				base -= randomnum(40, 20);
			if(vampire(plyptr))
				base += randomnum(30, 10);
			if(plyptr->spy[plyptr->cont] >= 0)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_GENERAL1:
		case HERO_GENERAL2:
			base = 30;
			if(fighter(plyptr))
				base += randomnum(40, 20);
			if(mage(plyptr))
				base -= randomnum(40, 20);
			if(cleric(plyptr))
				base -= randomnum(20, 10);
			if(plyptr->honor[plyptr->cont] < (plyptr->level[plyptr->cont] + 1) * 3)
				base -= randomnum(20, 10);
			else
				base += randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 95)
				base += randomnum(40, 20);
			else
				base -= randomnum(40, 20);
			break;
		case HERO_DESTROYER:
			base = 40;
			if(fighter(plyptr) || mage(plyptr))
				base += randomnum(20, 10);
			if(barbarian(plyptr))
				base += randomnum(30, 20);
			if(cleric(plyptr))
				base -= randomnum(40, 20);
			if(vampire(plyptr))
				base += randomnum(40, 20);
			if(ranger(plyptr))
				base -= randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 60)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_ALCHEMIST:
			base = 40;
			if(fighter(plyptr))
				base += randomnum(30, 20);
			if(barbarian(plyptr) || vampire(plyptr))
				base += randomnum(20, 10);
			if(mage(plyptr))
				base += randomnum(40, 20);
			if(cleric(plyptr))
				base -= randomnum(20, 10);
			if(ranger(plyptr))
				base -= randomnum(40, 20);
			if(plyptr->items[0] > 0)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_DIPLOMAT:
			base = 75;
			if(vampire(plyptr))
				base -= randomnum(40, 20);
			break;
		case HERO_VAMPIRE:
			base = 30;
			if(barbarian(plyptr))
				base += randomnum(30, 20);
			if(cleric(plyptr))
				base -= randomnum(40, 20);
			if(ranger(plyptr))
				base -= randomnum(20, 10);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 80)
				base += randomnum(20, 10);
			else
				base -= randomnum(20, 10);
			break;
		case HERO_DRAGON:
			base = 30;
			if(fighter(plyptr) || cleric(plyptr))
				base -= randomnum(20, 10);
			if(barbarian(plyptr))
				base -= randomnum(30, 10);
			if(mage(plyptr))
				base += randomnum(40, 20);
			if(ranger(plyptr))
				base += randomnum(30, 10);
			if(vampire(plyptr))
				base -= randomnum(40, 20);
			if(plyptr->copper[plyptr->cont] >= MAXCOPPER / 4)
				base += randomnum(20, 10);
			break;
		case HERO_NECROMANCER:
			base = 25;
			if(fighter(plyptr) || cleric(plyptr))
				base -= randomnum(20, 10);
			if(barbarian(plyptr) || ranger(plyptr))
				base += randomnum(20, 10);
			if(mage(plyptr))
				base -= randomnum(30, 10);
			if(vampire(plyptr))
				base += randomnum(40, 20);
			if(plyptr->honor[plyptr->cont] < (plyptr->level[plyptr->cont] + 1) * 2)
				base += randomnum(30, 10);
			break;
		case HERO_QUEENSPIDER:
			base = 35;
			if(fighter(plyptr) || cleric(plyptr))
				base -= randomnum(20, 10);
			if(mage(plyptr) || vampire(plyptr))
				base += randomnum(20, 10);
			if(ranger(plyptr))
				base += randomnum(40, 20);
			if(plyptr->gender == 0)
				base -= randomnum(20, 10);
			else
				base += randomnum(30, 10);
			break;
		case HERO_LORDCHAOS:
			base = 20;
			if(plyptr->peasant[plyptr->cont] >= promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont] + 1][2])
				base += randomnum(50, 20);
			else
				base -= randomnum(50, 20);
			break;
		case HERO_MERCGENERAL:
			base = 30;
			if(plyptr->copper[plyptr->cont] >= MAXCOPPER / 2)
				base += randomnum(40, 20);
			total = plyptr->battles_won + plyptr->battles_lost;
			if(total < 1)
				total = 1;
			if(plyptr->battles_won * 100 / total >= 60)
				base += randomnum(30, 20);
			else
				base -= randomnum(30, 20);
			break;
		case HERO_MERCHANTKING:
			base = 60;
			if(plyptr->copper[plyptr->cont] >= MAXCOPPER / 4)
				base += randomnum(40, 20);
			break;
		case HERO_BISHOP:
			base = 40;
			if(fighter(plyptr) || cleric(plyptr))
				base += randomnum(30, 10);
			if(mage(plyptr))
				base -= randomnum(30, 10);
			if(plyptr->honor[plyptr->cont] < (plyptr->level[plyptr->cont] + 1) * 2)
				base -= randomnum(30, 10);
			else
				base += randomnum(30, 10);
			break;
		case HERO_TITAN:
			base = 25;
			if(fighter(plyptr) || cleric(plyptr))
				base += randomnum(30, 10);
			if(mage(plyptr))
				base -= randomnum(30, 10);
			if(plyptr->honor[plyptr->cont] < (plyptr->level[plyptr->cont] + 1) * 2)
				base -= randomnum(40, 20);
			else
				base += randomnum(40, 20);
			break;
	}
	base /= 20;
	if(base < 1)
		base = 1;
	if(base > 4)
		base = 4; 
	return(base);
}

void quest_5(char *name, player *plyptr)
{
	int count;
	int num;
	int shpnum;

	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "8028%c%s%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < QUEST_5)
	{
		sprintf(message, "1%c%d%cto quest for a Hero%c", MSGDEL, QUEST_5, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < 50000)
	{
		sprintf(message, "16%c%s%c50000%cto hire a Hero%c", MSGDEL, money_name, MSGDEL, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		sprintf(message, "14%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((num = find_hero(plyptr)) >= 0)
	{
		sprintf(message, "8029%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((num = find_heroname(name)) < 0)
	{
		sprintf(message, "8030%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(heroes[num].alive == 0)
	{
		sprintf(message, "8031%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(heroes[num].owner != 0)
	{
		sprintf(message, "8032%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!hero_serve(num, plyptr))
	{
		sprintf(message, "8033%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender),  MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= QUEST_5;
	plyptr->copper[plyptr->cont] -= 50000;
	heroes[num].serve = conq_info.today;
	heroes[num].owner = playernum(plyptr);
	save_hero();
	sprintf(message, "8034%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL);
	send_message(message, plyptr->name, 1);
	switch(num)
	{
		case HERO_BARBLEADER:
			if(num_ships < 2)
				break;
			for(shpnum = 0; shpnum < MAXFLT; ++shpnum)
				if(plyptr->ships[shpnum].type == 0 || plyptr->ships[shpnum].type == num_ships - 1)
					break;
			if(shpnum == MAXFLT)
				shpnum = 0;
			if(plyptr->ships[shpnum].type > 0)
			{
				sprintf(message, "8035%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, shiptypes[plyptr->ships[shpnum].type].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				remove_ship(shpnum, plyptr);
			}
			add_ship(shpnum, num_ships - 1, plyptr);
			strcpy(plyptr->ships[shpnum].name, "The Nemesis");
			sprintf(message, "8036%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, shiptypes[plyptr->ships[shpnum].type].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case HERO_DESTROYER:
			if(conq_info.vampire == 0)
				make_vampire(plyptr);
			break;
		case HERO_NECROMANCER:
			sprintf(message, "8037%c%s%c", MSGDEL, heroes[HERO_NECROMANCER].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			for(count = 0; count < num_conts; ++count)
			{
				if(plyptr->level[count] >= 0)
				{
					if(plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] > 0 || plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] > 0)
					{
						sprintf(message, "8038%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[HERO_NECROMANCER].name, MSGDEL, plyptr->army[0][count].total[BLOODGUARDNUM] + plyptr->army[1][count].total[BLOODGUARDNUM], MSGDEL, 
						units[plyptr->troop_type][BLOODGUARDNUM].name, MSGDEL, conts[count].name, MSGDEL);
						send_message(message, plyptr->name, 1);
						plyptr->army[ARMY_DEFENSE][count].total[BLOODGUARDNUM] = 0;
						plyptr->army[ARMY_CAMPAIGN][count].total[BLOODGUARDNUM] = 0;
					}
				}
			}
			plyptr->troop_type = VAMPIRE;
			check_troops(plyptr);
			break;
		case HERO_BISHOP:
			for(count = 0; count < num_conts; ++count)
				tithe(count, plyptr);
		default:
			break;
	}
}

void read_heroes()
{
	char input[1024] = {0};
	char *mid;
	char *start;
	char str[32] = {0};
	FILE *fp;
	int current = 0;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, HROFIL);
	if((fp = fopen(HROFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, HROFIL, strerror(errno));
		exit_conquest();
	}
	while((fgets(input, 1024, fp)) != NULL && current < TOTHRO)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		sscanf(input, "%d %d %d %d %15s", &heroes[current].alive, &heroes[current].cost, &heroes[current].owner, &heroes[current].serve, str);
		if((start = strstr(input, str)) == NULL)
			continue;
		if((mid = strchr(start, '@')) == NULL)
			continue;
		if(heroes[current].alive < 0 || heroes[current].alive > 1)
			heroes[current].alive = 0;
		if(heroes[current].cost < 0)
			heroes[current].cost = 0;
		strncpy(heroes[current].name, start, (mid - start) > 15?15:(mid - start));
		strncpy(heroes[current++].type, mid + 1, 31);
	}
	fclose(fp);
	if(current < TOTHRO)
	{
		fprintf(stderr, "%s Error reading %s: count should be %d not %d\n", NOWSTR, HROFIL, TOTHRO, current);
		exit_conquest();
	}
}

void revive(char *name, player *plyptr)
{
	int chance;
	int num;

	if(!cleric(plyptr) || (titan(plyptr) && !emperor(plyptr->name)))
	{
		sprintf(message, "3003%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < 8 && !emperor(plyptr->name))
	{
		sprintf(message, "3000%c%s%c", MSGDEL, select_title(plyptr->classnum, 8, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1); 
		return;
	}
	if((strlen(name)) < 1)
	{
		help(plyptr->name, "revive", "", "");
		return;
	}
	if((num = find_heroname(name)) < 0)
	{
		sprintf(message, "8030%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < REVIVE)
	{
		sprintf(message, "1%c%d%cto revive %s%c", MSGDEL, REVIVE, MSGDEL, heroes[num].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(heroes[num].alive == 1)
	{
		sprintf(message, "8039%c%s%c", MSGDEL, heroes[num].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= REVIVE;
	if(titan(plyptr))
		chance = 0;
	else
		chance = 50 + ((conq_info.today - heroes[num].serve) * 4);
	if((randomnum(100, 1)) < chance)
	{
		sprintf(message, "8040%c%s%c%s%c", MSGDEL, heroes[num].name, MSGDEL, heroes[num].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	heroes[num].alive = 1;
	save_hero();
	sprintf(message, "8041%c%d%c%s%c", MSGDEL, randomnum(7, 2), MSGDEL, heroes[num].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "8042%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, heroes[num].type, MSGDEL);
	send_channel(message, 0);
	sprintf(message, "%s %s %s resurrected the %s.", NOWSTR, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), plyptr->name, heroes[num].type);
	add_log(EMPLOG, message);
}

void revive_heroes()
{
	int count;
	int diff;
	int save = 0;

	for(count = 0; count < TOTHRO; ++count)
	{
		if(heroes[count].alive == 1)
			continue;
		diff = conq_info.today - heroes[count].serve;
		if(diff >= 2 && randomnum(100, 1) >= 60 - ((diff - 2) * 10))
		{
			save = 1;
			heroes[count].alive = 1;
			switch(randomnum(2, 0))
			{
				case 0:
					sprintf(message, "8049%c%s%c", MSGDEL, heroes[count].type, MSGDEL);
					send_channel(message, 0);
					break;
				case 1:
					sprintf(message, "8050%c%c", MSGDEL, MSGDEL);
					send_channel(message, 0);
					break;
				default:
					break;
			}
			sprintf(message, "%s Conquest resurrected the %s.", NOWSTR, heroes[count].type);
			add_log(EMPLOG, message);
		}
	}
	if(save == 1)
		save_hero();
}

void save_hero()
{
	char fname[128] = {0};
	char input[1024] = {0};
	FILE *in;
	FILE *out;
	int count;

	strcpy(fname, tempnam("./", NULL));
	if((in = fopen(HROFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, HROFIL, strerror(errno));
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
	for(count = 0; count < TOTHRO; ++count)
	{
		sprintf(input, "%d %d %d %d %s@%s\n", heroes[count].alive, heroes[count].cost, heroes[count].owner, heroes[count].serve, heroes[count].name, heroes[count].type);
		fputs(input, out);
	}
	fclose(out);
	remove(HROFIL);
	rename(fname, HROFIL);
}
