

#include "stdafx.h"
char gameover = 0;
bool getmana, turnused;



#include "lilhelpers.h"

#include "Array2D.h"
#include "BitArray.h"

#include <vector>
using std::vector;
#include <iostream>
#include <fstream>
#include <string>
using std::string;
using std::to_string;
#include <array>
using std::array;
#include <list>
using std::list;
#include <map>

#include <cmath>
#include <chrono>
#include <conio.h>
#include <limits>

#include <algorithm>
using std::find;
#include <array>
#include <unordered_map>
using std::unordered_map;
#include <cmath>
using std::pair;
using std::make_pair;
#include "SDL.h"

void scorescreen();

void pangodelay(unsigned int ms){
	unsigned int cur_t = SDL_GetTicks();
	while (SDL_GetTicks() < cur_t + ms){
		SDL_Delay(10);
	}
}

void renderscreen(); //forward declaration

typedef unsigned int uint;

inline unsigned int clamp(unsigned int x,unsigned int low, unsigned int high){
	if (x <= low)return low;
	if (x >= high)return high;
	return x;
}

int fprintf(FILE * stream, const char * format, ...){ return 0; }





#define MAX(a,b) ( (a>b)?a:b )
#define MIN(a,b) ( (a<b)?a:b )


SDL_Window *window;
SDL_Renderer *renderer;

int originx, originy;



#include "sprites.h"
#include "items.h"
#include "player.h"
Player player;
#include "RLMap.h"



void loadsprites(void);


//"options"
//these are the defaults
bool OPTION_fullscreen = false;
char OPTION_res = 1;//0 1 2//myevent.key.keysym.scancode
enum class commands { N, S, E, W, NW, NE, SW, SE, WAIT, LANTERN, SUCKBLOW, VAC, SUPERVAC, USE,  ESCAPE,
MUSUP,MUSDOWN,FXUP,FXDOWN};

vector<int> OPTION_buttons = {
	SDL_SCANCODE_KP_8,//N
	SDL_SCANCODE_KP_2,//S
	SDL_SCANCODE_KP_6,//E							
	SDL_SCANCODE_KP_4,//W							
	SDL_SCANCODE_KP_7,//NW							
	SDL_SCANCODE_KP_9,//NE							
	SDL_SCANCODE_KP_1,//SW							
	SDL_SCANCODE_KP_3,//SE							
	SDL_SCANCODE_KP_5,//WAIT						
	SDL_SCANCODE_KP_DIVIDE, //LANTERN	
	SDL_SCANCODE_KP_MINUS,//SUCKBLOW TOGGLE
	SDL_SCANCODE_KP_0,//VAC
	SDL_SCANCODE_KP_PLUS,//SUPER VAC
	SDL_SCANCODE_KP_MULTIPLY };//, //USE
	//SDL_SCANCODE_ESCAPE }; //ESCAPE};

							
							
bool options_dirty = false; //if set to true in titlepage we need to write options on getting back


unsigned char Scancode_to_command[512];

const string button_names [] = {
	"NORTH","SOUTH", "EAST", "WEST",
	"NORTH-WEST","NORTH-EAST","SOUTH-WEST" , "SOUTH-EAST",
	"WAIT",  "LANTERN", "TOGGLE SUCK/BLOW",
"ACTIVATE VACUUM", "SUPER ACTIVATE VACUUM", "USE HELD ITEM"};

RLMap* map;



void killamob(item_instance* i,int x,int y){
	//ADDSOUND kill mob , mob dies
	messagelog("You kill the " + i->type->name);
	if (i->type->type==Eitemtype::MOB_BROOM ||
		i->type->type == Eitemtype::MOB_TZIKEN){
		player.score -= 500;
		//lose the 500 you got for rescuing broom or tziken if they die
	}
	else {
		player.score += 50; //SCORE killing a mob- 50 points
	}
	//TODO procedural blood. not important
	//lil::removevalue(map->moblist, i); //because of problem we don't remove from moblist yet
	lil::replacevalue(map->moblist, i, (item_instance*)nullptr); // new because of problem
	if (i->invacuum){
		lil::replacevalue(player.vacuum_chamber,i,
		 new item_instance(i->type->deadoneofthem, x, y));
	}
	else {
		//imp die on map leave lava
		//square itself plus 2 neighbours
		if (i->type->type == Eitemtype::MOB_IMP){
			map->lavaize(x, y);
			for (char lavatwice = 0; lavatwice < 2; lavatwice++){
				int x2, y2; //ALL HAIL TEH LAMB-DAH
				if (map->squarecheck(x, y, x2, y2,
					[](int x, int y){return map->displaychar.at(x, y) == ' '; }
					))
					map->lavaize(x2, y2);
			}
		}
		item_instance* i2=new item_instance(i->type->deadoneofthem, x, y);
		map->itemput(i2, x, y);
		
	}

	
	delete (i);//free mob memory
}
void damagemob(item_instance* i, int x, int y, int amount){
	i->hp -= amount;
	if (i->hp <= 0)
		killamob(i, x, y);
}


inline void  MINIMAP_PIXEL(int THISX, int  THISY) {
	SDL_RenderDrawPoint(renderer, 640 - map->width * 2 + THISX * 2, THISY * 2);
	SDL_RenderDrawPoint(renderer, 1 + 640 - map->width * 2 + THISX * 2, THISY * 2);
	SDL_RenderDrawPoint(renderer, 640 - map->width * 2 + THISX * 2, 1 + THISY * 2);
	SDL_RenderDrawPoint(renderer, 1 + 640 - map->width * 2 + THISX * 2, 1 + THISY * 2);
}

#include "sound.h"

#include "mapgen.h"

#include "titlepage.h"






void loadopts(){
	char buffer[128];
	std::ifstream file;
	file.open((ASSET("optionz.dat")), std::ios::in | std::ios::binary);
	if (!file)return;
	file.read(buffer, 128);
	OPTION_fullscreen = (buffer[0] == 0) ? false : true;
	OPTION_res = buffer[1];
	fxvol = buffer[2];
	musvol = buffer[3];
	int siez=OPTION_buttons.size();
	for (int upto=0; upto < siez; upto++){
		OPTION_buttons[upto] = buffer[upto+4];
	}
	file.close();
}

void saveopts(){
	char buffer[128];
	std::ofstream file;
	file.open(ASSET("optionz.dat"),std::ios::out | std::ios::binary);
	buffer[0] = (OPTION_fullscreen == false) ? 0 : 1;
	buffer[1] = OPTION_res;
	buffer[2] = fxvol;
	buffer[3] = musvol;

	int upto = 4;
	for (auto f: OPTION_buttons){
		buffer[upto++] = f;
	}
	file.write(buffer, 128);
	file.close();

	options_dirty = false;
}

void comeup(){
	int x = 640, y = 360;
	x *= OPTION_res + 1;
	y *= OPTION_res + 1;
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Sucker [7DRL 2014] by graspee",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		x, y,
		0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(renderer, 640, 360);
	SDL_RenderClear(renderer);
	if (OPTION_fullscreen)SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	soundinit();
	spriteinit();
	setupitems();
}

void godown(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	soundexit();
	spriteexit();
	SDL_Quit();

}

void setupscanners(){
	for (auto f = 0; f < 512; f++)Scancode_to_command[f] = 255;
	char b = 0;
	for (auto f : OPTION_buttons)Scancode_to_command[f] = b++;
	Scancode_to_command[41] = (int)commands::ESCAPE;
	Scancode_to_command[SDL_SCANCODE_F1] = (int) commands::MUSDOWN;
	Scancode_to_command[SDL_SCANCODE_F2] = (int) commands::MUSUP;
	Scancode_to_command[SDL_SCANCODE_F3] = (int) commands::FXDOWN;
	Scancode_to_command[SDL_SCANCODE_F4] = (int) commands::FXUP;

}
void moveplayer(){
	map->do_fov_rec_shadowcast(player.posx, player.posy, 11);
	map->dynamiclight.Fill({ 0, 0, 0, 0 });
	if (player.lantern)
		map->do_fov_foradynamiclight(player.posx, player.posy, 9, { 255, 255, 255 });

	if (map->staticlight.at(player.posx, player.posy).total == 0 &&
		map->dynamiclight.at(player.posx, player.posy).total == 0)
		player.stealthed = true;
	else
		player.stealthed = false;
}

#include "suck.h"
#include "blow.h"

bool trytomove(int deltax, int deltay){
	
	
	int tentx = player.posx + deltax;
	int tenty = player.posy + deltay;
	//if proposed move is off the edge of map- denied. no turn consumed
	if (tentx < 0 || tentx >= map->width || tenty < 0 || tenty >= map->height)
		return false;
	//is there an item/monster on the tile?
	item_instance* i = map->itemgrid.at(tentx, tenty);
	if (i != nullptr){
		//if it's an item not a monster
		if (!i->type->ismob){
			//ADDSOUND pick up an item
			//messagelog("You pick up the " + i->type->name);
			//if player not carrying anything
			if (player.held == nullptr){
				//pick it up
				messagelog("You pick up the " + i->type->name);
				player.held = i;//object -> player hand
				map->itemremovefrom(i);//take obj off the map
				//drawsprite(16 * 12, 21 * 16, i->type->sprite);//draw held item in hand frame
				//map->redrawcell(tentx, tenty);//redraw the map cell
				return true;
			}
			else {//player carrying something
				//swap with current
				item_instance* temp = player.held;//put player's obj in temp store
				player.held = i;//object -> player hand
				map->itemput(temp, tentx, tenty);//put player's prev obj on the map
				messagelog("You swap your " + temp->type->name+" for the " + i->type->name+".");
				//drawsprite(16 * 12, 21 * 16, "frame");//draw the frame of player held
				//drawsprite(16 * 12, 21 * 16, i->type->sprite);//draw held item in hand frame
				//map->redrawcell(tentx, tenty);//redraw the map cell
				return true;
			}
		}
		else {//it's a monster
			//if you have a sword
			if (player.held != nullptr && player.held->type->type == Eitemtype::ITEM_SWORD){
				playsound("swoosh");
				messagelog("You attack the " + i->type->name+".");
				damagemob(i, tentx, tenty, 1);
				player.held->uses--;
				playsound("hit");
				if (player.held->uses <= 0){
					//ADDSOUND SWORD SNOP
					messagelog("Your sword snops and becomes useless!",25,225,25);
					delete player.held;
					player.held = nullptr;
				}
				else {
					messagelog("Sword durabiliteh: " + to_string(player.held->uses) + " /5.", 25, 225, 25);
				}
				return true;
			}
			else {//no sword
				return false;
			}
		}
	}
	else {//no item/monster on tile
		switch (map->displaychar.at(tentx, tenty)){
		case 'E'://if it's an exit
			//if it's locked
			if (map->locked.get(tentx, tenty)){
				//if player has key
				if (player.held != nullptr && player.held->type->type == Eitemtype::ITEM_KEY){
					playsound("lock");
					messagelog("You unlock the exit.");
					delete(player.held);//free obj memory
					player.held = nullptr;//remove key from hand
					map->locked.set(tentx, tenty, false);//unlock it
					return true;
				}
				else {//not carrying a key
					//just move onto the exit
					player.posx = tentx, player.posy = tenty;
					moveplayer();
					return true;
				}
			}
			else {//exit is not locked
				playsound("endlevel");
				messagelog("You exit the level.");
				scorescreen();
				//std::cout << "you leave the level";//TODO Actually leave level
				if (player.level == 10){
					messagelog("You win the game.");
					player.posx = tentx, player.posy = tenty;
					player.stealthed = true;
					gameover = 2;
					return true;//is it true we want here?

				}
				delete map;
				map = new RLMap(mapwidth, mapheight);
				CLS();
				moveplayer();
				//renderscreen();

				return true;//delete this
			}
			break;
		
		
		case 'C'://broom cupboard
			//if it's locked
			if (map->locked.get(tentx, tenty)){
				//if player has key
				if (player.held != nullptr && player.held->type->type == Eitemtype::ITEM_KEY){
					playsound("lock");
					messagelog("You unlock the broom cupboard.");
					delete(player.held);//free obj memory
					player.held = nullptr;//remove key from hand
					map->locked.set(tentx, tenty, false);//unlock it
					return true;
				}
				else {//not carrying a key
					//just move onto the broom cupboard
					player.posx = tentx, player.posy = tenty;
					moveplayer();
					return true;
				}
			}
			else {//cupboard  is not locked
				playsound("creak");
				messagelog("You open the broom cupboard.", 225, 225, 25);
				messagelog("Independent Entity Princess", 225, 225, 25);
				messagelog("Spacealine Broom emerges!", 225, 225, 25);
				
				player.score += 500; //SCORE : RESCUE BROOM FROM CUPBOARD 500 POINTS
				//std::cout << "broom emerges";//TODO Actually make broom emerge
				
				
				map->displaychar.at(tentx, tenty) = ' ';
				item_instance* broom = new item_instance(Eitemtype::MOB_BROOM, tentx, tenty);
				broom->behaviour = EBehaviour::BEHAVIOUR_STATIC;
				map->itemput(broom, tentx, tenty);
				
				map->moblist.push_back(broom);

				return true;//delete this
			}
			break;
		case 'c'://chest of golden tziken
			//if it's locked
			if (map->locked.get(tentx, tenty)){
				//if player has key
				if (player.held != nullptr && player.held->type->type == Eitemtype::ITEM_KEY){
					playsound("lock");
					messagelog("You unlock the chest.");
					delete(player.held);//free obj memory
					player.held = nullptr;//remove key from hand
					map->locked.set(tentx, tenty, false);//unlock it
					return true;
				}
				else {//not carrying a key
					//just move onto the tziken chest
					player.posx = tentx, player.posy = tenty;
					moveplayer();
					return true;
				}
			}
			else {//chest  is not locked
				playsound("creak");
				messagelog("You open the chest.",225, 225, 25);
				messagelog("Independent Entity Golden Tziken Emerges!", 225, 225, 25);
				player.score += 500; //SCORE : RESCUE TZIKEN FROM CHEST 500 POINTS
				//std::cout << "broom emerges";//TODO Actually make broom emerge


				map->displaychar.at(tentx, tenty) = ' ';
				item_instance* tziken = new item_instance(Eitemtype::MOB_TZIKEN, tentx, tenty);
				tziken->behaviour = EBehaviour::BEHAVIOUR_STATIC;
				map->itemput(tziken, tentx, tenty);

				map->moblist.push_back(tziken);

				return true;//delete this
			}
			break;
		//case 'L'://lava
			
			//break;
		case '#'://wall
			//do nothing no turn consumed
			//ADD SOUND WALL BUMP?
			return false;
			break;
		case ' ':case 'L':case 'G': // floor, lava, rabbit gate
			player.posx = tentx, player.posy = tenty;
			moveplayer();
			return true;
			break;
		}
	}
	//if (!map->passable.get(tentx, tenty))
		//return false;
	
}

//returns true if you should give mana
void waitonplayer(){
	static SDL_Event myevent;

	while (1){
		
		bool playermoved = false;

		int a = SDL_WaitEvent(&myevent);

		if (myevent.type == SDL_KEYDOWN){
			commands command = (commands)Scancode_to_command[myevent.key.keysym.scancode];

				if ((int)command < 8)//moobment
					if (trytomove(lil::deltax[(int)command], lil::deltay[(int)command]))
					{
						getmana = true; turnused = true; return;
					}
					else { continue; }
				
				switch (command){
				case commands::WAIT:
					messagelog("You wait. Time passes...");
					getmana = true; turnused = true; return;
				case commands::LANTERN:
					player.lantern = !player.lantern;
					if (player.lantern){
						playsound("lantern");
						messagelog("You light your torch.");
					}
					else {
						messagelog("You put your torch out.");
					}
					moveplayer();
					getmana = true; turnused = true; return;
				case commands::ESCAPE://esc. can't be changed
					gameover = 3;
					return;
					//godown();
					//exit(0);
					break;
				case commands::SUCKBLOW:
					player.selectsuck = ! player.selectsuck;
					playsound(player.selectsuck ? "suckswitch" : "blow");
					getmana = true; turnused = true; return;
				case commands::VAC:
					if (player.mana < 1){
						messagelog("Not enough steam!");
						//ADDSOUND nothing happening with a machine
						
						getmana = true; turnused = true; return;
						//hopefully a turn is used and steam is given.
					}
					if (player.selectsuck){
						if (suck(player.posx, player.posy, 1))
						{
							getmana = false; turnused = true; return;
						}
						else continue;
					}
					else {
						if (blow(player.posx, player.posy, 1)){
							getmana = false; turnused = true; return;
						}
						else continue;
						
					}
					//return true;
					break;
				case commands::SUPERVAC:
					if (player.mana < 5){
						messagelog("Not enough steam!");
						//ADDSOUND nothing happening with a machine
						
						 getmana = true; turnused = true; return;
						//hopefully a turn is used and steam is given.
					}
					if (player.selectsuck){
						if (suck(player.posx, player.posy, 10))
						{
							getmana = false; turnused = true; return;
						}
						else continue;
					}
					else {
						if (blow(player.posx, player.posy, 10))
						{
							getmana = false; turnused = true; return;
						}
						else continue;
					}
					//return true;
					break;
					
				case commands::MUSDOWN:
					if (musvol == 0){
						messagelog("Music already on 00%");
						SHOW();
						continue;
					}
					else {
						musvol -= 5;
						messagelog("Music turned down to " + to_string(musvol) + "%");
						SHOW();
						setvol(fxvol, musvol);
						options_dirty = true;
						continue;
					}
					break;
				case commands::MUSUP:
					if (musvol == 100){
						messagelog("Music already on 100%");
						SHOW();
						continue;
					}
					else {
						musvol += 5;
						messagelog("Music turned up to " + to_string(musvol) + "%");
						SHOW();
						setvol(fxvol, musvol);
						options_dirty = true;
						continue;
					}
					break;
				case commands::FXDOWN:
					if (fxvol == 0){
						messagelog("FX already on 00%");
						SHOW();
						continue;
					}
					else {
						fxvol -= 5;
						messagelog("FX turned down to " + to_string(fxvol) + "%");
						SHOW();
						setvol(fxvol, musvol);
						options_dirty = true;
						continue;
					}
					break;
				case commands::FXUP:
					if (fxvol == 100){
						messagelog("FX already on 100%");
						SHOW();
						continue;
					}
					else {
						fxvol += 5;
						messagelog("FX turned up to " + to_string(fxvol) + "%");
						SHOW();
						setvol(fxvol, musvol);
						options_dirty = true;
						continue;
					}
					break;
				case commands::USE:
					if (player.held == nullptr){
						messagelog("You are holding nothing to use.");
						continue;
					}
					else {
						switch (player.held->type->type){
						case Eitemtype::ITEM_BATTERY:
							player.mana = 5;
							goto usedsomething;
							break;
						case Eitemtype::ITEM_STOPWATCH:
							player.stasis = 10;
							goto usedsomething;
							break;
						case Eitemtype::ITEM_MEDPACK:
							player.hp += 10;//can overheal
							if (heartplaying)stopsound();
							goto usedsomething;
							break;
						default:
							messagelog("You can't 'use' the " + player.held->type->name + ".");
							continue;
							break;
						}
					usedsomething:
						messagelog("You use the " + player.held->type->name + ".");
						delete player.held;  player.held = nullptr;
						playsound("bonus");
						getmana = true; turnused = true; return;
						
					}
					break;

				}
			}
	}
}



void timepasses(){
	//time passes.

	player.turns++;
	if (player.turns == 100){
		messagelog("The Extra Spaec Gate is active!", 139, 0, 204);
	}
	if (getmana){
		if (player.mana < 5){
			playsound("steam");
			player.mana++;
			if (player.mana == 5){
				playsound("ready");
			}
		}
		getmana = false;
	}//PROBLEM (DELETING ITEMS FROM VECTOR WHILE ITERATING OVER IT)
	for (auto f : map->moblist){
		if (f != nullptr){
			if (!f->invacuum){
				//std::cout << "first time " << (int) originx << " " << (int) originy << std::endl;
				map->item_getsaturn(f);
			}
		}
	}
	map->moblist.remove(0);//for problem

	//rabbit gating in chance
	if (player.turns > 100){
		if (lil::rand(1, 100) <2){ // rabbit love <3 RIP Hetmann 16/03/14
			int rx, ry;
			bool check = map->squarecheck(map->rabbitgatex, map->rabbitgatey, rx, ry,
				[](int x, int y){return map->passable.get(x, y); }
			);
			if (check){
				messagelog("ESR Rifle Trooper gated in.", 200, 162, 200);
				item_instance* m = new item_instance(Eitemtype::MOB_ESR, rx, ry);
				map->moblist.push_back(m);
				map->itemput(m, rx, ry);
			}
		}
	}


	if (player.stasis > 0){
		player.stasis--;
		if (player.stasis == 0){
			messagelog("Stasis of vacuum chamber ended", 25, 25, 225);
		}
	}
	else {
		//possible items in vac action
		;
	}


	
}

void renderscreen(){
	//render dungeon
	//std::cout << "r";
	//ORIG 19 AND 9
#define XSIZE 21
#define YSIZE 21

#define XHALF 10
#define YHALF 10




	originx = (player.posx < XHALF) ? 0 : player.posx - XHALF;
	originy = (player.posy < YHALF) ? 0 : player.posy - YHALF;

	if (player.posx>(map->width - (XHALF + 1)))originx = map->width - XSIZE;
	if (player.posy>(map->height - (YHALF + 1)))originy = map->height - YSIZE;

	SDL_Rect rect{ 0, 0, 16, 16 };

	for (size_t y = originy; y < originy + YSIZE; y++)
	{
		for (size_t x = originx; x < originx + XSIZE; x++)
		{
			rect.x = (x - originx) * 16;
			rect.y = (y - originy) * 16;

			SDL_Texture* ti;
			int lir = 0, lig = 0, lib = 0;


			switch (map->displaychar.at(x, y)){
			case '.':
				ti = dicosprite.at("floor");
				break;
			case '#':
				ti = dicosprite.at("wall");
				break;
			case '>':
				ti = dicosprite.at("light e");
				break;
			case '<':
				ti = dicosprite.at("light w");
				break;
			case 'v':
				ti = dicosprite.at("light s");
				break;
			case '^':
				ti = dicosprite.at("light n");
				break;
			case 'L':
				ti = dicosprite.at("lava");
				break;
			case 'G':
				ti = dicosprite.at("gate");
				break;
			case 'E':
				ti = dicosprite.at("exit");
				break;
			case 'C':
				ti = dicosprite.at("broom cupboard");
				break;
			case 'c':
				ti = dicosprite.at("coffer");
				break;
			default:
				ti = dicosprite.at("floor");
				break;
			}

			if (map->in_FOV.get(x, y)){
				lir = map->staticlight.at(x, y).r + map->dynamiclight.at(x, y).r;
				lig = map->staticlight.at(x, y).g + map->dynamiclight.at(x, y).g;
				lib = map->staticlight.at(x, y).b + map->dynamiclight.at(x, y).b;

				lir = clamp(lir, 25, 255);
				lig = clamp(lig, 25, 255);
				lib = clamp(lib, 25, 255);
				SDL_SetTextureColorMod(ti, lir, lig, lib);

				SDL_RenderCopy(renderer, ti, NULL, &rect);


				//render mob on square if there is one
				//rect.x = (testmap.playerx - originx) * 16;
				//rect.y = (testmap.playery - originy) * 16;
				item_instance*m = map->itemgrid.at(x, y);
				if (m != nullptr){
					SDL_RenderCopy(renderer, m->type->sprite, NULL, &rect);
					if (m->noticedplayer)
						SDL_RenderCopy(renderer, dicosprite.at("noticed"), NULL, &rect);

				}
				//end render mob

				//render locked icon if square is locked
				if (map->locked.get(x, y)){
					SDL_Texture* t = dicosprite.at("locked");
					SDL_SetTextureColorMod(t, lir, lig, lib);
					SDL_RenderCopy(renderer, t, NULL, &rect);
				}
				//

				//minimap
				//if (testmap.displaychar.at(x, y) !=' '){
				switch (map->displaychar.at(x, y)){
				case 'E':
					SDL_SetRenderDrawColor(renderer, 225, 0, 0, 255);
					break;
				case 'C':
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
					break;
				case 'c':
					SDL_SetRenderDrawColor(renderer, 225, 225, 25, 255);
					break;
				case '+':
					SDL_SetRenderDrawColor(renderer, 225, 225, 0, 255);
					break;
				case ' ':
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					break;
				default:
					SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
					break;
				}




				MINIMAP_PIXEL(x, y);

			}//begin not in fov
			else {
				if (!map->fogofwar.get(x, y)){
					//SDL_SetTextureColorMod(ti, 0, 0, 5);
					switch (map->playermemory.at(x, y)){
					case '.':
						ti = dicosprite.at("floor");
						break;
					case '#':
						ti = dicosprite.at("wall");
						break;
					case '+':
						ti = dicosprite.at("torch lit");
						break;
					case 'L':
						ti = dicosprite.at("lava");
						break;
					case '~':
						ti = dicosprite.at("water");
						break;
					default:
						ti = dicosprite.at("floor");
						break;
					}

					SDL_SetTextureColorMod(ti, 0, 0, 75);
				}
				else {
					SDL_SetTextureColorMod(ti, 0, 0, 0);
				}
				SDL_RenderCopy(renderer, ti, NULL, &rect);
			}
		}
	}

	rect.x = (player.posx - originx) * 16;
	rect.y = (player.posy - originy) * 16;

	//DRAW PLAYER
	//if (map->staticlight.at(player.posx, player.posy).total == 0 &&
	//	map->dynamiclight.at(player.posx, player.posy).total == 0)
	if (player.stealthed)
		SDL_RenderCopy(renderer, dicosprite.at("player stealth"), NULL, &rect);
	else
		SDL_RenderCopy(renderer, dicosprite.at("player"), NULL, &rect);
	//

	SDL_SetRenderDrawColor(renderer, 0, 225, 225, 255);
	MINIMAP_PIXEL(player.posx, player.posy);

	//USER INTERFACE
	//maybe move this to a function. think about does it need to get printed each time
	char sworddura;
	if (player.held != nullptr && player.held->type->type ==Eitemtype::ITEM_SWORD)
		sworddura = player.held->uses;
	else sworddura = 0;
	print("LEBEL: " + lil::numformat(player.level, 2)
		+ " SKOER: " + lil::numformat(player.score, 6)
		+ " HP " + lil::numformat(player.hp, 3)
		+ " turn " + lil::numformat(player.turns,4)
		+ " stasis " + lil::numformat(player.stasis, 2)
		+ " sword " + to_string(sworddura)
		, 0, 352, 0, 225, 225);
	//print("7DRL 2014 DAY SIX", 400, 0, 225, 225, 225);
	const int botline = 21 * 16;// 360 - 16;
	const int rightedge = (21 * 16);
	for (int i = 0; i < 10; i++)
	{
		drawsprite(rightedge, i * 16, "vacuum base");
	}
	int dota = player.vacuum_chamber.size();
	for (int i = 10; i < 20; i++)
	{
		drawsprite(rightedge , i * 16, "vacuum cell");
		if (dota>i - 10)drawsprite(rightedge, i * 16, player.vacuum_chamber[i - 10]->type->sprite);
	}
	drawsprite(rightedge , 16 * 20, "vacuum end");

	drawsprite(0, botline, "player portrait");
	for (char i = 0; i < 10; i++)
		if (i<player.hp)drawsprite(i * 16 + 16, botline, "heart full");
		else drawsprite(i * 16 + 16, botline, "heart empty");
	drawsprite(16 * 11, botline, "hand");
	drawsprite(16 * 12, botline, "frame");
	if (player.held != nullptr)drawsprite(16 * 12, 21 * 16, player.held->type->sprite);
	drawsprite(16 * 13, botline, (player.selectsuck) ? "suck" : "blow");
	drawsprite(16 * 14, botline, "clockwork box");
	for (char i = 0; i < 5; i++)
		drawsprite(16 * (i + 15), botline, (player.mana>i) ? "power full" : "power empty");
	drawsprite(16 * 20, botline, "power dial");
	//

	

	SHOW();
}

#if _DEBUG
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* args[])
#endif
{
	
	loadopts(); //load options from disk if there

	comeup(); //initialize stuff


	setupscanners();//need to do this once anyway. we do it again if options changed (even thought it may not have been keys that were changed omgomg)


	//this is where we go back to at the end of a game
gameloop:
	playmusic();

	if (!dotitlepage()){
		godown();
		return 0;
	}
	
	CLS();
	playmusic(false);

	if (options_dirty){
		saveopts();
		setupscanners();
	}

	//new game begins
	for (char f = 0; f < 9; f++)
		messagelog("");
	messagelog("Welcome to Sucker [7DRL 2014]",25,225,225);
	lil::randseed();

	//setup player
	player.init();

	if (map != nullptr)delete map;
	map = new RLMap(mapwidth, mapheight);
	//map->genlevel_rooms();
	//map->QuickdumpToConsole();

	moveplayer();
	renderscreen();
	
	//////////////////////////////////main loop
	while (1){


		//wait for player to do something
		waitonplayer();

		

		if (gameover == 3){
			gameover = 0;
			if (options_dirty)saveopts();//save options if music and sound vol changed in game
			goto gameloop;
		}

		if (turnused){
			turnused = false;
			//std::cout << "first time " << (int) originx << " " << (int) originy << std::endl;
			timepasses();
			//std::cout << "2nd time " << (int) originx << " " << (int) originy << std::endl;
			if (map->displaychar.at(player.posx, player.posy) == 'L'){
				player.damage(1, false, " lava");
				//ADDSOUND BURN
				messagelog("You burn!", 225, 0, 0);
				player.damage(2, false, "burning lava.");
			}
		}


		renderscreen();
		
	
		
		if (gameover==1){
			superprint("GAME OVER", 100, 225, 225, 225, 200);
			SHOW();
			GetKey();
			gameover = 0;
			if (options_dirty)saveopts();//save options if music and sound vol changed in game
			goto gameloop;
		}
		else if (gameover == 2){
			superprint("YOU WON!", 100, 35, 225, 35,200);
			SHOW();
			GetKey();
			gameover = 0;
			if (options_dirty)saveopts();//save options if music and sound vol changed in game
			goto gameloop;
		}
	}
	///////////////////////////////////////////

CleanupAndExit:

	godown();
	return 0;
}

void scorescreen(){
	//pangodelay(2000);
	//ADDSOUND MAYBE A NOIZ

	CLS();

	middleprint("You return to the depot after a hard level's work.",8);
	middleprint("You are greeted by Boss Man Barnaby",16);
	middleprint("'Alright, mate. Let's see whatcha got in ya vac'.",24);
	if (player.vacuum_chamber.size() == 0){
		middleprint("'...Nowt?! Fx me, mate, ya gotta try harder!", 32);
		middleprint("We got all sorts of fxen expenses we gotta pay, ya know.", 40);
		middleprint("Now get back out there and suck some snip up!'", 48);
	}
	else {
		int y = 64;
		for (auto f : player.vacuum_chamber){
			drawsprite(200, y,f->type->sprite);
			string s;
			switch (f->type->type){
			case Eitemtype::MOB_SKEL:case Eitemtype::MOB_KOBBY:
			case Eitemtype::MOB_GOLEM:case Eitemtype::MOB_CUEB:
			case Eitemtype::MOB_IMP:case Eitemtype::MOB_BAT:
			case Eitemtype::MOB_FIRESKEL:case Eitemtype::MOB_CUEBOBJ:
			s = "We'll release this back into the wild.";
			
			break;

			case Eitemtype::MOB_ESR:case Eitemtype::MOB_ESRSHIELD:
			s = "We'd better send this little fella back to Extra Spaec";
			break;

			case Eitemtype::MOB_BROOM:case Eitemtype::MOB_BROOMSWORD:
				s = "YOU DID IT! YOU RESCUED PRINCESS BROOM! [+5000]";
				player.score += 5000;
				break;

			case Eitemtype::MOB_TZIKEN:
				s = "WELL DONE! YOU'RE IN LINE FOR A SUPER PROMOTIONZ! +[20,000]";
				player.score += 20000;
				break;

			case Eitemtype::DEAD_BROOM:
				s = "How sad! But I don't think we've seen the last of him!";
				break;

			case Eitemtype::DEAD_TZIKEN:
				s = "AMG THIS IS AWFUL! TRAGIC! THIS IS ... um, tasty actually!";
				break;

			case Eitemtype::ITEM_GEM:
				s = "Nice work! Have a bonus! [+500]";
				player.score += 500;
				break;

			case Eitemtype::ITEM_GOLD:
				s = "Good stuff. A small something for you. [+100]";
				player.score += 100;
				break;
				//junk lava
			case Eitemtype::ITEM_BATTERY: case Eitemtype::ITEM_SHIELD:
			case Eitemtype::ITEM_STOPWATCH: case Eitemtype::ITEM_SWORD:
			case Eitemtype::ITEM_MEDPACK: case Eitemtype::ITEM_KEY:
				s = "I'll put this in the stock cupboard";
				break;
			default:
				s = "No-one wants this crap.";
				break;
			}
			delete f;
			print(s, 232, y + 8, 134, 201, 66);
				
			
		
			y += 20;
		}
	}

	player.vacuum_chamber.clear();

	middleprint("Press more or less any key to continue", 352);
	SHOW();
	GetKey();
}