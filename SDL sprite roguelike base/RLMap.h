#pragma once

#include <functional>



struct ColouredLight {
	signed int r, g, b,total;
};

//RLMAP REPRESENTS ONE RL GAME MAP OR LEVEL
class RLMap {
private:

	
	
	static const int multipliers[4][8];

	struct todoitem {
		int x, y;
	};
	int maxint; //FIXME NEEDS TO BE STATIC

public:

	

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

	vector<pair<int, int>> emptyspaces;			//free squares

	//we don't want array2d itemgrid because that makes a mob for
	//each square. we want an array2d of mob*
	Array2D<item_instance*> itemgrid;
	vector<item_instance*> moblist;
	vector<item_instance*> itemlist;

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

		
		
	}

	//CLEAR THE MAP
	void Clear(void){
		displaychar.Fill('^');
	}

	~RLMap(){
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

#ifdef DIJKSTRA_DIRECTIONS_CARDINAL_ONLY
		static const int dircount = 8;
		static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0 };
#else	
		static const int dircount = 16;
		static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0, 1, -1, 1, 1, -1, 1, -1, -1 };
#endif

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

#undef ASTAR_DIRECTIONS_CARDINAL_ONLY


	bool PathfindAStar(int startx, int starty, int goalx, int goaly){
		std::chrono::high_resolution_clock::time_point timestart = std::chrono::high_resolution_clock::now();

#ifdef ASTAR_DIRECTIONS_CARDINAL_ONLY
		static const int dircount = 8;
		static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0 };
#else	
		static const int dircount = 16;
		static const int dxdy[dircount] = { 0, -1, 1, 0, 0, 1, -1, 0, 1, -1, 1, 1, -1, 1, -1, -1 };
#endif

		BitArray visited(false, width, height);

		distance.Fill(maxint);


		int debugnodecount = 0;

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
			displaychar.at(t.x, t.y) = '.';
			debugnodecount++;

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
		float c1 = 1.0 / (1.0 + Distance_Squared(x1, y1, x2, y2) / 10);
		float c2 = c1 - 1.0 / (1.0 + r*r);
		float c3 = c2 / (1.0 - 1.0 / (1.0 + r*r));

		store.r += colour.r * c3;
		store.g += colour.g * c3;
		store.b += colour.b * c3;

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

			for (int f = 0; f < howmany; f++){
				int x, y;
				freespace(x, y);
				item_instance* m = new item_instance(it, x, y);
				itemlist.push_back(m);
				itemgrid.at(x, y) = itemlist.back();
			}

		}

};

const int RLMap::multipliers[4][8] = {
	{ 1, 0, 0, -1, -1, 0, 0, 1 },
	{ 0, 1, -1, 0, 0, -1, 1, 0 },
	{ 0, 1, 1, 0, 0, -1, -1, 0 },
	{ 1, 0, 0, 1, -1, 0, 0, -1 }
};
