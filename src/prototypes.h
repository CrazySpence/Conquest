
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

// conqd.c
void accept_connect(SOCKET sock);
void add_msg(char *cmd, char *host, char *msg, char *user, int num);
void check_sockets(void);
void clear_arthro(void);
void dataready(int num);
void dcc_chat(char *host, int plynum);
void dcc_close(int plynum);
void do_events(int *events, struct tm *pnt);
void exit_conquest(void);
int find_sock(SOCKET sock);
void free_msg(int num);
void free_sock(int num);
void init_age(void);
void init_cities(void);
void init_files(void);
void init_info(void);
void init_server(void);
void kick(char *msg, int plynum);
void op(int plynum);
int recv_data(char *buf, int num);
void send_channel(char *buf, int critical);
void send_data(void *data);
void send_heartbeat(void);
void send_message(char *buf, char *nick, int valcmd);
void set_mode(void);
void set_signals(void);

// conquest.c
void abdicate(player *plyptr);
void abilities(char *classname, player *plyptr);
void alliance(char *name, player *plyptr);
void ally(char *contname, player *plyptr);
void autoenlist(char *arg1, player *plyptr);
void balance(player *plyptr);
void bestow(char *itemname, int amount, int *gitem, int *item, int type, player *allyptr, player *plyptr);
void break_alliance(player *plyptr);
int buy(char *itemdesc, char *itemname, int amount, int cprice, int *cqty, int honor, int *item, int marketnum, int move, int special_troop, player *plyptr);
void church(char *arg, player *plyptr);
void church_pray(player *plyptr);
void color(char *arg1, player *plyptr);
void corpse(player *plyptr);
void courier(char *name, char *msg, player *plyptr);
void describe(char *args, player *plyptr);
void display_news(char *name);
void done(player *plyptr);
void drop(player *plyptr);
void email(char *address, char *filename, char *name);
void enlist(char *str_amount, player *plyptr);
void escape(char *cityname, player *plyptr); 
void espionage(char *name, char *type, player *plyptr);
int fighter(player *plyptr);
int find_max(char *itemname, int cprice, int cqty, player *plyptr);
void formation(char *formstr, char *typestr, player *plyptr);
void game(char *name);
void gender(char *arg1, player *plyptr);
void give(char *itemname, int amount, int *gitem, int *item, int type, player *allyptr, player *plyptr);
void information(char *name);
void kia(char *name);
void kings(char *name);
void kingdomname(char *name, player *plyptr);
void levels(char *classname, player *plyptr);
void list(char *name);
void map(char *contname, player *plyptr);
void market(int ctynum, player *plyptr);
void mystic(player *plyptr);
void newplayer(char *name, char *host, char *lord);
void password(char *newpassword, player *plyptr);
void person(char *contname, player *plyptr, player *whoptr);
void petition(int nextlevel, player *plyptr);
void post(char *msg, player *plyptr);
void rank(char *name);
void read_log(char *name);
void reprieve(char *contname, player *plyptr);
void review(char *name, int color, int cont, int show, int type, player *plyptr, player *whoptr);
void review_army(char *contname, player *plyptr);
void roll(char *arg, player *plyptr);
void search(char *location, player *plyptr);
void sell(char *itemdesc, char *itemname, int amount, int *ccopper, int cprice, int *cqty, int *item, int type, player *plyptr);
void set(char *msg, player *plyptr);
void settax(char *arg, player *plyptr);
void show_retaliation(player *plyptr);
void show_structures(char *classname, player *plyptr);
void spy(char *name, char *arg, player *plyptr);
void spy_emperor(char *arg, player *plyptr);
void spy_report(int cont, player *plyptr);
void surrender(char *arg, player *plyptr);
void tavern(player *plyptr);
void tips(char *arg1, player *plyptr);
void titans(char *name);
void train(int amount, player *plyptr);
void transfer_army(char *tostr, char *unit, int amount, player *plyptr);
void transfer_army_all(char *tostr, char *unit, player *plyptr);
void travel(char *cityname, player *plyptr);
void troop_info(char *classname, player *plyptr);
void vacation(int len, player *plyptr);
void validate(char *name, char *host, char *passwd);
void view(char *name, player *plyptr);
void winmsg(char *wmsg, player *plyptr);
void winsound(char *sound, player *plyptr);

// barb.c
int barbarian(player *plyptr);
void berserk(char *name, player *plyptr);
void besiege(char *cityname, player *plyptr);
void blitz(player *plyptr);
void garrison_lost(int ctynum, player *plyptr);
void garrison_won(int ctynum, player *plyptr);
void loot(char *cityname, player *plyptr);
void loot_city(int citynum, player *plyptr);
void make_barb(int level, player *plyptr);
void rebel(player *plyptr);

// battle.c
void add_balance(int adj, player *plyptr);
void add_battles(player *winner);
void add_casualties(int enemynum, int num, player *plyptr);
void adjust_honor(int cont, player *loser, player *winner);
void adjust_unit(int ambush, int attack, int defend, int num_attacks, int range, int regen, int shield, int swarm, int troopnum, player *plyptr, unit *mod);
int allow_attack(char *name, int from, player *plyptr);
int army_size(int cont, int type, player *plyptr);
void attack(char *name, player *plyptr);
int attack_level(int cont, player *plyptr);
int attacks_allowed(player *plyptr);
void auto_transfer(int cont, int from, int to, player *plyptr);
int battle(int *num_castles, int *round, int *total_attack, int *total_defend);
void battle_end(player *loser, player *winner, int num_castles, int round, int troops);
void capture_castle(int cont, int land, int num_castles, player *loser, player *winner);
int castles_captured(int cont, player *plyptr);
void check_ally(int cont, int num, player *plyptr);
void check_batmod(int cont, player *plyptr, unit *mod);
void check_catapult(int cont, int num, int type, player *plyptr);
void check_horde(int cont, int max, int num, int type, player *plyptr);
void check_nightblade(int cont, player *loser, player *winner);
void check_weathermod(int cont, player *plyptr, unit *mod);
int combined_army(int cont, player *plyptr);
void compute_casualties(int cont, int damage[], int misfires[], int num, int round, int type, player *plyptr);
float compute_ratio(int troops);
int count_casualties(int num);
int count_side_casualties(int start);
void damage_catapult(int cont, int num, int type, player *plyptr);
void enlist_ren(int cont, player *loser);
void parse_winmsg(char *winmsg, player *loser, player *winner);
void refresh_army(int cont, player *plyptr);
void reset_casualties(void);
void reset_combatants(void);
void show_allies(int start, int type, player *attacker, player *defender);
void show_casualties(int num, int type, player *plyptr);
void show_formation(char *name, int cont, int type, player *plyptr);
void show_surrender(char *name, player *plyptr);
void start_battle(void);
void subtract_casualties(int cont, int *damage, int num, int round, int type, player *plyptr);
int subtract_troop(int cont, int damage, int mode, int num, int round, int type, int unit, player *plyptr);
void take_artifact(player *loser, player *winner);
void take_assets(int cont, int num_castles, int troops, player *loser, player *winner);
void take_items(player *loser, player *winner);
void take_ship(player *loser, player *winner);
void vassal_aid(int cont, int type, player *enemy, player *plyptr);
void vassal_postbattle(int *copper, int *food, int *land, int *peasant, player *loser, player *winner);

// emp.c
void cosine(char *itemname, char *name, int amount, int *item, int type, player *plyptr);
void create_titan(char *name, int type, player *plyptr);
void erase(int plynum, player *plyptr);
void find_artifacts(player *plyptr);
void hero_list(player *plyptr);
void hk(int type, player *plyptr);
void hklog(player *plyptr);
void list_locks(player *plyptr);
void lock(char *name, player *plyptr);
void make_questchar(player *plyptr);
void make_titan(int type, player *plyptr);
void motd(char *msg, player *plyptr);
void newtopic(char *topic, int flag, SOCKET sock, player *plyptr);
void nick(char *nick, SOCKET sock, player *plyptr);
void ninja(char *contname, char *name, player *plyptr);
void rainbow(player *plyptr);
void reset(char *name, char *var, char *arg, char *arg2, player *plyptr);
void set_event(char *name, char *arg, player *plyptr);
void show_player(player *plyptr);
void shut_down(player *plyptr);
void tangent(char *name, char *str_offset, player *plyptr);
void thunder(player *plyptr);

// help.c
void help(char *name, char *arg1, char *arg2, char *arg3);
void scroll_head(char *name);
void scroll_tail(char *name);

// house.c
void add_incr_move(void);
void add_move(player *plyptr);
void cast_ar(void);
void check_castles(int cont, player *plyptr);
void check_chest(int cont, player *plyptr);
void check_crown(int cont, player *plyptr);
void check_reign(int cont, player *plyptr);
void check_rod(int cont, int type, player *plyptr);
void create_questchar(void);
void feed_army(int cont, int type, player *plyptr);
void feed_army_ships(int cont, player *plyptr);
int find_market(int ctynum, int num, int type);
int find_merc_market(int ctynum, int num, int type);
void hk_main(int cont, int type, player *plyptr);
void housekeeping(int type);
void init_city(int ctynum, int type);
int make_price(int current, int price, int qty);
int market_type(void);
void new_year(int cont, player *plyptr);
int open_othermarket(int ctynum);
void open_troopmarket(int ctynum, int force_num, int force_type);
void pay_army(int cont, int type, player *plyptr);
void pay_army_ships(int cont, player *plyptr);
void produce_food(int cont, player *plyptr);
void produce_peasant(int cont, player *plyptr);
void promote_knight(int cont, player *plyptr);
void random_event(int cont, int setnum, player *plyptr);
void regen_kraken(void);
void weather_event(int cont, player *plyptr);

// items.c
void add_item(int itemnum, player *plyptr);
void buy_item(char *item, player *plyptr);
void destroy(char *item, player *plyptr);
void dsp_items(player *plyptr);
int find_randomitem(void);
int has_item(int itemnum, player *plyptr);
void item_decay(void);
void item_hk(int cont, player *plyptr);
void quest_3(player *plyptr);
void read_items(void);
void remove_charge(int itemnum, player *plyptr);
void remove_all_items(player *plyptr);
void remove_item(int itemnum, player *plyptr);
void sell_item(char *item, player *plyptr);
void use(char *item, char *name, player *plyptr);
int use_always(int itemnum, player *plyptr);
int use_amulet(char *name, player *plyptr);
int use_axe(player *plyptr);
int use_boat(player *plyptr);
int use_boulder(char *cityname, player *plyptr);
int use_builders(player *plyptr);
int use_carpet(char *cityname, player *plyptr);
int use_cow(player *plyptr);
int use_cowl(char *name, player *plyptr);
int use_dust(char *name, player *plyptr);
int use_emblem(char *name, player *plyptr);
int use_endurance(player *plyptr);
int use_eye(char *name, player *plyptr);
int use_flag(player *plyptr);
int use_lure(player *plyptr);
int use_orb(player *plyptr);
int use_pouch(player *plyptr);
int use_quiver(player *plyptr);
int use_ring(char *wish, player *plyptr);
int use_sceptre(player *plyptr);
int use_shield(char *name, player *plyptr);
int use_speed(player *plyptr);
int use_stone(player *plyptr);
int use_thieves(player *plyptr);
int use_tome(player *plyptr);
int use_tricks(player *plyptr);
int use_trumpet(player *plyptr);
int use_mask(char *name, player *plyptr);
int use_vial(char *name, player *plyptr);
int use_wand(char *name, player *plyptr);

// mage.c
int allow_spell(char *name, player *plyptr);
void beast_lost(player *plyptr, char *desc, int type);
void beast_won(player *plyptr, char *desc, int type);
void book(player *plyptr);
void cast(char *spellname, char *arg1, char *arg2, char *arg3, player *plyptr);
void change_player(int classnum, int cont, player *plyptr);
void component_market(int ctynum, player *plyptr);
void create_guise(int plynum);
void devote(player *plyptr);
void fail_save(char *desc, char *spellname, player *plyptr, player *target);
void get_components(int type, player *plyptr);
void made_save(char *spellname, player *plyptr, player *target);
int mage(player *plyptr);
void need_components(char *spellname, int component, player *plyptr);
void quest_4(char *component, int type, player *plyptr);
void read_spells(void);
int save_throw(player *plyptr);
void show_components(player *plyptr);
void spell_clue(player *plyptr);
int spell_gender(char *name, player *plyptr);
int spell_createfood(player *plyptr);
int spell_fireball(char *name, player *plyptr);
int spell_truesight(char *name, player *plyptr);
int spell_forget(char *name, player *plyptr);
int spell_chaos(char *name, player *plyptr);
int spell_giant(char *name, player *plyptr);
int spell_polymorph(char *name, player *plyptr);
int spell_strength(player *plyptr);
int spell_famine(char *name, player *plyptr);
int spell_shadowwarriors(char *name, player *plyptr);
int spell_teleport(char *cityname, player *plyptr);
int spell_weather(char *arg1, char *arg2, char *arg3, player *plyptr);
int spell_enchant(player *plyptr);
int spell_flamestrike(char *name, player *plyptr);
int spell_move(char *name, player *plyptr);
int spell_sleep(char *name, player *plyptr);
int spell_vanish(char *name, player *plyptr);
int spell_charm(char *name, player *plyptr);
int spell_dragon(char *name, player *plyptr);
int spell_maelstorm(char *name, player *plyptr);
int spell_meteorstorm(char *name, player *plyptr);
int spell_portal(char *contname, player *plyptr);
int spell_behemoth(char *name, player *plyptr);
int spell_rampage(char *name, player *plyptr);
int spell_wizardlock(char *cityname, player *plyptr);
int spell_death(char *name, player *plyptr);
int spell_juggernaut(char *name, player *plyptr);
int spell_plague(char *name, player *plyptr);
int spell_earthquake(char *name, player *plyptr);
int spell_kraken(player *plyptr);
int spell_wish(char *str_amount, char *item, player *plyptr);
int spell_season(player *plyptr);
int spell_alterreality(int chance, player *plyptr);
void summon_creature(int ally, int creature, int enemy_army, int horde_army, int strength, player *eptr, player *plyptr);

// msclib.c
void add_bg(int cont, int mult, player *plyptr);
void add_host(char *host);
int add_player(char *name, char *host, int classnum, int cont, int lord);
void add_log(char *file, char *msg);
void add_ret(int plynum, player *plyptr);
int can_ret(player *attacker, player *defender);
int castle_max(int classnum);
int change_class(int newclass, player *plyptr);
void change_level(int cont, int offset, player *plyptr);
int check_artifact(int artnum, player *plyptr);
void check_assets(int cont, int level, player *plyptr);
int check_buy(int amount, int type, player *plyptr);
int check_file(char *ext, char *name);
int check_password(char *name, char *word);
void check_rank(int plynum);
void check_ret(int cont, player *attacker, player *defender);
int check_sell(int amount, int type, player *plyptr);
void check_troops(player *plyptr);
int check_vacation(char *name, player *plyptr);
int check_word(char *name, char *word, int max, int min);
void closed(char *str, int num);
int cmp_command(const void *ptr1, const void *ptr2);
void default_formation(int cont, player *plyptr);
void delete_player(int plynum);
int demigod(player *plyptr);
int demote(int cont, player *plyptr);
int emperor(char *name);
int equal_class(int class1, int class2);
int fill_castles(int amount, int cont, player *plyptr);
int find_artifact(player *plyptr);
int find_city(char *cityname);
int find_distance(int from, int to, player *plyptr);
int find_class(char *classname);
int find_cont(char *contname);
int find_host(char *host);
int find_openmarket(int ctynum);
int find_player(char *name);
int find_randomartifact(int city);
int find_randomplayer(int cont, int level);
int find_ret(int plynum, player *plyptr);
int find_slot(void);
void highway_robbers(char *cityname, player *plyptr);
void hk_text(player *plyptr);
void init(void);
void kia_change(int cont, char *killed, char *killer);
void king_pic(int classnum);
void list_change(char *loser, char *winner);
void log_event(char *from, char *msg, char *name);
void lose_exp(player *plyptr);
int playernum(player *plyptr);
void pre_init(void);
void post_init(void);
void promote(int cont, player *plyptr);
void promote_max(int cont, player *plyptr);
int randomnum(int max, int min);
void read_army(void);
void read_art(void);
void read_classes(void);
void read_dfts(int type);
void read_info(void);
void read_legend(char *name);
void read_promotions(void);
void read_riddles(void);
void read_structures(void);
char *read_tip(void);
void remove_all_hosts(void);
void remove_all_ret(player *plyptr);
void remove_host(char *host);
void remove_ret(int plynum, player *plyptr);
void reset_ally(int plynum);
void reset_done(void);
void reset_ret(int plynum);
void reset_special(player *plyptr);
void reset_spy(int newspy, int plynum);
void reset_val(void);
void save_art(void);
void save_city(int ctynum);
void save_info(void);
void save_player(int plynum);
char *select_class(int classnum);
char *select_classname(int classnum);
char *select_title(int classnum, int level, int gender);
void signal_fatal(int signum);
void signal_reset(int signum);
void signal_shutdown(int signum);
void sort_rank(void);
void strip(char *string);
char *strtolower(char *instr);
int tax(int amount, int price, int taxrate);
void timediff(int *hours, int *minutes, int *seconds, time_t from);
int titan(player *plyptr);
int troop_type(int classnum);
void update_levelmax(player *plyptr);
void update_ret(int cont, player *attacker, player *defender);

// parser.c
void buy_spy(player *plyptr);
void dcc_cmd(char *arg, char *host, char *type, char *user);
void parser(char *cmd, char *host, char *msg, char *type, char *user);
void process_buy(char *arg1, char *arg2, player *plyptr);
int process_max(char *arg1, player *plyptr);
void process_sell(char *arg1, char *arg2, player *plyptr);
void process_transfer(char *name, char *arg1, char *arg2, int type, player *plyptr);
void select_item(int **cqty, int *cprice, int *honor, int **item, int marketnum, int *move, int special_troop, player *plyptr);

// quest.c
void enemy(int ally, int army_type, int enemy_army, int horde_army, int strength, player *eptr, player *plyptr);
void horde_lost(player *horptr, player *plyptr);
void horde_won(player *horptr, player *plyptr);
void quest(char *arg1, char *arg2, char *arg3, player *plyptr);
void quest_0(player *plyptr);
void quest_1(player *plyptr);
void quest_2(player *plyptr);
void quest_6(char *cmd, char *solution, player *plyptr);
int summoned(player *plyptr);

// continent.c
void add_ship(int shpnum, int type, player *plyptr);
void atlantis(player *plyptr);
void blockade(player *plyptr);
void build_ships(void);
void buy_ship(char *typename, player *plyptr);
int check_blockade(int cont, player *plyptr);
void check_contspc(int cont);
int check_siren(player *plyptr);
int check_weather(int cont, int to, player *plyptr);
void damage_cargo(int damage, int shpnum, player *plyptr);
int compare_cargo(char *typename, int type, player *plyptr);
int compute_shpdmg(int shpnum, player *plyptr);
void engage(char *name, player *plyptr);
int find_maxcargo(int shpnum, int type, player *plyptr);
int find_randomshpnum(int type, player *plyptr);
int find_shpnum(player *plyptr);
void fleet(player *plyptr);
void invade(char *name, player *plyptr);
int kraken(player *plyptr);
void loadship(char *typename, int amount, int shpnum, player *plyptr);
int pirates(player *plyptr);
void population(char *contname, player *plyptr);
void read_ships(void);
void remove_ship(int shpnum, player *plyptr);
void repair(int shpnum, player *plyptr);
void sail(char *dest, player *plyptr);
int sail_move(player *plyptr);
void scuttle(int shpnum, player *plyptr);
player *seabattle(player *attacker, player *defender);
void sell_ship(int shpnum, player *plyptr);
void show_fleet(char *name, char *type, player *plyptr);
void sname(char *name, int shpnum, player *plyptr);
void take_cargo(int dshpnum, player *loser, player *winner);
void unload(int amount, int shpnum, player *plyptr);

// vampire.c
void charm(char *args, player *plyptr);
int create_vampire(int cont, player *loser, player *winner);
void destroy_vampires(player *vampptr);
void guise(char *name, player *plyptr);
int lifedrain(int cont, player *loser, player *winner);
void make_vampire(player *plyptr);
void pestilence(char *name, player *plyptr);
void reset_seek(void);
void ritual(player *plyptr);
void seek(player *plyptr);
void show_vampire(player *plyptr);
void slay(char *name, player *plyptr);
void take_vampassets(player *plyptr, player *vampptr);
int vampire(player *plyptr);
void vampire_death(int cont, player *plyptr, player *vampptr);

// cleric.c
void bless(char *name, player *plyptr);
int cleric(player *plyptr);
void crusade(char *name, player *plyptr);
void divination(char *name, player *plyptr);
void divine_intervention(player *plyptr);
void excommunicate(char *name, player *plyptr);
void grace(char *name, player *plyptr);
void grace_player(int plynum, player *plyptr);
void make_fighter(int cont, player *cleric, player *plyptr);
void penance(char *name, player *plyptr);
void resurrect(char *name, player *plyptr);
void resurrect_player(int plynum, player *plyptr);
void sanctuary(char *name, player *plyptr);
void sanctuary_player(int plynum, player *plyptr);
void tithe(int cont, player *plyptr);
void turn_undead(char *name, player *plyptr);
void vocation(player *plyptr);

// ranger.c
void ceasefire(player *plyptr);
void commune(player *plyptr);
int ranger(player *plyptr);
void serve(player *plyptr);
void shield(player *plyptr);
void summon(char *name, player *plyptr);
void tend(char *name, player *plyptr);
void vision(char *name, player *plyptr);

// heroes.c
int check_hero(int heronum, player *plyptr);
void dismiss(player *plyptr);
int find_hero(player *plyptr);
int find_heroname(char *name);
void hero_info(player *plyptr);
void hero_hk(int type);
void hero_hkpower(int num, int type, player *plyptr);
int hero_prebattle(player *defend, player *plyptr);
void hero_postbattle(int cont, int type, player *loser, player *winner);
int hero_serve(int num, player *plyptr);
void lost_hero(int heronum, player *plyptr);
void lost_necromancer(player *plyptr);
void lost_titan(player *plyptr);
int opinion(int num, player *plyptr);
void quest_5(char *name, player *plyptr);
void read_heroes(void);
void revive(char *name, player *plyptr);
void revive_heroes(void);
void save_hero(void);

// weather.c
void change_rating(int cont, int *rating);
void change_weather(int cont, int humidity, int temp, int wind);
void find_weather(int cont, int troop_num, unit *modifiers);
void show_weather(char *classname, char *contname, player *plyptr);

// feudalism.c
void add_vassal(int lord, player *plyptr);
int attack_vassal(player *plyptr, player *target);
void check_vassals(player *plyptr);
int is_vassal(player *plyptr, player *vassal);
void log_vassalmsg(char *msg, player *plyptr);
void release(char *name, player *plyptr);
void remove_all_vassal(player *plyptr);
void remove_vassal(player *plyptr);
void vassalaid(char *arg1, player *plyptr);
void vassal_hk(int type, player *plyptr);
void vassals(char *name, player *plyptr);

// mercs.c
void disband(char *typestr, char *unit, int amount, player *plyptr);
void hire(char *name, int amount, player *plyptr);
void show_disband(player *plyptr);
void show_hire(int ctynum, player *plyptr);

// message.c
void parse_msg(char *old, char *new);
void read_messages(void);

// wsaerrors.c
#ifdef WIN32
char *convert_wsaerror(int errnum);
#endif

// bounty.c
void bounty(char *name, int amount, player *plyptr);
void check_bounty(int cont, int plynum, player *plyptr);
void read_bounty(void);
void remove_bounty(int plynum);
void save_bounty(void);
void show_bounty(char *contname, player *plyptr);

// steward.c
void check_steward(int cont, int type, player *enemy, player *plyptr);
void parse_cmdstr(char *cmdstr, int cont, player *enemy, player *plyptr);
void show_steward(int cont, player *plyptr);
void steward(char *cmd, char *cmdstr, player *plyptr);
void steward_hk(int cont, player *plyptr);

// xml.c
void build_xml(char *buf, char *cmd, char *host, char *msg, char *user);
void parse_xml(char *buf, char *cmd, char *host, char *msg, char *type, char *user);
