
#include "stdafx.h"

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
#include <map>
#include <cmath>
#include <chrono>
#include <conio.h>
#include <limits>


#include <array>
#include <unordered_map>
#include <cmath>
using std::pair;
using std::make_pair;
#include "SDL.h"



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

#include "sprites.h"
#include "mobs.h"
#include "RLMap.h"



void loadsprites(void);


//"options"
//these are the defaults
bool OPTION_fullscreen = false;
char OPTION_res = 1;//0 1 2//myevent.key.keysym.scancode
enum class commands {N,S,E,W,NW,NE,SW,SE,WAIT,LANTERN,ESCAPE,SUCKBLOW};
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
	SDL_SCANCODE_T, //LANTERN	
	SDL_SCANCODE_ESCAPE, //ESCAPE
	SDL_SCANCODE_Z };//SUCKBLOW TOGGLE
							
							
bool options_dirty = false; //if set to true in titlepage we need to write options on getting back


unsigned char Scancode_to_command[512];

const string button_names [] = { "WAIT", "NORTH", "NORTH-EAST", "EAST", "SOUTH-EAST",
"SOUTH", "SOUTH-WEST", "WEST", "NORTH-WEST", "LANTERN", "TOGGLE SUCK/BLOW" };
#include "player.h"
Player player;
RLMap* map;

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
	int siez=OPTION_buttons.size();
	for (int upto=0; upto < siez; upto++){
		OPTION_buttons[upto] = buffer[upto+2];
	}
	file.close();
}

void saveopts(){
	char buffer[128];
	std::ofstream file;
	file.open(ASSET("optionz.dat"),std::ios::out | std::ios::binary);
	buffer[0] = (OPTION_fullscreen == false) ? 0 : 1;
	buffer[1] = OPTION_res;
	int upto = 2;
	for (auto f: OPTION_buttons){
		buffer[upto++] = f;
	}
	file.write(buffer, 128);
	file.close();
}

void comeup(){
	int x = 640, y = 360;
	x *= OPTION_res + 1;
	y *= OPTION_res + 1;
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("genericRL",
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
	setupmobs();
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
}
void moveplayer(){
	map->do_fov_rec_shadowcast(player.posx, player.posy, 11);
	map->dynamiclight.Fill({ 0, 0, 0, 0 });
	if (player.lantern)
		map->do_fov_foradynamiclight(player.posx, player.posy, 9, { 255, 255, 255 });
}
bool trytomove(int deltax, int deltay){
	int tentx = player.posx + deltax;
	int tenty = player.posy + deltay;
	if (tentx < 0 || tentx >= map->width || tenty < 0 || tenty >= map->height)
		return false;
	if (!map->passable.get(tentx, tenty))
		return false;
	player.posx = tentx, player.posy = tenty;
	
	moveplayer();
	
	return true;
}


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
						return;
					}
					else { continue; }
				
				switch (command){
				case commands::WAIT:
					//do nothing at all
					playsound("downstairs");
					return;
				case commands::LANTERN:
					player.lantern = !player.lantern;
					if (player.lantern)playsound("lantern");
					moveplayer();
					return;
				case commands::ESCAPE://esc. can't be changed
					godown();
					exit(0);
					break;
				case commands::SUCKBLOW:
					player.selectsuck = ! player.selectsuck;
					playsound(player.selectsuck ? "suck" : "blow");
					return;
				}
			}
	}
}



void timepasses(){

}

void renderscreen(){
	//render dungeon
	//std::cout << "r";
	//ORIG 19 AND 9
#define XSIZE 21
#define YSIZE 21

#define XHALF 10
#define YHALF 10




	int originx = (player.posx < XHALF) ? 0 : player.posx - XHALF;
	int originy = (player.posy < YHALF) ? 0 : player.posy - YHALF;

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
			case '~':
				ti = dicosprite.at("water");
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
				mob_instance*m = map->mobgrid.at(x, y);
				if (m != nullptr){
					SDL_RenderCopy(renderer, m->type->sprite, NULL, &rect);
				}
				//end render mob


				//minimap
				//if (testmap.displaychar.at(x, y) !=' '){
				switch (map->displaychar.at(x, y)){
				case 'L':
					SDL_SetRenderDrawColor(renderer, 225, 0, 0, 255);
					break;
				case '~':
					SDL_SetRenderDrawColor(renderer, 0, 0, 225, 255);
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

	//SDL_SetTextureBlendMode(dicosprite.at("player"), SDL_BLENDMODE_BLEND);
	if (map->staticlight.at(player.posx, player.posy).total==0 &&
		map->dynamiclight.at(player.posx, player.posy).total == 0)
			SDL_RenderCopy(renderer, dicosprite.at("player stealth"), NULL, &rect);
		else
			SDL_RenderCopy(renderer, dicosprite.at("player"), NULL, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 225, 225, 255);
	MINIMAP_PIXEL(player.posx, player.posy);

	print("7DRL 2014 DAY TWO", 400, 0, 225, 225, 225);

	//maybe move this to a function. think about does it need to get printed each time
	const int botline = 360 - 16;
	drawsprite(0, botline, "player portrait");
	for (char i = 0; i < player.hp; i++)
		drawsprite(i * 16 + 16, botline, "heart");
	drawsprite(16 * 11, botline, "hand");
	drawsprite(16 * 12, botline, "frame");
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

	playmusic(false);

	if (options_dirty){
		saveopts();
		setupscanners();
	}

	//new game begins

	lil::randseed();

	//setup player

	player.init();

	map = new RLMap(mapwidth, mapheight);
	map->genlevel_rooms();
	map->QuickdumpToConsole();

	moveplayer();
	renderscreen();
	

	while (1){


		//wait for player to do something
		waitonplayer();

		//time passes.
		if (player.mana < 5){
			playsound("steam");
			player.mana++;
			if (player.mana == 5){
				playsound("ready");
			}
		}
		
		
		
		renderscreen();

	}

CleanupAndExit:

	godown();
	return 0;
}
