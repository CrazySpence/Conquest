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
	Global variables
*****************************************************************************/
/* Conquest Defaults */
	char age[8] = {"1st"};
	char basenames[][16] = 
	{
		"Fighter", "Barbarian", "Mage", "Vampire", "Cleric", "Ranger", "Titan", ""
	};
	char city_names[MAXCTY][16] = 
	{
		"Exeter", 
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
	};
	char components[MAXCMP][16] = 
	{
		"Gensing", "Cardamom", "Sage", "Nightshade", "Mandrake", "Ether"
	};
	char dft_kname[32] = {"My Kingdom"};
	char dft_sname[32] = {"My Ship"};
	char dft_winmsg[256] = {"I can't believe I won!?!"};
	char elitenames[][16] = 
	{
		"", "Skilled", "Experienced", "Veteran", "Master", "Elite",
		"Elite Class I", "Elite Class II", "Elite Class III",
		"Elite Class IV", "Elite Class V", "", "", "", "", "", "",
		"", "", "", ""
	};
	char emp[MAXCNT][32] = {"Romulus", "", "", ""};
	char emp_passwd[MAXCNT][32] = {"emperor", "emperor", "emperor", "emperor"};
	char food_name[16] = {"food"};
	char log_file[128] = {0};
	char master_server[128] = {0};
	char money_name[16] = {"copper"};
	char peasant_name[16] = {"peasant"};
	char pirate_names[][16] = 
	{
		"RedBeard", "YellowBeard", "BlueBeard", "BlackBeard"
	};
	char stralt_ip[16] = {0};
	char tavern_names[MAXCTY][32] = 
	{
		"Warrior's Den", 
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
	};
	char classnames[][16] =
	{
		/* Fighter 0-9 */
		"Norman", "", "", "", "", "", "", "", "", "",
		/* Barbarian 10-19 */
		"Viking", "", "", "", "", "", "", "", "", "",
		/* Mage 20-29 */
		"Norman", "", "", "", "", "", "", "", "", "",
		/* Vampire 30-39 */
		"Vampire", "", "", "", "", "", "", "", "", "",
		/* Cleric 40-49 */
		"Norman", "", "", "", "", "", "", "", "", "",
		/* Ranger 50-59 */
		"StarRunner", "", "", "", "", "", 
		"", "", "", "",
		/* Titans 60-69 */
		"Immortal", "", "", "", "", "", "", "", "", "",
		/* Summoned 70-79 */
		"Summoned", "", "", "", "", "", "", "", "", "",
	};
	char b_titles[][10][16] =
	{
		{"Grunt", "Fighter", "Savage", "Berserker", "Warrior",
		"Mercenary", "IronMark", "BattleMaster", "Warlord",
		"DeathLord"}, 
		{"Grunt", "Fighter", "Savage", "Berserker", "Warrior",
		"Mercenary", "IronMark", "BattleMistress", "Warlord",
		"DeathLord"}
	};
	char c_titles[][10][16] = 
	{
		{"Initiate", "Disciple", "Acolyte", "Adept", "Priest",
		"Vicar", "Bishop", "Patriarch", "Arch-Bishop", "Cardinal"},
		{"Initiate", "Disciple", "Acolyte", "Adept", "Priestess",
		"Vicar", "Bishop", "Matriarch", "Arch-Bishop", "Cardinal"}
	};
	char f_titles[][10][16] =
	{
		{"Knight", "Paladin", "Champion", "Baron", "Viscount",
		"Earl", "Marquis", "Duke", "King", "Emperor"},
		{"Knight", "Paladin", "Champion", "Baroness", "Viscountess",
		"Countess", "Marquise", "Duchess", "Queen", "Empress"}
	};
	char m_titles[][10][16] =
	{
		{"Novice", "Trickster", "Conjurer", "Magician", "Enchanter",
		"Spellbinder", "Warlock", "Sorceror", "Wizard", "Arch-Mage"},
		{"Novice", "Trickster", "Conjurer", "Magician", "Enchantress",
		"Spellbinder", "Witch", "Sorceress", "Wizard", "Arch-Mage"}
	};
	char r_titles[][10][16] = 
	{
		{"Scout", "Hunter", "Protector", "Tracker", "Ranger", 
		"Lord", "Hunt Master", "Forest Lord", "", ""},
		{"Scout", "Hunter", "Protector", "Tracker", "Ranger",
		"Lady", "Hunt Mistress", "Forest Goddess", "", ""}
	};
	char v_titles[][10][16] =
	{
		{"Childe", "Fledgling", "Neophyte", "Stalker", "Master", 
		"Tempter", "Nemesis", "Elder", "Prince", "DarkLord"}, 
		{"Childe", "Fledgling", "Neophyte", "Stalker", "Mistress", 
		"Temptress", "Nemesis", "Elder", "Princess", "DarkLady"}
	};

	FILE *logfile = NULL;
	float dft_surrender = .25;
	int advanced_armies = 0;
	int advanced_combat = 0;
	int allow_barb = 1;
	int allow_cleric = 1;
	int allow_formation = 1;
	int allow_mage = 1;
	int allow_ranger = 1;
	int allow_surrender = 1;
	int allow_vampire = 1; 
	int associate_races = 1;
	int attack_reprieve = 1;
	int attack_time = 1800;
	int auto_ar = -1;
	int capture_foreign = 0;
	int class_change[MAXCLS] = {0, 15000, 50000, 0, 25000, 40000, 0, 0};
	int end_date = 0;
	int enforce_peace = 0;
	int fog_of_war = 0;
	int force_cont[] = 
	{
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};
	int max_player = 256;
	int mps_15 = 1;
	int mult_vampires = 0;
	int normal_hks[MAXHKS] = {06, 12, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	int num_cities = 0;
	int num_conts = 0;
	int num_remorts = 10;
	int num_riddles = 0;
	int num_ships = 0;
	int ny_hks[MAXHKS] = {23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	int promote_knights = 1;
	int quest_chars = 0;
	int quest_riddle = 0;
	int promote_protection = 0;
	int remove_items = 1;
	int restrict_combat = 0;
	int restrict_markets = 0;
	int sail_protection = 0;
	int start_date = 1000;
	int use_dns = 1;
	unsigned short port_num = 9999;
/* End Defaults */
	int ARMY_DEFENSE = 0;
	int ARMY_CAMPAIGN = 1;

	char NOWSTR[TIMESIZ] = {0};
	int sockout = INVALID_SOCKET;
	messages *msghead[MAX_SOCKETS];
	msgdef *msgdefs = NULL;
	socktype sockets[MAX_SOCKETS];

	artifact artifacts[MAXCTY];
	bountylist *bountyhead = NULL;
	chanlist *chanhead = NULL;
	chanlist *chantail = NULL;
	commandlist commands[MAXCMD];

	char ability_level[][16] = 
	{
		"Novice", "Advanced", "Master"
	};
	char s_titles[][16] = 
	{
		"Weak", "Mediocre", "Strong", "Mighty", "Menancing", "Red", "Fierce", ""
	};
	char t_titles[][2][16] =
	{
		{"Titan", "Demi-God"},
		{"Titaness", "Demi-Goddess"}
	};

	char adjectives[][6][16] = 
	{
		{"his", "him", "he", "Sire", "Lord", "Master"},
		{"her", "her", "she", "Madam", "Lady", "Mistress"}
	};
	char army_names[][8] =
	{
		"Defense", "Campaign"
	};
	char function_name[128] = {"init"};
	char joustlosers[15][32];
	char joustwinners[15][32];
	char message[2048] = {0};
	char plybuf[MAXCBT][PLYSIZ];
	char server_ip[16] = {0};
	char symbols[4][70][8];

	city cities[MAXCTY];
	continent conts[MAXCNT];
	hero heroes[TOTHRO];
	hosts *hostshead = NULL;
	info conq_info;
	int antimagic_city = -1;
	int auction_city = -1;
	int city_fp;
	int ctyqty;
	int current_player;
	int info_fp;
	int maxlevel[MAXCLS] = {9, 9, 9, 9, 9, 7, 9, 9};
	int magic_city = -1;
	int player_fp;
	int promotions[MAXCLS][10][3];
	int recv_sig = -1;
	int secret_city = -1;
	int total_races[] = 
	{
		3, 5, 3, 1, 3, 3, 1, 3
	};
	int totalcasualties[MAXCBT][MAXTRP];
	int totalmdf;
	int total_player;
	int totlvl[MAXCNT][10];
	int totply[80];
	items itemd[TOTITM];
	player *combatants[MAXCBT];
	player *players = NULL;
	riddle riddles[MAXRID];
	shiptype shiptypes[MAXSHP];
	spell spells[TOTSPL];
	structure structures[MAXCLS];
	time_t uptime;
	unit batmod[MAXCBT][MAXTRP];
	unit units[MAXCBT][MAXTRP];
	weather forecast[MAXCNT];
/* Batmod Specials */
	int spcbless[MAXCNT] = {1, 2, 4, 8};
	int spcshd[MAXCNT] = {16, 32, 64, 128};
	int spcstr[MAXCNT] = {256, 512, 1024, 2048};
/* Kingdom Specials */
	int spcaid[MAXCNT] = {1, 2, 4, 8};
	int spcenlist[MAXCNT] = {16, 32, 64, 128};
	int spctax[MAXCNT] = {256, 512, 1024, 2048};

