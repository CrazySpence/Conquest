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

void bounty(char *name, int amount, player *plyptr)
{
	bountylist *new;
	bountylist *next;
	bountylist *prev;
	int plynum;

	if((strlen(name)) < 1 || (find_cont(name) >= 0 && amount == 0))
	{
		show_bounty(name, plyptr);
		return;
	}
	if(plyptr->level[plyptr->cont] < 1)
	{
		sprintf(message, "0%c%s%c", MSGDEL, select_title(plyptr->classnum, 1, plyptr->gender), MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(titan(plyptr))
	{
		sprintf(message, "19011%c%c", MSGDEL, MSGDEL);
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
		sprintf(message, "19005%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->cont != players[plynum].cont)
	{
		sprintf(message, "12%c%s%c%s%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL, players[plynum].name, MSGDEL, conts[plyptr->cont].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->movepts < BOUNTY)
	{
		sprintf(message, "1%c%d%cto set a bounty on %s%c", MSGDEL, BOUNTY, MSGDEL, players[plynum].name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(amount < 10000)
	{
		sprintf(message, "19009%c%c", MSGDEL, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if(plyptr->copper[plyptr->cont] < amount)
	{
		sprintf(message, "19004%c%s%c", MSGDEL, money_name, MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((new = (bountylist *)malloc(BNTSIZ)) == NULL)
	{
		sprintf(message, "999%c%s", MSGDEL, ERRMSG1);
		send_message(message, plyptr->name, 1);
		return;
	}
	next = bountyhead;
	prev = bountyhead;
	while(next != NULL)
	{
		prev = next;
		next = next->next;
	}
	plyptr->movepts -= BOUNTY;
	plyptr->copper[plyptr->cont] -= amount;
	new->amount = amount;
	new->cont = plyptr->cont;
	new->dateset = conq_info.today;
	new->who = plynum;
	strcpy(new->setby, plyptr->nick);
	new->next = NULL;
	if(bountyhead != NULL)
		prev->next = new;
	else
		bountyhead = new;
	save_bounty();
	sprintf(message, "19006%c%d%c%s%c%s%c%s%c%s%c%s%c", MSGDEL, amount, MSGDEL, money_name, MSGDEL, conts[plyptr->cont].name, 
	MSGDEL, select_class(players[plynum].classnum), MSGDEL, select_title(players[plynum].classnum, players[plynum].level[plyptr->cont], players[plynum].gender), MSGDEL, players[plynum].nick, MSGDEL);
	send_message(message, plyptr->name, 1);
	send_channel(message, 0);
	sprintf(message, "19007%c%d%c%s%c%s%c%s%c%s%c%s%c", 
	MSGDEL, amount, MSGDEL, money_name, MSGDEL, conts[plyptr->cont].name, MSGDEL, select_class(plyptr->classnum), MSGDEL, select_title(plyptr->classnum, plyptr->level[plyptr->cont], plyptr->gender), MSGDEL, plyptr->nick, MSGDEL);
	send_message(message, players[plynum].name, 1);
	log_event("", message, players[plynum].name);
}

void check_bounty(int cont, int plynum, player *plyptr)
{
	bountylist *next;
	bountylist *prev = NULL;
	int paid = 0;

	if(bountyhead == NULL)
		return;
	next = bountyhead;
	while(next != NULL)
	{
		if(next->who != plynum || next->cont != cont)
		{
			prev = next;
			next = next->next;
			continue;
		}
		paid = 1;
		plyptr->copper[plyptr->cont] += next->amount;
		sprintf(message, "19008%c%d%c%s%c%s%c", MSGDEL, next->amount, MSGDEL, money_name, MSGDEL, strcmp(next->setby, plyptr->name) == 0?"yourself":next->setby, MSGDEL);
		send_message(message, plyptr->name, 1);
		if(next == bountyhead)
			bountyhead = next->next;
		if(prev)
			prev->next = next->next;
		if(next)
			free(next);
		if(prev)
			next = prev->next;
		else
			next = bountyhead;
	}
	if(paid == 1)
		save_bounty();
}

void read_bounty()
{
	bountylist new;
	bountylist *next;
	bountylist *prev = NULL;
	int fp;

	fprintf(stderr, "%s Reading %s\n", NOWSTR, BNTFIL);
	if((fp = open(BNTFIL, O_RDONLY)) == -1)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, BNTFIL, strerror(errno));
		return;
	}
	while((read(fp, &new, BNTSIZ)) == BNTSIZ)
	{
		if((next = (bountylist *)malloc(BNTSIZ)) == NULL)
		{
			fprintf(stderr, "%s %s\n", NOWSTR, ERRMSG1);
			break;
		}
		memcpy(next, &new, BNTSIZ);
		if(prev)
			prev->next = next;
		else
			bountyhead = next;
		next->next = NULL;
		prev = next;
	}
	close(fp);
}

void remove_bounty(int plynum)
{
	bountylist *next;
	bountylist *prev = NULL;
	int found = 0;

	if(bountyhead == NULL)
		return;
	next = bountyhead;
	while(next != NULL)
	{
		if(next->who != plynum)
		{
			prev = next;
			next = next->next;
			continue;
		}
		found = 1;
		if(next == bountyhead)
			bountyhead = next->next;
		if(prev)
			prev->next = next->next;
		if(next)
			free(next);
		if(prev)
			next = prev->next;
		else
			next = bountyhead;
	}
	if(found)
		save_bounty();
}

void save_bounty()
{
	bountylist *next;
	int fp;

#ifdef WIN32
	if((fp = open(BNTFIL, O_RDWR | O_CREAT | O_TRUNC, _O_BINARY, DFTPERM)) == -1)
#else
	if((fp = open(BNTFIL, O_RDWR | O_CREAT | O_TRUNC, DFTPERM)) == -1)
#endif
	{
		fprintf(stderr, "%s Error opening/creating %s: %s\n", NOWSTR, BNTFIL, strerror(errno));
		return;
	}
	if(bountyhead == NULL)
	{
		close(fp);
		return;
	}
	next = bountyhead;
	while(next != NULL)
	{
		if((write(fp, next, BNTSIZ)) != BNTSIZ)
		{
			fprintf(stderr, "%s Error writing %s: %s\n", NOWSTR, BNTFIL, strerror(errno));
			break;
		}
		next = next->next;
	}
	close(fp);
}

void show_bounty(char *contname, player *plyptr)
{
	char amount[16] = {0};
	int cont;
	bountylist *next;

	if(bountyhead == NULL)
	{
		sprintf(message, "19000%c%s%c", MSGDEL, adjectives[plyptr->gender][3], MSGDEL);
		send_message(message, plyptr->name, 1);
		return;
	}
	if((cont = find_cont(contname)) < 0)
		cont = plyptr->cont;
	sprintf(message, "19010%c%s%c", MSGDEL, conts[cont].name, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "19001%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	sprintf(message, "19002%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
	next = bountyhead;
	while(next)
	{
		if(next->cont == cont)
		{
			sprintf(amount, "%d", next->amount);
			sprintf(message, "19003%c%-20s%c%-20s%c%-8s%c%d%c", MSGDEL, players[next->who].nick, MSGDEL, next->setby, MSGDEL, amount, MSGDEL, next->dateset, MSGDEL);
			send_message(message, plyptr->name, 1);
		}
		next = next->next;
	}
	sprintf(message, "56%c%c", MSGDEL, MSGDEL);
	send_message(message, plyptr->name, 1);
}
