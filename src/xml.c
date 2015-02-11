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

void build_xml(char *buf, char *cmd, char *host, char *msg, char *user)
{
	sprintf(buf, "<?xml version=\"%s\"?><message><type>%s</type><user>%s</user><cmd>%s</cmd><host>%s</host><msg>%s</msg></message>\n", 
	XMLVER, XMLTYPE, user, cmd, host, msg);
}

void parse_xml(char *buf, char *cmd, char *host, char *msg, char *type, char *user)
{
	char *eptr;
	char *sptr;
	char etypes[][8] = {"</type>", "</user>", "</cmd>", "</host>", "</msg>", ""};
	char stypes[][8] = {"<type>", "<user>", "<cmd>", "<host>", "<msg>", ""};
	int count = 0;
	int len;
	int sizes[] = {31, 31, 31, 127, 255};
	int start;

	if((sptr = strstr(buf, "<message>")) == NULL)
	{
		fprintf(stderr, "%s Error parsing XML (missing <message> tag): %s\n", NOWSTR, buf);
		return;
	}
	if((eptr = strstr(buf, "</message>")) == NULL)
	{
		fprintf(stderr, "%s Error parsing XML (missing </message> tag): %s\n", NOWSTR, buf);
		return;
	}
	while((strlen(stypes[count])) > 0)
	{
		if((sptr = strstr(buf, stypes[count])) == NULL)
		{
			++count;
			continue;
		}
		if((eptr = strstr(buf, etypes[count])) == NULL)
		{
			++count;
			continue;
		}
		start = strlen(stypes[count]);
		if((len = eptr - (sptr + start)) > sizes[count])
			len = sizes[count];
		switch(count)
		{
			case 0:
				strncpy(type, sptr + start, len);
				break;
			case 1:
				strncpy(user, sptr + start, len);
				break;
			case 2:
				strncpy(cmd, sptr + start, len);
				break;
			case 3:
				strncpy(host, sptr + start, len);
				break;
			case 4:
				strncpy(msg, sptr + start, len);
				break;
		}
		++count;
	}
}
