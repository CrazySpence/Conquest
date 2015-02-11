
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

void abdicate(player *plyptr)
{
	if(plyptr->level[plyptr->cont] != maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(titan(plyptr))
	{
		sprintf(message, "4000%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->king[plyptr->cont] = conq_info.today - REIGN;
	check_reign(plyptr->cont, plyptr);
}

void abilities(char *classname, player *plyptr)
{
	char str[512] = {0};
	int class_num;
	int level;

	if((class_num = find_class(classname)) < 0)
		class_num = BASE_CLASSNUM(plyptr->classnum);
	if((strlen(basenames[class_num])) < 1)
		class_num = FIGHTER;
	sprintf(message, "4331%c%s%c", MSGDEL, basenames[class_num], MSGDEL);
	send_message(message, plyptr->name, 0);
	switch(class_num)
	{
		case BARBARIAN:
			level = -1;
			if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 2;
				sprintf(str, "+4/-2,-1-5%%");
			}
			else if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 1;
				sprintf(str, "+3/-1,-5-10%%");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 0;
				sprintf(str, "+2/+0,-10-15%%");
			}
			sprintf(message, "4332%cBerserk%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 2;
				sprintf(str, "Attack 4 levels down");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 1;
				sprintf(str, "Attack 3 levels down");
			}
			else 
			{
				level = 0;
				sprintf(str, "Attack 2 levels down");
			}
			sprintf(message, "4332%cBloodLust%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 2;
				sprintf(str, "80%%");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 1;
				sprintf(str, "60%%");
			}
			else if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 0;
				sprintf(str, "40%%");
			}
			sprintf(message, "4332%cLoot%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
		case CLERIC:
			level = -1;
			if(plyptr->level[plyptr->cont] >= 7)
			{
				level = 2;
				sprintf(str, "1-4 players,levels 3-9");
			}
			else if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 1;
				sprintf(str, "1-3 players,levels 3-7");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 0;
				sprintf(str, "1-2 players,levels 3-5");
			}
			sprintf(message, "4332%cCrusade%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 2;
				sprintf(str, "Assets + Troops");
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 1;
				sprintf(str, "Assets + Troops - Quest");
			}
			else
			{
				level = 0;
				sprintf(str, "Assets only");
			}
			sprintf(message, "4332%cDivination%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 2;
				sprintf(str, "+8-10 honor");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 1;
				sprintf(str, "+4-6 honor");
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 0;
				sprintf(str, "+1-2 honor");
			}
			sprintf(message, "4332%cGrace%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 7)
			{
				level = 2;
				sprintf(str, "3-5%% restored");
			}
			else if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 1;
				sprintf(str, "2-4%% restored");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 0;
				sprintf(str, "1-3%% restored");
			}
			sprintf(message, "4332%cResurrect%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 7)
			{
				level = 2;
				sprintf(str, "15-20%%,all troops");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 1;
				sprintf(str, "15-20%%,troops 1-5");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 0;
				sprintf(str, "10-15%%,troops 1-3");
			}
			sprintf(message, "4332%cTurn Undead%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
		case FIGHTER:
			level = -1;
			if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 2;
				sprintf(str, "+75-100,troops 1-4");
			}
			else if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 1;
				sprintf(str, "+50-75,troops 1-3");
			}
			else if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 0;
				sprintf(str, "+25-50,troops 1-2");
			}
			sprintf(message, "4332%cCall-to-Arms%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 2;
				sprintf(str, "+1/+1,swm");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 1;
				sprintf(str, "+1/+1");
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 0;
				sprintf(str, "+0/+1");
			}
			sprintf(message, "4332%cConditioning%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 2;
				sprintf(str, "+15%%");
			}
			else if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 1;
				sprintf(str, "+10%%");
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 0;
				sprintf(str, "+5%%");
			}
			sprintf(message, "4332%cJoust%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
		case MAGE:
			level = -1;
        
			if(plyptr->levelmax >= 6)
			{
				level = 2;
				sprintf(str, "+1-4 components per HK");
			} else if(plyptr->levelmax >= 4)
			{
				level = 1;
				sprintf(str, "+1-3 components per HK");
			} else if(plyptr->levelmax >= 2)
			{
				level = 0;
				sprintf(str, "+1-2 components per HK");
			}
			sprintf(message, "4332%cAlchemy%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 7)
			{
				level = 2;
				sprintf(str, "Spells levels 1-5 cost 1/2 MPs");
			}
			else if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 1;
				sprintf(str, "Spells levels 1-4 cost 1/2 MPs");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 0;
				sprintf(str, "Spells levels 1-3 cost 1/2 MPs");
			}
			sprintf(message, "4332%cProficiency%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
		case RANGER:
			level = -1;
			if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 2;
				sprintf(str, "+25-100%%");
			}
			else if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 1;
				sprintf(str, "+25-75%%");
			}
			else if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 0;
				sprintf(str, "+25-50%%");
			}
			sprintf(message, "4332%cCamouflage%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 2;
				sprintf(str, "0%% spoilage");
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 1;
				sprintf(str, "25%% spoilage");
			}
			else 
			{
				level = 0;
				sprintf(str, "40%% spoilage");
			}
			sprintf(message, "4332%cPreserve%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 2;
				sprintf(str, "%s Horde", s_titles[MIGHTY]);
			}
			else if(plyptr->level[plyptr->cont] >= 3)
			{
				level = 1;
				sprintf(str, "%s Horde", s_titles[STRONG]);
			}
			else if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 0;
				sprintf(str, "%s Horde", s_titles[MEDIOCRE]);
			}
			sprintf(message, "4332%cSummon%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 2;
				sprintf(str, "+15-20%% %s", food_name);
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 1;
				sprintf(str, "+10-15%% %s", food_name);
			}
			else
			{
				level = 0;
				sprintf(str, "+5-10%% %s", food_name);
			}
			sprintf(message, "4332%cTend%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 2;
				sprintf(str, "Assets + Troops");
			}
			else if(plyptr->level[plyptr->cont] >= 1)
			{
				level = 1;
				sprintf(str, "Assets + Troops - Quest");
			}
			else
			{
				level = 0;
				sprintf(str, "Assets only");
			}
			sprintf(message, "4332%cVision%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
		case VAMPIRE:
			if(mult_vampires == 1)
			{
				level = -1;
				if(plyptr->level[plyptr->cont] >= 8)
				{
					level = 2;
					sprintf(str, "+25%%");
				}
				else if(plyptr->level[plyptr->cont] >= 5)
				{
					level = 1;
					sprintf(str, "+15%%");
				}
				else if(plyptr->level[plyptr->cont] >= 2)
				{
					level = 0;
					sprintf(str, "+5%%");
				}
				sprintf(message, "4332%cCorruption%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
				send_message(message, plyptr->name, 0);
			}
			level = -1;
			if(plyptr->level[plyptr->cont] >= 6)
			{
				level = 2;
				sprintf(str, "-25-30%% food,sickness");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 1;
				sprintf(str, "-20-25%% food");
			}
			else if(plyptr->level[plyptr->cont] >= 4)
			{
				level = 0;
				sprintf(str, "-10-15%% food");
			}
			sprintf(message, "4332%cPestilence%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			level = -1;
			if(plyptr->level[plyptr->cont] >= 8)
			{
				level = 2;
				sprintf(str, "-5-10%% troops");
			}
			else if(plyptr->level[plyptr->cont] >= 5)
			{
				level = 1;
				sprintf(str, "-5-10%% %ss", peasant_name);
			}
			else if(plyptr->level[plyptr->cont] >= 2)
			{
				level = 0;
				sprintf(str, "-5-10%% %s", food_name);
			}
			sprintf(message, "4332%cRitual%c%s%c%s%c", MSGDEL, MSGDEL, level < 0?"N/A":ability_level[level], MSGDEL, level < 0?"N/A":str, MSGDEL);
			send_message(message, plyptr->name, 0);
			break;
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 0);
}

void alliance(char *name, player *plyptr)
{
	int plynum;
	player *allyptr;

	if(strlen(name) < 1)
	{
		help(plyptr->name, "alliance", "", "");
		return;
	}
	if((plynum = find_player(name)) <= 0) 
	{
		sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	allyptr = &players[plynum];
	if(plyptr == allyptr)
	{
		sprintf(message, "4001%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_hero(HERO_DIPLOMAT, plyptr) && !check_hero(HERO_DIPLOMAT, &players[plynum]))
	{
		if(barbarian(plyptr))
		{
			sprintf(message, "4002%c%s%c", MSGDEL, select_classname(BARBARIAN), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(mage(plyptr))
		{
			sprintf(message, "4003%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(cleric(plyptr))
		{
			sprintf(message, "4004%c%s%c", MSGDEL, select_classname(CLERIC), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	}
	if(vampire(plyptr))
	{

		sprintf(message, "4005%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->levelmax >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "4007%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->levelmax - allyptr->levelmax > MAXALLY)
	{
		sprintf(message, "4006%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->ally != 0)
	{
		sprintf(message, "4009%c%s%c", MSGDEL, players[plyptr->ally].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < ALLIANCE)
	{
		sprintf(message, "1%c%d%cto request an alliance%c", MSGDEL, ALLIANCE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(is_vassal(plyptr, allyptr))
	{
		sprintf(message, "4010%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_hero(HERO_DIPLOMAT, plyptr) && !check_hero(HERO_DIPLOMAT, &players[plynum]))
	{
		if(barbarian(allyptr))
		{
			sprintf(message, "4002%c%s%c", MSGDEL, select_classname(BARBARIAN), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(mage(allyptr))
		{
			sprintf(message, "4003%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(cleric(allyptr))
		{
			sprintf(message, "4004%c%s%c", MSGDEL, select_classname(CLERIC), MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	}
	if(vampire(allyptr))
	{
		sprintf(message, "4005%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(allyptr->levelmax >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "4008%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(allyptr->levelmax - plyptr->levelmax > MAXALLY)
	{
		sprintf(message, "4011%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(allyptr->ally != 0)
	{
		if(plyptr->ally_request == playernum(allyptr))
			plyptr->ally_request = 0;
		sprintf(message, "4012%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, players[allyptr->ally].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!equal_class(plyptr->classnum, allyptr->classnum) && !check_hero(HERO_DIPLOMAT, plyptr) && !check_hero(HERO_DIPLOMAT, &players[plynum]))
	{
		sprintf(message, "4013%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_class(allyptr->classnum), MSGDEL, allyptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(is_vassal(allyptr, plyptr))
	{
		sprintf(message, "4010%c%c", MSGDEL, MSGDEL);
		send_message(message, allyptr->name, 1);
		return;
	}
	if(plyptr->ally_request == plynum)
	{
		allyptr->ally = playernum(plyptr);
		allyptr->ally_request = 0;
		plyptr->ally = plynum;
		plyptr->ally_request = 0;
		plyptr->movepts -= ALLIANCE;
		sprintf(message, "4014%c%s%c", MSGDEL, allyptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "4014%c%s%c", MSGDEL, plyptr->name, MSGDEL);
		send_message(message, allyptr->name, 1);
		log_event("", message, allyptr->name);
		save_player(plynum);
	}
	else
	{
		if(allyptr->ally_request == 0)
		{
			allyptr->ally_request = playernum(plyptr);
			plyptr->movepts -= ALLIANCE;
			sprintf(message, "4015%c%s%c", MSGDEL, allyptr->kname[allyptr->cont], MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4016%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->name, MSGDEL);
			send_message(message, allyptr->name, 1);
			sprintf(message, "4017%c%s%c", MSGDEL, plyptr->name, MSGDEL);
			log_event("", message, allyptr->name);
			save_player(plynum);
		}
		else
		{
			if(allyptr->ally_request == playernum(plyptr))
				sprintf(message, "4320%c%s%c", MSGDEL, allyptr->name, MSGDEL);
			else
				sprintf(message, "4018%c%s%c%s%c", MSGDEL, players[allyptr->ally_request].name, MSGDEL, allyptr->name, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
	}
}

void ally(char *contname, player *plyptr)
{
	int cont = 0;
	player *allyptr;

	if(plyptr->ally < 1)
	{
		sprintf(message, "4019%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < ALLY)
	{
		sprintf(message, "1%c%d%cto view your ally's kingdom%c", MSGDEL, ALLY, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	allyptr = &players[plyptr->ally];
	plyptr->movepts -= ALLY;
	if((cont = find_cont(contname)) < 0 || allyptr->level[cont] < 0)
		cont = allyptr->cont;
	person(conts[cont].name, plyptr, allyptr);
	show_surrender(plyptr->name, allyptr);
	sprintf(message, "10025%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
	send_message(message, plyptr->name, 1);
	review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_DEFENSE, allyptr, allyptr);
	show_formation(plyptr->name, cont, ARMY_DEFENSE, allyptr);
	if(advanced_armies == 1)
	{
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, ARMY_CAMPAIGN, allyptr, allyptr);
		show_formation(plyptr->name, cont, ARMY_CAMPAIGN, allyptr);
	}
}
	
void autoenlist(char *arg1, player *plyptr)
{
	if((STRCASECMP(arg1, "on")) == 0 && !(plyptr->special_kingdom & spcenlist[plyptr->cont]))
		plyptr->special_kingdom += spcenlist[plyptr->cont];
	else if((STRCASECMP(arg1, "off")) == 0 && plyptr->special_kingdom & spcenlist[plyptr->cont])
		plyptr->special_kingdom -= spcenlist[plyptr->cont];
	sprintf(message, "14016%c%s%c", MSGDEL, plyptr->special_kingdom & spcenlist[plyptr->cont]?"ON":"OFF", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void balance(player *plyptr)
{
	char str[32] = {0};
	int classnum;
	int count;

	sprintf(message, "4057%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(classnum = 0; classnum < total_races[FIGHTER]; ++classnum)
	{
		memset(str, 0, 32);
		for(count = 0; count < conq_info.balance[classnum] / 5; ++count)
			strcat(str, ":");
		sprintf(message, "4058%c%-16s%c%s%c", MSGDEL, classnames[classnum], MSGDEL, str, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void bestow(char *itemname, int amount, int *gitem, int *item, int type, player *allyptr, player *plyptr)
{
	if(plyptr->level[plyptr->cont] < 3)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 3, 0), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!equal_class(plyptr->classnum, allyptr->classnum) && !is_vassal(plyptr, allyptr))
	{
		sprintf(message, "4020%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_class(allyptr->classnum), MSGDEL, allyptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, allyptr))
		return;
	if(plyptr->city != allyptr->city)
	{
		sprintf(message, "4021%c%s%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, allyptr->city == secret_city?"??????":cities[allyptr->city].name, MSGDEL, conts[allyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] < allyptr->level[allyptr->cont])
	{
		sprintf(message, "4022%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, select_title(allyptr->classnum, allyptr->level[allyptr->cont], allyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] == maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "4023%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BESTOW)
	{
		sprintf(message, "1%c%d%cto bestow %s%c", MSGDEL, BESTOW, MSGDEL, allyptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount < 1)
	{
		sprintf(message, "37%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(*gitem < amount)
	{
		sprintf(message, "4024%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, allyptr->name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type < 7 || type > 10)
	{
		sprintf(message, "4025%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type == 10)
	{
		if(plyptr->peasant[plyptr->cont] - amount < promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][2])
		{
			sprintf(message, "4026%c%s%c", MSGDEL, peasant_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(allyptr->peasant[allyptr->cont] + amount > promotions[BASE_CLASSNUM(allyptr->classnum)][allyptr->level[allyptr->cont] + 1][2])
		{
			sprintf(message, "4027%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	}
	plyptr->movepts -= BESTOW;
	if(!barbarian(plyptr))
	{
		++plyptr->honor[plyptr->cont];
		if(type == 9)
			plyptr->honor[plyptr->cont] += amount / 10000;
		else
			plyptr->honor[plyptr->cont] += amount / 1000;
	}
	*gitem -= amount;
	*item += amount;
	sprintf(message, "4028%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, allyptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4029%c%s%c%d%c%s%c", MSGDEL, plyptr->nick, MSGDEL, amount, MSGDEL, itemname, MSGDEL);
	send_message(message, allyptr->name, 1);
	log_event("", message, allyptr->name);
	save_player(playernum(allyptr));
}

void break_alliance(player *plyptr)
{
	player *allyptr;

	if(plyptr->ally < 1)
	{
		sprintf(message, "4019%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	allyptr = &players[plyptr->ally];
	sprintf(message, "4030%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	log_event("", message, allyptr->name);
	sprintf(message, "4031%c%s%c", MSGDEL, plyptr->name, MSGDEL);
	send_message(message, allyptr->name, 1);
	sprintf(message, "4031%c%s%c", MSGDEL, allyptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	allyptr->ally = 0;
	plyptr->ally = 0;
	save_player(playernum(allyptr));
}

int buy(char *itemdesc, char *itemname, int amount, int cprice,
	int *cqty, int honor, int *item, int marketnum, int move, 
	int special_troop, player *plyptr)
{
	int citynum;
	int plynum;
	int price = 0;
	int taxamt = 0;
	int totalmove;
	int troop_num = 0;
	int trooptype = 0;

	citynum = plyptr->city;
	if(amount < 1)
	{
		sprintf(message, "37%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(marketnum < MAXMRK)
	{
		if(special_troop == 1)
			troop_num = marketnum;
		else
			troop_num = cities[plyptr->city].troop_num[marketnum];
		if(restrict_markets == 0 || special_troop == 1)
			trooptype = plyptr->troop_type;
		else
			trooptype = cities[plyptr->city].troop_type[marketnum];
		switch(troop_num)
		{
			case RENNUM:
			case SOLDIERNUM:
			case ARCHERNUM:
			case KNIGHTNUM:
			case BLOODGUARDNUM:
				if(plyptr->troop_type != trooptype)
				{
					if(special_troop == 1)
						sprintf(message, "14011%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][5], MSGDEL, itemname, MSGDEL);
					else
						sprintf(message, "4032%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, itemname, MSGDEL);
					send_message(message, plyptr->name, 1);
					return(0);
				}
				break;
			case CATAPULTNUM:
				if(restrict_markets == 1 && cities[plyptr->city].troop_type[marketnum] != plyptr->troop_type)
				{
					sprintf(message, "4032%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, itemname, MSGDEL);
					send_message(message, plyptr->name, 1);
					return(0);
				}
				break;
			default:
				break;
		}
	}
	if(!check_buy(amount, (marketnum < MAXMRK?troop_num:marketnum), plyptr))
		return(0);
	if(amount > *cqty)
	{
		if(special_troop == 1)
			sprintf(message, "4033%c%s%c%d%c%s%c", MSGDEL, adjectives[plyptr->gender][5], MSGDEL, *cqty, MSGDEL, itemname, MSGDEL);
		else
			sprintf(message, "4034%c%d%c%s%c", MSGDEL, *cqty, MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(marketnum < MAXMRK && special_troop == 1)
	{
		switch(troop_num)
		{
			case RENNUM:
			case SOLDIERNUM:
			case ARCHERNUM:
			case KNIGHTNUM:
				totalmove = troop_num;
				break;
			case BLOODGUARDNUM:
				sprintf(message, "32%c%s%c%s%c", 
				MSGDEL, units[FIGHTER][BLOODGUARDNUM].name, MSGDEL, units[FIGHTER][BLOODGUARDNUM].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return(0);
			default:
				totalmove = move * amount;
				break;
		}
	}
	else
		totalmove = move * amount;
	if(plyptr->movepts < totalmove)
	{
		sprintf(message, "1%c%d%cto buy %d %s(s)%c", MSGDEL, totalmove, MSGDEL, amount, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if(plyptr->honor[plyptr->cont] < honor)
	{
		sprintf(message, "4035%c%s%c", MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	if((special_troop == 0 || troop_num > BLOODGUARDNUM) && !check_hero(HERO_MERCHANTKING, plyptr))
		taxamt = tax(amount, cprice, cities[citynum].tax);
	price = cprice * amount;
	if(check_hero(HERO_MERCHANTKING, plyptr) && randomnum(100, 1) <= plyptr->level[plyptr->cont] * 10)
	{
		price /= 2;
		if(price < 1)
			price = 1;
		sprintf(message, "8044%c%s%c", MSGDEL, heroes[HERO_MERCHANTKING].type, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	if(plyptr->copper[plyptr->cont] < (price + taxamt))
	{
		if(special_troop == 1)
			sprintf(message, "4036%c%s%c%d%c%s%c%s%c%d%c%s%c%d%c%s%c", MSGDEL, 
			money_name, MSGDEL, amount, MSGDEL, itemname, MSGDEL, itemname, MSGDEL, cprice, MSGDEL, money_name, MSGDEL, plyptr->copper[plyptr->cont], MSGDEL, money_name, MSGDEL);
		else
			sprintf(message, "4037%c%s%c%d%c%s%c%s%c%d%c%s%c%d%c%s%c", MSGDEL, 
			money_name, MSGDEL, amount, MSGDEL, itemname, MSGDEL, itemname, 
			MSGDEL, cprice + tax(1, cprice, cities[citynum].tax), MSGDEL, money_name, MSGDEL, 
			plyptr->copper[plyptr->cont], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return(0);
	}
	*item += amount;
	*cqty -= amount;
	plyptr->copper[plyptr->cont] -= (price + taxamt);
	if(heroes[HERO_MERCHANTKING].owner > 0 && taxamt > 0 && !titan(plyptr) && randomnum(100, 1) >= 50)
	{
		plynum = heroes[HERO_MERCHANTKING].owner;
		if(players[plynum].city == plyptr->city)
		{
			players[plynum].copper[plyptr->cont] += taxamt;
			sprintf(message, "8047%c%s%c%d%c%s%c%s%c", MSGDEL, heroes[HERO_MERCHANTKING].name, MSGDEL, taxamt, MSGDEL, money_name, MSGDEL, cities[citynum].name, MSGDEL);
			send_message(message, players[plynum].name, 1);
		}
	}
	else
		price += taxamt;
	cities[citynum].copper += price;
	plyptr->movepts -= totalmove;
	if(honor > 0)
		plyptr->honor[plyptr->cont] -= (int)(honor * .30 + 1);
	if(special_troop == 1)
	{
		if(check_hero(HERO_NECROMANCER, plyptr))
			sprintf(message, "4038%c%s%c%d%c%s%c", MSGDEL, heroes[HERO_NECROMANCER].name, MSGDEL, amount, MSGDEL, itemname, MSGDEL);
		else
			sprintf(message, "4039%c%d%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL);
	}
	else
		sprintf(message, "4040%c%d%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL);
	send_message(message, plyptr->name, 1);
	if((strlen(itemdesc)) > 0)
		send_message(itemdesc, plyptr->name, 1);
	save_city(citynum);
	return(1);
}

void church(char *arg, player *plyptr)
{
	int honor;

	if(barbarian(plyptr))
	{
		sprintf(message, "4042%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}		
	if(vampire(plyptr))
	{
		sprintf(message, "17051%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}		
	if((STRCASECMP(arg, "pray")) == 0)
	{
		church_pray(plyptr);
		return;
	}
	honor = (plyptr->level[plyptr->cont] + 1) * (plyptr->level[plyptr->cont] + 1);
	if(cleric(plyptr) || ranger(plyptr))
		honor += plyptr->level[plyptr->cont] + 1;
	if(plyptr->honor[plyptr->cont] >= honor)
		sprintf(message, "4043%c%c", MSGDEL, MSGDEL);
	else if(plyptr->honor[plyptr->cont] > honor * .75)
		sprintf(message, "4044%c%c", MSGDEL, MSGDEL);
	else if(plyptr->honor[plyptr->cont] > honor * .50)
		sprintf(message, "4045%c%c", MSGDEL, MSGDEL);
	else if(plyptr->honor[plyptr->cont] > honor * .25)
		sprintf(message, "4046%c%c", MSGDEL, MSGDEL);
	else
		sprintf(message, "4042%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void church_pray(player *plyptr)
{
	if(plyptr->movepts < PRAY)
	{
		sprintf(message, "1%c%d%cto visit your Church%c", MSGDEL, PRAY, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= PRAY;
	sprintf(message, "4047%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(randomnum(100, 1) <= 25 + (plyptr->level[plyptr->cont] + 1))
		plyptr->honor[plyptr->cont] += randomnum(3, 1);
	if(cleric(plyptr) || check_artifact(GRAIL, plyptr) || check_hero(HERO_BISHOP, plyptr))
		++plyptr->honor[plyptr->cont];
}

void color(char *arg1, player *plyptr)
{
	if((STRCASECMP(arg1, "on")) == 0 && !(plyptr->special_misc & SPC_COLOR))
		plyptr->special_misc += SPC_COLOR;
	else if((STRCASECMP(arg1, "off")) == 0 && plyptr->special_misc & SPC_COLOR)
		plyptr->special_misc -= SPC_COLOR;
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "4048%c%c%02dON%c%c", MSGDEL, CNTLC, GREEN, CNTLC, MSGDEL);
	else
		sprintf(message, "4048%cOFF%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void corpse(player *plyptr)
{
	if(plyptr->level[plyptr->cont] < 1 || vampire(plyptr))
	{
		sprintf(message, "4049%c%s%c", MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->castle[plyptr->cont] == 1 && plyptr->isprotected == 1 && plyptr->attacks_lost[plyptr->cont] == 0)
	{
		sprintf(message, "4050%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->attacks_lost[plyptr->cont] = 0;
	plyptr->castle[plyptr->cont] = 1;
	plyptr->isprotected = 1;
	sprintf(message, "4051%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4052%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
	send_channel(message, 0);
}

void courier(char *name, char *msg, player *plyptr)
{
	char fname[64] = {0};
	int count;
	int fp;
	int plynum;
	loginfo lptr;

	if(strlen(name) < 1)
	{
		if(!check_file(".msg", plyptr->name))
		{
			sprintf(message, "4053%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		sprintf(fname, "logs/%s.msg", plyptr->name);
#ifdef WIN32
		if((fp = open(fname, O_RDWR | O_CREAT | O_BINARY, DFTPERM)) == -1)
#else
		if((fp = open(fname, O_RDWR | O_CREAT, DFTPERM)) == -1)
#endif
		{
			sprintf(message, "4053%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		sprintf(message, "57%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		memset(&lptr, 0, LOGSIZ);
		while((read(fp, &lptr, LOGSIZ)) == LOGSIZ)
		{
			sprintf(message, "4054%c%s%c%s%c", MSGDEL, lptr.time, MSGDEL, lptr.message, MSGDEL);
			send_message(message, plyptr->name, 1);
			memset(&lptr, 0, LOGSIZ);
		}
		sprintf(message, "56%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		close(fp);
		remove(fname);
		return;
	}
	if((STRCASECMP(name, "titans")) == 0)
	{
		plynum = playernum(plyptr);
		for(count = 1; count < current_player; ++count)
			if(titan(&players[count]) && count != plynum)
				courier(players[count].name, msg, plyptr);
		return;
	}
	if(plyptr->movepts < MESSAGE)
	{
		sprintf(message, "1%c%d%cto send a message%c", MSGDEL, MESSAGE, MSGDEL, MSGDEL);
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
		sprintf(message, "38%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!(players[plynum].special_misc & SPC_MESSAGES))
	{
		sprintf(message, "4059%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_word(plyptr->name, msg, 192, 1))
		return;
	plyptr->movepts -= MESSAGE;
	log_event(plyptr->name, msg, players[plynum].name);
	sprintf(message, "4055%c%s%c", MSGDEL, players[plynum].kname[players[plynum].cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4056%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->name, MSGDEL);
	send_message(message, players[plynum].name, 1);
	check_vacation(plyptr->name, &players[plynum]);
}

void display_news(char *name)
{
	char input[128];
	FILE *fp;

	if((fp = fopen(NEWFIL, "r")) == NULL)
	{
		sprintf(message, "4062%cNothing new to report.%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	while((fgets(input, 128, fp)) != NULL)
	{
		strip(input);
		sprintf(message, "4062%c%s%c", MSGDEL, input, MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	fclose(fp);
}

void done(player *plyptr)
{
	if(plyptr->validate == NOT_VAL)
		return;
	if(restrict_combat == 1 && !titan(plyptr))
	{
		sprintf(message, "4041%c%d%c", MSGDEL, DONETIME / 60, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "4318%c%s%c%s%c%s%c", MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
		send_channel(message, 0);
		plyptr->done = time(NULL);
		return;
	}
	sprintf(message, "4063%c%s%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->name, MSGDEL);
	send_message(message, plyptr->name, 0);
	plyptr->sock = INVALID_SOCKET;
	plyptr->validate = NOT_VAL;
}

void drop(player *plyptr)
{
	int artnum;
	int count;

	if((artnum = find_artifact(plyptr)) < 0)
	{
		sprintf(message, "4064%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(artnum == ROD)
	{
		sprintf(message, "11066%c%s%cyou%c", MSGDEL, artifacts[artnum].name, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	artifacts[artnum].owner = 0;
	save_art();
	sprintf(message, "4065%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(artnum == CROWN && !barbarian(plyptr) && !vampire(plyptr))
	{
		sprintf(message, "4066%c%s%c", MSGDEL, artifacts[CROWN].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		for(count = 0; count < num_conts; ++count)
		{
			plyptr->honor[count] /= 2;
			if(plyptr->level[count] >= 0)
				random_event(count, EVENT_SICKNESS, plyptr);
		}
	}
}

void email(char *address, char *filename, char *name)
{
	char cmd[256];
	char fname[64];
	char tfname[64];
	loginfo plog;
	FILE *ofp;
	int found = 0;
	int fp;

#ifdef WIN32
	sprintf(message, "999%cE-mail not available for WIN32 server.", MSGDEL);
	send_message(message, name, 0);
	return;
#endif
	if(strlen(address) < 1)
	{
		help(name, "email", "", "");
		return;
	}
	if(!strchr(address, '@') || !strchr(address, '.'))
	{
		sprintf(message, "4067%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	if((STRCASECMP(filename, "kia")) == 0)
	{
		found = 1;
		sprintf(cmd, "mail -s \"KIA from Conquest!\" %s < logs/kia.log 2>/dev/null", address);
		system(cmd);
	}
	if((STRCASECMP(filename, "log")) == 0)
	{
		found = 1;
		sprintf(fname, "logs/%s", name);
		strcpy(tfname, tempnam("./logs", NULL));
		if((fp = open(fname, O_RDONLY)) == -1)
		{
			sprintf(message, "4068%c%c", MSGDEL, MSGDEL);
			send_message(message, name, 1);
			return;
		}
		if((ofp = fopen(tfname, "w")) == NULL)
		{
			sprintf(message, "4068%c%c", MSGDEL, MSGDEL);
			send_message(message, name, 1);
			close(fp);
			return;
		}
		memset(&plog, 0, LOGSIZ);
		while((read(fp, &plog, LOGSIZ)) == LOGSIZ)
		{
			fprintf(ofp, "%s: %s\n", plog.time, plog.message);
			memset(&plog, 0, LOGSIZ);
		}
		close(fp);
		fclose(ofp);
		sprintf(cmd, "mail -s \"LOG from Conquest!\" %s < %s 2>/dev/null", address, tfname);
		system(cmd);
		remove(tfname);
	}
	if(found)
		sprintf(message, "4069%c%s%c%s%c", MSGDEL, filename, MSGDEL, address, MSGDEL);
	else
		sprintf(message, "4070%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void enlist(char *str_amount, player *plyptr)
{
	int amount;

	amount = atoi(str_amount);
	if(amount < 1)
	{
		help(plyptr->name, "enlist", "", "");
		return;
	}
	if(plyptr->peasant[plyptr->cont] < amount)
	{
		sprintf(message, "4071%c%s%c%d%c%s%c", MSGDEL, peasant_name, MSGDEL, amount, MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(combined_army(plyptr->cont, plyptr) + amount > plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum))
	{
		sprintf(message, "31%c%s%c%d%c%d%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL, plyptr->castle[plyptr->cont] * castle_max(plyptr->classnum), MSGDEL, combined_army(plyptr->cont, plyptr), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->peasant[plyptr->cont] -= amount;
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[RENNUM] += amount;
	sprintf(message, "4072%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL, units[plyptr->troop_type][RENNUM].desc, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void escape(char *cityname, player *plyptr)
{
	int ctynum;
	int oldcity;

	if(strlen(cityname) < 1)
	{
		help(plyptr->name, "escape", "", "");
		return;
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(ctynum == plyptr->city)
	{
		sprintf(message, "19%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "20%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < 5 + (plyptr->level[plyptr->cont] + 1))
	{
		sprintf(message, "1%c%d%cto escape from %s%c", MSGDEL, 5 + plyptr->level[plyptr->cont] + 1, MSGDEL, cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[plyptr->city].lock == 0)
	{
		sprintf(message, "4073%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].lock == 1)
	{
		sprintf(message, "4074%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(randomnum(100, 1) < (plyptr->level[plyptr->cont] + 1) * 8)
	{
		plyptr->movepts -= 2;
		sprintf(message, "4075%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "4076%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= (5 + plyptr->level[plyptr->cont]); 
	oldcity = plyptr->city;
	plyptr->city = ctynum;
	sprintf(message, "4075%c%s%c", MSGDEL, cities[oldcity].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4077%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4088%c%s%c%s%c%s%c%s%c", MSGDEL, 
	select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL, oldcity == secret_city?"??????":cities[oldcity].name, MSGDEL);
	send_channel(message, 0);
}

void espionage(char *name, char *type, player *plyptr)
{
	char actions[][8] = 
	{
		"arson", "destroy", "poison", "steal"
	};
	char desc[][32] = 
	{
		"arson", "",
		"", "to steal"
	};
	char flowers[][16] = 
	{
		"roses", "daisies", "sunflowers", "poms"
	};
	char insects[][16] =
	{
		"firefly", "ant", "spider", "beetle"
	};
	int amount;
	int chance = 0;
	int cont;
	int copper = 0;
	int count;
	int move = 0;
	int plynum;

	if(plyptr->spy[plyptr->cont] < 0)
	{
		sprintf(message, "4089%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((cleric(plyptr) || mage(plyptr)) && !titan(plyptr))
	{
		sprintf(message, "4090%c%s%c", MSGDEL, select_classname(BASE_CLASSNUM(plyptr->classnum)), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
        }
	if(plyptr->level[plyptr->cont] < 2)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 2, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
        }
	if(strlen(name) < 1 || strlen(type) < 1)
	{
		help(plyptr->name, "espionage", "", "");
		return;
	}
	if((plynum = find_player(name)) <= 0)
	{
		if((plynum = find_player(type)) <= 0)
		{
			sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		strcpy(message, name);
		strcpy(name, type);
		strcpy(type, message);
        }
	if(plyptr == &players[plynum])
	{
		sprintf(message, "4091%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	cont = players[plynum].cont;
	if(plyptr->cont != cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(count = 0; count < 4; ++count)
		if((STRCASECMP(type, actions[count])) == 0)
			break;
	if(count == 4)
	{
		sprintf(message, "4092%c%s%c%s%c%s%c%s%c", MSGDEL, actions[0], MSGDEL, actions[1], MSGDEL, actions[2], MSGDEL, actions[3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	switch(count)
	{
		case 0:
			if(players[plynum].castle[cont] < 2)
			{
				sprintf(message, "4093%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				return;
			}
			chance = 45 - ((players[plynum].level[cont] + 1) * 5);
			copper = 15000;
			move = 7;
			break;
		case 1:
			chance = 40 - ((players[plynum].level[cont] + 1) * 2);
			copper = 10000;
			move = 5;
			sprintf(desc[count], "to destroy %s", food_name);
			break;
		case 2:
			chance = 40 - ((players[plynum].level[cont] + 1) * 2);
			copper = 20000;
			move = 6;
			sprintf(desc[count], "to poison %ss", peasant_name);
			break;
		default:
			chance = 40 - ((players[plynum].level[cont] + 1) * 3);
			copper = 15000;
			move = 8;
			break;
	}
	if(barbarian(plyptr))
	{
		++move;
		chance += 10;
	}
	if(vampire(plyptr))
	{
		move -= 2;
		chance -= 25;
	}
	if(check_hero(HERO_NINJA, plyptr))
	{
		chance -= 25;
		if(check_artifact(DAGGER, plyptr))
			chance -= 25;
	}
	else
		if(check_artifact(DAGGER, plyptr))
			chance -= 15;
	if(players[plynum].spy[players[plynum].cont] >= 0)
		chance += 5;
	if(check_hero(HERO_NINJA, &players[plynum]))
	{
		chance += 15;
		if(check_artifact(DAGGER, &players[plynum]))
			chance += 15;
	}
	else
		if(check_artifact(DAGGER, &players[plynum]))
			chance += 10;
	if(chance < 1)
		chance = 1;
	if(plyptr->movepts < move)
	{
		sprintf(message, "1%c%d%cto send your operative%c", MSGDEL, move, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[cont] < copper)
	{
		sprintf(message, "4094%c%s%c%s%c%s%c%d%c%s%c", MSGDEL, 
		insects[count], MSGDEL, flowers[count], MSGDEL, insects[count], MSGDEL, copper, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
        if(plyptr->attacks >= attacks_allowed(plyptr))
	{
		sprintf(message, "4%c%d%c", MSGDEL, plyptr->attacks, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->copper[cont] -= copper;
	plyptr->movepts -= move;
	if(randomnum(100, 1) >= chance)
	{
		plyptr->spy[plyptr->cont] = -1;
		sprintf(message, "4095%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		if(check_hero(HERO_NINJA, plyptr))
			dismiss(plyptr);
		sprintf(message, "4096%c%s%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL, desc[count], MSGDEL);
		send_message(message, players[plynum].name, 1);
		log_event("", message, players[plynum].name);
		return;
	}
	++plyptr->attacks;
	switch(count)
	{
		case 0:
			--players[plynum].castle[cont];
			sprintf(message, "4097%c%s%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4098%c%s%c%s%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case 1:
			amount = randomnum(25, 10);
			players[plynum].food[cont] -= players[plynum].food[cont] * amount / 100;
			sprintf(message, "4099%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, players[plynum].name, MSGDEL, food_name, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4100%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL, amount, MSGDEL, food_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		case 2:
			amount = randomnum(10, 5);
			players[plynum].peasant[cont] -= players[plynum].peasant[cont] * amount / 100;
			sprintf(message, "4101%c%d%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, players[plynum].name, MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4102%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL, amount, MSGDEL, peasant_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
		default:
			amount = randomnum(20, 10);
			amount = players[plynum].copper[cont] * amount / 100;
			players[plynum].copper[cont] -= amount;
			count = (int)(amount * .50);
			plyptr->copper[cont] += count;
			sprintf(message, "4103%c%s%c%d%c%s%c", MSGDEL, conts[cont].name, MSGDEL, count, MSGDEL, money_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4104%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL, amount, MSGDEL, money_name, MSGDEL, conts[cont].name, MSGDEL);
			break;
	}
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
	check_ret(players[plynum].cont, plyptr, &players[plynum]);
	save_player(plynum);
}
		
int fighter(player *plyptr)
{
	if(!plyptr)
		return(0);
	if(plyptr->classnum < 10)
		return(1);
	else
		return(0);
}

int find_max(char *itemname, int cprice, int cqty, player *plyptr)
{
	int citynum;
	int price;
	int total = 0;

	citynum = plyptr->city;
	price = cprice + tax(1, cprice, cities[citynum].tax);
	if(price > 0)
		total = plyptr->copper[plyptr->cont] / price;
	if(total > cqty)
		total = cqty;
	sprintf(message, "4105%c%s%c%d%c%s%c%s%c", MSGDEL, cities[plyptr->city].name, MSGDEL, total, MSGDEL, itemname, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	return(total);
}

void formation(char *formstr, char *typestr, player *plyptr)
{
	int count;
	int formnum;
	int ranknum;
	int troopnum;
	int troops[MAXTRP];
	int type = ARMY_DEFENSE;

	if(allow_formation == 0 || ((tolower(typestr[0])) != 'c' && (tolower(typestr[0])) != 'd'))
	{
		help(plyptr->name, "formation", "", "");
		return;
	}
	if((tolower(typestr[0])) == 'c')
		type = ARMY_CAMPAIGN;
	if((strlen(formstr)) < 1)
	{
		show_formation(plyptr->name, plyptr->cont, type, plyptr);
		return;
	}
	if(plyptr->movepts < FORMATION)
	{
		sprintf(message, "1%c%d%cto change your formation%c", MSGDEL, FORMATION, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(count = 0; count < MAXTRP; ++count)
		troops[count] = -1;
	for(ranknum = 0; ranknum < MAXTRP; ++ranknum)
		for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
			plyptr->army[type][plyptr->cont].formation[ranknum][troopnum] = -1;
	for(count = 0, ranknum = -1, troopnum = 0; count < (int)strlen(formstr); ++count)
	{
		if(formstr[count] == '[')
		{
			if(ranknum < MAXTRP)
				++ranknum;
			troopnum = 0;
			continue;
		}
		if(formstr[count] == ']' || !isdigit(formstr[count]) || ranknum < 0 || troopnum >= MAXTRP)
			continue;
		sscanf(&formstr[count], "%d", &formnum);
		while(isdigit(formstr[++count])); 
		--formnum;
		if(formnum < 0 || formnum >= MAXTRP || formnum == CATAPULTNUM || (units[plyptr->troop_type][formnum].attack < 1 && units[plyptr->troop_type][formnum].defend < 1))
			continue;
		if(troops[formnum] < 0)
		{
			troops[formnum] = formnum;
			plyptr->army[type][plyptr->cont].formation[ranknum][troopnum++] = formnum;
			if(formnum == SOLDIERNUM)
			{
				troops[CATAPULTNUM] = CATAPULTNUM;
				plyptr->army[type][plyptr->cont].formation[ranknum][troopnum++] = CATAPULTNUM;
			}
		}
	}
	for(count = 0; count < MAXTRP; ++count)
		if(troops[count] < 0 && (units[plyptr->troop_type][count].attack > 0 || units[plyptr->troop_type][count].defend > 0))
		{
			for(formnum = 0, ranknum = 0; formnum == 0 && ranknum < MAXTRP; ++ranknum)
				for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
					if(plyptr->army[type][plyptr->cont].formation[ranknum][troopnum] < 0)
					{
						plyptr->army[type][plyptr->cont].formation[ranknum][troopnum] = count;
						++formnum;
						break;
					}
		}
	plyptr->movepts -= FORMATION;
	show_formation(plyptr->name, plyptr->cont, type, plyptr);
}

void game(char *name)
{
	char str[16] = {0};

	sprintf(message, "4106%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4107%c%-30s%c%s%c", MSGDEL, select_classname(BARBARIAN), MSGDEL, allow_barb == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4107%c%-30s%c%s%c", MSGDEL, select_classname(CLERIC), MSGDEL, allow_cleric == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4107%c%-30s%c%s%c", MSGDEL, select_classname(MAGE), MSGDEL, allow_mage == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4107%c%-30s%c%s%c", MSGDEL, select_classname(RANGER), MSGDEL, allow_ranger == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4107%c%-30s%c%s%c", MSGDEL, select_classname(VAMPIRE), MSGDEL, allow_vampire == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4108%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Associate Races/Conts.", MSGDEL, associate_races == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Advanced Armies", MSGDEL, advanced_armies == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Advanced Combat", MSGDEL, advanced_combat == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Allow Formation", MSGDEL, allow_formation == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Allow Surrender", MSGDEL, allow_surrender == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	if(attack_time > 0)
		sprintf(str, "%d minute%s", attack_time / 60, attack_time / 60 > 1?"s":"");
	else
		strcpy(str, "N/A");
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Attack Delay", MSGDEL, str, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Attack Reprieve", MSGDEL, attack_reprieve == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Auto AlterReality", MSGDEL, auto_ar >= 0?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Capture Foreign Structures", MSGDEL, capture_foreign == 0?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%c", MSGDEL, "Cities", MSGDEL, num_cities, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%c", MSGDEL, "Continents", MSGDEL, num_conts, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%%%c", MSGDEL, "Default Surrender", MSGDEL, (int)(dft_surrender * 100), MSGDEL);
	send_message(message, name, 0);
	if(end_date > 0)
		sprintf(str, "%d", end_date);
	else
		strcpy(str, "N/A");
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "End Date", MSGDEL, str, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Enforce Peace", MSGDEL, enforce_peace == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Fog of War", MSGDEL, fog_of_war == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Items Disappear at NYHK", MSGDEL, remove_items == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%c", MSGDEL, "Maximum Players", MSGDEL, max_player, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%c", MSGDEL, "MPs per 15 minutes", MSGDEL, mps_15, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Multiple Vampires", MSGDEL, mult_vampires == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Promote Knights at NYHK", MSGDEL, promote_knights == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Protection on Promotion", MSGDEL, promote_protection == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Protection on Sail", MSGDEL, sail_protection == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s (%d max)%c", MSGDEL, "Quest Characters", MSGDEL, quest_chars >= 1?"Yes":"No", quest_chars, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Quest Riddle", MSGDEL, quest_riddle == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%d%c", MSGDEL, "Remorts to Win", MSGDEL, num_remorts, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Restrict Combat", MSGDEL, restrict_combat == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4109%c%-30s%c%s%c", MSGDEL, "Restrict Markets", MSGDEL, restrict_markets == 1?"Yes":"No", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void gender(char *arg1, player *plyptr)
{
	if((tolower(arg1[0])) == 'm')
		plyptr->gender = 0;
	else if((tolower(arg1[0])) == 'f')
		plyptr->gender = 1;
	sprintf(message, "4110%c%s%c", MSGDEL, plyptr->gender == 0?"Male":"Female", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void give(char *itemname, int amount, int *gitem, int *item, int type, player *allyptr, player *plyptr)
{
	if(plyptr->ally < 1)
	{
		sprintf(message, "4019%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(allyptr != &players[plyptr->ally])
	{
		sprintf(message, "4009%c%s%c", MSGDEL, players[plyptr->ally].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < GIVE)
	{
		sprintf(message, "1%c%d%cto transfer items to your ally%c", MSGDEL, GIVE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount < 1)
	{
		sprintf(message, "37%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(*gitem < amount)
	{
		sprintf(message, "4024%c%d%c%s%cyour ally%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, allyptr))
		return;
	if(plyptr->city != allyptr->city)
	{
		sprintf(message, "4021%c%s%c%s%c%s%c", MSGDEL, allyptr->name, MSGDEL, allyptr->city == secret_city?"??????":cities[allyptr->city].name, MSGDEL, conts[allyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type < 8 || type > 10)
	{
		sprintf(message, "4025%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(type == 10)
	{
		if(plyptr->peasant[plyptr->cont] - amount < promotions[BASE_CLASSNUM(plyptr->classnum)][plyptr->level[plyptr->cont]][2])
		{
			sprintf(message, "4026%c%s%c", MSGDEL, peasant_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if(allyptr->peasant[allyptr->cont] + amount > promotions[BASE_CLASSNUM(allyptr->classnum)][allyptr->level[allyptr->cont] + 1][2])
		{
			sprintf(message, "4027%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
	}
	plyptr->movepts -= GIVE;
	*gitem -= amount;
	*item += amount;
	sprintf(message, "4028%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, allyptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4029%c%s%c%d%c%s%c", MSGDEL, plyptr->name, MSGDEL, amount, MSGDEL, itemname, MSGDEL);
	send_message(message, allyptr->name, 1);
	log_event("", message, allyptr->name);
	save_player(playernum(allyptr));
}

void information(char *name)
{
	char str[16] = {0};
	double diff;
	int count;
	int days = 0;
	int hk_hours;
	int hours = 0;
	int min = 24;
	int mins = 0;
	int secs = 0;
	int timediff;
	int total[MAXCLS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int totalp = 0;
	struct tm *pnt;
	time_t current;

	time(&current);
	pnt = localtime(&current);
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4111%c%s%s%c", MSGDEL, VER, auto_ar >= 0?" (AR)":"", MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4362%ccopyright 1993 by James D. Bennett%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4363%c%s%c", MSGDEL, BUILD, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4364%c%s%c", MSGDEL, NOWSTR, MSGDEL);
	send_message(message, name, 0);
	if(emperor(name))
	{
		if(auto_ar >= 0)
		{
			sprintf(message, "4365%c%d%c", MSGDEL, auto_ar, MSGDEL);
			send_message(message, name, 1);
		}
		if(conq_info.kraken.alive == 1)
		{
			sprintf(message, "4366%c%d%c%d%c", MSGDEL, conq_info.kraken.health, MSGDEL, conq_info.kraken.maxhealth, MSGDEL);
			send_message(message, name, 1);
		}
	}
	diff = difftime(current, uptime);
	if(diff >= 86400)
	{
		days += (int)(diff / 86400);
		diff -= days * 86400;
	} 
	if(diff >= 3600)
	{
		hours += (int)(diff / 3600);
		diff -= hours * 3600;
	}
	if(diff >= 60)
	{
		mins += (int)(diff / 60);
		diff -= mins * 60;
	}
	secs = (int)diff;
	sprintf(message, "4367%c%d%c%d%c%d%c%d%c", MSGDEL, days, MSGDEL, hours, MSGDEL, mins, MSGDEL, secs, MSGDEL);
	send_message(message, name, 0);
	hours = pnt->tm_hour;
	mins = pnt->tm_min;
	for(count = 0; count < MAXHKS; ++count)
	{
		if(normal_hks[count] >= 0)
		{
			if(normal_hks[count] == 0)
				hk_hours = 24;
			else
				hk_hours = normal_hks[count];
			if(hk_hours > hours)
				timediff = hk_hours - hours;
			else
				timediff = 24 - hours + hk_hours;
			if(timediff > 0 && timediff < min)
				min = timediff;
		}
		if(ny_hks[count] >= 0)
		{
			if(ny_hks[count] == 0)
				hk_hours = 24;
			else
				hk_hours = ny_hks[count];
			if(hk_hours > hours)
				timediff = hk_hours - hours;
			else
				timediff = 24 - hours + hk_hours;
			if(timediff > 0 && timediff < min)
				min = timediff;
		}
	}
	hours = min - 1;
	mins = 60 - mins;
	if(mins == 60)
	{
		mins = 0;
		++hours;
	}
	sprintf(message, "4368%c%d%c%d%c", MSGDEL, hours, MSGDEL, mins, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4369%c%s%c%s%c", MSGDEL, conq_info.hktime, MSGDEL, conq_info.hkby, MSGDEL);
	send_message(message, name, 0);
	if(mps_15 > 0)
	{
		mins = ((pnt->tm_min / 15) + 1) * 15;
		mins -= pnt->tm_min + 1;
		secs = 60 - pnt->tm_sec;
		if(secs == 60)
		{
			++mins;
			secs = 0;
		}
		sprintf(message, "4370%c%d%c%d%c", MSGDEL, mins, MSGDEL, secs, MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "4371%c%d%c%s%c", MSGDEL, conq_info.today, MSGDEL, age, MSGDEL);
	send_message(message, name, 0);
	if(end_date > 0)
	{
		sprintf(message, "4372%c%d%c", MSGDEL, end_date, MSGDEL);
		send_message(message, name, 0);
	}
	for(count = 0; count < 70; ++count)
	{
		if(count % 10 == 0)
			total[count / 10] = 0;
		total[count / 10] += totply[count];
		totalp += totply[count];
	} 		
	sprintf(message, "4373%c%d%c", MSGDEL, totalp, MSGDEL);
	send_message(message, name, 0);
	strcpy(message, "");
	for(count = 0; count <= TITAN; ++count)
	{
		sprintf(str, "%ss", select_classname(count));
		sprintf(message, "4374%c%-16s%c%d%c", MSGDEL, str, MSGDEL, total[count], MSGDEL);
		send_message(message, name, 0);
	}
	for(count = 0; count < MAXCNT; ++count)
		total[count] = 0;
	for(count = 1; count < current_player; ++count)
	{
		if(strlen(players[count].name) < 1 || players[count].level[players[count].cont] < 0)
			continue;
		++total[players[count].cont];
	}
	for(count = 0; count < num_conts; ++count)
	{
		sprintf(message, "4375%c%-16s%c%d%c", MSGDEL, conts[count].name, MSGDEL, total[count], MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void kia(char *name)
{
	int count;

	sprintf(message, "4283%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4284%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4285%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	for(count = 0; count < 10; ++count)
	{
		sprintf(message, "%d%c%-16s%c%s%c", 4286 + count, MSGDEL, conq_info.killed[count], MSGDEL, conq_info.killer[count], MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "4296%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void kingdomname(char *name, player *plyptr)
{
	if(check_word(plyptr->name, name, 31, 1))
	{
		memset(plyptr->kname[plyptr->cont], 0, 32);
		strncpy(plyptr->kname[plyptr->cont], name, 31);
		sprintf(message, "4114%c%s%c", MSGDEL, name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void kings(char *name)
{
	char input[128];
	FILE *fp;

	sprintf(message, "4115%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	if((fp = fopen(KNGFIL, "r")) != NULL)
	{
		while((fgets(input, 128, fp)) != NULL)
		{
			strip(input);
			sprintf(message, "4116%c%s%c", MSGDEL, input, MSGDEL);
			send_message(message, name, 0);
		}
		fclose(fp);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void levels(char *classname, player *plyptr)
{
	int classnum;
	int count;

	if((classnum = find_class(classname)) < 0)
		classnum = BASE_CLASSNUM(plyptr->classnum);
	if((strlen(basenames[classnum])) < 1 || classnum == TITAN)
		classnum = FIGHTER;
	sprintf(message, "4117%c%s%c", MSGDEL, basenames[classnum], MSGDEL);
	send_message(message, plyptr->name, 0);
	for(count = 0; count <= maxlevel[classnum]; ++count)
	{
		switch(classnum)
		{
			case FIGHTER:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, f_titles[0][count], MSGDEL, f_titles[1][count], MSGDEL);
				break;
			case BARBARIAN:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, b_titles[0][count], MSGDEL, b_titles[1][count], MSGDEL);
				break;
			case MAGE:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, m_titles[0][count], MSGDEL, m_titles[1][count], MSGDEL);
				break;
			case VAMPIRE:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, v_titles[0][count], MSGDEL, v_titles[1][count], MSGDEL);
				break;
			case CLERIC:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, c_titles[0][count], MSGDEL, c_titles[1][count], MSGDEL);
				break;
			case RANGER:
				sprintf(message, "4118%c%02d%c%s%c%s%c", MSGDEL, count + 1, MSGDEL, r_titles[0][count], MSGDEL, r_titles[1][count], MSGDEL);
				break;
		}
		send_message(message, plyptr->name, 0);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 0);
}

void list(char *name)
{
	int count;

	sprintf(message, "4297%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4298%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4299%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	for(count = 0; count < 15; ++count)
	{
		sprintf(message, "%d%c%-16s%c%s%c", 4300 + count, MSGDEL, joustlosers[count], MSGDEL, joustwinners[count], MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "4315%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4316%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void map(char *contname, player *plyptr)
{
	int cont;
	int count;
	int distance;

	if((cont = find_cont(contname)) < 0)
		cont = plyptr->cont;
	sprintf(message, "4121%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4122%c", MSGDEL);
	for(count = 0; count < conts[cont].totalcty; ++count)
	{
		if(conts[cont].cities[count] == secret_city && !titan(plyptr))
			continue;
		if(plyptr ->cont == cont && plyptr->city != conts[cont].cities[count])
			distance = find_distance(plyptr->city, conts[cont].cities[count], plyptr) + (plyptr->level[plyptr->cont] + 1);
		else
			distance = 0;
		sprintf(message, "%s%s%s(%d) ", message, cities[conts[cont].cities[count]].name, cities[conts[cont].cities[count]].lock == 1?"*":"", distance);
	}
	sprintf(message, "%s%c", message, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4123%c%s%c", MSGDEL, cities[conts[cont].port].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4124%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void market(int ctynum, player *plyptr)
{
	char img[MAXCNT][MAXMRK][32] = 
	{
		{" |~             |~ ",
		"/ \\            / \\ ",
		"XXX   |~   |~  XXX ",
		"[ ]  []_[]_[]  [ ] ",
		"[#]   [V^v^]   [#] ",
		"[ [___I_^_vI___] ] ",
		"[      ^__      ^] ", ""},
		{" p                 ",
		"/\\   p  p          ",
		"TT  /\\ /\\   p      ",
		"||  TT/p \\ /\\ p    ",
		"|| p|//\\  \\||/\\    ",
		"||/\\/ TT   \\|TT    ",
		"||TT _||_____||    ",
		"| | T |       |p   "},
		{"         |~        ",
		"         |    |~   ",
		"      [===]   |    ",
		"  |~   |==| [==] |~",
		"  |    |==| |=|  | ",
		"[^^] [^^]_[^^]|[^^]",
		" | |/ |~    |~\\| | ",
		" | ^^^|^^^^^|^^^ | "},
		{"         |~        ",
		"         |    |~   ",
		"      [===]   |    ",
		"  |~   |==| [==] |~",
		"  |    |==| |=|  | ",
		"[^^] [^^]_[^^]|[^^]",
		" | |/ |~    |~\\| | ",
		" | ^^^|^^^^^|^^^ | "}
	};
	char lock1[MAXCNT][2][8] =
	{
		{" (  ) ", "||||||"},
		{"", ""},
		{"", ""},
		{"", ""}
	};
	char lock2[MAXCNT][2][8] =
	{
		{" |  | ", "||||||"},
		{"", ""},
		{"", ""},
		{"", ""}
	};
	char lock3[MAXCNT][2][8] =
	{
		{" |__| ", "||||||"},
		{" [] ", "-XX-"},
		{"   ", "|||"},
		{"   ", "|||"}
	};
	char lock4[MAXCNT][2][8] =
	{
		{"/____\\", "||||||"},
		{" [] ", "-XX-"},
		{"   ", "|||"},
		{"   ", "|||"}
	};
	char str1[16] = {0};
	char str2[16] = {0};
	char str3[16] = {0};
	int count;

	if(plyptr->city != ctynum && !check_hero(HERO_MERCHANTKING, plyptr) && plyptr->movepts < MARKET)
	{
		sprintf(message, "1%c%d%cto check a remote market%c", MSGDEL, MARKET, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != ctynum && !check_hero(HERO_MERCHANTKING, plyptr))
		plyptr->movepts -= MARKET;
	sprintf(message, "4125%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	closed(str1, cities[ctynum].food_qty);
	closed(str2, cities[ctynum].food_pr);
	sprintf(message, "4126%c%-15s%c%-16s%c%-10s%c%s%c%d%c", MSGDEL, cities[ctynum].name, MSGDEL, food_name, MSGDEL, str1, MSGDEL, str2, MSGDEL, cities[ctynum].food_buy, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXMRK; ++count)
	{
		closed(str1, cities[ctynum].troop_qty[count]);
		closed(str2, cities[ctynum].troop_pr[count]);
		if(restrict_markets == 0)
			cities[ctynum].troop_type[count] = plyptr->troop_type;
		if((strlen(img[cities[ctynum].cont][count])) < 1)
			switch(cities[ctynum].cont)
			{
				case 0:
					sprintf(img[cities[ctynum].cont][count], "[  ^  %s  ^  ] ", lock1[cities[ctynum].cont][cities[ctynum].lock]);
					break;
			}
		sprintf(message, "4127%c%s%c%-16s%c%-10s%c%-10s%c", MSGDEL, 
		img[cities[ctynum].cont][count], MSGDEL, cities[ctynum].troop_pr[count] > 0?units[cities[ctynum].troop_type[count]][cities[ctynum].troop_num[count]].name:"[-closed-]", MSGDEL, str1, MSGDEL, str2, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	closed(str1, cities[ctynum].spy_qty);
	closed(str2, cities[ctynum].spy_pr);
	switch(cities[ctynum].cont)
	{
		case 0:
			sprintf(message, "4127%c[     %s   ^ ] %cSpy             %c%-10s%c%-10s%c", MSGDEL, lock2[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 1:
			sprintf(message, "4127%c|  \\  |       /\\   %cSpy             %c%-10s%c%-10s%c", MSGDEL, MSGDEL, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 2:
			sprintf(message, "4127%c |  [^^]___[^^]  | %cSpy             %c%-10s%c%-10s%c", MSGDEL, MSGDEL, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		default:
			sprintf(message, "4127%c |  [^^]___[^^]  | %cSpy             %c%-10s%c%-10s%c", MSGDEL, MSGDEL, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	closed(str1, cities[ctynum].peasant_qty);
	closed(str2, cities[ctynum].peasant_pr);
	if(cities[ctynum].peasant_pr > 0)
		strcpy(str3, peasant_name);
	else
		strcpy(str3, "[-closed-]");
	switch(cities[ctynum].cont)
	{
		case 0:
			sprintf(message, "4127%c[ _^_^%s^_ _^] %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock3[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 1:
			sprintf(message, "4127%c|     \\   %s T   %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock3[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 2:
			sprintf(message, "4127%c |   | |%s| |   | %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock3[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		default:
			sprintf(message, "4127%c |   | |%s| |   | %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock3[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	closed(str1, cities[ctynum].movepts_qty);
	closed(str2, cities[ctynum].movepts_pr);
	if(cities[ctynum].movepts_pr > 0)
		strcpy(str3, "Move");
	else
		strcpy(str3, "[-closed-]");
	switch(cities[ctynum].cont)
	{
		case 0:
			sprintf(message, "4127%c,,.,,,%s,,,.,, %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock4[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 1:
			sprintf(message, "4127%c|         %s |   %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock4[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		case 2:
			sprintf(message, "4127%c |   | |%s| |   | %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock4[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
		default:
			sprintf(message, "4127%c |   | |%s| |   | %c%-16s%c%-10s%c%-10s%c", MSGDEL, lock4[cities[ctynum].cont][cities[ctynum].lock], MSGDEL, str3, MSGDEL, str1, MSGDEL, str2, MSGDEL);
			break;
	}
	send_message(message, plyptr->name, 1);
	sprintf(message, "4128%c%-9s%c%-11s%c%-10d%c%d%c", MSGDEL, players[cities[ctynum].owner].name, MSGDEL, money_name, MSGDEL, cities[ctynum].copper, MSGDEL, cities[ctynum].tax, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->city != ctynum && restrict_markets == 1)
		show_hire(ctynum, plyptr);
}

void mystic(player *plyptr)
{
	int artnum;
	int hero;

	if(plyptr->movepts < MYSTIC)
	{
		sprintf(message, "1%c%d%cto seek out the mystic%c", MSGDEL, MYSTIC, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= MYSTIC;
	if(randomnum(100, 1) <= 85)
	{
		sprintf(message, "4150%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		read_legend(plyptr->name);
		return;
	}
	sprintf(message, "4149%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(mage(plyptr) && plyptr->city == magic_city && randomnum(100, 1) <= 20) 
	{
		spell_clue(plyptr);
		return;
	}
	hero = randomnum(TOTHRO - 1, 0);
	if(heroes[hero].alive == 1 && randomnum(100, 1) <= 10)
	{
		sprintf(message, "4153%c%s%c%s%c", MSGDEL, heroes[hero].name, MSGDEL, heroes[hero].type, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((artnum = find_randomartifact(plyptr->cont)) < 0)
	{
		read_legend(plyptr->name);
		return;
	}
	if(artifacts[artnum].city == plyptr->city && randomnum(100, 1) <= 15)
	{
		if(artnum == WINGS || artnum == BOOTS)
			sprintf(message, "4151%c%s%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL, artifacts[artnum].location, MSGDEL);
		else
			sprintf(message, "4152%c%s%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL, artifacts[artnum].location, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(artnum == WINGS || artnum == BOOTS)
		sprintf(message, "4154%c%s%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL, artifacts[artnum].city == secret_city?"??????":cities[artifacts[artnum].city].name, MSGDEL);
	else
		sprintf(message, "4155%c%s%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL, artifacts[artnum].city == secret_city?"??????":cities[artifacts[artnum].city].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void newplayer(char *name, char *host, char *lord)
{
	char notallowed[][16] = 
	{
		"emperor", "horde", "list", "conquest", ""
	};
	int class = -1;
	int cont = -1;
	int count = 0;
	int lrd = -1;
	int plynum;
	player *plyptr;

	if((find_player(name)) > 0)
	{
		sprintf(message, "4156%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	if(current_player >= max_player - 1 && find_slot() == current_player)
	{
		sprintf(message, "4157%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	if((find_host(host)) != 0)
	{
		sprintf(message, "4158%c%s%c", MSGDEL, host, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	while((strlen(notallowed[count])) > 0)
		if((STRNCASECMP(name, notallowed[count], strlen(notallowed[count++]))) == 0)
		{
			sprintf(message, "4159%c%c", MSGDEL, MSGDEL);
			send_message(message, name, 0);
			return;
		}
	for(count = 0; count < (int)strlen(name); ++count)
	{
		if(!isprint(name[count]) || isspace(name[count]))
		{
			sprintf(message, "4160%c%c", MSGDEL, MSGDEL);
			send_message(message, name, 0);
			return;
		}
	}
	if(strlen(lord))
	{
		if((plynum = find_player(lord)) > 0)
		{
			plyptr = &players[plynum];
			if(plyptr->levelmax < MINVASRANK)
			{
				sprintf(message, "4161%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, select_title(plyptr->classnum, MINVASRANK, plyptr->gender), MSGDEL);
				send_message(message, name, 0);
				return;
			}
			if(plyptr->vassals.total == MAXVAS)
			{
				sprintf(message, "4162%c%s%c%d%c", MSGDEL, plyptr->name, MSGDEL, MAXVAS, MSGDEL);
				send_message(message, name, 0);
				return;
			}
			if(!vampire(plyptr) && !titan(plyptr))
			{
				class = plyptr->classnum;
				cont = plyptr->cont;
				lrd = plynum;
			}
		}
		else
		{
			sprintf(message, "4163%c%c", MSGDEL, MSGDEL);
			send_message(message, name, 0);
			for(count = 1; count < current_player; ++count)
				if(players[count].levelmax >= MINVASRANK && !titan(&players[count]) && !vampire(&players[count]))
				{
					sprintf(message, "4164%c%s%c%s%c%s%c%s%c", MSGDEL, 
					select_class(players[count].classnum), MSGDEL, select_title(players[count].classnum, players[count].levelmax, players[count].gender), MSGDEL, players[count].nick, MSGDEL, conts[players[count].cont].name, MSGDEL);
					send_message(message, name, 0);
				}
			return;
		}
	}
	if(check_word(name, name, 31, 1))
	{
		plynum = add_player(name, host, class, cont, lrd);
		dcc_chat(host, plynum);
		sprintf(message, "4165%c%s%c%s%c", MSGDEL, select_title(players[plynum].classnum, 0, players[plynum].gender), MSGDEL, name, MSGDEL);
		send_message(message, name, 1);
		sprintf(message, "4166%c%s%c%s%c%s%c%s%c", MSGDEL, select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, 0, players[plynum].gender), MSGDEL, name, MSGDEL, conts[players[plynum].cont].name, MSGDEL);
		send_channel(message, 0);
		sprintf(message, "14008%c%s%c", MSGDEL, players[plynum].password, MSGDEL);
		send_message(message, name, 1);
		sprintf(message, "4167%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 1);
		sprintf(message, "4168%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 1);
		sprintf(message, "4169%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 1);
	}
}

void password(char *newpassword, player *plyptr)
{
	if(strlen(newpassword) < 1)
	{
		sprintf(message, "14008%c%s%c", MSGDEL, plyptr->password, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strstr(newpassword, "None"))
	{
		sprintf(message, "4170%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(!check_word(plyptr->name, newpassword, 16, 8))
		return;
	if(!check_password(plyptr->name, newpassword))
		return;
	memset(plyptr->password, 0, 32);
	strncpy(plyptr->password, newpassword, 31);
	sprintf(message, "14008%c%s%c", MSGDEL, newpassword, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void person(char *contname, player *plyptr, player *whoptr)
{
	char city[32] = {0};
	char desc[][16] =
	{
		"Generous", "Kindly", "Normal", "Oppressive", "Tyrannical"
	}; 
	char lord[32] = {0};
	char spyon[32] = {0};
	char str[32] = {0};
	char str2[32] = {0};
	int artnum;
	int cont;
	int count;
	int heronum;
	int total;

	if((cont = find_cont(contname)) < 0 || whoptr->level[cont] < 0)
		cont = whoptr->cont;
	if(plyptr == whoptr && cont != whoptr->cont)
	{
		sprintf(message, "4171%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, whoptr->name, 1);
	}
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "4172%c%c%02d%s%c%c", MSGDEL, CNTLC, GREEN, whoptr->kname[cont], CNTLC, MSGDEL);
	else
		sprintf(message, "4172%c%s%c", MSGDEL, whoptr->kname[cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "4173%c%-8s%c%c%02d%s%c%s%c%s%c%c", MSGDEL, 
		symbols[0][whoptr->classnum], MSGDEL, CNTLC, PURPLE, select_class(whoptr->classnum), MSGDEL, select_title(whoptr->classnum, whoptr->level[cont], whoptr->gender), MSGDEL, whoptr->name, CNTLC, MSGDEL);
	else
		sprintf(message, "4173%c%-8s%c%s%c%s%c%s%c", MSGDEL, symbols[0][whoptr->classnum], MSGDEL, select_class(whoptr->classnum), MSGDEL, select_title(whoptr->classnum, whoptr->level[cont], whoptr->gender), MSGDEL, whoptr->name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->special_misc & SPC_COLOR)
	{
		for(count = 0, total = 0; count < MAXTRP; ++count)
			total += units[whoptr->troop_type][count].copper * (whoptr->army[0][cont].total[count] + whoptr->army[1][cont].total[count]);
		sprintf(str, "%c%02d%d%c Movept(s)", CNTLC, whoptr->movepts < 5?RED:GREEN, whoptr->movepts, CNTLC);
		sprintf(str2, "%c%02d%d%c %s(s)", CNTLC, whoptr->castle[cont] < 2?RED:GREEN, whoptr->castle[cont], CNTLC, structures[BASE_CLASSNUM(whoptr->classnum)].name);
		sprintf(message, "4174%c%-8s%c%-16s%c%-16s%c%c%02d%d%c %s%c", 
		MSGDEL, symbols[1][whoptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, CNTLC, whoptr->copper[cont] < total?RED:ORANGE, whoptr->copper[cont], CNTLC, money_name, MSGDEL);
	}
	else
	{
		sprintf(str, "%d Movept(s)", whoptr->movepts);
		sprintf(str2, "%d %s(s)", whoptr->castle[cont], structures[BASE_CLASSNUM(whoptr->classnum)].name);
		sprintf(message, "4174%c%-8s%c%-16s%c%-16s%c%d %s%c", MSGDEL, symbols[1][whoptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, whoptr->copper[cont], money_name, MSGDEL);
	}
	send_message(message, plyptr->name, 1);
	if(plyptr->special_misc & SPC_COLOR)
	{
		for(count = 0, total = 0; count < MAXTRP; ++count)
			total += units[whoptr->troop_type][count].food * (whoptr->army[0][cont].total[count] + whoptr->army[1][cont].total[count]);
		sprintf(str, "%c%02d%d%c Land", CNTLC, whoptr->land[cont] < promotions[BASE_CLASSNUM(whoptr->classnum)][whoptr->level[cont]][1]?RED:GREEN, whoptr->land[cont], CNTLC);
		sprintf(str2, "%c%02d%d%c %s(s)", CNTLC, whoptr->peasant[cont] < promotions[BASE_CLASSNUM(whoptr->classnum)][whoptr->level[cont]][2]?RED:GREEN, whoptr->peasant[cont], CNTLC, peasant_name);
		sprintf(message, "4175%c%-8s%c%-16s%c%-16s%c%c%02d%d%c %s%c", 
		MSGDEL, symbols[2][whoptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, CNTLC, whoptr->food[cont] < total?RED:GREEN, whoptr->food[cont], CNTLC, food_name, MSGDEL);
	}
	else
	{
		sprintf(str, "%d Land", whoptr->land[cont]);
		sprintf(str2, "%d %s(s)", whoptr->peasant[cont], peasant_name);
		sprintf(message, "4175%c%-8s%c%-16s%c%-16s%c%d %s%c", 
		MSGDEL, symbols[2][whoptr->classnum], MSGDEL, str, MSGDEL, str2, MSGDEL, whoptr->food[cont], food_name, MSGDEL);
	}
	send_message(message, plyptr->name, 1);
	if(whoptr->lord == 0 || conq_info.vampire == whoptr->lord)
		strcpy(lord, "???");
	else
		strcpy(lord, players[whoptr->lord].name);
	if(plyptr == whoptr)
	{
		if(plyptr->special_misc & SPC_COLOR)
			sprintf(message, "4176%c%-8s%c%c%02d%s%c%c%c%02d%s%c%c", 
			MSGDEL, symbols[3][whoptr->classnum], MSGDEL, CNTLC, LTGREY, lord, 
			CNTLC, MSGDEL, CNTLC, LTGREY, whoptr->lord == 0 || players[whoptr->lord].level[cont] < 0?"???":players[whoptr->lord].kname[cont], CNTLC, MSGDEL);
		else
			sprintf(message, "4176%c%-8s%c%s%c%s%c", MSGDEL, symbols[3][whoptr->classnum], MSGDEL, lord, MSGDEL, whoptr->lord == 0 || players[whoptr->lord].level[cont] < 0?"???":players[whoptr->lord].kname[cont], MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
	{
		if(plyptr->special_misc & SPC_COLOR)
			sprintf(message, "4188%c%-8s%c%c%02d%s%c%c%c%02d%s%c%c%c%02d%s%c%c", 
			MSGDEL, symbols[3][whoptr->classnum], MSGDEL, CNTLC, PURPLE, whoptr->name, CNTLC, MSGDEL, CNTLC, LTGREY, lord, 
			CNTLC, MSGDEL, CNTLC, LTGREY, whoptr->lord == 0 || players[whoptr->cont].level[cont] < 0?"???":players[whoptr->lord].kname[cont], CNTLC, MSGDEL);
		else
			sprintf(message, "4188%c%-8s%c%s%c%s%c%s%c", MSGDEL, symbols[3][whoptr->classnum], MSGDEL, whoptr->name, MSGDEL, lord, MSGDEL, whoptr->lord == 0 || players[whoptr->lord].level[cont] < 0?"???":players[whoptr->lord].kname[cont], MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	artnum = find_artifact(whoptr);
	heronum = find_hero(whoptr);
	if(whoptr->ally > 0 && players[whoptr->ally].ally != playernum(whoptr))
		whoptr->ally = 0;
	if(whoptr->cont == cont)
		strcpy(city, cities[whoptr->city].name);
	else
		strcpy(city, cities[conts[cont].port].name);
	if(whoptr->city == secret_city && plyptr != whoptr)
		strcpy(city, "??????");
	if(whoptr->spy[cont] >= 0)
	{
		if(players[whoptr->spy[cont]].level[cont] < 0)
			whoptr->spy[cont] = 0;
		if(whoptr->spy[cont] == 0 || vampire(&players[whoptr->spy[cont]]))
			strcpy(spyon, "???");
		else
			strcpy(spyon, players[whoptr->spy[cont]].name);
	}
	if(players[whoptr->ninja].level[players[whoptr->ninja].cont] < 0)
		whoptr->ninja = 0;
	if(plyptr->special_misc & SPC_COLOR)
	{
		sprintf(message, "59%c%c%02d%s%c%c%c%02d%s%c%c", MSGDEL, CNTLC, GREEN, city, CNTLC, MSGDEL, CNTLC, BROWN, conts[cont].name, CNTLC, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "60%c%c%02d%-13s%c%c%c%02d%-11s%c%c%c%02d%s%c%c", MSGDEL, 
		CNTLC, GREEN, whoptr->remorts > 0?elitenames[whoptr->remorts]:"None", CNTLC, MSGDEL, 
		CNTLC, whoptr->isprotected == 0?GREEN:RED, whoptr->isprotected == 0?"Yes":"No", CNTLC, MSGDEL, 
		CNTLC, structures[BASE_CLASSNUM(whoptr->classnum)].tax == 0?GREEN:
		(plyptr->tax[cont] <= 2?GREEN:RED), structures[BASE_CLASSNUM(whoptr->classnum)].tax == 0?"N/A":desc[whoptr->tax[cont]], CNTLC, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "61%c%c%02d%-15s%c%c%c%02d%-16s%c%c%c%02d%s%c%c", 
		MSGDEL, CNTLC, ORANGE, whoptr->ally > 0?players[whoptr->ally].name:"None", CNTLC, MSGDEL, 
		CNTLC, PURPLE, whoptr->spy[cont] > 0?spyon:"None", CNTLC, MSGDEL, 
		CNTLC, PURPLE, plyptr == whoptr && titan(plyptr)?players[whoptr->ninja].name:"N/A", CNTLC, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "62%c%c%02d%-15s%c%c%c%02d%-16s%c%c%c%02d%d%c%c", 
		MSGDEL, CNTLC, LTGREY, artnum >= 0?artifacts[artnum].name:"None", CNTLC, MSGDEL, 
		CNTLC, TEAL, heronum >= 0?heroes[heronum].type:"None", CNTLC, MSGDEL, 
		CNTLC, BLUE, whoptr->fleet > 0?whoptr->fleet:0, CNTLC, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
	{
		sprintf(message, "59%c%s%c%s%c", MSGDEL, city, MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "60%c%-13s%c%-11s%c%s%c", MSGDEL, 
		whoptr->remorts > 0?elitenames[whoptr->remorts]:"None", MSGDEL, whoptr->isprotected == 0?"Yes":"No", MSGDEL, 
		structures[BASE_CLASSNUM(whoptr->classnum)].tax == 0?"N/A":desc[whoptr->tax[cont]], MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "61%c%-15s%c%-16s%c%s%c", 
		MSGDEL, whoptr->ally > 0?players[whoptr->ally].name:"None", MSGDEL, 
		whoptr->spy[cont] > 0?spyon:"None", MSGDEL, 
		plyptr == whoptr && titan(plyptr)?players[whoptr->ninja].name:"N/A", MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "62%c%-15s%c%-16s%c%d%c", 
		MSGDEL, artnum >= 0?artifacts[artnum].name:"None", MSGDEL, 
		heronum >= 0?heroes[heronum].type:"None", MSGDEL, 
		whoptr->fleet > 0?whoptr->fleet:0, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	if(plyptr != whoptr && whoptr->fleet > 0)
		show_fleet(plyptr->name, "spy", whoptr);
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 0);
}

void petition(int nextlevel, player *plyptr)
{
        int army;
        int land;
        int peasants;

	if(plyptr->level[plyptr->cont] >= maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1)
	{
		sprintf(message, "4200%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	--nextlevel;
	if(nextlevel < 0 || plyptr->level[plyptr->cont] == nextlevel)
	{
		nextlevel = plyptr->level[plyptr->cont] + 1;
		if(nextlevel > maxlevel[BASE_CLASSNUM(plyptr->classnum)])
			nextlevel = maxlevel[BASE_CLASSNUM(plyptr->classnum)];
	}
	if(nextlevel >= maxlevel[BASE_CLASSNUM(plyptr->classnum)])
	{
		sprintf(message, "4201%c%s%c", MSGDEL, select_title(plyptr->classnum, maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->level[plyptr->cont] > nextlevel)
	{
		sprintf(message, "4202%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, select_title(plyptr->classnum, nextlevel, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(promote_knights == 1 && plyptr->level[plyptr->cont] == 0 && nextlevel == 1 && plyptr->first_play[plyptr->cont] == conq_info.today)
	{
		sprintf(message, "4199%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	army = promotions[BASE_CLASSNUM(plyptr->classnum)][nextlevel][0];
	land = promotions[BASE_CLASSNUM(plyptr->classnum)][nextlevel][1];
	peasants = promotions[BASE_CLASSNUM(plyptr->classnum)][nextlevel][2];
	sprintf(message, "4203%c%d%c%d%c%s%c%d%c%s%c", MSGDEL, land, MSGDEL, peasants, MSGDEL, peasant_name, MSGDEL, army, MSGDEL, select_title(plyptr->classnum, nextlevel, plyptr->gender), MSGDEL);
	send_message(message, plyptr->name, 1);
}

void post(char *msg, player *plyptr)
{
	char input[257] = {0};
	FILE *fp;
	struct stat finfo;

	if((strlen(msg)) < 1)
	{
		if((fp = fopen(PSTFIL, "r")) == NULL)
		{
			sprintf(message, "4068%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		if((stat(PSTFIL, &finfo)) == -1)
		{
			fprintf(stderr, "%s Error fstating %s: %s\n", NOWSTR, PSTFIL, strerror(errno));
			fclose(fp);
			return;
		}
		if(finfo.st_size > MAXLINES * 256)
		{
			fseek(fp, 0, SEEK_END);
			fseek(fp, -(int)(MAXLINES * 256), SEEK_CUR);
		}
		sprintf(message, "57%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		while((fgets(input, 257, fp)) != NULL)
		{
			strip(input);
			sprintf(message, "4354%c%s%c", MSGDEL, input, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		sprintf(message, "56%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		fclose(fp);
		return;
	}
	if(plyptr->levelmax < 1)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
        }
	if(!check_word(plyptr->name, msg, 128, 1))
		return;
	if((fp = fopen(PSTFIL, "a")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, PSTFIL, strerror(errno));
		return;
	}
	sprintf(message, "%s %s %s %s: %s", NOWSTR, select_class(plyptr->classnum), select_title(plyptr->classnum, plyptr->levelmax, plyptr->gender), plyptr->nick, msg);
	sprintf(message, "%-255s\n", message);
	fputs(message, fp);
	fclose(fp);
	sprintf(message, "4352%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void rank(char *name)
{
	char bw[8] = {0};
	char killed[16] = {0};
	char lvl[4] = {0};
	char plyname[32] = {0};
	int count;

	sprintf(message, "4204%c%s%c", MSGDEL, age, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4205%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4206%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	for(count = 0; count < MAXRNK; ++count)
	{
		if(players[conq_info.ranks[count]].level[players[conq_info.ranks[count]].cont] < 0)
			conq_info.ranks[count] = 0;
		sprintf(bw, "%d", players[conq_info.ranks[count]].battles_won);
		sprintf(lvl, "%d", players[conq_info.ranks[count]].levelmax + 1);
		sprintf(killed, "%d", players[conq_info.ranks[count]].troops_killed);
		if(vampire(&players[conq_info.ranks[count]]))
			strcpy(plyname, select_classname(VAMPIRE));
		else
			strcpy(plyname, players[conq_info.ranks[count]].name);
		sprintf(message, "4207%c%-23s%c%-13s%c%-7s%c%-7s%c%d%c", MSGDEL, plyname, MSGDEL, lvl, MSGDEL, bw, MSGDEL, killed, MSGDEL, players[conq_info.ranks[count]].remorts, MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void read_log(char *name)
{
	char fname[64] = {0};
	loginfo plog;
	int fp;

	memset(&plog, 0, LOGSIZ);
	sprintf(fname, "logs/%s", name);
	if((fp = open(fname, O_RDONLY)) == -1)
	{
		sprintf(message, "4068%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 1);
		return;
	}
	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 1);
	while((read(fp, &plog, LOGSIZ)) == LOGSIZ)
	{
		sprintf(message, "4054%c%s%c%s%c", MSGDEL, plog.time, MSGDEL, plog.message, MSGDEL);
		send_message(message, name, 1);
		memset(&plog, 0, LOGSIZ);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 1);
	close(fp);
	remove(fname);
}

void reprieve(char *contname, player *plyptr)
{
	int cont;
	int count;

	if(attack_reprieve == 0)
	{
		sprintf(message, "4330%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < REPRIEVE)
	{
		sprintf(message, "1%c%d%cto view the reprieve list%c", MSGDEL, REPRIEVE, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= REPRIEVE;
	if((cont = find_cont(contname)) < 0)
		cont = plyptr->cont;
	sprintf(message, "4326%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4327%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4328%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 1; count < current_player; ++count)
	{
		if((strlen(players[count].name)) < 1)
			continue;
		if(players[count].attacks_lost[cont] < attacks_allowed(&players[count]))
			continue;
		sprintf(message, "4329%c%-16s%c%-16s%c%d%c", MSGDEL, vampire(&players[count])?select_classname(VAMPIRE):players[count].name, MSGDEL, select_classname(BASE_CLASSNUM(players[count].classnum)), MSGDEL, players[count].attacks_lost[cont], MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void show_retaliation(player *plyptr)
{
	int count;
	int plynum;

	sprintf(message, "57%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4385%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4386%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXRET; ++count)
	{
		if(plyptr->ret[count].plynum > 0)
		{
			plynum = plyptr->ret[count].plynum;
			sprintf(message, "4387%c%-32s%c%d%c", MSGDEL, vampire(&players[plynum])?select_classname(VAMPIRE):players[plynum].name, MSGDEL, plyptr->ret[count].attacks, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void review(char *name, int color, int cont, int show, int type, player *plyptr, player *whoptr)
{
	char line_info[128] = {0};
	char mode_info[5] = {0};
	char num_attacks[8] = {0};
	char str[16] = {0};
	char str_bg[16] = {0};
	char swm_info[8] = {0};
	char unit_info[64] = {0};
	int cam[MAXTRP];
	int count;
	int max = 50;
	int min = 25;
	int swarm;
	int total = 0;
	unit mod[MAXTRP];

	for(count = 0; count < MAXTRP; ++count)
	{
		memset(&mod[count], 0, UNTSIZ);
		if(count != BLOODGUARDNUM && count != CATAPULTNUM)
			total += plyptr->army[type][cont].total[count];
		cam[count] = 0;
	}
	if(show == REVIEW_ALL)
	{
		total += plyptr->army[type][cont].total[BLOODGUARDNUM];
		if(color)
			sprintf(str_bg, "%c%02d%d%c", CNTLC, GREEN, plyptr->army[type][cont].total[BLOODGUARDNUM], CNTLC);
		else
			sprintf(str_bg, "%d", plyptr->army[type][cont].total[BLOODGUARDNUM]);
	}
	else
	{
		strcpy(str_bg, "???");
		if(ranger(plyptr) && plyptr->level[cont] >= 2)
		{
			if(plyptr->level[cont] >= 6)
			{
				max = 100;
				min = 25;
			}
			else if(plyptr->level[cont] >= 4)
			{
				max = 75;
				min = 25;
			}
			for(count = 0; count < MAXTRP; ++count)
			{
				if(units[plyptr->troop_type][count].attack == 0 && units[plyptr->troop_type][count].defend == 0)
					continue;
				if(count == BLOODGUARDNUM || count == CATAPULTNUM)
					continue;
				cam[count] = plyptr->army[type][cont].total[count] * randomnum(max, min) / 100;
				total += cam[count];
			}
		}
	}			
	if(color)
		sprintf(message, "4208%c%c%02d%d%c%c", MSGDEL, CNTLC, GREEN, total, CNTLC, MSGDEL);
	else
		sprintf(message, "4208%c%d%c", MSGDEL, total, MSGDEL);
	send_message(message, name, 1);
	check_batmod(cont, whoptr, mod);
	for(count = 0; count < MAXTRP; ++count)
	{
		if(units[plyptr->troop_type][count].attack == 0 && units[plyptr->troop_type][count].defend == 0)
			continue;
		if(advanced_combat == 1)
			switch(units[plyptr->troop_type][count].mode)
			{
				case MODE_WALK:
					strcpy(mode_info, "inf,");
					break;
				case MODE_RIDE:
					strcpy(mode_info, "cav,");
					break;
				case MODE_FLY:
					strcpy(mode_info, "air,");
					break;
			}
		if(color)
			sprintf(num_attacks, "%c%02d,x%d%c", CNTLC, RED, units[plyptr->troop_type][count].num_attacks + mod[count].num_attacks, CNTLC);
		else
			sprintf(num_attacks, ",x%d", units[plyptr->troop_type][count].num_attacks + mod[count].num_attacks);
		if(count == BLOODGUARDNUM)
			strcpy(str, str_bg);
		else
		{
			if(color)
				sprintf(str, "%c%02d%d%c", CNTLC, GREEN, plyptr->army[type][cont].total[count] + cam[count], CNTLC);
			else
				sprintf(str, "%d", plyptr->army[type][cont].total[count] + cam[count]);
		}
		if(units[plyptr->troop_type][count].swarm + mod[count].swarm > 0)
		{
			swarm = 0;
			if(plyptr->army[type][cont].total[count] >= 1000)
				++swarm;
			if(plyptr->army[type][cont].total[count] >= 2000)
				++swarm;
			if(plyptr->army[type][cont].total[count] >= 4000)
				++swarm;
			sprintf(swm_info, "(x%d)", swarm);
		}
		sprintf(unit_info, "%s %s(s) (%s%d/%d%s%s%s%s%s%s%s)", str, 
		units[plyptr->troop_type][count].name, mode_info, units[plyptr->troop_type][count].attack + mod[count].attack, units[plyptr->troop_type][count].defend + mod[count].defend, 
		units[plyptr->troop_type][count].num_attacks + mod[count].num_attacks > 1?num_attacks:"", 
		units[plyptr->troop_type][count].ambush + mod[count].ambush > 0?",amb":"", 
		units[plyptr->troop_type][count].range + mod[count].range > 0?",rng":"", 
		units[plyptr->troop_type][count].regenerate + mod[count].regenerate > 0?",rgn":"", 
		units[plyptr->troop_type][count].shield + mod[count].shield > 0?",shd":"", 
		units[plyptr->troop_type][count].swarm + mod[count].swarm > 0?",swm":"", 
		units[plyptr->troop_type][count].swarm + mod[count].swarm > 0?swm_info:"");
		if((strlen(line_info)) < 1)
			sprintf(line_info, "%c%-30s%c", MSGDEL, unit_info, MSGDEL);
		else
			sprintf(line_info, "%s%s%c", line_info, unit_info, MSGDEL);
		strcpy(unit_info, "");
		if((count + 1) % 2 == 0)
		{
			sprintf(message, "4209%s", line_info);
			send_message(message, name, 1);
			strcpy(line_info, "");
		}
	}
	if((strlen(line_info)) > 0)
	{
		sprintf(message, "4209%s%c%c", line_info, MSGDEL, MSGDEL);
		send_message(message, name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 1);
}

void review_army(char *contname, player *plyptr)
{
	int attacks_left;
	int attacks_made; 
	int cont = 0;
	int cost = 0;
	int count;
	int surr;

	if(plyptr->movepts < REVIEW)
	{
		sprintf(message, "1%c%d%cto call your army to assembly%c", MSGDEL, REVIEW, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((cont = find_cont(contname)) < 0 || plyptr->level[cont] < 0)
		cont = plyptr->cont;
	if(cont != plyptr->cont)
	{
		sprintf(message, "4171%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, plyptr->kname[cont], MSGDEL, conts[cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	plyptr->movepts -= REVIEW;
	sprintf(message, "4343%c%s%c", MSGDEL, plyptr->kname[cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	attacks_left = attacks_allowed(plyptr);
	if(plyptr->attacks < 0)
	{
		attacks_left += plyptr->attacks * -1;
		attacks_made = 0;
	}
	else
		attacks_made = plyptr->attacks;
	attacks_left = attacks_allowed(plyptr);
	surr = (int)(plyptr->surrender[cont] * 100);
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "63%c%c%02d%d%c%c%c%02d%d%c%c%c%02d%d%c%c%c%02d%d%c%c", 
		MSGDEL, CNTLC, RED, surr < 1?1:surr, CNTLC, MSGDEL, CNTLC, attacks_made >= attacks_left - 1?RED:GREEN, attacks_made, CNTLC, MSGDEL, CNTLC, GREEN, attacks_left, CNTLC, MSGDEL, 
		CNTLC, GREEN, plyptr->castle[cont] * castle_max(plyptr->classnum), CNTLC, MSGDEL);
	else
		sprintf(message, "63%c%d%c%d%c%d%c%d%c", MSGDEL, surr < 1?1:surr, MSGDEL, attacks_made, MSGDEL, attacks_left, MSGDEL, plyptr->castle[cont] * castle_max(plyptr->classnum), MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = MERC; count < MAXTRP; ++count)
	{
		cost += plyptr->army[0][cont].total[count] * units[plyptr->troop_type][count].copper;
		cost += plyptr->army[1][cont].total[count] * units[plyptr->troop_type][count].copper;
	}
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "64%c%c%02d%-3s%c%c%c%02d%d%c%c%c%02d%d%c%c%c%02d%d%c%c%s%c", 
		MSGDEL, CNTLC, plyptr->special_kingdom & spcenlist[cont]?GREEN:RED, plyptr->special_kingdom & spcenlist[cont]?"ON":"OFF", CNTLC, MSGDEL, CNTLC,  plyptr->attacks_lost[cont] >= attacks_left - 1?RED:GREEN, plyptr->attacks_lost[cont], CNTLC, 
		MSGDEL, CNTLC, GREEN, attacks_left, CNTLC, MSGDEL, CNTLC, cost > 0?RED:GREEN, cost, CNTLC, MSGDEL, money_name, MSGDEL);
	else
		sprintf(message, "64%c%-3s%c%d%c%d%c%d%c%s%c", MSGDEL, plyptr->special_kingdom & spcenlist[cont]?"ON":"OFF", MSGDEL, plyptr->attacks_lost[cont], MSGDEL, attacks_left, MSGDEL, cost, MSGDEL, money_name, MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = ARMY_DEFENSE; count <= ARMY_CAMPAIGN; ++count)
	{
		sprintf(message, "4282%c%s%c", MSGDEL, army_names[count], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_ALL, count, plyptr, plyptr);
		show_formation(plyptr->name, cont, count, plyptr);
	}
}

void roll(char *arg, player *plyptr)
{
	int amount;
	int chance = 1;
	int house_roll;
	int player_roll;

	if(cleric(plyptr) && !titan(plyptr))
	{
		sprintf(message, "3059%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < ROLL)
	{
		sprintf(message, "1%c%d%cto gamble%c", MSGDEL, ROLL, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	amount = atoi(arg);
	if(amount < 1)
	{
		help(plyptr->name, "roll", "", "");
		return;
	}
	if(plyptr->copper[plyptr->cont] < amount)
	{
		sprintf(message, "4216%c%s%c", MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= ROLL;
	plyptr->copper[plyptr->cont] -= amount;
	house_roll = randomnum(6, 1) + randomnum(6, 1);
	player_roll = randomnum(6, 1) + randomnum(6, 1);
	sprintf(message, "4217%c%d%c", MSGDEL, house_roll, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(check_hero(HERO_TITAN, plyptr))
		chance += 9;
	if(randomnum(100, 1) <= chance)
	{
		sprintf(message, "4218%c%d%c", MSGDEL, house_roll, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "13016%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		player_roll = house_roll;
	}
	sprintf(message, "4219%c%d%c", MSGDEL, player_roll, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(player_roll == house_roll)
	{
		switch(player_roll)
		{
			case 7:
				amount *= 6;
				break;
			case 6:
			case 8:
				amount *= 7;
				break;
			case 5:
			case 9:
				amount *= 9;
				break;
			case 4:
			case 10:
				amount *= 12;
				break;
			case 3:
			case 11:
				amount *= 18;
				break;
			case 2:
			case 12:
				amount *= 36;
				break;
		}
		plyptr->copper[plyptr->cont] += amount;
		sprintf(message, "4220%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	else
	{
		sprintf(message, "4221%c%d%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void search(char *location, player *plyptr)
{
	int artnum;
	int count;
	int ctynum;

	if(strlen(location) < 1)
	{
		help(plyptr->name, "search", "", "");
		return;
	}
	strip(location);
	for(count = 0; count < TOTART; ++count)
		if((strlen(artifacts[count].name)) > 0 && artifacts[count].city >= 0 && STRCASECMP(location, artifacts[count].location) == 0)
			break;
	if((plyptr->levelmax == maxlevel[BASE_CLASSNUM(plyptr->classnum)] - 1) && (!mage(plyptr) || count != STAFF))
	{
		sprintf(message, "35%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((artnum = find_artifact(plyptr)) >= 0)
	{
		sprintf(message, "4222%c%s%c", MSGDEL, artifacts[artnum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < SEARCH)
	{
		sprintf(message, "1%c%d%cto search for an Artifact%c", MSGDEL, SEARCH, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < SEARCHCOPPER)
	{
		sprintf(message, "4223%c%d%c%s%c", MSGDEL, SEARCHCOPPER, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((ctynum = find_city(location)) >= 0)
	{
		sprintf(message, "4061%c%s%c", MSGDEL, ctynum == secret_city?"??????":cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(count == TOTART)
	{
		sprintf(message, "39%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(vampire(plyptr) && count == GRAIL)
	{
		sprintf(message, "17061%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->city != artifacts[count].city)
	{
		sprintf(message, "4224%c%s%c%s%c", MSGDEL, artifacts[count].city == secret_city?"??????":cities[artifacts[count].city].name, MSGDEL, conts[cities[artifacts[count].city].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= SEARCH;
	plyptr->copper[plyptr->cont] -= SEARCHCOPPER;
	if(artifacts[count].owner > 0)
	{
		sprintf(message, "4225%c%s%c", MSGDEL, artifacts[count].location, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	artifacts[count].owner = playernum(plyptr);
	save_art();
	sprintf(message, "4226%c%s%c%s%c", MSGDEL, artifacts[count].location, MSGDEL, artifacts[count].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(count == ORB && plyptr->items[0] > 0)
	{
		sprintf(message, "2062%c%s%c", MSGDEL, artifacts[count].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		remove_all_items(plyptr);
	}
}

void sell(char *itemdesc, char *itemname, int amount, int *ccopper, int cprice,
	int *cqty, int *item, int type, player *plyptr)
{
	int citynum;

	citynum = plyptr->city;
	if(amount < 1)
	{
		help(plyptr->name, "sell", "", "");
		return;
	}
	if(amount > *item)
	{
		sprintf(message, "4227%c%d%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cprice < 1)
	{
		sprintf(message, "4228%c%s%c", MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(*ccopper < amount * cprice)
	{
		sprintf(message, "4229%c%d%c%s%c", MSGDEL, cities[citynum].copper / cprice, MSGDEL, itemname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_sell(amount, type, plyptr))
	{
		*item -= amount;
		*cqty += amount;
		plyptr->copper[plyptr->cont] += amount * cprice;
		*ccopper -= amount * cprice;
		sprintf(message, "4230%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, itemname, MSGDEL, itemdesc, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void set(char *msg, player *plyptr)
{
	char args[256] = {0};
	char cmd[32] = {0};
	char cmds[][16] =
	{
		"autoenlist", "color", "gender", "kingdomname", "messages",
		"shipname", "surrender", "tax", "tips", "vassalaid", "winmsg",
		"winsound", ""
	};
	char str[32] = {0};
	int count;
	int len;

	sscanf(msg, "%31s", cmd);
	if((len = strlen(cmd)) < 1)
	{
		help(plyptr->name, "set", "", "");
		return;
	}
	for(count = 0; strlen(cmds[count]) > 1; ++count)
		if((STRNCASECMP(cmds[count], cmd, len)) == 0)
			break;
	if((strlen(cmds[count])) < 1)
	{
		help(plyptr->name, "set", "", "");
		return;
	}
	strcpy(args, msg + len + 1);
	switch(count)
	{
		case 0:
			autoenlist(args, plyptr);
			break;
		case 1:
			color(args, plyptr);
			break;
		case 2:
			gender(args, plyptr);
			break;
		case 3:
			kingdomname(args, plyptr);
			break;
		case 4:
			if((STRCASECMP(args, "on")) == 0 && !(plyptr->special_misc & SPC_MESSAGES))
				plyptr->special_misc += SPC_MESSAGES;
			else if((STRCASECMP(args, "off")) == 0 && plyptr->special_misc & SPC_MESSAGES)
				plyptr->special_misc -= SPC_MESSAGES;
			sprintf(message, "4060%c%s%c", MSGDEL, plyptr->special_misc & SPC_MESSAGES?"On":"Off", MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 5:
			sscanf(args, "%31s", str);
			sname(args + strlen(str) + 1, atoi(str), plyptr);
			break;
		case 6:
			surrender(args, plyptr);
			break;
		case 7:
			settax(args, plyptr);
			break;
		case 8:
			tips(args, plyptr);
			break;
		case 9:
			vassalaid(args, plyptr);
			break;
		case 10:
			winmsg(args, plyptr);
			break;
		case 11:
			winsound(args, plyptr);
			break;
	}
}

void settax(char *arg, player *plyptr)
{
	char desc[][16] = 
	{
		"generous", "kindly", "normal", "oppressive", "tyrannical"
	};
	int taxrate;

	taxrate = atoi(arg);
	if(structures[BASE_CLASSNUM(plyptr->classnum)].tax == 0)
	{
		sprintf(message, "4377%c%s%c", MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(arg) < 1)
	{
		if(plyptr->special_misc & SPC_COLOR)
			sprintf(message, "4180%c%s%c%c%02d%s%c%c%d%c%s%c%s%c", 
			MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, CNTLC, plyptr->tax[plyptr->cont] <= 2?GREEN:RED, desc[plyptr->tax[plyptr->cont]], 
			CNTLC, MSGDEL, plyptr->tax[plyptr->cont] * BASETAX, MSGDEL, money_name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		else
			sprintf(message, "4180%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, 
			plyptr->kname[plyptr->cont], MSGDEL, desc[plyptr->tax[plyptr->cont]], MSGDEL, plyptr->tax[plyptr->cont] * BASETAX, MSGDEL, money_name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}

	if(!isdigit(arg[0]) || taxrate < 0 || taxrate > BASETAX * 4 || taxrate % BASETAX != 0)
	{
		help(plyptr->name, "tax", "", "");
		return;
	}
	if(taxrate == plyptr->tax[plyptr->cont] * BASETAX)
	{
		sprintf(message, "4180%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, desc[plyptr->tax[plyptr->cont]], MSGDEL, taxrate, MSGDEL, money_name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < TAX)
	{
		sprintf(message, "1%c%d%cto adjust your taxation%c", MSGDEL, TAX, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->special_kingdom & spctax[plyptr->cont])
	{
		sprintf(message, "4231%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= TAX;
	plyptr->special_kingdom += spctax[plyptr->cont];
	plyptr->tax[plyptr->cont] = taxrate / BASETAX;
	sprintf(message, "4232%c%s%c%s%c%d%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, desc[plyptr->tax[plyptr->cont]], MSGDEL, taxrate, MSGDEL, money_name, MSGDEL, structures[BASE_CLASSNUM(plyptr->classnum)].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void show_structures(char *classname, player *plyptr)
{
	int classnum;

	if((classnum = find_class(classname)) < 0)
		classnum = BASE_CLASSNUM(plyptr->classnum);
	if((strlen(basenames[classnum])) < 1)
		classnum = FIGHTER;
	sprintf(message, "4378%c%s%c", MSGDEL, basenames[classnum], MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4379%c%s%c", MSGDEL, structures[classnum].name, MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4380%c%d%c", MSGDEL, structures[classnum].cost, MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4381%c%d%c", MSGDEL, structures[classnum].land, MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4382%c%d%c", MSGDEL, structures[classnum].mpcost, MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4383%c%s%c", MSGDEL, structures[classnum].tax == 1?"Yes":"No", MSGDEL);
	send_message(message, plyptr->name, 0);
	sprintf(message, "4384%c%d%c", MSGDEL, structures[classnum].troops, MSGDEL);
	send_message(message, plyptr->name, 0);
}

void spy(char *name, char *arg, player *plyptr)
{
	int bg = REVIEW_NOBG;
	int plynum;

	if(plyptr->spy[plyptr->cont] < 0)
	{
		sprintf(message, "4089%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(strlen(name) < 1)
	{
		help(plyptr->name, "spy", "", "");
		return;
	}
	if(STRCASECMP(name, "conquest") == 0)
	{
		spy_emperor(arg, plyptr);
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
		sprintf(message, "4091%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_vacation(plyptr->name, &players[plynum]))
		return;
	if(plyptr->movepts < SPY && plyptr->spy[plyptr->cont] != plynum)
	{
		sprintf(message, "1%c%d%cto send your operative%c", MSGDEL, SPY, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "16017%c%s%c%s%coperative%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL, MSGDEL, adjectives[players[plynum].gender][2], MSGDEL, conts[players[plynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_artifact(MIRROR, &players[plynum]) || check_hero(HERO_NINJA, &players[plynum]))
	{
		sprintf(message, "11042%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((randomnum(100, 1)) >= 60 - 5 * (players[plynum].level[players[plynum].cont] + 1) || check_artifact(DAGGER, plyptr) || check_hero(HERO_NINJA, plyptr))
	{
		if(plyptr->spy[plyptr->cont] != plynum)
			plyptr->movepts -= SPY;
		plyptr->spy[plyptr->cont] = plynum;
		sprintf(message, "15%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		person("", plyptr, &players[plynum]);
		show_surrender(plyptr->name, &players[plynum]);
		if(check_artifact(DAGGER, plyptr) || check_hero(HERO_NINJA, plyptr))
			bg = REVIEW_ALL;
		sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
		send_message(message, plyptr->name, 1);
		review(plyptr->name, plyptr->special_misc & SPC_COLOR, players[plynum].cont, bg, ARMY_DEFENSE, &players[plynum], &players[plynum]);
		show_formation(plyptr->name, players[plynum].cont, ARMY_DEFENSE, &players[plynum]);
		if(advanced_armies == 1)
		{
			sprintf(message, "10025%c%s%c%s%c", MSGDEL, players[plynum].name, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
			send_message(message, plyptr->name, 1);
			review(plyptr->name, plyptr->special_misc & SPC_COLOR, players[plynum].cont, bg, ARMY_CAMPAIGN, &players[plynum], &players[plynum]);
			show_formation(plyptr->name, players[plynum].cont, ARMY_CAMPAIGN, &players[plynum]);
		}
		return;
	}
	plyptr->spy[plyptr->cont] = -1;
	sprintf(message, "4233%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->nick, MSGDEL);
	send_message(message, players[plynum].name, 1);
	if(randomnum(100, 1) <= 20 - plyptr->level[plyptr->cont])
	{
		if(randomnum(100, 1) <= 25 && players[plynum].spy[players[plynum].cont] < 0)
		{
			sprintf(message, "4234%c%s%c", MSGDEL, players[plynum].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			players[plynum].spy[players[plynum].cont] = 0;
			sprintf(message, "4235%c%d%c", MSGDEL, randomnum(8, 2), MSGDEL);
			send_message(message, players[plynum].name, 1);
			sprintf(message, "4236%c%s%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL, plyptr->name, MSGDEL);
			log_event("", message, players[plynum].name);
		}
		else
		{
			sprintf(message, "4237%c%s%c", MSGDEL, plyptr->kname[plyptr->cont], MSGDEL);
			send_message(message, plyptr->name, 1);
			sprintf(message, "4238%c%d%c", MSGDEL, randomnum(8, 2), MSGDEL);
			send_message(message, players[plynum].name, 1);
			if(randomnum(100, 1) <= 35)
				person("", &players[plynum], plyptr);
			else
			{
				if(check_artifact(DAGGER, &players[plynum]))
					bg = REVIEW_ALL;
				sprintf(message, "10025%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, army_names[ARMY_DEFENSE], MSGDEL);
				send_message(message, players[plynum].name, 1);
				review(players[plynum].name, players[plynum].special_misc & SPC_COLOR, plyptr->cont, bg, ARMY_DEFENSE, plyptr, plyptr);
				if(advanced_armies == 1)
				{
					sprintf(message, "10025%c%s%c%s%c", MSGDEL, plyptr->nick, MSGDEL, army_names[ARMY_CAMPAIGN], MSGDEL);
					send_message(message, players[plynum].name, 1);
					review(players[plynum].name, players[plynum].special_misc & SPC_COLOR, plyptr->cont, bg, ARMY_CAMPAIGN, plyptr, plyptr);
				}
			}
		}
	}
	else
	{
		sprintf(message, "4095%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		sprintf(message, "4239%c%d%c", MSGDEL, randomnum(8, 2), MSGDEL);
		send_message(message, players[plynum].name, 1);
	}
}

void spy_emperor(char *arg, player *plyptr)
{
	char types[][8] = 
	{
		"list"
	};
	int cnt;
	int count = 0;
	int found;
	int hit[10] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	int next = 0;
	int plynum;
	int total;

	if(plyptr->movepts < SPY)
	{
		sprintf(message, "1%c%d%cto send your operative%c", MSGDEL, SPY, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= SPY;
	if((randomnum(100, 1)) <= 60 - 5 * (maxlevel[BASE_CLASSNUM(plyptr->classnum)] + 1) && !check_artifact(DAGGER, plyptr) && !check_hero(HERO_NINJA, plyptr))
	{
		plyptr->spy[plyptr->cont] = -1;
		sprintf(message, "4095%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((STRCASECMP(arg, types[0])) != 0)
	{
		if(current_player > 10)
			total = 10;
		else
			total = current_player - 1;
		while(count < total)
		{
			plynum = randomnum(current_player - 1, 1);
			if(strlen(players[plynum].name) > 1)
			{
				for(cnt = 0, found = 0; cnt < 10; ++cnt)
					if(hit[cnt] == plynum)
						found = 1;
				if(!found)
				{
					hit[next++] = plynum;
					sprintf(message, "4240%c%s%c%s%c%s%c%d%c%s%c%d%c", MSGDEL, 
					select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL, 
					players[plynum].nick, MSGDEL, players[plynum].castle[players[plynum].cont], MSGDEL, structures[BASE_CLASSNUM(players[plynum].classnum)].name, MSGDEL, players[plynum].land[players[plynum].cont], MSGDEL);
					send_message(message, plyptr->name, 1);
					++count;
				}
			}
		}
	}
	else
	{
		for(count = 0; count < 10; ++count)
		{
			sprintf(message, "4241%c%d%c%d%c%s%c", MSGDEL, totlvl[plyptr->cont][count], MSGDEL, count + 1, MSGDEL, conts[plyptr->cont].name, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
	}
	plyptr->spy[plyptr->cont] = 0;
}

void spy_report(int cont, player *plyptr)
{
	int chance;
	int count;
	int plynum;

	plynum = playernum(plyptr);
	for(count = 1; count < current_player; ++count)
	{
		if(players[count].spy[cont] == plynum)
		{
			if(check_artifact(DAGGER, &players[count]) || check_hero(HERO_NINJA, &players[count]))
				chance = 101;
			else
				chance = 50 - 5 * (plyptr->level[cont] + 1);
			if(randomnum(100, 1) <= chance + 10)
			{
				if(randomnum(100, 1) <= chance + 5)
				{
					players[count].spy[cont] = 0;
					sprintf(message, "4321%c%s%c%s%c", MSGDEL, plyptr->city == secret_city?"??????":cities[plyptr->city].name, MSGDEL, players[count].nick, MSGDEL);
				}
				else
					sprintf(message, "4322%c%s%c", MSGDEL, plyptr->city == secret_city?"??????":cities[plyptr->city].name, MSGDEL);
				send_message(message, plyptr->name, 1);
				sprintf(message, "4323%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, plyptr->city == secret_city?"??????":cities[plyptr->city].name, MSGDEL);
			}
			else
				sprintf(message, "4324%c%s%c%s%c", MSGDEL, plyptr->name, MSGDEL, plyptr->city == secret_city?"??????":cities[plyptr->city].name, MSGDEL);
			send_message(message, players[count].name, 1);
			log_event("", message, players[count].name);
		}
	}
}

void surrender(char *arg, player *plyptr)
{
	int num;
	int surr;

	if(allow_surrender == 0 && plyptr->surrender[plyptr->cont] < dft_surrender)
		plyptr->surrender[plyptr->cont] = dft_surrender;
	surr = (int)(plyptr->surrender[plyptr->cont] * 100);
	if(strlen(arg) < 1)
	{
		if(plyptr->special_misc & SPC_COLOR)
			sprintf(message, "4211%c%c%02d%d%c%c", MSGDEL, CNTLC, RED, surr < 1?1:surr, CNTLC, MSGDEL);
		else
			sprintf(message, "4211%c%d%c", MSGDEL, surr < 1?1:surr, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(allow_surrender == 0)
	{
		surr = (int)(dft_surrender * 100);
		sprintf(message, "4388%c%d%c", MSGDEL, surr < 1?1:surr, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	num = atoi(arg);
	if(num < 1 || num > 100)
	{
		sprintf(message, "4242%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->surrender[plyptr->cont] = (float)((float)num / 100.0);
	if(plyptr->special_misc & SPC_COLOR)
		sprintf(message, "4211%c%c%02d%d%c%c", MSGDEL, CNTLC, RED, num < 1?1:num, CNTLC, MSGDEL);
	else
		sprintf(message, "4211%c%d%c", MSGDEL, num < 1?1:num, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void tavern(player *plyptr)
{
	char drink[32] = {0};
	char troop_name[64] = {0};
	int choice;
	int cont = 0;
	int count;
	int maxtries = 0;
	int num;
	int plynum;

	if(vampire(plyptr))
	{
		sprintf(message, "17051%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < TAVERN)
	{
		sprintf(message, "1%c%d%cto go to the tavern%c", MSGDEL, TAVERN, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < 10)
	{
		sprintf(message, "4243%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		kick("Come back when you can pay for your drinks!", playernum(plyptr));
		return;
	}
	if(plyptr->tavern[plyptr->cont] >= 10 && !barbarian(plyptr))
	{
		sprintf(message, "4244%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		kick("Get outta here you drunkard!", playernum(plyptr));
		return;
	}
	if(plyptr->tavern[plyptr->cont]++ == 9 && !barbarian(plyptr))
	{
		sprintf(message, "4245%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		if(!vampire(plyptr) && !check_artifact(GRAIL, plyptr))
			--plyptr->honor[plyptr->cont];
		kick("Get outta here you drunkard!", playernum(plyptr));
		return;
	}
	plyptr->movepts -= TAVERN;
	plyptr->copper[plyptr->cont] -= 10;
	switch(randomnum(4, 0))
	{
		case 0:
			strcpy(drink, "tall glass of wine");
			break;
		case 1:
			strcpy(drink, "mug of beer");
			break;
		case 2:
			strcpy(drink, "goblet of ale");
			break;
		case 3:
			strcpy(drink, "tankard of mead");
			break;
		default:
			strcpy(drink, "flask of spirits");
			break;
	}
	sprintf(message, "4246%c%s%c%s%c", MSGDEL, cities[plyptr->city].tname, MSGDEL, drink, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(plyptr->tavern[plyptr->cont] > 6)
	{
		sprintf(message, "4247%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	if(auto_ar >= 0 && auto_ar - conq_info.today >= 2 && randomnum(100, 1) <= 5)
	{
		sprintf(message, "11117%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "4248%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
	send_message(message, plyptr->name, 1);
	for(count = 0; count < MAXMEC; ++count)
	{
		if(cities[plyptr->city].merc_num[count] < MERC || (strlen(units[cities[plyptr->city].merc_type[count]][cities[plyptr->city].merc_num[count]].name)) < 1)
			continue;
		sprintf(troop_name, "%ss", units[cities[plyptr->city].merc_type[count]][cities[plyptr->city].merc_num[count]].name);
		sprintf(message, "4249%c%-30s%c%d%c", MSGDEL, troop_name, MSGDEL, cities[plyptr->city].merc_qty[count], MSGDEL);
		send_message(message, plyptr->name, 1);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	plynum = playernum(plyptr);
	do {
		if((num = find_randomplayer(plyptr->cont, -1)) < 1 || plynum == num || vampire(&players[num]))
			continue;
		break;
	} while(maxtries++ < 1000);
	if(maxtries > 1000)
		choice = randomnum(7, 5);
	else
	{
		cont = players[num].cont;
		choice = randomnum(7, 0);
	}
	switch(choice)
	{
		case 0:
			sprintf(message, "4250%c%s%c%s%c%d%c%s%c", MSGDEL, select_class(players[num].classnum), MSGDEL, players[num].name, MSGDEL, players[num].copper[cont], MSGDEL, money_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 1:
			sprintf(message, "4250%c%s%c%s%c%d%c%s%c", MSGDEL, select_class(players[num].classnum), MSGDEL, players[num].name, MSGDEL, players[num].food[cont], MSGDEL, food_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 2:
			sprintf(message, "4250%c%s%c%s%c%d%c%s(s)%c", MSGDEL, select_class(players[num].classnum), MSGDEL, players[num].name, MSGDEL, players[num].peasant[cont], MSGDEL, peasant_name, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 3:
			sprintf(message, "4251%c%s%c%s%c%s%c", MSGDEL, select_class(players[num].classnum), MSGDEL, players[num].name, MSGDEL, select_title(players[num].classnum, players[num].level[cont], players[num].gender), MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 4:
			sprintf(message, "4254%c%s%c%s%c%s%c", MSGDEL, select_class(players[num].classnum), MSGDEL, select_title(players[num].classnum, players[num].level[cont], players[num].gender), MSGDEL, players[num].name, MSGDEL);
			send_message(message, plyptr->name, 1);
			review(plyptr->name, plyptr->special_misc & SPC_COLOR, cont, REVIEW_NOBG, ARMY_DEFENSE, &players[num], &players[num]);
			break;
		case 5:
			if(secret_city < 0)
				sprintf(message, "4252%c%c", MSGDEL, MSGDEL);
			else
				sprintf(message, "4253%c%s%c", MSGDEL, cities[plyptr->city].clue, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		case 6:
			sprintf(message, "4255%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
		default:
			sprintf(message, "4256%c%s%c", MSGDEL, plyptr->gender == 0?"barmaid":"bartender", MSGDEL);
			send_message(message, plyptr->name, 1);
			break;
	}
}

void tips(char *arg1, player *plyptr)
{
	if((STRCASECMP(arg1, "on")) == 0 && !(plyptr->special_misc & SPC_TIP))
		plyptr->special_misc += SPC_TIP;
	else if((STRCASECMP(arg1, "off")) == 0 && plyptr->special_misc & SPC_TIP)
		plyptr->special_misc -= SPC_TIP;
	sprintf(message, "4257%c%s%c", MSGDEL, plyptr->special_misc & SPC_TIP?"ON":"OFF", MSGDEL);
	send_message(message, plyptr->name, 1);
}

void titans(char *name)
{
	char input[128];
	FILE *fp;
	int count;
	int total;

	sprintf(message, "4258%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	sprintf(message, "4259%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	for(count = 1, total = 0; count < current_player; ++count)
		if(titan(&players[count]))
		{
			if(total++ == 0)
				sprintf(message, "4260%c%s %s %s", MSGDEL, select_class(players[count].classnum), select_title(players[count].classnum, players[count].level[players[count].cont], players[count].gender), players[count].name);
			else
				sprintf(message, "%s, %s %s %s", message, select_class(players[count].classnum), select_title(players[count].classnum, players[count].level[players[count].cont], players[count].gender), players[count].name);
		}
	if(total > 0)
	{
		sprintf(message, "%s%c", message, MSGDEL);
		send_message(message, name, 0);
	}
	sprintf(message, "4261%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
	if((fp = fopen(TITFIL, "r")) != NULL)
	{
		while((fgets(input, 128, fp)) != NULL)
		{
			strip(input);
			sprintf(message, "4262%c%s%c", MSGDEL, input, MSGDEL);
			send_message(message, name, 0);
		}
		fclose(fp);
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 0);
}

void train(int amount, player *plyptr)
{
	int cost;

	if(amount < 1)
	{
		help(plyptr->name, "train", "", "");
		return;
	}
	if(units[plyptr->troop_type][RENNUM].train == 0)
	{
		sprintf(message, "4263%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][5], MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->army[ARMY_DEFENSE][plyptr->cont].total[RENNUM] < amount)
	{
		sprintf(message, "4264%c%d%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < plyptr->level[plyptr->cont] + 1)
	{
		sprintf(message, "1%c%d%cto Train any %ss%c", MSGDEL, plyptr->level[plyptr->cont] + 1, MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_hero(HERO_GENERAL1, plyptr) || check_hero(HERO_GENERAL2, plyptr))
		cost = 0;
	else
		cost = amount * 20;
	if(plyptr->copper[plyptr->cont] < cost)
	{
		sprintf(message, "4265%c%d%c%s%c%s%c%d%c%s%c", MSGDEL, cost, MSGDEL, money_name, MSGDEL, peasant_name, MSGDEL, plyptr->copper[plyptr->cont], MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= plyptr->level[plyptr->cont] + 1;
	plyptr->copper[plyptr->cont] -= cost;
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[RENNUM] -= amount;
	plyptr->army[ARMY_DEFENSE][plyptr->cont].total[SOLDIERNUM] += amount;
	sprintf(message, "4266%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][RENNUM].name, MSGDEL, units[plyptr->troop_type][SOLDIERNUM].name, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void transfer_army(char *tostr, char *unit, int amount, player *plyptr)
{
	int troopnum;
	int from = ARMY_CAMPAIGN;
	int len;
	int to = ARMY_DEFENSE;

	if(advanced_armies == 0 || (len = strlen(unit)) < 1)
	{
		help(plyptr->name, "transfer", "", "");
		return;
	}
	if((tolower(tostr[0])) != 'c' && (tolower(tostr[0])) != 'd')
	{
		help(plyptr->name, "transfer", "", "");
		return;
	}
	if(amount < 1)
	{
		sprintf(message, "37%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < TRANSFER)
	{
		sprintf(message, "1%c%d%cto transfer your troops%c", MSGDEL, TRANSFER, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
		if((STRNCASECMP(units[plyptr->troop_type][troopnum].name, unit, len)) == 0)
			break;
	if(troopnum == MAXTRP)
	{
		sprintf(message, "4398%c%s%c", MSGDEL, unit, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((tolower(tostr[0])) == 'c')
	{
		from = ARMY_DEFENSE;
		to = ARMY_CAMPAIGN;
	}
	if(plyptr->army[from][plyptr->cont].total[troopnum] < amount)
	{
		sprintf(message, "4399%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][troopnum].name, MSGDEL, army_names[from], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->movepts -= TRANSFER;
	plyptr->army[from][plyptr->cont].total[troopnum] -= amount;
	plyptr->army[to][plyptr->cont].total[troopnum] += amount;
	sprintf(message, "4400%c%d%c%s%c%s%c", MSGDEL, amount, MSGDEL, units[plyptr->troop_type][troopnum].name, MSGDEL, army_names[to], MSGDEL);
	send_message(message, plyptr->name, 1);
}

void transfer_army_all(char *tostr, char *unit, player *plyptr)
{
	int from = ARMY_CAMPAIGN;
	int len;
	int to = ARMY_DEFENSE;
	int troopnum;

	if(advanced_armies == 0)
	{
		help(plyptr->name, "transfer", "", "");
		return;
	}
	if((tolower(tostr[0])) == 'c')
	{
		from = ARMY_DEFENSE;
		to = ARMY_CAMPAIGN;
	}
	if((len = strlen(unit)) < 1)
	{
		for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
			if(plyptr->army[from][plyptr->cont].total[troopnum] > 0)
				transfer_army(tostr, units[plyptr->troop_type][troopnum].name, plyptr->army[from][plyptr->cont].total[troopnum], plyptr);
		return;
	}
	for(troopnum = 0; troopnum < MAXTRP; ++troopnum)
		if((STRNCASECMP(units[plyptr->troop_type][troopnum].name, unit, len)) == 0)
			break;
	if(troopnum == MAXTRP)
	{
		sprintf(message, "4398%c%s%c", MSGDEL, unit, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	transfer_army(tostr, units[plyptr->troop_type][troopnum].name, plyptr->army[from][plyptr->cont].total[troopnum], plyptr);
}


void travel(char *cityname, player *plyptr)
{
	char ride[][16] =
	{
		"a few days", "several days", "many days"
	}; 
	int ctynum;
	int distance;

	if(strlen(cityname) < 1)
	{
		help(plyptr->name, "travel", "", "");
		return;
	}
	if((ctynum = find_city(cityname)) < 0)
	{
		sprintf(message, "2%c%s%c", MSGDEL, cityname, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(ctynum == plyptr->city)
	{
		sprintf(message, "19%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].cont != plyptr->cont)
	{
		sprintf(message, "20%c%s%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL, conts[cities[ctynum].cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	distance = find_distance(plyptr->city, ctynum, plyptr) + (plyptr->level[plyptr->cont] + 1);
	if(plyptr->movepts < distance)
	{
		sprintf(message, "1%c%d%cto travel to %s%c", MSGDEL, distance, MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[plyptr->city].lock == 1 && !check_artifact(WINGS, plyptr) && !ranger(plyptr) && !vampire(plyptr))
	{
		sprintf(message, "4267%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, cities[plyptr->city].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(cities[ctynum].lock == 1 && !check_artifact(WINGS, plyptr) && !ranger(plyptr) && !vampire(plyptr))
	{
		sprintf(message, "4268%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	highway_robbers(cities[ctynum].name, plyptr);
	plyptr->movepts -= distance;
	plyptr->city = ctynum;
	distance -= (plyptr->level[plyptr->cont] + 1);
	if(distance <= 2)
		distance = 0;
	else
		if(distance > 2 && distance <= 4)
			distance = 1;
		else
			distance = 2;
	sprintf(message, "4269%c%s%c%s%c", MSGDEL, ride[distance], MSGDEL, cities[ctynum].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	if(ctynum == antimagic_city && plyptr->items[0] > 0)
	{
		sprintf(message, "2062%c%s%c", MSGDEL, cities[ctynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		remove_all_items(plyptr);
	}
	spy_report(plyptr->cont, plyptr);
}

void troop_info(char *classname, player *plyptr)
{
	char mode_info[5] = {0};
	char num_attacks[8] = {0};
	char unit_info[64] = {0};
	int count;
	int troop_num;

	if((troop_num = find_class(classname)) < 0)
		troop_num = troop_type(plyptr->classnum);
	if((strlen(basenames[troop_num])) < 1)
		troop_num = FIGHTER;
	sprintf(message, "4270%c%s%c", MSGDEL, basenames[troop_num], MSGDEL);
	send_message(message, plyptr->name, 0);
	for(count = 0; count < MAXTRP; ++count)
	{
		if(units[troop_num][count].attack == 0 && units[troop_num][count].defend == 0)
			continue;
		if(count == MERC)
		{
			sprintf(message, "4271%c%c", MSGDEL, MSGDEL);
			send_message(message, plyptr->name, 0);
		}
		if(advanced_combat == 1)
			switch(units[troop_num][count].mode)
			{
				case MODE_WALK:
					strcpy(mode_info, "inf,");
					break;
				case MODE_RIDE:
					strcpy(mode_info, "cav,");
					break;
				case MODE_FLY:
					strcpy(mode_info, "air,");
					break;
			}
		sprintf(num_attacks, ",x%d", units[troop_num][count].num_attacks);
		sprintf(unit_info, "%02d:%s (%s%d/%d%s%s%s%s%s%s)", count + 1, 
		units[troop_num][count].name, mode_info, units[troop_num][count].attack, units[troop_num][count].defend, 
		units[troop_num][count].num_attacks > 1?num_attacks:"",
		units[troop_num][count].ambush == 1?",amb":"", units[troop_num][count].range == 1?",rng":"", 
		units[troop_num][count].regenerate == 1?",rgn":"", units[troop_num][count].shield == 1?",shd":"", 
		units[troop_num][count].swarm == 1?",swm":"");
		sprintf(message, "4272%c%-30s%c%d%c%s%c%d%c%s%c", MSGDEL, unit_info, MSGDEL, units[troop_num][count].food, MSGDEL, food_name, MSGDEL, units[troop_num][count].copper, MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 0);
	}
	sprintf(message, "4344%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 0);
	if(advanced_combat == 1)
	{
		sprintf(message, "4389%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 0);
	}
	for(count = 0; count < 7; ++count)
	{
		sprintf(message, "%d%c%c", 4345 + count, MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 0);
	}
}

void vacation(int len, player *plyptr)
{
	char str[TIMESIZ] = {0};
	time_t current;

	time(&current);
	if(len == -1 && plyptr->vacation > 0)
	{
		plyptr->vacation = 0;
		sprintf(message, "4356%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->vacation > current)
	{
		strftime(str, TIMESIZ, TIMEFORMAT, localtime(&(plyptr->vacation)));
		sprintf(message, "4355%c%s%c", MSGDEL, str, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(len == 0)
	{
		help(plyptr->name, "vacation", "", "");
		return;
	}
	if(len < 72)
	{
		sprintf(message, "4357%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	plyptr->vacation = current + (3600 * len);
	strftime(str, TIMESIZ, TIMEFORMAT, localtime(&(plyptr->vacation)));
	sprintf(message, "4358%c%s%c", MSGDEL, str, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4359%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void validate(char *name, char *host, char *passwd)
{
	char str[TIMESIZ] = {0};
	char tip[1024] = {0};
	int nummsg;
	int plynum;
	int sock;
	int time_left;

	if((plynum = find_player(name)) <= 0)
	{
		sprintf(message, "41%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		sprintf(message, "42%c%c", MSGDEL, MSGDEL);
		send_message(message, name, 0);
		return;
	}
	if(strlen(passwd) < 1)
	{
		help(name, "validate", "", "");
		return;
	}
	if((players[plynum].sock != INVALID_SOCKET && players[plynum].sock != sockets[sockout].sock) && players[plynum].validate == VAL_NORMAL)
	{
		sprintf(message, "43%c%c", MSGDEL, MSGDEL);
		send_message(message, players[plynum].name, 0);
		return;
	}
	if(check_vacation(players[plynum].name, &players[plynum]))
		return;
	if(players[plynum].special_misc & SPC_LOCK)
	{
		sprintf(message, "999%cYour player has been locked.", MSGDEL);
		send_message(message, players[plynum].name, 0);
		return;
	}
	if(players[plynum].trynum > 2)
	{
		sprintf(message, "45%c%c", MSGDEL, MSGDEL);
		send_message(message, players[plynum].name, 0);
		return;
	}
	if((strcmp(passwd, players[plynum].password)) != 0)
	{
		sprintf(message, "44%c%c", MSGDEL, MSGDEL);
		send_message(message, players[plynum].name, 0);
		sprintf(message, "%s Failed login for %s!%s", NOWSTR, players[plynum].name, host);
		if(titan(&players[plynum]))
			add_log(EMPLOG, message);
		else
		{
			add_log(PLYLOG, message);
			++players[plynum].trynum;
		}
		return;
	}
	if((difftime(time(NULL), players[plynum].lastplay)) <= 30)
	{
	        sock = find_sock(players[plynum].sock);
	        if(sock >= 0 && STRCASECMP(sockets[sock].type, "WEB") != 0)
		{
			sprintf(message, "54%c%d%c", MSGDEL, ++players[plynum].trynum, MSGDEL);
			send_message(message, players[plynum].name, 0);
			return;
		}
	}
	sprintf(message, "%s Successful login for %s!%s", NOWSTR, players[plynum].name, host);
	if(titan(&players[plynum]))
		add_log(EMPLOG, message);
	else
		add_log(PLYLOG, message);
	players[plynum].done = 0;
	players[plynum].sock = sockets[sockout].sock;
	players[plynum].trynum = 0;
	players[plynum].validate = VAL_NORMAL;
	dcc_chat(host, plynum);
	sprintf(message, "46%c%c", MSGDEL, MSGDEL);
	send_message(message, players[plynum].name, 0);
	strftime(str, TIMESIZ, TIMEFORMAT, localtime(&players[plynum].lastplay));
	if(players[plynum].special_misc & SPC_COLOR)
		sprintf(message, "48%c%c%02d%d%c%c%c%02d%s%c%c%c%02d%s%c%c", MSGDEL, 
		CNTLC, BLUE, conq_info.today, CNTLC, MSGDEL, CNTLC, BLUE, age, CNTLC, MSGDEL, CNTLC, BLUE, str, CNTLC, MSGDEL);
	else
		sprintf(message, "48%c%d%c%s%c%s%c", MSGDEL, 
		conq_info.today, MSGDEL, age, MSGDEL, str, MSGDEL);
	send_message(message, name, 1);
	if(players[plynum].level[players[plynum].cont] == maxlevel[BASE_CLASSNUM(players[plynum].classnum)] - 1 && !titan(&players[plynum]))
	{
		time_left = (players[plynum].king[players[plynum].cont] + REIGN) - conq_info.today;
		if(players[plynum].special_misc & SPC_COLOR)
			sprintf(message, "49%c%c%02d%d%c%c%c%0d2%s%c%c", MSGDEL, 
			CNTLC, time_left < 2?RED:BLUE, time_left, CNTLC, MSGDEL, CNTLC, PURPLE, select_title(players[plynum].classnum, 
			players[plynum].level[players[plynum].cont], players[plynum].gender), CNTLC, MSGDEL);
		else
			sprintf(message, "49%c%d%c%s%c", MSGDEL, time_left, MSGDEL, select_title(players[plynum].classnum, players[plynum].level[players[plynum].cont], players[plynum].gender), MSGDEL);
		send_message(message, players[plynum].name, 1);
	}
	sprintf(message, "47%c%c", MSGDEL, MSGDEL);
	send_message(message, name, 1);
	if(check_file("", players[plynum].name) > 0)
	{
		sprintf(message, "55%c%c", MSGDEL, MSGDEL);
		send_message(message, players[plynum].name, 1);
	}
	if((nummsg = check_file(".msg", players[plynum].name)) > 0)
	{
		if(players[plynum].special_misc & SPC_COLOR)
			sprintf(message, "50%c%c%02d%d%c%c", MSGDEL, CNTLC, RED, nummsg, CNTLC, MSGDEL);
		else
			sprintf(message, "50%c%d%c", MSGDEL, nummsg, MSGDEL);
		send_message(message, players[plynum].name, 1);
	}
	sprintf(message, "51%c%s%c", MSGDEL, conq_info.motd, MSGDEL);
	send_message(message, name, 1);
	if(players[plynum].special_misc & SPC_TIP)
	{
		strcpy(tip, read_tip());
		sprintf(message, "52%c%s%c", MSGDEL, tip, MSGDEL);
		send_message(message, players[plynum].name, 1);
		sprintf(message, "53%c%c", MSGDEL, MSGDEL);
		send_message(message, players[plynum].name, 1);
	}
	players[plynum].lastplay = time(NULL);
	players[plynum].trynum = 0;
	if(players[plynum].remorts > 2 && (players[plynum].special_misc & SPC_OPS))
		op(plynum);
	if((demote(players[plynum].cont, &players[plynum])) == 0)
		promote(players[plynum].cont, &players[plynum]);
}

void view(char *name, player *plyptr)
{
	int plynum;
	int total;
	player *whoptr;

	if(strlen(name) > 0)
	{
		if((plynum = find_player(name)) <= 0)
		{
			sprintf(message, "10%c%s%c", MSGDEL, name, MSGDEL);
			send_message(message, plyptr->name, 1);
			return;
		}
		whoptr = &players[plynum];
	}
	else
		whoptr = plyptr;
	total = whoptr->battles_won + whoptr->battles_lost;
	if(whoptr == plyptr)
		sprintf(message, "4273%c%s%c%d%c", MSGDEL, conts[whoptr->cont].name, MSGDEL, whoptr->first_play[whoptr->cont], MSGDEL);
	else
		sprintf(message, "4274%c%s%c%s%c%d%c", MSGDEL, whoptr->name, MSGDEL, conts[whoptr->cont].name, MSGDEL, whoptr->first_play[whoptr->cont], MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4275%c%d%c", MSGDEL, total, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4276%c%d%c", MSGDEL, whoptr->battles_won, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4277%c%d%c", MSGDEL, whoptr->battles_lost, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4278%c%d%c", MSGDEL, whoptr->troops_killed, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "4279%c%d%c", MSGDEL, whoptr->troops_lost, MSGDEL);
	send_message(message, plyptr->name, 1);
}

void winmsg(char *wmsg, player *plyptr)
{
	if(strlen(wmsg) < 1)
	{
		sprintf(message, "4177%c%s%c", MSGDEL, plyptr->winmsg, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(check_word(plyptr->name, wmsg, 255, 1))
	{
		memset(plyptr->winmsg, 0, 256);
		strncpy(plyptr->winmsg, wmsg, 255);
		sprintf(message, "4177%c%s%c", MSGDEL, wmsg, MSGDEL);
		send_message(message, plyptr->name, 1);
	}
}

void winsound(char *sound, player *plyptr)
{
	if(strlen(sound) < 1)
	{
		if(strlen(plyptr->winsound) < 1)
			sprintf(message, "4280%c%c", MSGDEL, MSGDEL);
		else
			sprintf(message, "4281%c%s%c", MSGDEL, plyptr->winsound, MSGDEL);
		send_message(message, plyptr->name, 0);
		return;
	}
	memset(plyptr->winsound, 0, 32);
	strncpy(plyptr->winsound, sound, 31);
	sprintf(message, "4281%c%s%c", MSGDEL, plyptr->winsound, MSGDEL);
	send_message(message, plyptr->name, 0);
}
