
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

#ifdef WIN32
char *convert_wsaerror(int errnum)
{
	switch(errnum)
	{
		case WSAEACCES:
			strcpy(message, "Permission denied");
			break;
		case WSAEADDRINUSE:
			strcpy(message, "Address already in use");
			break;
		case WSAECONNREFUSED:
			strcpy(message, "Connection refused");
			break;
		case WSAECONNRESET:
			strcpy(message, "Connection reset by peer");
			break;
		case WSAEFAULT:
			strcpy(message, "Bad address");
			break;
		case WSAEHOSTUNREACH:
			strcpy(message, "No route to host");
			break;
		case WSAENOTSOCK:
			strcpy(message, "Socket operation on non-socket");
			break;
		case WSAETIMEDOUT:
			strcpy(message, "Connection timed out");
			break;
		case WSAHOST_NOT_FOUND :
			strcpy(message, "Host not found");
			break;
		default:
			sprintf(message, "%d", errnum);
			break;
	}
	return(message);
}
#endif
