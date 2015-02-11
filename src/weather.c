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

void change_rating(int cont, int *rating)
{
	int add;

	if(randomnum(100, 1) <= 50)
		add = -1;
	else
		add = 1;
	if(*rating + add < 0 || *rating + add > 3)
		add *= -1;
	*rating += add;
}

void change_weather(int cont, int humidity, int temp, int wind)
{
	int count;
	int diff;

	if(humidity < 0)
		change_rating(cont, &forecast[cont].humidity);
	else
		forecast[cont].humidity = humidity;
	if(temp < 0)
		change_rating(cont, &forecast[cont].temp);
	else
		forecast[cont].temp = temp;
	if(wind < 0)
		change_rating(cont, &forecast[cont].wind);
	else
		forecast[cont].wind = wind;
	if(forecast[cont].wind >= W_WINDY)
		if(forecast[cont].temp > 0)
			--forecast[cont].temp;
	switch(forecast[cont].temp)
	{
		case W_FREEZE:
		case W_COLD:
			--forecast[cont].humidity;
			break;
		case W_HOT:
			++forecast[cont].humidity;
			break;
		default:
			break;
	}
	if(forecast[cont].humidity < W_DRY)
		forecast[cont].humidity = W_DRY;
	else if(forecast[cont].humidity > W_HEAVY)
		forecast[cont].humidity = W_HEAVY;
	if(forecast[cont].humidity == W_HEAVY)
		if(forecast[cont].temp < W_HOT)
			++forecast[cont].temp;
	forecast[cont].violent = 0;
	for(count = 0; count < num_conts; ++count)
	{
		if(count == cont)
			continue;
		diff = 0;
		if(abs(forecast[cont].humidity - forecast[count].humidity) >= 2)
			++diff;
		if(abs(forecast[cont].temp - forecast[count].temp) >= 2)
			++diff;
		if(abs(forecast[cont].wind - forecast[count].wind) >= 2)
			++diff;
		if(diff >= 2)
		{
			forecast[cont].violent = 1;
			break;
		}
	}
	forecast[cont].food = (float)0.0;
	forecast[cont].joust = 1;
	forecast[cont].sail_damage = 0;
	if(forecast[cont].humidity == W_DRY && forecast[cont].temp == W_FREEZE)
	{
		forecast[cont].food = (float)-.05;
		forecast[cont].joust = 0;
		forecast[cont].type = WT_FREEZE;
		forecast[cont].temperature = randomnum(20, 10);
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.10;
			forecast[cont].msgnum = 18016;
			forecast[cont].sail_damage = 1;
			forecast[cont].temperature -= randomnum(20, 10);
		}
		else
			forecast[cont].msgnum = 18004;
		return;
	}
	if(forecast[cont].humidity == W_NORMAL && forecast[cont].temp == W_FREEZE)
	{
		forecast[cont].sail_damage = 1;
		forecast[cont].temperature = randomnum(35, 20);
		forecast[cont].type = WT_SNOW;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.10;
			forecast[cont].joust = 0;
			forecast[cont].msgnum = 18017;
			forecast[cont].temperature -= randomnum(10, 5);
		}
		else
			forecast[cont].msgnum = 18005;
		return;
	}
	if(forecast[cont].humidity == W_MOIST && forecast[cont].wind <= W_BREEZY)
	{
		forecast[cont].food = (float).10;
		forecast[cont].joust = 0;
		forecast[cont].sail_damage = 1;
		forecast[cont].temperature = randomnum(80, 50);
		forecast[cont].type = WT_RAIN;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].msgnum = 18007;
			forecast[cont].temperature -= 15;
		}
		else
			forecast[cont].msgnum = 18006;
		return;
	}
	if(forecast[cont].humidity == W_HEAVY && forecast[cont].temp == W_HOT)
	{
		forecast[cont].temperature = randomnum(105, 80);
		forecast[cont].type = WT_HOT;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.05;
			forecast[cont].msgnum = 18019;
			forecast[cont].temperature += 10;
		}
		else
			forecast[cont].msgnum = 18008;
		return;
	}
	if(forecast[cont].temp == W_COLD && (forecast[cont].humidity == W_NORMAL || forecast[cont].humidity == W_MOIST))
	{
		forecast[cont].sail_damage = 1;
		forecast[cont].temperature = randomnum(60, 40);
		forecast[cont].type = WT_FOG;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].joust = 0;
			forecast[cont].msgnum = 18020;
			forecast[cont].temperature -= 5;
		}
		else
			forecast[cont].msgnum = 18009;
		return;
	}
	if((forecast[cont].humidity == W_MOIST && forecast[cont].wind == W_GUSTY) || (forecast[cont].humidity == W_HEAVY && forecast[cont].wind >= W_WINDY))
	{
		forecast[cont].food = (float).05;
		forecast[cont].joust = 0;
		forecast[cont].temperature = randomnum(55, 35);
		forecast[cont].type = WT_STORM;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.05;
			forecast[cont].msgnum = 18018;
			forecast[cont].sail_damage = 1;
			forecast[cont].temperature -= 5;
		}
		else
			forecast[cont].msgnum = 18007;
		return;
	}
	if(forecast[cont].humidity == W_NORMAL && forecast[cont].temp == W_HOT)
	{
		forecast[cont].temperature = randomnum(90, 70);
		forecast[cont].type = WT_DRY;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.15;
			forecast[cont].msgnum = 18021;
			forecast[cont].temperature += 5;
		}
		else
			forecast[cont].msgnum = 18011;
		return;
	}
	if(forecast[cont].temp == W_COLD && forecast[cont].humidity <= W_NORMAL)
	{
		forecast[cont].msgnum = 18010;
		forecast[cont].temperature = randomnum(70, 50);
		forecast[cont].type = WT_COOL;
		return;
	}
	if(forecast[cont].temp == W_TEMPERATE && forecast[cont].wind == W_GUSTY)
	{
		forecast[cont].joust = 0;
		forecast[cont].temperature = randomnum(60, 30);
		forecast[cont].type = WT_WIND;
		if(forecast[cont].violent == 1)
		{
			forecast[cont].food = (float)-.05;
			forecast[cont].joust = 0;
			forecast[cont].msgnum = 18022;
			forecast[cont].sail_damage = 1;
		}
		else
			forecast[cont].msgnum = 18012;
		return;
	}
	if(forecast[cont].humidity == W_DRY && forecast[cont].temp == W_TEMPERATE)
	{
		forecast[cont].food = (float).05;
		forecast[cont].msgnum = 18013;
		forecast[cont].temperature = randomnum(80, 60);
		forecast[cont].type = WT_WARM;
		return;
	}
	forecast[cont].food = (float).10;
	forecast[cont].msgnum = 18014;
	forecast[cont].temperature = randomnum(75, 60);
	forecast[cont].type = WT_PERFECT;
}

void find_weather(int cont, int troop_num, unit *modifiers)
{
	int count;

	for(count = 0; count < MAXTRP; ++count)
		memset(&modifiers[count], 0, UNTSIZ);
	switch(forecast[cont].type)
	{
		case WT_FREEZE:
			for(count = 0; count < MAXTRP; ++count)
				if(units[troop_num][count].mode == MODE_FLY)
				{
					--modifiers[count].defend;
					if(forecast[cont].violent == 1)
						--modifiers[count].defend;
				}
			break;
		case WT_SNOW:
			if(forecast[cont].violent == 0)
				break;
			for(count = 0; count < MAXTRP; ++count)
			{
				switch(units[troop_num][count].mode)
				{
					case MODE_WALK:
						--modifiers[count].attack;
						--modifiers[count].defend;
						break;
					case MODE_RIDE:
						modifiers[count].attack -= 2;
						modifiers[count].defend -= 2;
						break;
				}
				if(units[troop_num][count].swarm == 1)
					--modifiers[count].swarm;
			}
		case WT_RAIN:
			if(forecast[cont].violent == 0)
				break;
			for(count = 0; count < MAXTRP; ++count)
				if(units[troop_num][count].mode == MODE_WALK)
					--modifiers[count].defend;
			break;
		case WT_STORM:
			for(count = 0; count < MAXTRP; ++count)
			{
				if(forecast[cont].violent == 0 && units[troop_num][count].mode != MODE_WALK)
					continue;
				switch(units[troop_num][count].mode)
				{
					case MODE_WALK:
						--modifiers[count].defend;
						break;
					case MODE_RIDE:
						--modifiers[count].attack;
						--modifiers[count].defend;
						break;
					case MODE_FLY:
						modifiers[count].attack -= 2;
						modifiers[count].defend -= 2;
						break;
				}
				if(forecast[cont].violent == 1 && units[troop_num][count].range == 1)
					--modifiers[count].attack;
			}
			break;
		case WT_HOT:
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[troop_num][count].num_attacks > 1)
				{
					modifiers[count].attack -= 2;
					if(forecast[cont].violent == 1)
						--modifiers[count].num_attacks;
				}
				if(units[troop_num][count].swarm > 0)
					--modifiers[count].swarm;
			}
			break;
		case WT_FOG:
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[troop_num][count].ambush == 1)
					++modifiers[count].attack;
				if(units[troop_num][count].range == 1)
				{
					if(forecast[cont].violent == 1)
						--modifiers[count].range;
					else
						--modifiers[count].attack;
				}
				if(units[troop_num][count].mode == MODE_FLY)
				{
					--modifiers[count].attack;
					--modifiers[count].defend;
				}
			}
			break;
		case WT_DRY:
			if(forecast[cont].violent == 0)
				break;
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[troop_num][count].mode == MODE_RIDE)
				{
					++modifiers[count].attack;
					--modifiers[count].defend;
				}
				if(units[troop_num][count].shield == 1)
					--modifiers[count].attack;
			}
			break;
		case WT_COOL:
			for(count = 0; count < MAXTRP; ++count)
				if(units[troop_num][count].shield == 1)
					++modifiers[count].attack;
			break;
		case WT_WIND:
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[troop_num][count].mode == MODE_FLY)
				{
					--modifiers[count].attack;
					if(forecast[cont].violent == 1)
					{
						--modifiers[count].attack;
						--modifiers[count].defend;
					}
				}
				if(units[troop_num][count].range == 1)
				{
					--modifiers[count].attack;
					if(forecast[cont].violent == 1)
						--modifiers[count].attack;
				}
			}
			break;
		default:
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[troop_num][count].range == 1)
				{
					++modifiers[count].attack;
					++modifiers[count].defend;
				}
				if(units[troop_num][count].swarm == 1)
				{
					++modifiers[count].attack;
					++modifiers[count].defend;
				}
			}
			break;
	}
	modifiers[CATAPULTNUM].defend = 0;
}

void show_weather(char *classname, char *contname, player *plyptr)
{
	int cont;
	int count;
	int troop_num;
	char humidity_names[][8] = 
	{
		"dry", "normal", "moist", "heavy"
	};
	char num_attacks[8] = {0};
	char str1[256] = {0};
	char str2[256] = {0};
	char temp_names[][16] = 
	{
		"freeze", "cold", "temperate", "hot"
	};
	char unit_info[64] = {0};
	char wind_names[][8] = 
	{
		"still", "breezy", "windy", "gusty"
	};
	unit modifiers[MAXTRP];

	if((cont = find_cont(contname)) < 0)
		cont = plyptr->cont;
	if((troop_num = find_class(classname)) < 0)
		troop_num = plyptr->troop_type;
	sprintf(message, "18000%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "%d%c%c", forecast[cont].msgnum, MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "18023%c%s%c%s%c%s%c", MSGDEL, humidity_names[forecast[cont].humidity], MSGDEL, temp_names[forecast[cont].temp], MSGDEL, wind_names[forecast[cont].wind], MSGDEL);
	send_message(message, plyptr->name, 1); 
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "18015%c%c%02d%d%c%c", MSGDEL, CNTLC, forecast[cont].temperature >= 80?RED:forecast[cont].temperature >= 50?GREEN:BLUE, forecast[cont].temperature, CNTLC, MSGDEL);
	else
		sprintf(message, "18015%c%d%c", MSGDEL, forecast[cont].temperature, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "18001%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(str1, "%s Production: %c%02d%+d%%%c", food_name, CNTLC, (int)(forecast[cont].food * 100) >= 0?GREEN:RED, (int)(forecast[cont].food * 100), CNTLC);
	else
		sprintf(str1, "%s Production: %+d%%", food_name, (int)(forecast[cont].food * 100));
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(str2, "Jousting: %c%02d%s%c", CNTLC, forecast[cont].joust == 1?GREEN:RED, forecast[cont].joust == 1?"Yes":"No", CNTLC);
	else
		sprintf(str2, "Jousting: %s", forecast[cont].joust == 1?"Yes":"No");
	sprintf(message, "18002%c%-30s%c%s%c", MSGDEL, str1, MSGDEL, str2, MSGDEL);
	send_message(message, plyptr->name, 1);
	find_weather(cont, troop_num, modifiers);
	strcpy(message, "");
	for(count = 0; count < MAXTRP; ++count)
	{
		if((strlen(units[troop_num][count].name)) < 1)
			continue;
		if(modifiers[count].num_attacks < 0)
			sprintf(num_attacks, ",%+d", modifiers[count].num_attacks);
		sprintf(unit_info, "%ss: (%+d/%+d%s%s%s%s)", units[troop_num][count].name, modifiers[count].attack, modifiers[count].defend, 
		modifiers[count].num_attacks < 0?num_attacks:"", modifiers[count].ambush < 0?",no amb":"", 
		modifiers[count].range < 0?",no rng":"", modifiers[count].swarm < 0?",no swm":"");
		if((strlen(message)) < 1)
			sprintf(message, "18003%c%-30s%c", MSGDEL, unit_info, MSGDEL);
		else
			sprintf(message, "%s%s%c", message, unit_info, MSGDEL);
		strcpy(unit_info, "");
		if((count + 1) % 2 == 0)
		{
			send_message(message, plyptr->name, 1);
			strcpy(message, "");
		}
	}
	if((strlen(message)) > 0)
		send_message(message, plyptr->name, 1); 
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1); 
	if(forecast[cont].sail_damage == 1)
	{
		sprintf(message, "18024%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1); 
	}
}
