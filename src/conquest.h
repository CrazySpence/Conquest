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

/*****************************************************************************
	Includes
*****************************************************************************/
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stddef.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/stat.h>
#include<time.h>
#include<limits.h>
#include<errno.h>
#include<malloc.h>
#include<signal.h>
#ifdef WIN32
#define FD_SETSIZE 128
#include<winsock2.h>
#include<io.h>
#include<direct.h>
#else
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>
#endif
#include<fcntl.h>
#include"build.h"
#include"message.h"

/*****************************************************************************
	Maximums
*****************************************************************************/
#define MAXADV 4
#define MAXCBT 8
#define MAXCLS 8
#define MAXCMD 192
#define MAXCMP 6
#define MAXCNT 4
#define MAXCTY 64
#define MAXFLT 8
#define MAXHNR 100
#define MAXITMINV 25
#define MAXLINES 20
#define MAXMEC 3
#define MAXMRK 8
#define MAXRET 32
#define MAXRNK 10
#define MAXSHP 16
#define MAXSHPINV 20
#define MAXTRP 8
#define MAXVAS 16
#define TOTITM 38

/*****************************************************************************
	Structures
*****************************************************************************/
#ifndef WIN32
typedef int SOCKET;
#endif

typedef struct _armytype
{
	int formation[MAXTRP][MAXTRP];
	int total[MAXTRP];
} armytype;

typedef struct _artifact
{
	int city;
	int owner;
	char location[128];
	char name[32];
} artifact;

typedef struct _bountylist
{
	int amount;
	int cont;
	int dateset;
	int who;
	char setby[32];
	struct _bountylist *next;
} bountylist;

typedef struct _chanlist
{
	char nick[16];
	struct _chanlist *next;
} chanlist;

typedef struct _city
{
	int cont;
	int copper;
	int food_buy;
	int food_pr;
	int food_qty;
	int artifact;
	int locx;
	int locy;
	int lock;
	int merc_num[MAXMEC];
	int merc_pr[MAXMEC];
	int merc_qty[MAXMEC];
	int merc_type[MAXMEC];
	int movepts_pr;
	int movepts_qty;
	int owner;
	int peasant_pr;
	int peasant_qty;
	int spy_pr;
	int spy_qty;
	int tax;
	int troop_num[MAXTRP];
	int troop_pr[MAXTRP];
	int troop_qty[MAXTRP];
	int troop_type[MAXTRP];
	char clue[128];
	char name[16];
	char tname[32];
} city;

typedef struct _commandlist
{
	char cmdname[32];
	int num;
} commandlist;

typedef struct _continent
{
	int cities[MAXCTY];
	int port;
	int totalcty;
	char name[16];
} continent;

typedef struct _hero
{
	int alive;
	int cost;
	int owner;
	int serve;
	char name[16];
	char type[32];
} hero;

typedef struct _hosts
{
	char *host;
	struct _hosts *next;
} hosts;

typedef struct _krakens
{
	int alive;
	int damage;
	int health;
	int maxdamage;
	int maxhealth;
} krakens;

typedef struct _info
{
	int today;
	int agewon;
	int balance[10];
	int cont_special[MAXCNT];
	int item_qty[MAXCNT][TOTITM];
	int ranks[MAXRNK];
	int riddlenum;
	int ship_qty[MAXCNT][MAXSHP];
	int vampire;
	char hkby[32];
	char hktime[32];
	char killed[10][32];
	char killer[10][32];
	char motd[256];
	krakens kraken;
} info;

typedef struct _items
{
	int active;
	int cost;
	int mpcost;
	int uses;
	char keyword[16];
	char name[64];
} items;

typedef struct _loginfo
{
	char time[32];
	char message[256];
} loginfo;

typedef struct _messages
{
	SOCKET sock;
	char *cmd;
	char *host;
	char *msg;
	char *user;
	struct _messages *next;
} messages;

typedef struct _msgdef
{
	char msgnum[8];
	char msg[1024];
} msgdef;

typedef struct _retaliation
{
	int attacks;
	int plynum;
} retaliation;

typedef struct _ship
{
	int type;
	int hull;
	int cargo;
	int cargotype;
	char name[32];
} ship;

typedef struct _vassaltype
{
	int plynum[MAXVAS];
	int release[MAXVAS];
	int total;
} vassaltype;

typedef struct _player
{
	int ally;
	int ally_request;
	int attacks;
	int battles_lost;
	int battles_won;
	int city;
	int classnum;
	int cont;
	int fleet;
	int gender;
	int isprotected;
	int lord;
	int kingdoms;
	int levelmax;
	int movepts;
	int ninja;
	int remorts;
	int special_batmod;
	int special_kingdom;
	int special_misc;
	int troops_lost;
	int troops_killed;
	int troop_type;
	int trynum;
	int validate;
	SOCKET sock;
	time_t can_attack;
	time_t done;
	time_t lastattack;
	time_t lastplay;
	time_t vacation;
	int attacks_lost[MAXCNT];
	int castle[MAXCNT];
	int copper[MAXCNT];
	int event[MAXCNT];
	int first_play[MAXCNT];
	int food[MAXCNT];
	int honor[MAXCNT];
	int king[MAXCNT];
	int land[MAXCNT];
	int level[MAXCNT];
	int peasant[MAXCNT];
	int promotion[MAXCNT];
	int spy[MAXCNT];
	int tavern[MAXCNT];
	int tax[MAXCNT];
	int components[MAXCMP];
	int items[TOTITM];
	float surrender[MAXCNT];
	char kname[MAXCNT][32];
	char name[32];
	char nick[32];
	char password[32];
	char host[128];
	char steward[MAXCNT][3][64];
	char winmsg[256];
	char winsound[32];
	armytype army[2][MAXCNT];
	retaliation ret[MAXRET];
	ship ships[MAXFLT];
	vassaltype vassals;
} player;

typedef struct _riddle
{
	char clue[512];
	char solution[16];
} riddle;

typedef struct _shiptype
{
	int cost;
	int damage;
	int hull;
	int maxcargo;
	int maxdamage;
	int mpcost;
	int speed;
	int type;
	char modifier[16];
	char name[16];
} shiptype;

typedef struct _socktype
{
	SOCKET sock;
	char *ip;
	char type[8];
} socktype;

typedef struct _spell
{
	int attack;
	int components;
	int level;
	int move;
	char name[32];
} spell;

typedef struct _structure
{
	char desc[128];
	char name[16];
	int cost;
	int land;
	int mpcost;
	int tax;
	int troops;
} structure;

typedef struct _unit
{
	int ambush;
	int attack;
	int copper;
	int defend;
	int food;
	int mode;
	int num_attacks;
	int range;
	int regenerate;
	int shield;
	int swarm;
	int train;
	char desc[256];
	char name[32];
} unit;

typedef struct _weather
{
	float food;
	int humidity;
	int joust;
	int msgnum;
	int sail_damage;
	int temp;
	int temperature;
	int type;
	int violent;
	int wind;
} weather;

/*****************************************************************************
	Constants
*****************************************************************************/
/*
#define DEBUG 1
*/

#define CLASSCHANGE 3

#define READ_NEWAGE 0
#define READ_NORMAL 1
#define READ_SIGNAL 2

#define CARGOSHIP 0
#define ATTACKSHIP 1
#define ANYSHIP 2

#define KRAKENCOPPER 500000
#define PENANCECOPPER 100000
#define SEARCHCOPPER 20000

#define GENSING 0
#define CARDAMOM 1
#define SAGE 2
#define NIGHTSHADE 3
#define MANDRAKE 4
#define ETHER 5

#define WOLF 0
#define BEAR 1
#define LION 2
#define GIANT 3
#define DRAGON 4
#define BEHEMOTH 6

#define HORDE 0
#define UNDEAD 1
#define PACK 2
#define CRUSADERS 3
#define GARRISON 4
#define MERCENARY 5
#define IMMORTAL 6

#define ATTACKER 0
#define ATTACKER_ALLY 1
#define ATTACKER_HORDE1 2
#define ATTACKER_HORDE2 3
#define DEFENDER 4
#define DEFENDER_ALLY 5
#define DEFENDER_HORDE1 6
#define DEFENDER_HORDE2 7

#define WEAK 0
#define MEDIOCRE 1
#define STRONG 2
#define MIGHTY 3

#define PLOW 0
#define BOW 1
#define CHEST 2
#define SWORD 3
#define MIRROR 4
#define DAGGER 5
#define AEGIS 6
#define STAFF 7
#define HAMMER 8
#define GRAIL 9
#define ROD 10
#define WINGS 11
#define BOOTS 12
#define ORB 13
#define CROWN 14
#define DSHIELD 15
#define LANCE 16
#define NIGHTBLADE 17

#define HERO_WKNIGHT 0
#define HERO_WARLOCK 1
#define HERO_PRIESTESS 2
#define HERO_FARMER 3
#define HERO_BARBLEADER 4
#define HERO_HIGHLORD 5
#define HERO_GIANT 6
#define HERO_NINJA 7
#define HERO_GENERAL1 8
#define HERO_GENERAL2 9
#define HERO_DESTROYER 10
#define HERO_ALCHEMIST 11
#define HERO_DIPLOMAT 12
#define HERO_VAMPIRE 13
#define HERO_DRAGON 14
#define HERO_NECROMANCER 15
#define HERO_QUEENSPIDER 16
#define HERO_LORDCHAOS 17
#define HERO_MERCGENERAL 18
#define HERO_MERCHANTKING 19
#define HERO_BISHOP 20
#define HERO_TITAN 21

#define ARTSIZ (sizeof(artifact))
#define BNTSIZ (sizeof(bountylist))
#define CHNSIZ (sizeof(chanlist))
#define CMDSIZ (sizeof(commandlist))
#define CNTSIZ (sizeof(continent))
#define CTYSIZ (sizeof(city))
#define HROSIZ (sizeof(hero))
#define HSTSIZ (sizeof(hosts))
#define INFOSIZ (sizeof(info))
#define ITEMSIZ (sizeof(items))
#define LOGSIZ (sizeof(loginfo))
#define MDFSIZ (sizeof(msgdef))
#define MSGSIZ (sizeof(messages))
#define PLYSIZ (sizeof(player))
#define RIDSIZ (sizeof(riddle))
#define SPLSIZ (sizeof(spell))
#define SHTSIZ (sizeof(shiptype))
#define SHPSIZ (sizeof(ships))
#define STRSIZ (sizeof(structure))
#define UNTSIZ (sizeof(unit))
#define VASSIZ (sizeof(vassal))

#define ARMFIL "conquest.armies"
#define ARTFIL "conquest.artifacts"
#define BANFIL "conquest.banned"
#define BNTFIL "conquest.bounty"
#define CLAFIL "conquest.classes"
#define CTYFIL "conquest.city"
#define DFTFIL "conqd.conf"
#define FBACK "conquest.save.bak"
#define FSAVE "conquest.save"
#define HKLOG "conquest.hk"
#define HROFIL "conquest.heroes"
#define INFOFIL "conquest.info"
#define ITEMFIL "conquest.items"
#define KNGFIL "conquest.kings"
#define LEGFIL "conquest.legend"
#define MDFFIL "conquest.messages"
#define NEWFIL "conquest.news"
#define PROFIL "conquest.promo"
#define PSTFIL "conquest.posts"
#define RIDFIL "conquest.riddles"
#define SHPFIL "conquest.ships"
#define SPLFIL "conquest.spells"
#define SRVFIL "conquest.servers"
#define STRFIL "conquest.structures"
#define TIPFIL "conquest.tips"
#define TITFIL "conquest.titans"

#define ATTACKTIME 15
#define BASETAX 500
#define DEMIGOD 60
#define DFTMAGE "Anshar the Wise"
#define DFTPWD "None"
#define DONETIME 300
#define INCREMENT 8
#define MINVASRANK 3
#define REIGN 10
#define TIMESIZ 64
#define VASRANK 5
#ifdef WIN32
#define DFTPERM _S_IREAD | _S_IWRITE
#define VER "3.16C (WIN32)"
#else
#define DFTPERM S_IREAD | S_IWRITE
#define VER "3.16C (UNIX)"
#endif
#define XMLTYPE "SERVER"
#define XMLVER "1.0"

#define NOCARGO -1
#define CARGOREN 0
#define CARGOSOLDIER 1
#define CARGOARCHER 2
#define CARGOKNIGHT 3
#define CARGOCAT 5
#define CARGOMERC1 6
#define CARGOMERC2 7
#define CARGOPEASANT 8
#define CARGOMONEY 9
#define CARGOFOOD 10

#define ITEM_CHAOS 1
#define ITEM_EYE 2
#define ITEM_SPEED 3
#define ITEM_STONE 4
#define ITEM_ORB 5
#define ITEM_COW 6
#define ITEM_TRUMPET 7
#define ITEM_CARPET 8
#define ITEM_SCEPTRE 9
#define ITEM_ENDURANCE 10
#define ITEM_QUIVER 11
#define ITEM_BUILDERS 12
#define ITEM_AXE 13
#define ITEM_THIEVES 14
#define ITEM_LURE 15
#define ITEM_MASK 16
#define ITEM_WAND 17
#define ITEM_DUST 18
#define ITEM_BOAT 19
#define ITEM_BLADE 20
#define ITEM_AMULET 21
#define ITEM_RING 22
#define ITEM_POUCH 23
#define ITEM_GAUNTLETS 24
#define ITEM_TRICKS 25
#define ITEM_HOLDING 26
#define ITEM_TOME 27
#define ITEM_SIREN 28
#define ITEM_LETTER 29
#define ITEM_BANNER 30
#define ITEM_SHIELD 31
#define ITEM_FLAG 32
#define ITEM_EMBLEM 33
#define ITEM_BOULDER 34
#define ITEM_COWL 35
#define ITEM_VIAL 36
#define ITEM_WART 37

#define MAXALLY 3
#define MAXCOMPONENT 10
#define MAXCOPPER 50000000
#define MAXFOOD 100000
#define MAXHKS 24
#define MAXMSG 5
#define MAXPEASANT 100000
#define MAXRID 24
#define MAXSRV 4

#define TOTART 18
#define TOTHRO 22
#define TOTSPL 34

#define ALLIANCE 4
#define ALLY 1
#define ATTACK 5
#define BESTOW 4
#define BUY_ITEM 1
#define DEVOTE 20
#define ENGAGE 6
#define FLEET 1
#define GIVE 2
#define HIRE 1
#define LOADSHIP 2
#define LOOT 4
#define MARKET 1
#define MESSAGE 0
#define MYSTIC 1
#define POPULATION 1
#define PRAY 2
#define QUEST_0 2
#define QUEST_1 4
#define QUEST_2 4
#define REBEL 10
#define REPAIR 3
#define REVIEW 1
#define ROLL 1
#define SAIL 2
#define SEARCH 10
#define SPY 2
#define TAVERN 1
#define UNLOAD 2
#define CHARM 15
#define PESTILENCE 10
#define SEEK 4
#define SLAY 15
#define BLESS 10
#define CRUSADE 20
#define DIVINATION 2
#define DIVINEINTERVENTION 30
#define EXCOMMUNICATE 25
#define GRACE 2
#define RESURRECT 15
#define SANCTUARY 10
#define TURNUNDEAD 6
#define VOCATION 15
#define SERVE 10
#define SHIELD 10
#define SUMMON 7
#define TEND 4
#define VISION 3
#define QUEST_5 15
#define COMMUNE 8
#define BERSERK 7
#define PENANCE 15
#define BESIEGE 10
#define REVIVE 20
#define TAX 1
#define QUEST_6 20
#define FORMATION 1
#define REPRIEVE 1
#define BOUNTY 6
#define STEWARD 5
#define DESTROY_ITEM 3
#define INVADE 15
#define RITUAL 20
#define BLOCKADE 30
#define BLITZ 30
#define CEASEFIRE 30
#define TRANSFER 1

#define RENNUM 0
#define SOLDIERNUM 1
#define ARCHERNUM 2
#define KNIGHTNUM 3
#define BLOODGUARDNUM 4
#define CATAPULTNUM 5
#define MERC 6
#define MERC2 7

#define MODE_WALK 0
#define MODE_RIDE 1
#define MODE_FLY 2
#define MODE_RNG 3

#define REVIEW_ALL 0
#define REVIEW_NOBG 1

#define EMPLOG "logs/emp.log"
#define LOGFILE "conqd.log"
#define MASTER_PORT 8888
#define MAX_SOCKETS 32
#define MSGDEL '\x10'
#define PLYLOG "logs/player.log"
#define RECV_TIMEOUT 8
#define SELF 0

#define CNTLC 3
#define WHITE 0
#define BLACK 1
#define BLUE 2
#define GREEN 3
#define RED 4
#define BROWN 5
#define PURPLE 6
#define ORANGE 7
#define YELLOW 8
#define LTGREEN 9
#define TEAL 10
#define CYAN 11
#define LTBLUE 12
#define PINK 13
#define GREY 14
#define LTGREY 15

#define FIGHTER 0
#define BARBARIAN 1
#define MAGE 2
#define VAMPIRE 3
#define CLERIC 4
#define RANGER 5
#define TITAN 6
#define SUMMONED 7

#define W_DRY 0
#define W_NORMAL 1
#define W_MOIST 2
#define W_HEAVY 3
#define W_FREEZE 0
#define W_COLD 1
#define W_TEMPERATE 2
#define W_HOT 3
#define W_STILL 0
#define W_BREEZY 1
#define W_WINDY 2
#define W_GUSTY 3

#define WT_FREEZE 0
#define WT_SNOW 1
#define WT_RAIN 2
#define WT_STORM 3
#define WT_HOT 4
#define WT_FOG 5
#define WT_DRY 6
#define WT_COOL 7
#define WT_WIND 8
#define WT_WARM 9
#define WT_PERFECT 10

#define STEWARD_HK 0
#define STEWARD_LOSE 1
#define STEWARD_WIN 2

#define NOT_VAL 0
#define VAL_NORMAL 1
#define VAL_STEWARD 2
#define VAL_VAMPIRE 3
#define VAL_QUESTCHAR 4

#define EVENT_NONE 0
#define EVENT_MERCHANT 1
#define EVENT_SOLDIERS 2
#define EVENT_RECOVERFOOD 3
#define EVENT_REPAIRCAT 4
#define EVENT_PEASANTS 5
#define EVENT_EXPLORERS 6
#define EVENT_FINDSHIP 7
#define EVENT_GRACE 8
#define EVENT_FINDCASTLE 9
#define EVENT_PLAGUE 10
#define EVENT_LOCUSTS 11
#define EVENT_UPRISING 12
#define EVENT_ARSON 13
#define EVENT_THIEVES 14
#define EVENT_SICKNESS 15
#define EVENT_POISON 16


/* Misc specials */
#define SPC_GUISE 1
#define SPC_SEEK 2
#define SPC_TIP 4
#define SPC_COLOR 8
#define SPC_OPS 16
#define SPC_RITUAL 32
#define SPC_LOCK 64
#define SPC_VACATION 128
#define SPC_MESSAGES 256
#define SPC_QUESTCHAR 512

/* Continent specials */
#define SPC_BLOCKADE 1
#define SPC_BLITZ 2
#define SPC_CEASEFIRE 4

#define BASE_CLASSNUM(classnum) (troop_type(classnum))
#define EXIT_MESSAGE exit_conquest()
#ifdef WIN32
#define CLOSE_FLAGS SD_BOTH
#define CLOSE_SOCKET(sock) closesocket(sock)
#define SET_FUNCTION strcpy(function_name, __FILE__)
#define SOCKET_ERRORMSG convert_wsaerror(WSAGetLastError())
#define STRCASECMP(str1, str2) _stricmp(str1, str2)
#define STRNCASECMP(str1, str2, len) _strnicmp(str1, str2, len)
#else
#define CLOSE_FLAGS SHUT_RDWR
#define CLOSE_SOCKET(sock) close(sock)
#define SET_FUNCTION strcpy(function_name, __FUNCTION__)
#define SOCKET_ERRORMSG strerror(errno)
#define STRCASECMP(str1, str2) strcasecmp(str1, str2)
#define STRNCASECMP(str1, str2, len) strncasecmp(str1, str2, len)
#endif
#define TIMEFORMAT "[%b %d %H:%M:%S %Z]"

/*****************************************************************************
	Global variables
*****************************************************************************/
	extern char age[];
	extern char basenames[][16];
	extern char city_names[][16];
	extern char components[][16];
	extern char dft_kname[];
	extern char dft_sname[];
	extern char dft_winmsg[];
	extern char elitenames[][16];
	extern char emp[][32];
	extern char emp_passwd[][32];
	extern char food_name[];
	extern char log_file[];
	extern char master_server[];
	extern char money_name[];
	extern char peasant_name[];
	extern char pirate_names[][16];
	extern char stralt_ip[];
	extern char tavern_names[][32];
	extern char classnames[][16];
	extern char b_titles[][10][16];
	extern char c_titles[][10][16];
	extern char f_titles[][10][16];
	extern char m_titles[][10][16];
	extern char r_titles[][10][16];
	extern char v_titles[][10][16];

	FILE *logfile;
	extern float dft_surrender;
	extern int advanced_armies;
	extern int advanced_combat;
	extern int allow_barb;
	extern int allow_cleric;
	extern int allow_formation;
	extern int allow_mage;
	extern int allow_ranger;
	extern int allow_surrender;
	extern int allow_vampire; 
	extern int associate_races;
	extern int attack_reprieve;
	extern int attack_time;
	extern int auto_ar;
	extern int capture_foreign;
	extern int class_change[];
	extern int end_date;
	extern int enforce_peace;
	extern int fog_of_war;
	extern int force_cont[]; 
	extern int max_player;
	extern int mps_15;
	extern int mult_vampires;
	extern int normal_hks[];
	extern int num_cities;
	extern int num_conts;
	extern int num_remorts;
	extern int num_riddles;
	extern int num_ships;
	extern int ny_hks[];
	extern int promote_knights;
	extern int quest_chars;
	extern int quest_riddle;
	extern int promote_protection;
	extern int remove_items;
	extern int restrict_combat;
	extern int restrict_markets;
	extern int sail_protection;
	extern int start_date;
	extern int use_dns;
	extern unsigned short port_num;

	extern int ARMY_DEFENSE;
	extern int ARMY_CAMPAIGN;

	extern char NOWSTR[];
	extern int sockout;
	extern messages *msghead[];
	extern msgdef *msgdefs;
	extern socktype sockets[];

	extern artifact artifacts[];
	extern bountylist *bountyhead;
	extern chanlist *chanhead;
	extern chanlist *chantail;
	extern commandlist commands[];

	extern char ability_level[][16]; 
	extern char s_titles[][16]; 
	extern char t_titles[][2][16]; 

	extern char adjectives[][6][16];
	extern char army_names[][8];
	extern char function_name[];
	extern char joustlosers[][32];
	extern char joustwinners[][32];
	extern char message[];
	extern char plybuf[][PLYSIZ];
	extern char server_ip[];
	extern char symbols[4][70][8];

	extern city cities[];
	extern continent conts[];
	extern hero heroes[];
	extern hosts *hostshead;
	extern info conq_info;
	extern int antimagic_city;
	extern int auction_city;
	extern int city_fp;
	extern int ctyqty;
	extern int current_player;
	extern int info_fp;
	extern int magic_city;
	extern int maxlevel[];
	extern int player_fp;
	extern int promotions[][10][3];
	extern int recv_sig;
	extern int secret_city;
	extern int total_races[];
	extern int totalcasualties[][MAXTRP];
	extern int totalmdf;
	extern int total_player;
	extern int totlvl[][10];
	extern int totply[];
	extern items itemd[];
	extern player *combatants[];
	extern player *players;
	extern riddle riddles[];
	extern shiptype shiptypes[];
	extern spell spells[];
	extern structure structures[];
	extern time_t uptime;
	extern unit batmod[][MAXTRP];
	extern unit units[][MAXTRP];
	extern weather forecast[];

	extern int spcbless[];
	extern int spcshd[];
	extern int spcstr[];
	extern int spcaid[];
	extern int spcenlist[];
	extern int spctax[];

#include"prototypes.h"
