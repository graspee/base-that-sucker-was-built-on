
#include "stdafx.h"

#include "lilhelpers.h"

#include "Array2D.h"
#include "BitArray.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <cmath>
#include <chrono>
#include <conio.h>
#include <limits>


#include <array>
#include <unordered_map>
#include <cmath>

#include "SDL.h"
//#include "SDL_main.h"
//#include "SDL_events.h"


#include "windows.h"
#undef max

inline unsigned int clamp(unsigned int x,unsigned int low, unsigned int high){
	if (x <= low)return low;
	if (x >= high)return high;
	return x;
}

int fprintf(FILE * stream, const char * format, ...){ return 0; }

#define NORMAL FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED
#define BRIGHT FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED
#define RED FOREGROUND_RED|FOREGROUND_INTENSITY
#define BLUE FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define GREEN FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define CYAN FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define YELLOW FOREGROUND_GREEN | FOREGROUND_RED| FOREGROUND_INTENSITY
#define DARKBLUE FOREGROUND_BLUE
#define DARKYELLOW FOREGROUND_GREEN | FOREGROUND_RED

void concol(int c){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleTextAttribute(hConsole, c);
}

void concurspos(int x, int y){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleCursorPosition(hConsole, { x, y });
}

#define MAX(a,b) ( (a>b)?a:b )
#define MIN(a,b) ( (a<b)?a:b )

#include "RLMap.h"



void loadsprites(void);
SDL_Window *window;
SDL_Renderer *renderer;

#include "sound.h"
#include "sprites.h"

#include "mapgen.h"

#if _DEBUG
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* args[])
#endif
{
	RLMap testmap(80, 50);


	randomlevel();//REMOVE

#include "notomap.h"


	testmap.playerx = 15;
	testmap.playery = 11;
	bool lantern = false;

	//int koboldx = 69, koboldy = 46;

	//bool result=testmap.PathfindDijkstra(koboldx,koboldy, playerx,playery);
	//bool result = testmap.PathfindAStar(koboldx, koboldy, playerx, playery);



	//testmap.InitializeLightmap();


	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("HalloweenRL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1280, 720,
		0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(renderer, 640, 360);



	SDL_RenderClear(renderer);
	SDL_Rect rect{ 0, 0, 16, 16 };



	SDL_Event myevent;



	soundinit();
	spriteinit();


	playsound();

	while (1){
		//while (!_kbhit()){}

		int a = SDL_WaitEvent(&myevent);

		//int a = _getch();


		if (myevent.type == SDL_KEYDOWN){
			switch (myevent.key.keysym.scancode){
			case SDL_SCANCODE_T:
				lantern = !lantern;
				break;
			case SDL_SCANCODE_KP_2:
				if (testmap.playery<testmap.height - 1 &&
					testmap.passable.get(testmap.playerx, testmap.playery + 1))
					testmap.playery++; break;
			case SDL_SCANCODE_KP_8:
				if (testmap.playery>0 &&
					testmap.passable.get(testmap.playerx, testmap.playery - 1))
					testmap.playery--; break;
			case SDL_SCANCODE_KP_6:
				if (testmap.playerx<testmap.width - 1 &&
					testmap.passable.get(testmap.playerx + 1, testmap.playery))
					testmap.playerx++; break;
			case SDL_SCANCODE_KP_4:
				if (testmap.playerx>0 &&
					testmap.passable.get(testmap.playerx - 1, testmap.playery))
					testmap.playerx--; break;
			case SDL_SCANCODE_ESCAPE:
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
					//SDL_RenderDrawPoint(renderer, 640 - testmap.width*2 + x*2, y*2);
					//SDL_RenderDrawPoint(renderer, 1+640 - testmap.width * 2 + x * 2, y * 2);
					//SDL_RenderDrawPoint(renderer, 640 - testmap.width * 2 + x * 2, 1+y * 2);
					//SDL_RenderDrawPoint(renderer, 1+640 - testmap.width * 2 + x * 2, 1+y * 2);
					//}
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
							/*case '.':
							ti = dicosprite.at("floor bp");
							break;
						case '#':
							ti = dicosprite.at("wall diag bp");
							break;
						case '+':
							ti = dicosprite.at("torch lit bp");
							break;
						case 'L':
							ti = dicosprite.at("lava bp");
							break;
						case '~':
							ti = dicosprite.at("water bp");
							break;
						default:
							ti = dicosprite.at("floor bp");
							break;*/
						//}
						SDL_SetTextureColorMod(ti, 0, 0, 75);
					}
					else {
						SDL_SetTextureColorMod(ti, 0, 0, 0);
					}
					SDL_RenderCopy(renderer, ti, NULL, &rect);
				}

				//if (testmap.fogofwar.get(x, y)){
				//if (x == testmap.playerx && y == testmap.playery){
				//if (testmap.in_FOV.get(x, y)){
				//if (testmap.staticlight.at(x, y) + testmap.dynamiclight.at(x, y)> 25){
				//else if (testmap.staticlight.at(x, y) + testmap.dynamiclight.at(x, y)> 0){
				//std::cout << testmap.displaychar.at(x, y);
				//std::cout << testmap.playermemory.at(x, y);
			}
		}







		rect.x = (testmap.playerx - originx) * 16;
		rect.y = (testmap.playery - originy) * 16;

		SDL_SetTextureBlendMode(dicosprite.at("player whm"), SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(renderer, dicosprite.at("player whm"), NULL, &rect);
		SDL_SetRenderDrawColor(renderer, 0, 225, 225, 255);
		MINIMAP_PIXEL(testmap.playerx, testmap.playery);

		print("train carriage", 500, 0, 225, 225, 225);
		print("train carriage", 0, 0, 225, 225, 225);

		SDL_RenderPresent(renderer);
	}

CleanupAndExit:

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	soundexit();
	spriteexit();
	SDL_Quit();
	return 0;
}
