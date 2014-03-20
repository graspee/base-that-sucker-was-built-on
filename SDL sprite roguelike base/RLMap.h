#pragma once

#include <functional>


void SHOW();
int  GetKey();
void damagemob(item_instance* i, int x, int y, int amount);

struct ColouredLight {
	signed int r, g, b,total;
};

static const ColouredLight walllight = { 255, 79, 0 };

//RLMAP REPRESENTS ONE RL GAME MAP OR LEVEL
class RLMap {
private:

	
	
	static const int multipliers[4][8];

	struct todoitem {
		int x, y;
	};
	int maxint; //FIXME NEEDS TO BE STATIC

public:

	
	//features to keep track of:
	int rabbitgatex = -1, rabbitgatey = -1;
	size_t width, height;						//width and height of the map in cells
	Array2D<unsigned char> displaychar;			//ASCII char for each cell
	BitArray passable;							//can player and mobs move through this square yes or no
	BitArray blocks_sight;						//does this square block sight? e.g. wall yes, open door no
	BitArray in_FOV;							//FOV routines set this to true in each square in FOV
	BitArray FOV_set_this_run;					//temp array to use in fov routine to avoid processing each in_fov square multiple times
	BitArray fogofwar;							//is cell covered by fog of war, i.e. not discovered yet? starts off true for every cell
	BitArray locked;							//is square "locked". can't take item from square, can't use item ON square
	Array2D<unsigned char> playermemory;		//this is what player last saw on that square (architecture only)			
	
	Array2D<ColouredLight> staticlight;			//light amount on cell e.g. torches. can be changed but not often
	Array2D<ColouredLight> dynamiclight;
	
	Array2D<int> distance;						//this is used by the pathfinding routines
	std::list<todoitem> lastpath;				//this is filled by the pathfinding routines
	int firststepx, firststepy;					//used by pathfinding

	vector<pair<int, int>> emptyspaces;			//free squares

	//we don't want array2d itemgrid because that makes a mob for
	//each square. we want an array2d of mob*
	Array2D<item_instance*> itemgrid;
	//vector<item_instance*> moblist;
	list<item_instance*> moblist;
	//vector<item_instance*> itemlist;

	//give it an x and y and a fn and it returns you a random 8neigbour that matches the fn
	bool squarecheck(int x, int y, int& outx, int &outy, 
		std::function<bool(int x, int y)> f){

		vector<char> v;

		for (char i = 0; i < 8; i++){
			int tentx = x + lil::deltax[i];
			int tenty = y + lil::deltay[i];
			if (tentx < 0 || tenty < 0 || tentx >= width || tenty >= height)continue;
			else {
				if (f(tentx, tenty))v.push_back(i);
			}
		}
		if (v.size() < 1)return false;
		else{
			char which = lil::randmember(v);
			outx = x + lil::deltax[which];
			outy = y + lil::deltay[which];
			return true;
		}
	}

	inline void itemremovefrom(item_instance* i){
		itemgrid.at(i->posx, i->posy) = nullptr;
		passable.set(i->posx, i->posy, true);
		i->posx = -1, i->posy = -1;
	}


	//puts this item (that wasn't on the map, onto the map)
	inline void itemput(item_instance* i,int x, int y){
		i->posx = x, i->posy = y;
		itemgrid.at(x, y) = i;
		passable.set(x, y, false);
		return;
	}
	//moves an item on the map from its current location to a new one
	inline void itemmoveto(item_instance* i,int x, int y){
		itemremovefrom(i);
		itemput(i,x, y);
		seewhatmobmakesofsquare(i, x, y);
	}

	char getrandomblanksurround(item_instance* i){
		vector<int> v;
		for (char f = 0; f < 8; f++)
		{
			int tentx = i->posx + lil::deltax[f];
			int tenty = i->posy + lil::deltay[f];

			if ( (tentx>0 && tenty>0 && tentx<width && tenty<height) &&
				(itemgrid.at(tentx, tenty) == nullptr)) {
				v.push_back(f);
			}
		}

		if (v.size() == 0)return 0;
		else return lil::randmember(v);

	}
	inline bool onscreen(int x, int y){
		return (x >= originx && y >= originy
			&&x < originx + 21 && y < originy + 21);
	}
	void item_getsaturn(item_instance* i){
		//std::cout << "getsaturn " << (int) originx << " " << (int) originy << std::endl;
		if (in_FOV.get(i->posx, i->posy)							//if this item is in your field of view
			&& i->type->ismob										//and it's a mob
			&& i->type->aggressive									//and that mob is of an aggressive type
			//&& i->behaviour != EBehaviour::BEHAVIOUR_CHASING		//and it's not already chasing you
			&& ! i->noticedplayer									//and it hasn't noticed player	
			&& !player.stealthed){									//and player is not stealthed
			i->noticedplayer = true;
			i->behaviour = EBehaviour::BEHAVIOUR_CHASING;
			//ADDSOUND METAL GEAR SOLID STYLE NOTICED YOU NOISE
			messagelog("The "+i->type->name+" has seen you!");
		}

		//mobs can notice you and attack you on the same turn- is this wise? especially now
		//we have death-dealing laser rabbits

		//ESR laser attack
		if (i->type->type == Eitemtype::MOB_ESR || i->type->type == Eitemtype::MOB_ESRSHIELD){
			if (i->noticedplayer && !player.stealthed &&
				onscreen(i->posx, i->posy)){
				int dx = abs(i->posx - player.posx);
				int dy = abs(i->posy - player.posy);
				if (
					(i->posx==player.posx || i->posy==player.posy || dx == dy)
					&& (dx>1 || dy>1)
					){
					//does rabbit have kleer shot?
					//god i'm so fucking tired
					//can't fucking think any more
					//11:36 am 16 march 2014
					//6h24m to deadline
					bool kleershot = true;
					int curx = i->posx, cury = i->posy;
					while (!(curx == player.posx && cury == player.posy)){
						if (curx > player.posx)curx--;
						if (curx < player.posx)curx++;
						if (cury > player.posy)cury--;
						if (cury < player.posy)cury++;
						if (passable.get(curx, cury) == false){
							kleershot = false;
							break;
						}
					}
					if (kleershot){
						playsound("laser");
						
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						renderscreen();
						//laser "effect"
						int curx = ((i->posx - originx)*16), cury = ((i->posy - originy)*16);
						int destx = ((player.posx - originx) * 16) ;
						int desty = ((player.posy - originy) * 16) ;
						
						
						
						while (!(curx == destx && cury == desty)){
							if (curx > destx)curx--;
							if (curx <destx)curx++;
							if (cury > desty)cury--;
							if (cury < desty)cury++;
							drawsprite(curx, cury, "laser");
							//SHOW();
							//GetKey();
						}
						
						messagelog("The ESR fires at you.");
						player.damage(1, false, "rabbit laser fire");
						SHOW();
						pangodelay(500);
						
						return;
					
					}

				}
			}
				//in_FOV.get(i->posx,i->posy))

			//if in fov
			//if its x is same as yours or its y is
			//or its x diff from you (abs) = its abs y diff from you
		}


		//check all directions from mob. if there's something the mob wants then 
		//it gets it
		//broom - treasure and sword, battle broom - mob
		//esr - batteries, cueb - anything
		//mob in attack mode- you or broom

		if (i->noticedplayer){
			if (abs(i->posx - player.posx) <= 1 && abs(i->posy - player.posy) <= 1){
				playsound("bite");
			
				messagelog("The " + i->type->name + " attacks you for " + to_string(i->type->damagetheydeal * 2) + " damage.", 225, 0, 0);
				player.damage(i->type->damagetheydeal, true,i->type->name);
				return;
			}
		}



		//stop mobs moving onto your square. even if mobs would attack you instead broom will move
		//on top of you

		//if stationary- 1 in 5 chance of starting to move
		if (i->behaviour == EBehaviour::BEHAVIOUR_STATIC && lil::rand(1, 100) <= 20){
			i->behaviour == EBehaviour::BEHAVIOUR_WANDERING;
			//when you start to move- pick a direction that isn't blocked
			i->movdircurrent = getrandomblanksurround(i);
			//i->movdircurrent = lil::rand(0, 7);
			return;
		}
		//if moving - 1 in 10 chance of it stopping or changing direction
		if (i->behaviour == EBehaviour::BEHAVIOUR_WANDERING && lil::rand(1, 100) <= 10){
			if (lil::rand(1, 100) <= 25){
				i->behaviour = EBehaviour::BEHAVIOUR_STATIC;
			}
			else {
				i->movdircurrent = lil::rand(0, 7);
			}
			return;
		}
		//at this point it may or may not have "noticed" the player this turn
		//its behaviour could be any type, but it has passed up an opportunity
		//to change dir or stop if not chasing player

		if (i->behaviour == EBehaviour::BEHAVIOUR_CHASING){
			//kludge- map cell containing mob or item is normally marked as unpassable
			//here we temporarily set it to passable for astar to work then set it back after
			passable.set(i->posx, i->posy, true);
			
			bool astar;
			if (i->type->fitsinvacuum)
				astar = PathfindAStar(i->posx, i->posy, player.posx, player.posy);
			else
				astar = PathfindAStar(i->posx, i->posy, player.posx, player.posy,false);
			passable.set(i->posx, i->posy, false);
			if (astar){
				itemmoveto(i, firststepx, firststepy);
				//std::cout << i->type->name << " moved to " << (int)i->posx << " " << (int) i->posy << std::endl;
				return;
			}
		}
		else {
			int tentx = i->posx + lil::deltax[i->movdircurrent];
			int tenty = i->posy + lil::deltay[i->movdircurrent];

			if ((tentx < 0 || tenty < 0 || tentx >= width || tenty >= height)//off map
				|| (!passable.get(tentx, tenty))){ //wall, light or object 
				i->behaviour = EBehaviour::BEHAVIOUR_STATIC;
				return;
			}

			this->itemmoveto(i, tentx, tenty);
			return;
		}
	}

	inline static int Distance_Chebyshev(int x, int y, int x2, int y2) {
		int dx = abs(x - x2);
		int dy = abs(y - y2);
		return (dx > dy) ? dx : dy;
	}

	inline static int Distance_Manhatten(int x, int y, int x2, int y2) {
		int dx = abs(x - x2);
		int dy = abs(y - y2);
		return  dx + dy;
	}

	inline static int Distance_Euclidean(int x, int y, int x2, int y2) {
		int dx = abs(x - x2);
		int dy = abs(y - y2);
		return sqrt((dx*dx) + (dy*dy));
	}

	inline static int Distance_Squared(int x, int y, int x2, int y2) {
		int dx = abs(x - x2);
		int dy = abs(y - y2);
		return (dx*dx) + (dy*dy);
	}
	//CREATE A RL MAP
	RLMap(int _w, int _h) :
		width(_w), height(_h) {
		locked.Init(false, width, height);
		itemgrid.Init(nullptr, width, height);
		displaychar.Init(0, width, height);
		passable.Init(true, width, height);
		blocks_sight.Init(false, width, height);
		distance.Init(width, height);
		in_FOV.Init(false, width, height);
		FOV_set_this_run.Init(false, width, height);
		maxint = std::numeric_limits<int>::max();
		playermemory.Init(0, width, height);
		fogofwar.Init(true, width, height);
		
		staticlight.Init({ 0, 0, 0, 0 }, width, height);
		dynamiclight.Init({ 0, 0, 0, 0 },width, height);

		genlevel_rooms();
		
	}

	//CLEAR THE MAP
	void Clear(void){
		displaychar.Fill('^');
	}

	~RLMap(){
		//std::cout << "~map going down";
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				if (itemgrid.at(x, y) != nullptr)
					delete itemgrid.at(x, y);
			}
		}
	}

	
	//PUT A RECTANGULAR BLOCK IN MAP
	void RectFill(int _x, int _y, int _w, int _h){
		for (size_t y = _y; y < _y + _h; y++)
		{
			for (size_t x = _x; x < _x + _w; x++)
			{
				displaychar.at(x, y) = '#';
				passable.set(x, y, false);
				blocks_sight.set(x, y, true);
			}
		}
	}

	//DIJKSTRA

#undef DIJKSTRA_DIRECTIONS_CARDINAL_ONLY


	bool PathfindDijkstra(int startx, int starty, int goalx, int goaly){
		std::chrono::high_resolution_clock::time_point timestart = std::chrono::high_resolution_clock::now();


		//static const int dircount = 8;
		//static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0 };
		static const int dircount = 16;
		static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0, 1, -1, 1, 1, -1, 1, -1, -1 };


		BitArray visited(false, width, height);

		distance.Fill(maxint);

		int debugnodecount = 0;

		std::list<todoitem> todolist;

		//int debugcounter = 0;

		//we start from the goal and try to find the start
		todolist.push_front({ goalx, goaly });

		//make the goal (actually the start) stand out. use distance
		distance.at(startx, starty) = -1;
		distance.at(goalx, goaly) = 0;

		while (!todolist.empty()){
			todoitem t = todolist.front();
			todolist.pop_front();

			for (auto f = 0; f < dircount; f += 2){
				int scanx = t.x + dxdy[f];
				int scany = t.y + dxdy[f + 1];

				if (scanx < 0 || scany < 0 || scanx >= width || scany >= height){
					//square is off the map
					continue;
				}
				if (visited.get(scanx, scany)){
					continue;
				}
				if (!passable.get(scanx, scany)){
					//not a walkable square
					visited.set(scanx, scany, true);
					continue;
				}
				if (distance.at(scanx, scany) == -1){
					//found the goal (the start)

					std::chrono::high_resolution_clock::time_point timestop = std::chrono::high_resolution_clock::now();

					std::chrono::duration<double> timespent =
						std::chrono::duration_cast<std::chrono::duration<double>>(timestop - timestart);

					std::cout << "Time in sec: " << timespent.count() << std::endl;
					std::cout << "nodes: " << debugnodecount << std::endl;


					distance.at(scanx, scany) = maxint - 1;
					int nodex = startx, nodey = starty;
					lastpath.clear();

					int lowest = maxint;
					while (lowest != 0){
						int lowestx = maxint - 1, lowesty = maxint - 1;
						for (auto f = 0; f < dircount; f += 2){
							int scanx = nodex + dxdy[f];
							int scany = nodey + dxdy[f + 1];
							if (scanx < 0 || scany < 0 || scanx >= width || scany >= height)continue;
							if (distance.at(scanx, scany) < lowest){
								lowest = distance.at(scanx, scany);
								lowestx = scanx, lowesty = scany;
							}
						}
						lastpath.push_front({ lowestx, lowesty });
						nodex = lowestx, nodey = lowesty;
						displaychar.at(lowestx, lowesty) = '*';
					}
					return true;
				}

				if (distance.at(scanx, scany) == maxint){
					todolist.push_back({ scanx, scany });
					//if (visited.at(scanx, scany)){
					//	std::cout << "1";
					//}
				}

				if (distance.at(t.x, t.y) + 1 < distance.at(scanx, scany)){
					distance.at(scanx, scany) = distance.at(t.x, t.y) + 1;
					displaychar.at(scanx, scany) = '.';// '0' + distance.at(t.x, t.y) + 1;
				}



			}
			visited.set(t.x, t.y, true); debugnodecount++;

			//	debugcounter++;
			//	if (debugcounter == 25){
			//		std::cout << "begin" << std::endl;
			//		for (auto f : todolist){
			//			std::cout << distance.at(f.x, f.y) << std::endl;
			//		}
			//		std::cout << "end" << std::endl;
			//				DisplayOnConsole();
			//		debugcounter = 0;
			//	}
		}
		return false;


	}



	bool PathfindAStar(int startx, int starty, int goalx, int goaly,bool diags=true,bool fillpath=false){
		//std::chrono::high_resolution_clock::time_point timestart = std::chrono::high_resolution_clock::now();

		////old version
		////static const int dircount = 8;
		////static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0 };
		//static const int dircount = 16;
		//static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0, 1, -1, 1, 1, -1, 1, -1, -1 };
		////end old version

		int dircount = (diags) ? 16 : 8;
		static const int dxdy[16] = {0, -1, 1, 0, 0, 1, -1, 0, 1, -1, 1, 1, -1, 1, -1, -1 };
		
		BitArray visited(false, width, height);

		distance.Fill(maxint);

		//int debugnodecount = 0; //NEW REM

		std::multimap<int, todoitem> todolist;
		typedef std::pair<int, todoitem> todopair;

		//we start from the goal and try to find the start
		todolist.insert(todopair{ 0 + Distance_Euclidean(startx, starty, goalx, goaly), { goalx, goaly } });

		//make the goal (actually the start) stand out. use distance
		distance.at(startx, starty) = -1;
		distance.at(goalx, goaly) = 0;

		while (!todolist.empty()){
			auto it = todolist.begin();
			todoitem t = (*it).second;
			//int test = (*it).first;
			todolist.erase(it);

			for (auto f = 0; f < dircount; f += 2){
				int scanx = t.x + dxdy[f];
				int scany = t.y + dxdy[f + 1];

				if (scanx < 0 || scany < 0 || scanx >= width || scany >= height){
					//square is off the map
					continue;
				}
				if (visited.get(scanx, scany)){
					continue;
				}
				if (!passable.get(scanx, scany)){
					//not a walkable square
					visited.set(scanx, scany, true);
					continue;
				}
				if (distance.at(scanx, scany) == -1){
					//found the goal (the start)
					//std::chrono::high_resolution_clock::time_point timestop = std::chrono::high_resolution_clock::now();

					//std::chrono::duration<double> timespent =
					//	std::chrono::duration_cast<std::chrono::duration<double>>(timestop - timestart);

					//std::cout << "Time in sec: " << timespent.count() << std::endl;
					//std::cout << "nodes: " << debugnodecount << std::endl;

					
					if (fillpath){
						distance.at(scanx, scany) = maxint - 1;
						int nodex = startx, nodey = starty;
						lastpath.clear();

						int lowest = maxint;
						while (lowest != 0){
							int lowestx = maxint - 1, lowesty = maxint - 1;
							for (auto f = 0; f < dircount; f += 2){
								int scanx = nodex + dxdy[f];
								int scany = nodey + dxdy[f + 1];
								if (scanx < 0 || scany < 0 || scanx >= width || scany >= height)continue;
								if (distance.at(scanx, scany) < lowest){
									lowest = distance.at(scanx, scany);
									lowestx = scanx, lowesty = scany;
								}
							}
							lastpath.push_front({ lowestx, lowesty });
							nodex = lowestx, nodey = lowesty;
							//displaychar.at(lowestx, lowesty) = '*'; //NEW REM
						}//end while lowest !=0
					}//end if fill path
					else {
						int lowest = maxint;
						for (char f = 0; f < dircount; f+=2){
							int scanx = startx + dxdy[f];
							int scany = starty + dxdy[f + 1];
							if (scanx < 0 || scany < 0 || scanx >= width || scany >= height)continue;
							if (distance.at(scanx, scany) < lowest){
								lowest = distance.at(scanx, scany);
								firststepx = scanx, firststepy = scany;
							}
						}
						
					}
					return true;
				}//end found start


				//test = test;
				bool sowasit = (distance.at(scanx, scany) == maxint);

				//int cs_txty = Chebyshev(t.x, t.y, startx, starty);
				int cs_sxsy = Distance_Euclidean(scanx, scany, startx, starty);

				int d_txty = distance.at(t.x, t.y) + 1;
				int d_sxsy = distance.at(scanx, scany);


				//if(sowasit || (d_txty+cs_txty < d_sxsy+cs_sxsy)){
				if (sowasit || (d_txty < d_sxsy)){
					distance.at(scanx, scany) = d_txty;


					//displaychar.at(scanx, scany) = '.';// '0' + distance.at(t.x, t.y) + 1;

				}

				if (sowasit){
					todolist.insert(todopair{ distance.at(scanx, scany) + cs_sxsy, { scanx, scany } });
				}

			}

			visited.set(t.x, t.y, true);
			//displaychar.at(t.x, t.y) = '.';//NEW REM
			//debugnodecount++; //NEW REM

			//	debugcounter++;
			//	if (debugcounter == 25){
			//		std::cout << "begin" << std::endl;
			//		for (auto f : todolist){
			//			std::cout << f.first << std::endl;
			//		}
			//		std::cout << "end" << std::endl; 
			//				DisplayOnConsole();
			//		debugcounter = 0;
			//}

		}
		return false;


	}

	//FOV STUFF
	//from http://www.roguebasin.com/index.php?title=C%2B%2B_shadowcasting_implementation
	// "A C++ implementation of Bjorn Bergstrom's recursive shadowcasting FOV algorithm."

	void cast_light(uint x, uint y, uint radius, uint row,
		float start_slope, float end_slope, uint xx, uint xy, uint yx,
		uint yy, std::function<void (uint x, uint y)> WhatToDo) {
		if (start_slope < end_slope) {
			return;
		}
		float next_start_slope = start_slope;
		for (uint i = row; i <= radius; i++) {
			bool blocked = false;
			for (int dx = 0 - i, dy = 0 - i; dx <= 0; dx++) {
				float l_slope = (dx - 0.5) / (dy + 0.5);
				float r_slope = (dx + 0.5) / (dy - 0.5);
				if (start_slope < r_slope) {
					continue;
				}
				else if (end_slope > l_slope) {
					break;
				}

				int sax = dx * xx + dy * xy;
				int say = dx * yx + dy * yy;
				if ((sax < 0 && (uint) std::abs(sax) > x) ||
					(say < 0 && (uint) std::abs(say) > y)) {
					continue;
				}
				uint ax = x + sax;
				uint ay = y + say;
				if (ax >= width || ay >= height) {
					continue;
				}

				uint radius2 = radius * radius;
				if ((uint) (dx * dx + dy * dy) < radius2) {
					if (!FOV_set_this_run.get(ax, ay)){
						FOV_set_this_run.set(ax, ay, true);
						WhatToDo(ax, ay);
					}
				}

				if (blocked) {
					if (blocks_sight.get(ax, ay)) {
						next_start_slope = r_slope;
						continue;
					}
					else {
						blocked = false;
						start_slope = next_start_slope;
					}
				}
				else if (blocks_sight.get(ax, ay)) {
					blocked = true;
					next_start_slope = r_slope;
					cast_light(x, y, radius, i + 1, start_slope, l_slope, xx,
						xy, yx, yy,WhatToDo);
				}
			}
			if (blocked) {
				break;
			}
		}
	}

	void do_fov_rec_shadowcast(uint x, uint y, uint radius) {
		FOV_set_this_run.Fill(false);
		in_FOV.Fill(false);
		
		auto ff = [this](uint x, uint y){
			in_FOV.set(x, y, true);
			fogofwar.set(x, y, false);
			playermemory.at(x, y) = displaychar.at(x, y);
		};

		for (uint i = 0; i < 8; i++) {
			cast_light(x, y, radius, 1, 1.0, 0.0, multipliers[0][i],
				multipliers[1][i], multipliers[2][i], multipliers[3][i],
				ff
			);
		}
		ff(x, y);
	}



	
	void do_fov_foralight(uint callx, uint cally, uint radius, ColouredLight clr,char directional=-1) {
		FOV_set_this_run.Fill(false);

		auto ff = [this, callx, cally, radius, clr,directional](uint xx, uint yy){

			if (directional == -1)goto lightit;
			if (callx == xx && cally == yy)goto lightit;
			switch (directional){
			case 0://n
				if (yy >= cally)return;
				break;
			case 1://s
				if (yy <= cally)return;
				break;
			case 2://e
				if (xx <= callx)return;
				break;
			case 3://w
				if (xx >= callx)return;
				break;
			}
		lightit:
			dolight(callx, cally, xx, yy, radius, clr, staticlight.at(xx, yy));
		


		};




		for (uint i = 0; i < 8; i++) {
			cast_light(callx, cally, radius, 1, 1.0, 0.0, multipliers[0][i],
				multipliers[1][i], multipliers[2][i], multipliers[3][i],
				ff
				);
		}
		ff(callx, cally);
	}

	inline void dolight(uint x1, uint y1, uint x2, uint y2,
		uint r, const ColouredLight& colour, ColouredLight& store){
		//nice lave with 3 + ds /5 and radius 9
		double c1 = 1.0 / (1.0 + (double)(Distance_Squared(x1, y1, x2, y2)) / 20.0);
		double c2 = c1 - 1.0 / (1.0 + (double)(r*r));
		double c3 = c2 / (1.0 - 1.0 / (1.0 + (double)(r*r)));

		store.r += (int)((double)(colour.r) * c3);
		store.g += (int)((double)(colour.g) * c3);
		store.b += (int)((double)(colour.b) * c3);

		store.total = store.r + store.g + store.g;
	
	}

		void do_fov_foradynamiclight(uint callx, uint cally, uint radius,ColouredLight clr) {
			FOV_set_this_run.Fill(false);
			
			//
		
		auto ff = [this, callx, cally, radius, clr](uint xx, uint yy){

			dolight(callx, cally, xx, yy, radius, clr, dynamiclight.at(xx, yy));
			
		};
		
		for (uint i = 0; i < 8; i++) {
			cast_light(callx, cally, radius, 1, 1.0, 0.0, multipliers[0][i],
				multipliers[1][i], multipliers[2][i], multipliers[3][i],
				ff
			);
		}
		ff(callx, cally);
	}

		void dostaticlights(){
			staticlight.Fill({ 0, 0, 0, 0 });
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){
					switch (displaychar.at(x, y)){
					case '^':
						do_fov_foralight(x, y, 9, walllight,0 );
						break;
					case 'v':
						do_fov_foralight(x, y, 9, walllight, 1);
						break;
					case '>':
						do_fov_foralight(x, y, 9, walllight, 3);
						break;
					case '<':
						do_fov_foralight(x, y, 9, walllight,4);
						break;
					case 'L':
						do_fov_foralight(x, y, 5, { 255, 0, 0 });
						break;
					default:
						break;
					}
				}
			}
		}


		void genlevel_rooms();
		//bool CheckAllSidesAndPickOne(int x,int y,char seek,int& xout, int&yout);
		void QuickdumpToConsole(){
			for (size_t y = 0; y < height; y++)
				{
					for (size_t x = 0; x < width; x++)
					{
						std::cout << displaychar.at(x, y);
					}
					std::cout << std::endl;
				}
		}

		int shootray(int x,int y,int deltax, int deltay, char seek);
		bool shootrays(int x, int y, char seek, int& outx, int& outy, char &direction,bool fudge=false);

		inline void freespace(int& a, int& b){
			pair<int, int> p = lil::onefromthetop(emptyspaces);
			a = p.first, b = p.second;
		}

		inline void mapinsert(char c){
			int x, y;
			freespace(x, y);
			displaychar.at(x, y) = c;
		}

		inline void additem(int howmany, Eitemtype it){
			//std::cout << "adding " << (int)howmany << " " << dicoarchetype[it].name << std::endl;
			for (int f = 0; f < howmany; f++){
				int x, y;
				freespace(x, y);
				item_instance* m = new item_instance(it, x, y);
				//itemlist.push_back(m);
				itemput(m, x, y);
			}
		}
		inline void addmob(int howmany, Eitemtype it){
			for (int f = 0; f < howmany; f++){
				int x, y;
				freespace(x, y);
				item_instance* m = new item_instance(it, x, y);
				moblist.push_back(m);
				itemput(m, x, y);
			}
		}

		//works in vac and out
		void transform_mob(item_instance* i,int x, int y, Eitemtype type){
			item_instance*i2= new item_instance(type, x, y);
			i2->invacuum = i->invacuum;
			if (i->invacuum){
				lil::replacevalue(player.vacuum_chamber, i, i2);
			}
			else {
				itemput(i2, x, y);
			}
			lil::replacevalue(moblist, i, i2);
			delete i;
		}

		void seewhatmobmakesofsquare(item_instance*i,int x,int y){
			if (displaychar.at(x, y) == 'L'){
				if (i->type->type == Eitemtype::MOB_SKEL){
					messagelog("The skelington burns!", 255, 165, 0);
					messagelog("A more dangerous foe appears!", 255, 165, 0);
					transform_mob(i, x, y, Eitemtype::MOB_FIRESKEL);
					return;
				}
				if (i->type->ismob && i->type->lavadamagesthem){
					messagelog("The " + i->type->name + " burns.");
					//ADDSOUND BURN
					damagemob(i, x, y, 1);
				}
			}
		}

		void lavaize(int x, int y){
			displaychar.at(x, y) = 'L';
			dostaticlights();
			//if skel on square- becomes fire skel
			item_instance* i = itemgrid.at(x, y);
			if (i != nullptr){
				seewhatmobmakesofsquare(i, x, y);
			}
			
		}
};

const int RLMap::multipliers[4][8] = {
	{ 1, 0, 0, -1, -1, 0, 0, 1 },
	{ 0, 1, -1, 0, 0, -1, 1, 0 },
	{ 0, 1, 1, 0, 0, -1, -1, 0 },
	{ 1, 0, 0, 1, -1, 0, 0, -1 }
};
