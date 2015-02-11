
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

int cmp_msgnum(const void *ptr1, const void *ptr2)
{
	return(STRCASECMP(((msgdef *)ptr1)->msgnum, ((msgdef *)ptr2)->msgnum));
}

void parse_msg(char *old, char *new)
{
	char msgnum[8] = {0};
	char *msg_index = NULL;
	char *msg_prev = NULL;
	char *next = NULL;
	char *old_index = NULL;
	char parm[512] = {0};
	int len;
	int newpos = 0;
	msgdef *mdfptr = NULL;

	if((old_index = strchr(old, MSGDEL)) == NULL)
	{
		strncpy(new, old, 1023);
		return;
	}
	strncpy(msgnum, old, (old_index - old > 7?7:old_index - old));
	if((mdfptr = (msgdef *)bsearch(msgnum, &msgdefs[0], totalmdf, MDFSIZ, cmp_msgnum)) == NULL)
		return;
	if((strcmp(mdfptr->msgnum, "999")) == 0)
	{
		strcpy(new, old_index + 1);
		return;
	}
	if((msg_index = strstr(mdfptr->msg, "%s")) == NULL)
	{
		strncpy(new, mdfptr->msg, 1023);
		return;
	}
	msg_prev = mdfptr->msg;
	while(msg_index != NULL && old_index != NULL)
	{
		if((strlen(new) + msg_index - msg_prev) > 2047)
			break;
		strncpy(new + newpos, msg_prev, msg_index - msg_prev);
		if((next = strchr(old_index + 1, MSGDEL)) == NULL)
			break;
		memset(parm, 0, strlen(parm));
		strncpy(parm, old_index + 1, next - old_index - 1);
		if((strncmp(msg_index, "%s^", 3)) == 0)
			len = 3;
		else
		{
			len = 2;
			if((strlen(new) + strlen(parm)) > 2047)
				break;
			strcat(new, parm);
		}
		old_index = next;
		msg_prev = msg_index + len;
		msg_index = strstr(msg_index + len, "%s");
		newpos = strlen(new);
	}
	if((strlen(new) + strlen(msg_prev)) < 2048)
		strcat(new, msg_prev);
}

void read_messages()
{
	char input[1024] = {0};
	char *ptr = NULL;
	FILE *fp;
	int current = 0;
	        
	if(msgdefs)
		free(msgdefs);
	totalmdf = 1;
	fprintf(stderr, "%s Reading %s\n", NOWSTR, MDFFIL);
	if((fp = fopen(MDFFIL, "r")) == NULL)
	{
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, MDFFIL, strerror(errno));
		EXIT_MESSAGE;
	}
	if((msgdefs = (msgdef *)calloc(1, MDFSIZ)) == NULL)
	{
		fprintf(stderr, "%s %s: %s\n", NOWSTR, ERRMSG1, strerror(errno));
		EXIT_MESSAGE;
	}
	while((fgets(input, 1024, fp)) != NULL)
	{
		if(input[0] == '#')
			continue;
		strip(input);
		if((ptr = strchr(input, '!')) == NULL)
			continue;
		if(current == totalmdf)
		{
			++totalmdf;
			if((msgdefs = (msgdef *)realloc(msgdefs, totalmdf * MDFSIZ)) == NULL)
			{
				fprintf(stderr, "%s %s: %s\n", NOWSTR, ERRMSG1, strerror(errno));
				EXIT_MESSAGE;
			}
		}
		memset(&msgdefs[current], 0, MDFSIZ);
		strncpy(msgdefs[current].msgnum, input, ptr - input);
		strncpy(msgdefs[current++].msg, ptr + 1, 1023);
	}
	fclose(fp);
	qsort(&msgdefs[0], totalmdf, MDFSIZ, cmp_msgnum);
}
