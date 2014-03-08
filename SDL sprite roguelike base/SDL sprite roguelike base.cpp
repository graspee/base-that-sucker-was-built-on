
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

#include "RLMap.h"



void loadsprites(void);
SDL_Window *window;
SDL_Renderer *renderer;

//"options"
//these are the defaults
bool OPTION_fullscreen = false;
char OPTION_res = 1;//0 1 2//myevent.key.keysym.scancode
vector<int> OPTION_buttons = {	SDL_SCANCODE_KP_5,//WAIT
							SDL_SCANCODE_KP_8,//N
							SDL_SCANCODE_KP_9,//NE
							SDL_SCANCODE_KP_6,//E
							SDL_SCANCODE_KP_3,//SE
							SDL_SCANCODE_KP_2,//S
							SDL_SCANCODE_KP_1,//SW
							SDL_SCANCODE_KP_4,//W
							SDL_SCANCODE_KP_7,//NW
							SDL_SCANCODE_T }; //LANTERN
bool options_dirty = false; //if set to true in titlepage we need to write options on getting back


unsigned char Scancode_to_command[512];

const string button_names [] = { "WAIT", "NORTH", "NORTH-EAST", "EAST", "SOUTH-EAST",
"SOUTH", "SOUTH-WEST", "WEST", "NORTH-WEST", "LANTERN" };

#include "sound.h"
#include "sprites.h"
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
}

void godown(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	soundexit();
	spriteexit();
	SDL_Quit();
}

#if _DEBUG
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* args[])
#endif
{
	
	loadopts(); //load options from disk if there

	comeup(); //initialize stuff


	
	
	


	//this is where we go back to at the end of a game
	gameloop:
	if (!dotitlepage()){
		godown();
		return 0;
	}
	
	if (options_dirty) saveopts();

	for (auto f = 0; f < 512; f++)Scancode_to_command[f] = 255;
	char b = 0;
	for (auto f : OPTION_buttons)Scancode_to_command[f] = b++;
	Scancode_to_command[41] = 200;


	

	lil::randseed();

	playsound();


	RLMap testmap(mapwidth, mapheight);
	testmap.genlevel_rooms();
	testmap.QuickdumpToConsole();

	testmap.playerx = 15;
	testmap.playery = 11;
	bool lantern = false;

	SDL_Event myevent;

	while (1){
		

		int a = SDL_WaitEvent(&myevent);


	

		if (myevent.type == SDL_KEYDOWN){
			unsigned char command = Scancode_to_command[myevent.key.keysym.scancode];
			
			switch (command){
			case 9:
				lantern = !lantern;
				break;
			case 5://s
				if (testmap.playery<testmap.height - 1 &&
					testmap.passable.get(testmap.playerx, testmap.playery + 1))
					testmap.playery++; break;
			case 1://n
				if (testmap.playery>0 &&
					testmap.passable.get(testmap.playerx, testmap.playery - 1))
					testmap.playery--; break;
			case 3://e
				if (testmap.playerx<testmap.width - 1 &&
					testmap.passable.get(testmap.playerx + 1, testmap.playery))
					testmap.playerx++; break;
			case 7://w
				if (testmap.playerx>0 &&
					testmap.passable.get(testmap.playerx - 1, testmap.playery))
					testmap.playerx--; break;
			case 2://ne
				if (testmap.playery > 0 && testmap.playerx<testmap.width - 1 &&
					testmap.passable.get(testmap.playerx + 1, testmap.playery - 1))
				{
					testmap.playery--; testmap.playerx++;
				} break;
			case 8://nw
				if (testmap.playery>0 && testmap.playerx > 0 &&
					testmap.passable.get(testmap.playerx - 1, testmap.playery - 1))
				{
					testmap.playery--; testmap.playerx--;
				} break;
			case 4://se
				if (testmap.playery < testmap.height - 1 && testmap.playerx < testmap.width - 1 &&
					testmap.passable.get(testmap.playerx + 1, testmap.playery + 1))
				{
					testmap.playery++; testmap.playerx++;
				}break;
			case 6://sw
				if (testmap.playery<testmap.height - 1 && testmap.playerx>0 &&
					testmap.passable.get(testmap.playerx - 1, testmap.playery + 1))
				{
					testmap.playery++; testmap.playerx--;
				}break;
			case 200://esc. can't be changed
				goto CleanupAndExit;
				break;


			}
		}
		testmap.do_fov_rec_shadowcast(testmap.playerx, testmap.playery, 11);

		testmap.dynamiclight.Fill({0,0,0});
		if (lantern)
			testmap.do_fov_foradynamiclight(testmap.playerx, testmap.playery, 9, { 255, 255, 255 });


		//render dungeon

		//ORIG 19 AND 9
#define XSIZE 21
#define YSIZE 21

#define XHALF 10
#define YHALF 10




		int originx = (testmap.playerx < XHALF) ? 0 : testmap.playerx - XHALF;
		int originy = (testmap.playery < YHALF) ? 0 : testmap.playery - YHALF;

		if (testmap.playerx>(testmap.width - (XHALF + 1)))originx = testmap.width - XSIZE;
		if (testmap.playery>(testmap.height - (YHALF + 1)))originy = testmap.height - YSIZE;

		SDL_Rect rect{ 0, 0, 16, 16 };

		for (size_t y = originy; y < originy + YSIZE; y++)
		{
			for (size_t x = originx; x < originx + XSIZE; x++)
			{
				rect.x = (x - originx) * 16;
				rect.y = (y - originy) * 16;

				SDL_Texture* ti;
				int lir = 0,lig=0,lib=0;


				switch (testmap.displaychar.at(x, y)){
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

				if (testmap.in_FOV.get(x, y)){
					lir = testmap.staticlight.at(x, y).r + testmap.dynamiclight.at(x, y).r;
					lig = testmap.staticlight.at(x, y).g + testmap.dynamiclight.at(x, y).g;
					lib = testmap.staticlight.at(x, y).b + testmap.dynamiclight.at(x, y).b;
					
					lir = clamp(lir, 25, 255);
					lig = clamp(lig, 25, 255);
					lib = clamp(lib, 25, 255);
					SDL_SetTextureColorMod(ti, lir, lig, lib);
					
					SDL_RenderCopy(renderer, ti, NULL, &rect);

					//minimap
					//if (testmap.displaychar.at(x, y) !=' '){
					switch (testmap.displaychar.at(x, y)){
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

#define MINIMAP_PIXEL(THISX,THISY) \
	SDL_RenderDrawPoint(renderer, 640 - testmap.width*2 + THISX*2, THISY*2); \
	SDL_RenderDrawPoint(renderer, 1 + 640 - testmap.width * 2 + THISX * 2, THISY * 2); \
	SDL_RenderDrawPoint(renderer, 640 - testmap.width * 2 + THISX * 2, 1 + THISY * 2); \
	SDL_RenderDrawPoint(renderer, 1 + 640 - testmap.width * 2 + THISX * 2, 1 + THISY * 2)


					MINIMAP_PIXEL(x, y);
					
				}//begin not in fov
				else {
					if (!testmap.fogofwar.get(x, y)){
						//SDL_SetTextureColorMod(ti, 0, 0, 5);
						switch (testmap.playermemory.at(x, y)){
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

		rect.x = (testmap.playerx - originx) * 16;
		rect.y = (testmap.playery - originy) * 16;

		SDL_SetTextureBlendMode(dicosprite.at("player whm"), SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(renderer, dicosprite.at("player whm"), NULL, &rect);
		SDL_SetRenderDrawColor(renderer, 0, 225, 225, 255);
		MINIMAP_PIXEL(testmap.playerx, testmap.playery);

		print("work in progress", 0, 340, 225, 225, 225);
		
		SDL_RenderPresent(renderer);
	}

CleanupAndExit:

	godown();
	return 0;
}
