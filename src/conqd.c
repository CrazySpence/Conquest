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
#include"global.h"

int main(int argc, char *argv[])
{
	int count = 1;
	int do_init = 0;
	int events = 0;
	struct tm *pnt;
	time_t current = 0;

#ifdef DEBUG
	printf("DEBUG: Checking command line arguments\n");
#endif
	if(argc > 1)
	{
		while(count < argc)
		{
			if((strcmp(argv[count++], "INIT")) == 0)
			{
				do_init = 1;
				continue;
			}
			printf("%s: <INIT>\n", argv[0]);
			exit(1);
		}
	}
#ifndef WIN32
#ifdef DEBUG
	printf("DEBUG: Forking process\n");
#endif
	if(fork())
	{
		fprintf(stderr, "Conquest Server Started\n");
		exit(1);
	}
#endif
#ifdef DEBUG
	printf("DEBUG: Initializing log file\n");
#endif
	time(&current);
	strftime(NOWSTR, TIMESIZ, TIMEFORMAT, localtime(&current));
#ifndef WIN32
	if((freopen(LOGFILE, "w", stderr)) == NULL)
	{
		printf("Error opening %s: %s\n", LOGFILE, strerror(errno));
		exit_conquest();
	}
#endif
#ifdef DEBUG
	printf("DEBUG: Init server\n");
#endif
	fprintf(stderr, "%s Conquest server version %s started\n", NOWSTR, VER);
	fprintf(stderr, "%s Build %s\n", NOWSTR, BUILD);
	set_signals();
	read_dfts(do_init == 1?READ_NEWAGE:READ_NORMAL);
	init_server();
	read_classes();
	read_army();
	read_structures();
	if(do_init)
		init_age();
	else
	{
		pre_init();
		post_init();
	}
	init();
	if(do_init)
		clear_arthro();
	if(auto_ar >= 0 && auto_ar < conq_info.today)
		auto_ar = -1;
	fprintf(stderr, "%s Accepting connections\n", NOWSTR);
	send_heartbeat();
#ifdef DEBUG
	printf("DEBUG: Begin listen loop\n");
#endif
	while(1)
	{
		time(&current);
		pnt = localtime(&current);
		strftime(NOWSTR, TIMESIZ, TIMEFORMAT, pnt);
		do_events(&events, pnt);
		check_sockets();
		sockout = INVALID_SOCKET;
#ifndef WIN32
		switch(recv_sig)
		{
			case SIGHUP:
				recv_sig = -1;
				read_dfts(READ_SIGNAL);
				read_messages();
			default:
				break;
		}
#endif
	}
	return(0);
}

void accept_connect(SOCKET sock)
{
	char client_ip[16] = {0};
	char input[1024] = {0};
	FILE *fp;
	int clilen;
	int count;
	struct hostent *hostent;
	struct linger lstr;
	struct sockaddr_in cliaddr;

	SET_FUNCTION;
	for(count = SELF + 1; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock == INVALID_SOCKET)
			break;
	if(count == MAX_SOCKETS)
	{
		fprintf(stderr, "%s No more sockets available\n", NOWSTR);
		return;
	}
	clilen = sizeof(cliaddr);
	if((sockets[count].sock = accept(sock, (struct sockaddr *)&cliaddr, &clilen)) == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Error accepting client: %s\n", NOWSTR, SOCKET_ERRORMSG);
		free_sock(count);
		return;
	}
	strcpy(client_ip, inet_ntoa(cliaddr.sin_addr));
	fprintf(stderr, "%s Connection established, %s:%d\n", NOWSTR, client_ip, sockets[count].sock);
	if(use_dns == 1 && (hostent = gethostbyaddr((const void *)&cliaddr.sin_addr, sizeof(struct in_addr), AF_INET)) == NULL)
	{
		fprintf(stderr, "%s Error resolving %s\n", NOWSTR, client_ip);
		free_sock(count);
		return;
	}
	if((fp = fopen(BANFIL, "r")) == NULL)
		fprintf(stderr, "%s Error opening %s: %s\n", NOWSTR, BANFIL, strerror(errno));
	else
	{
		while((fgets(input, 1024, fp)) != NULL)
		{
			if(input[0] == '#')
				continue;
			strip(input);
			if((strncmp(client_ip, input, strlen(input))) == 0)
			{
				fclose(fp);
				fprintf(stderr, "%s Refused connection from %s\n", NOWSTR, client_ip);
				free_sock(count);
				return;
			}
		}
		fclose(fp);
	}
	sockets[count].ip = strdup(client_ip);
#ifndef WIN32
	if(fcntl(sockets[count].sock, F_SETFL, fcntl(sockets[count].sock, F_GETFL, NULL) | O_NONBLOCK) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error setting nonblock socket option(%d): %s\n", NOWSTR, sockets[count].sock, SOCKET_ERRORMSG);
		free_sock(count);
		return;
	}
#endif
	lstr.l_linger = 1;
	if(setsockopt(sockets[count].sock, SOL_SOCKET, SO_LINGER, (void *)&lstr, sizeof(struct linger)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error setting socket linger(%d): %s\n", NOWSTR, sockets[count].sock, SOCKET_ERRORMSG);
		free_sock(count);
		return;
	}
}

void add_msg(char *cmd, char *host, char *msg, char *user, int num)
{
	messages *new;
	messages *next;
	messages *prev;

	if(sockets[num].sock == INVALID_SOCKET || cmd == NULL || host == NULL || msg == NULL || user == NULL)
		return;
	if((new = (messages *)malloc(MSGSIZ)) == NULL)
	{
		fprintf(stderr, "%s %s: %s\n", NOWSTR, ERRMSG1, strerror(errno));
		return;
	}
	new->sock = sockets[num].sock;
	new->cmd = strdup(cmd);
	new->host = strdup(host);
	new->msg = strdup(msg);
	new->user = strdup(user);
	new->next = NULL;
	if(!msghead[num])
	{
		msghead[num] = new;
		return;
	}
	next = msghead[num]->next;
	prev = msghead[num];
	while(next != NULL)
	{
		prev = next;
		next = next->next;
	}
	prev->next = new;
}

void check_sockets()
{
	fd_set rd, wd;
	int count;
	int sel_num;
	struct timeval tv;

	SET_FUNCTION;
	FD_ZERO(&rd);
	FD_ZERO(&wd);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	for(count = 0; count < MAX_SOCKETS; ++count)
	{
		if(sockets[count].sock != INVALID_SOCKET)
		{
			FD_SET(sockets[count].sock, &rd);
			if(msghead[count])
				FD_SET(sockets[count].sock, &wd);
		}
	}
	if((sel_num = select(FD_SETSIZE, &rd, &wd, NULL, &tv)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Select failed: %s\n", NOWSTR, SOCKET_ERRORMSG);
		return;
	}
	if(sel_num == 0)
		return;
	for(count = 0; count < MAX_SOCKETS; ++count)
	{
		if(sockets[count].sock == INVALID_SOCKET)
			continue;
		if(FD_ISSET(sockets[count].sock, &rd))
			dataready(count);
		if(sockets[count].sock == INVALID_SOCKET)
			continue;
		if(FD_ISSET(sockets[count].sock, &wd))
			send_data((void *)&count);
	}
}

void clear_arthro()
{
	int count;
	int found;

	for(count = 0, found = 0; count < TOTHRO; ++count)
		if(heroes[count].owner != 0)
		{
			heroes[count].owner = 0;
			found = 1;
		}
	if(found)
		save_hero();
	for(count = 0, found = 0; count < TOTART; ++count)
		if((strlen(artifacts[count].name)) > 0 && artifacts[count].owner != 0)
		{
			artifacts[count].owner = 0;
			found = 1;
		}
	if(found)
		save_art();
}

void dataready(int num)
{
	char buf[512] = {0};
	char cmd[32] = {0};
	char host[128] = {0};
	char msg[256] = {0};
	char type[32] = {0};
	char user[32] = {0};

	SET_FUNCTION;
	if(num == SELF)
	{
		accept_connect(sockets[num].sock);
		return;
	}
	if((recv_data(buf, num)) > 0)
	{
		sockout = num;
		parse_xml(buf, cmd, host, msg, type, user);
		strcpy(sockets[num].type, type);
		parser(cmd, host, msg, type, user);
	}
}

void dcc_chat(char *host, int plynum)
{
	int sock;

	if(players[plynum].sock == INVALID_SOCKET || sockout == INVALID_SOCKET)
	{
		fprintf(stderr, "%s DCC error: Invalid socket\n", NOWSTR);
		return;
	}
	sock = find_sock(players[plynum].sock);
	if(sock >= 0 && STRCASECMP(sockets[sock].type, "IRC") == 0)
	{
		if(players[plynum].sock == sockets[sockout].sock)
			add_msg("DCC_CHAT", host, "", players[plynum].name, sock);
		else
		{
			sprintf(message, "999%cDCC connection already established.", MSGDEL);
			send_message(message, players[plynum].name, 0);
		}
	}
	else
	{
		sprintf(message, "999%cCannot establish DCC with your client", MSGDEL);
		send_message(message, players[plynum].name, 0);
	}
}

void dcc_close(int plynum)
{
	int sock;

	if(players[plynum].sock == INVALID_SOCKET)
	{
		fprintf(stderr, "%s DCC error: Invalid socket\n", NOWSTR);
		return;
	}
	sock = find_sock(players[plynum].sock);
	if(sock >= 0 && STRCASECMP(sockets[sock].type, "IRC") == 0)
		add_msg("DCC_CLOSE", server_ip, "", players[plynum].name, sock);
}

void do_events(int *events, struct tm *pnt)
{
	int count;
	int hktype = -1;

	SET_FUNCTION;
	if(pnt->tm_sec % 10 == 0)
		fflush(NULL);
	if(pnt->tm_min % 15 == 0)
	{
		if(!(*events & 1))
		{
			*events += 1;
			add_incr_move();
		}
	}
	else
		if(*events & 1)
			*events -= 1;
	if(pnt->tm_min == 30)
	{
		if(!(*events & 2))
		{
			*events += 2;
			send_heartbeat();
		}
		if(!(*events & 16))
		{
			*events += 16;
			reset_val();
		}
	}
	else
	{
		if(*events & 2)
			*events -= 2;
		if(*events & 16)
			*events -= 16;
	}
	if(pnt->tm_min == 0)
	{
		if(!(*events & 4))
		{
			srand((unsigned)time(NULL));
			for(count = 0; count < MAXHKS; ++count)
			{
				if(pnt->tm_hour == normal_hks[count])
				{
					hktype = 0;
					break;
				}
				if(pnt->tm_hour == ny_hks[count])
				{
					hktype = 1;
					break;
				}
			}
			if(hktype >= 0)
			{
				*events += 4;
				strcpy(conq_info.hkby, "Conquest");
				housekeeping(hktype);
			}
			for(count = 0; count < num_conts; ++count)
			{
				check_contspc(count);
				change_weather(count, -1, -1, -1);
			}
		}
	}
	else
		if(*events & 4)
			*events -= 4;
	if(restrict_combat == 1)
	{
		if(!(*events & 8))
		{
			*events += 8;
			reset_done();
		}
		else
			*events -= 8;
	}
}

void exit_conquest()
{
	int count;

	close(city_fp);
	close(info_fp);
	close(player_fp);
	if(logfile != NULL)
		fclose(logfile);
	fflush(NULL);
	for(count = 0; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock != INVALID_SOCKET)
			free_sock(count);
#ifdef WIN32
	WSACleanup();
#endif
	exit(1);
}

int find_sock(SOCKET sock)
{
	int count;

	if(sock == INVALID_SOCKET)
		return(INVALID_SOCKET);
	for(count = 0; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock == sock)
			break;
	if(count == MAX_SOCKETS)
		return(INVALID_SOCKET);
	else
		return(count);
}

void free_msg(int num)
{
	messages *last;
	messages *next;

	next = msghead[num];
	while(next != NULL)
	{
		last = next->next;
		if(next->msg)
			free(next->msg);
		if(next->user)
			free(next->user);
		free(next);
		next = last;
	}
	msghead[num] = NULL;
}

void free_sock(int num)
{
	int count;

	if(sockets[num].ip)
		free(sockets[num].ip);
	sockets[num].ip = NULL;
	for(count = 1; count < current_player; ++count)
		if(players[count].sock == sockets[num].sock)
			players[count].sock = INVALID_SOCKET;
	free_msg(num);
	if(sockets[num].sock == INVALID_SOCKET)
		return;
	shutdown(sockets[num].sock, CLOSE_FLAGS);
	CLOSE_SOCKET(sockets[num].sock);
	sockets[num].sock = INVALID_SOCKET;
}

void init_age()
{
	int cont;
	int plynum;

	SET_FUNCTION;
	fprintf(stderr, "%s Initializing Age\n", NOWSTR);
	srand((unsigned)time(NULL));
	init_files();
	pre_init();
	init_info();
	init_cities();
	for(cont = 0; cont < num_conts; ++cont)
	{
		if((strlen(emp[cont])) < 1)
			continue;
		plynum = add_player(emp[cont], "emp@conquestgamesite.com", cont, cont, -1);
		players[plynum].validate = NOT_VAL;
		strcpy(players[plynum].password, emp_passwd[cont]);
		make_titan(1, &players[plynum]);
		--totply[players[plynum].classnum];
		players[plynum].classnum = cont;
		++totply[players[plynum].classnum];
		players[plynum].troop_type = BASE_CLASSNUM(players[plynum].classnum);
		default_formation(cont, &players[plynum]);
		save_player(plynum);
	}
}

void init_cities()
{
	int cont;
	int count;
	int ctynum;
	int found;
	int tries;
	int troopnum;
	int trooptype;

	for(ctynum = 0; ctynum < num_cities; ++ctynum)
	{
		if(cities[ctynum].cont < 0)
			cities[ctynum].cont = 0;
		cont = cities[ctynum].cont;
		cities[ctynum].copper = randomnum(300000, 100000);
		cities[ctynum].lock = 0;
		cities[ctynum].tax = randomnum(15, 1);
		strcpy(cities[ctynum].name, city_names[ctynum]);
		if((strlen(tavern_names[ctynum])) > 0)
			strcpy(cities[ctynum].tname, tavern_names[ctynum]);
		else
			strcpy(cities[ctynum].tname, "Tavern");
		do
		{
			found = 0;
			cities[ctynum].locx = randomnum(10, 1);
			cities[ctynum].locy = randomnum(10, 1);
			for(count = 0; count < conts[cont].totalcty; ++count)
				if(conts[cont].cities[count] != ctynum && cities[conts[cont].cities[count]].locx == cities[ctynum].locx && cities[conts[cont].cities[count]].locy == cities[ctynum].locy)
				{
					found = 1;
					break;
				}
		} while(found == 1);
		for(count = 0; count < MAXMRK; ++count)
		{
			cities[ctynum].troop_num[count] = -1;
			cities[ctynum].troop_type[count] = -1;
		}
		open_othermarket(ctynum);
		if(ctynum != secret_city)
		{
			if(secret_city >= 0 && cont != cities[secret_city].cont)
				sprintf(cities[ctynum].clue, "rumor has it there is a secret city on %s", conts[cities[secret_city].cont].name);
			for(count = 0; count < (restrict_markets == 1?5:4); ++count)
				open_troopmarket(ctynum, -1, -1);
		}
		for(count = 0; count < (restrict_markets == 1?MAXMEC:2); ++count)
		{
			do
			{
				trooptype = market_type();
				troopnum = randomnum(MAXTRP - 1, MERC);
				if((strlen(units[trooptype][troopnum].name)) < 1)
					continue;
			} while((find_merc_market(ctynum, troopnum, trooptype)) >= 0);
			cities[ctynum].merc_num[count] = troopnum;
			cities[ctynum].merc_qty[count] = randomnum(4000, 1000);
			cities[ctynum].merc_type[count] = trooptype;
		}
		save_city(ctynum);
	}
	if(secret_city < 0)
		return;
	open_troopmarket(secret_city, BLOODGUARDNUM, FIGHTER);
	if(restrict_markets == 1)
	{
		if(allow_barb == 1)
			open_troopmarket(secret_city, BLOODGUARDNUM, BARBARIAN);
		if(allow_mage == 1)
			open_troopmarket(secret_city, BLOODGUARDNUM, MAGE);
		if(allow_cleric == 1)
			open_troopmarket(secret_city, BLOODGUARDNUM, CLERIC);
		if(allow_ranger == 1)
			open_troopmarket(secret_city, BLOODGUARDNUM, RANGER);
	}
	cities[secret_city].movepts_pr = 2000;
	cities[secret_city].movepts_qty = 25;
	cities[secret_city].peasant_pr = 800;
	cities[secret_city].peasant_qty = 1000;
	strcpy(cities[secret_city].clue, "you know, you are in the secret city");
	save_city(secret_city);
	cont = cities[secret_city].cont;
	for(count = 0; count < (int)strlen(city_names[secret_city]); ++count)
	{
		tries = 0;
		do
		{
			ctynum = randomnum(conts[cont].totalcty - 1, 0);
		} while((strlen(cities[conts[cont].cities[ctynum]].clue)) > 0 && tries++ < 1000);
		if(tries > 1000)
			break;
		sprintf(cities[conts[cont].cities[ctynum]].clue, "one of the letters in the name of the secret city is a(n) %c.", tolower(city_names[secret_city][count]));
		save_city(conts[cont].cities[ctynum]);
	}
}

void init_info()
{
	int cont;
	int count;

	memset(&conq_info, 0, INFOSIZ);
	conq_info.today = start_date;
	for(cont = 0; cont < MAXCNT; ++cont)
		for(count = 0; count < MAXSHP; ++count)
			conq_info.ship_qty[cont][count] = randomnum(MAXSHPINV / 4, MAXSHPINV / 10);
	strcpy(conq_info.hkby, "Conquest");
	strcpy(conq_info.hktime, "??????");
	for(count = 0; count < 10; ++count)
	{
		strcpy(conq_info.killed[count], "??????");
		strcpy(conq_info.killer[count], select_title(0, maxlevel[FIGHTER], 0));
	}
	sprintf(conq_info.motd, "Welcome to the %s Age of Conquest!", age);
	save_info();
}

void init_files()
{
	char files[][128] = {BNTFIL, CTYFIL, FSAVE, INFOFIL, KNGFIL, PSTFIL, ""};
	char fname[128] = {0};
	int count = 0;

	while((strlen(files[count])) > 0)
	{
		sprintf(fname, "%s.old", files[count]);
		remove(fname);
		rename(files[count], fname);
		++count;
	}
}

void init_server()
{
	char host[256] = {0};
	int size;
	long alt_ip;
	struct sockaddr_in servaddr;
	struct hostent *server;
#ifdef WIN32
	WSADATA wsdata;
	WORD version;

	version = MAKEWORD(1, 1);
	if(WSAStartup(version, &wsdata))
	{
		fprintf(stderr, "%s WSAStartup failed: %s\n", strerror(errno));
		exit_conquest();
	}
#endif
	SET_FUNCTION;
	if((strlen(stralt_ip)) > 0)
		strcpy(host, stralt_ip);
	else
		if((gethostname(host, 256)) == SOCKET_ERROR)
		{
			fprintf(stderr, "%s Error getting hostname: %s\n", NOWSTR, strerror(errno));
			exit_conquest();
		}
	if((server = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "%s Error resolving hostname\n", NOWSTR);
		exit_conquest();
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(port_num);
	strcpy(server_ip, inet_ntoa(*((struct in_addr *)server->h_addr)));
	if((strlen(stralt_ip)) > 0)
	{
		if((alt_ip = inet_addr(stralt_ip)) == SOCKET_ERROR)
			fprintf(stderr, "%s Error validating %s: %s\n", NOWSTR, stralt_ip, SOCKET_ERRORMSG);
		else
		{
			servaddr.sin_addr.s_addr = alt_ip;
			strcpy(server_ip, stralt_ip);
		}
	}
	fprintf(stderr, "%s Server address %s\n", NOWSTR, server_ip);
	fprintf(stderr, "%s Creating socket on port %d\n", NOWSTR, port_num); 
	if((sockets[SELF].sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Error creating server socket: %s\n", NOWSTR, SOCKET_ERRORMSG);
		exit_conquest();
	}
	size = 1;
	if(setsockopt(sockets[SELF].sock, SOL_SOCKET, SO_REUSEADDR, (void *)&size, sizeof(size)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error setting server socket reuse: %s\n", NOWSTR, SOCKET_ERRORMSG);
		exit_conquest();
	}
	fprintf(stderr, "%s Binding socket to port\n", NOWSTR);
	if((bind(sockets[SELF].sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error binding server socket: %s\n", NOWSTR, SOCKET_ERRORMSG);
		exit_conquest();
	}
	if(listen(sockets[SELF].sock, 128) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error listening on server socket: %s\n", NOWSTR, SOCKET_ERRORMSG);
		exit_conquest();
	}
#ifndef WIN32
	if(fcntl(sockets[SELF].sock, F_SETFL, fcntl(sockets[SELF].sock, F_GETFL, NULL) | O_NONBLOCK) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error setting server socket options: %s\n", NOWSTR, SOCKET_ERRORMSG);
		exit_conquest();
	}
#endif
}

void kick(char *msg, int plynum)
{
	int sock;

	if(players[plynum].sock == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Kick error: Invalid socket\n", NOWSTR);
		return;
	}
	sock = find_sock(players[plynum].sock);
	if(sock >= 0 && STRCASECMP(sockets[sock].type, "IRC") == 0)
		add_msg("KICK", server_ip, msg, players[plynum].name, sock);
}

void op(int plynum)
{
	int sock;

	if(players[plynum].sock == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Op error: Invalid socket\n", NOWSTR);
		return;
	}
	sock = find_sock(players[plynum].sock);
	if(sock >= 0 && STRCASECMP(sockets[sock].type, "IRC") == 0)
		add_msg("OP", server_ip, "", players[plynum].name, sock);
}

int recv_data(char *buf, int num)
{
	char input[512] = {0};
	char *ptr = NULL;
	int count;
	int pos;
	int total;

	SET_FUNCTION;
	if((total = recv(sockets[num].sock, input, 512, MSG_PEEK)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error receiving data(%d): %s\n", NOWSTR, sockets[num].sock, SOCKET_ERRORMSG);
		free_sock(num);
		return(0);
	}
	if(total == 0)
	{
		free_sock(num);
		return(0);
	}
	if((ptr = strchr(input, '\n')) == NULL)
	{
		free_sock(num);
		return(0);
	}
	if((total = recv(sockets[num].sock, input, (ptr - input + 1), 0)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error receiving data(%d): %s\n", NOWSTR, sockets[num].sock, SOCKET_ERRORMSG);
		free_sock(num);
		return(0);
	}
	if(total == 0)
	{
		free_sock(num);
		return(0);
	}
	*ptr = 0;
	for(count = 0, pos = 0; count < (int)strlen(input); ++count)
	{
		if(input[count] != MSGDEL && !isprint(input[count]))
			continue;
		buf[pos++] = input[count];
	}
	return(pos);
}

void send_channel(char *buf, int critical)
{
	char newmsg[2056] = {0};
	int count;

	if(logfile != NULL)
	{
		parse_msg(buf, newmsg);
		fprintf(logfile, "%s %s\n", NOWSTR, newmsg);
	}
	for(count = SELF + 1; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock != INVALID_SOCKET)
			add_msg("CHANNEL_MSG", server_ip, buf, "CONQUEST", count);
}

void send_data(void *data)
{
	char outbuf[1024] = {0};
	int num;
	messages *next;

	SET_FUNCTION;
	num = *(int *)data;
	if(!msghead[num])
		return;
	build_xml(outbuf, msghead[num]->cmd, msghead[num]->host, msghead[num]->msg, msghead[num]->user);
	if(send(msghead[num]->sock, outbuf, strlen(outbuf), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error sending data(%d): %s\n", NOWSTR, sockets[num].sock, SOCKET_ERRORMSG);
		free_sock(num);
		return;
	}
	next = msghead[num]->next;
	if(msghead[num]->user)
		free(msghead[num]->user);
	if(msghead[num]->user)
		free(msghead[num]->cmd);
	if(msghead[num]->host)
		free(msghead[num]->host);
	if(msghead[num]->msg)
		free(msghead[num]->msg);
	free(msghead[num]);
	msghead[num] = next;
}

void send_heartbeat()
{
	char outbuf[1024] = {0};
	SOCKET sock;
	struct hostent *hostent;
	struct sockaddr_in servaddr;

	if(strlen(master_server) < 1)
		return;
	if((hostent = gethostbyname(master_server)) == NULL)
	{
		fprintf(stderr, "%s Error resolving master server\n", NOWSTR);
		return;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr = *(struct in_addr *)hostent->h_addr;
	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(MASTER_PORT);
	if((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "%s Error creating heartbeat socket: %s\n", NOWSTR, SOCKET_ERRORMSG);
		return;
	}
	if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		fprintf(stderr, "%s Error connecting to Master Server: %s\n", NOWSTR, SOCKET_ERRORMSG);
		shutdown(sock, CLOSE_FLAGS);
		CLOSE_SOCKET(sock);
		return;
	}
	fprintf(stderr, "%s Sending heartbeat to %s\n", NOWSTR, master_server);
	sprintf(outbuf, "%d ", port_num);
#ifdef WIN32
	strcat(outbuf, "WIN32\n");
#else
	strcat(outbuf, "UNIX\n");
#endif
	if((send(sock, outbuf, strlen(outbuf), 0)) == SOCKET_ERROR)
		fprintf(stderr, "%s Error sending heartbeat: %s\n", NOWSTR, SOCKET_ERRORMSG);
	shutdown(sock, CLOSE_FLAGS);
	CLOSE_SOCKET(sock);
}

void send_message(char *msg, char *user, int valcmd)
{
	char newmsg[2048] = {0};
	int socknum;
	int plynum;

	if(valcmd == 1)
	{
		if((plynum = find_player(user)) < 1)
			return;
		if(players[plynum].validate == VAL_STEWARD)
		{
			parse_msg(msg, newmsg);
			log_event("", newmsg, players[plynum].name);
			return;
		}
		if((socknum = find_sock(players[plynum].sock)) == INVALID_SOCKET)
			socknum = sockout;
		if(players[plynum].validate == NOT_VAL || players[plynum].sock == INVALID_SOCKET)
			return;
	}
	else
		socknum = sockout;
	add_msg("PRIVATE_MSG", server_ip, msg, user, socknum);
}

void set_mode()
{
	char today[8] = {0};
	int count;

	sprintf(today, "%d", conq_info.today);
	for(count = SELF + 1; count < MAX_SOCKETS; ++count)
		if(sockets[count].sock != INVALID_SOCKET)
			add_msg("SETMODE", server_ip, today, "unknown", count);
}

void set_signals()
{
	fprintf(stderr, "%s Trapping signals\n", NOWSTR);
#ifdef WIN32
	signal(SIGBREAK, signal_shutdown);
#else
	signal(SIGBUS, signal_fatal);
	signal(SIGHUP, signal_reset);
	signal(SIGIOT, signal_fatal);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, signal_fatal);
	signal(SIGSTKFLT, signal_fatal);
	signal(SIGURG, signal_fatal);
#endif
	signal(SIGABRT, signal_fatal);
	signal(SIGFPE, signal_fatal);
	signal(SIGILL, signal_fatal);
	signal(SIGINT, signal_fatal);
	signal(SIGSEGV, signal_fatal);
	signal(SIGTERM, signal_shutdown);
}
