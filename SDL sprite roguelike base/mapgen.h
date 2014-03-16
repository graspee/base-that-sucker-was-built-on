#pragma once

#define mapwidth 80
#define mapheight 50
#define patchwidth 10
#define patchheight 10
#define numberofpatches 75


#define EMPTY ' '

#include <vector>
#include "lilhelpers.h"

//tweak so large rooms rarer. most rooms medium


struct Patch {
	Array2D<uint8> cells;		//the actual meat of the patch (map part representing a room)
	int floodcount = 0;			//used in floodfill test to see if shape connected
	float deltax, deltay;		//vector as we move the patch outwards from centre
	float  posx, posy;				//position on the map
	bool out_of_bounds = false;		
	bool stamped_down = false;
	Patch(int w, int h){
		cells.Init('#', w, h);
	}
	void draw_onconsole(){
		for (size_t y = 0; y < cells.height; y++)
		{
			for (size_t x = 0; x < cells.width; x++)
			{
				std::cout << cells.at(x, y);
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}
	inline void drawchar(int x, int y){
		if (x < 0 || y < 0 || x >= cells.width || y >= cells.height){
			std::cout << "drawchar out of bounds!";
		}
		cells.at(x, y) = EMPTY;
	}
	void fillrect(int x0, int y0, int w, int h){
		for (size_t y = y0; y < y0+h; y++)
		{
			for (size_t x = x0; x < x0+w; x++)
			{
				drawchar(x, y);
			}
		}
	}
	void bresline(int x0, int y0, int x1, int y1){

		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		int err = dx - dy;

		while (1){
			drawchar(x0, y0);
			if (x0 == x1 && y0 == y1)break;
			int e2 = 2 * err;
			if (e2 > -dy){
				err -= dy;
				x0 += sx;
			}
			if (e2 < dx){
				err += dx;
				y0 += sy;
			}
		}
	}
	void fillcircle(int x0, int y0, int radius)
	{
		int f = 1 - radius;
		int ddF_x = 1;
		int ddF_y = -2 * radius;
		int x = 0;
		int y = radius;

		drawchar(x0, y0 + radius);
		drawchar(x0, y0 - radius);
		bresline(x0 + radius, y0, x0 - radius, y0);

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			bresline(x0 + x, y0 + y, x0 - x, y0 + y);
			bresline(x0 + x, y0 - y, x0 - x, y0 - y);
			bresline(x0 + y, y0 + x, x0 - y, y0 + x);
			bresline(x0 + y, y0 - x, x0 - y, y0 - x);
		}
	}
	void mirror_lr(){
		for (size_t y = 0; y < cells.height; y++)
		{
			for (size_t x = 0; x < cells.width / 2; x++)
			{
				cells.at(cells.width - 1 - x, y) = cells.at(x, y);
			}
		}
	}
	void mirror_rl(){
		for (size_t y = 0; y < cells.height; y++)
		{
			for (size_t x = 0; x < cells.width / 2; x++)
			{
				cells.at(x, y) = cells.at(cells.width - 1 - x, y);
			}
		}
	}
	void mirror_du(){
		for (size_t x = 0; x < cells.width; x++)
		{
			for (size_t y = 0; y < cells.height / 2; y++)
			{
				cells.at(x, y) = cells.at(x, cells.height - 1 - y);
			}
		}
	}
	void mirror_ud(){
		for (size_t x = 0; x < cells.width; x++)
		{
			for (size_t y = 0; y < cells.height / 2; y++)
			{
				cells.at(x, cells.height - 1 - y) = cells.at(x, y);
			}
		}
	}
	bool CellCountTest(){
		//returns true if there are 4 cells or more
		int cellcount = 0;
		for (size_t y = 0; y < cells.height; y++)
		{
			for (size_t x = 0; x < cells.width; x++)
			{
				if (cells.at(x, y) == EMPTY){
					cellcount++;
					if (cellcount>3)return true;
				}
			}
		}
		return false;
	}
	void patchfill(){
		//each patch has 1-3 shapes within it
		uint8 num_shapes_within_patch = lil::rand(1, 6);
		if (num_shapes_within_patch > 3)num_shapes_within_patch = 1;

		for (; num_shapes_within_patch > 0; num_shapes_within_patch--){

			//each shape is either a rect or a circle at the moment
			uint8 shape = lil::rand(1, 6); //was 15
			switch (shape){
			case 1:
				//circle
			{
				uint8 r = lil::rand(2, ((patchwidth - 2) / 2));
				fillcircle(
					lil::rand(r, patchwidth - r - 1),
					lil::rand(r, patchwidth - r - 1),
					r);
			}
				break;
				//case 2:
				//quad
				//	break;
				//case 3:
				//triangle
				//	break;
				//case 4:
				//ngon 3-8
				//	break;
			default:
				//rect
			{
				uint8 w = lil::rand(2, patchwidth);
				uint8 h = lil::rand(2, patchwidth);
				fillrect(
					lil::rand(0, patchwidth - w),
					lil::rand(0, patchheight - h),
					w, h);
			}
				break;
			}
		}
		//sometimes, take half the patch and mirror it
		if (lil::rand(1, 6) == 1){
			uint x = lil::rand(1, 6);
			switch (x){
			case 1://mirror left side onto right
				mirror_lr();
				break;
			case 2://mirror right side onto left
				mirror_rl();
				break;
			case 3://mirror top half to bottom
				mirror_ud();
				break;
			case 4://mirror bottom half to top
				mirror_du();
				break;
			case 5:case 6://mirror both ways. there's 4 ways to do that
			{
				uint8 which = lil::rand(1, 4);
				switch (which){
				case 1:
					mirror_lr(); mirror_ud();
					break;
				case 2:
					mirror_rl(); mirror_ud();
					break;
				case 3:
					mirror_lr(); mirror_du();
					break;
				case 4:
					mirror_rl(); mirror_du();
					break;
				}
			}
				break;
			}
		}
	}
	
};







inline void wtfprint(const std::string s){
	std::cout << s << std::endl;
}


//looks in neighbours of x,y for the sought character.
//bool RLMap::CheckAllSidesAndPickOne(int x,int y,char seek,int& xout, int&yout){
//	vector <pair<int,int>> pool;
//	if (x > 0 && displaychar.at(x - 1, y) == seek)pool.push_back(make_pair(x - 1, y));
//	if (y > 0 && displaychar.at(x, y - 1) == seek)pool.push_back(make_pair(x , y-1));
//	if (x < width - 1 && displaychar.at(x + 1, y) == seek)pool.push_back(make_pair(x +1, y));
//	if (y < height - 1 && displaychar.at(x, y + 1) == seek)pool.push_back(make_pair(x , y+1));
//	if (pool.size() == 0)return false;
//	pair<int, int> which = lil::randmember(pool);
//	xout=which.first, yout=which.second;
//	return true;
//}

int RLMap::shootray(int x, int y, int deltax, int deltay, char seek){
	int currentx = x, currenty = y;
	while (1){
		currentx += deltax, currenty += deltay;
		if (currentx < 0 || currenty < 0 || currentx >= width || currenty >= height)return -1;
		if (displaychar.at(currentx, currenty) == seek){
			return( (deltax != 0) ? abs(x - currentx) : abs(y - currenty));
		}
	}
}
bool RLMap::shootrays(int x, int y, char seek, int& outx, int& outy, char &direction,bool fudge){
	vector<char> v;
	int lowest = 999;
	for (char f = 0; f < 4; f++){
		int temp = shootray(x, y, lil::deltax[f], lil::deltay[f], seek);
		if (temp != -1){
			if (temp < lowest){
				lowest = temp;
				v.clear();
			}
			if (temp <= lowest){
				v.push_back(f);
			}
		}
	}
	if (lowest == 999)return false;
	direction = lil::randmember(v);
	if (fudge)lowest--;
	
	outx = x+(lowest*lil::deltax[direction]);
	outy = y+(lowest*lil::deltay[direction]);
	return true;

}


void RLMap::genlevel_rooms(){
	
	std::vector<Patch*> patches;
	Patch* patch;

	//create a set of patches. a patch is a small area representing one room. it's made of 1-3 shapes: rects or circles
	//the shapes are randomly positioned in the patch to make interesting room shapes. They are guaranteed to have at 
	//least 4 cells of "room" and to be connected.
	for (int f = 0; f < numberofpatches; f++){
		patch = new Patch(patchwidth, patchheight);							//make a new patch and
		patches.push_back(patch);											//add it to the list of patches
		patch->posx = (width - patchwidth) / 2;							//position patch in the middle of the map
		patch->posy = (height - patchheight) / 2;
		float degrees = (float)f * (360.0 / (float) numberofpatches);		//give patch a trajectory so they move away from the middle
		float radians = degrees*0.0174532925;
		patch->deltax = cos(radians);
		patch->deltay = sin(radians);										
	tryagain:
		patch->patchfill();													//put room shapes into the patch
		if (!patch->CellCountTest())goto tryagain;							//must have >=4 cells 
		//if (!patch->FloodConnectTest())goto tryagain;						//all cells must be connected
		if (!patch->cells.floodtest(EMPTY, 1))goto tryagain;
	}
		
	//now we actually have our set of patches, move them outwards and 
	//stamp them down when they don't overlap
	//RLMap patchmap(width, height);	
	int num_left = numberofpatches;

	while (num_left > 0){
	//	int upto = -1; print(" ");
		for (auto& p : patches){
		//	upto++;
		//	std::cout << upto << ((p->stamped_down) ? "SD" : "--") << "  " << ((p->out_of_bounds) ? "OOB" : "--" )
			//	<< p->posx << " " << p->posy  << std::endl;
			if (!p->stamped_down && !p->out_of_bounds){//if this patch is still moving
				for (int celly = 0; celly < patchheight; celly ++){
					for (int cellx = 0; cellx < patchwidth; cellx ++){//go through each cell
						if(p->cells.at(cellx,celly) == EMPTY){//if the cell is a "room" cell
							int mapx = p->posx + cellx;
							int mapy = p->posy + celly;//get map pos under this cell
							if (mapx < 0 || mapy < 0 || mapx >= width || mapy >= height){//if map cell under this cell=out of bounds
								p->out_of_bounds = true;//set this patch out of bounds to true
								//print("out of bounds");
								num_left--;//one fewer to process
								goto NEXT_PATCH;//break to next patch
							}//if mapx or mapy out of bounds
							if (displaychar.at(mapx, mapy) != 0){//if the map underneath this cell is already occupied
								goto MOVE_PHASE;//can't stamp down this turn. break to move phase
							}
						}//if cell is room cell
					}//next cellx
				}//next celly
				//having got through every cell of the patch without breaking
				for (int celly = 0; celly < patchheight; celly++){//stampdown! update map with patch
					for (int cellx = 0; cellx < patchwidth; cellx++){
						if (p->cells.at(cellx, celly) == EMPTY){
							displaychar.at(p->posx + cellx, p->posy + celly) 
								= p->cells.at(cellx, celly);
						}
					}
				}
				//print("stampdown");
				p->stamped_down = true;//set stamped down to true
				num_left--;//decrease num_left
				MOVE_PHASE://move phase
				p->posx += p->deltax;//move the patch along its trajectory
				p->posy += p->deltay;

			}//if not p stamped down and p out of bounds
		NEXT_PATCH:;
		}//auto p in patches
	}
	

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			if (displaychar.at(x, y) == 0){
				displaychar.at(x, y) = '#';
				passable.set(x, y, false);
				blocks_sight.set(x, y, true);
			}
			else {//change this when you have stuff other than wall and floor
				passable.set(x, y, true);
				blocks_sight.set(x, y, false);
				emptyspaces.push_back(make_pair(x, y));
			}
		}
	}
	lil::shuffle(emptyspaces);


	//cleanup
	for (auto p : patches)delete p;

	//add static lights
	for (int ly = 0; ly < mapheight; ly += 10){
		for (int lx = 0; lx < mapwidth; lx += 10){
			int tentx = lx + lil::rand(0, 9);
			int tenty = ly + lil::rand(0, 9);
			if (tentx >= mapwidth || tenty >= mapheight)print("ERROR OUT OF BOUNDS");
			//move light to wall
			int outx, outy;
			char direction;
			bool suc;
			if (displaychar.at(tentx, tenty) == '#'){
				if (shootrays(tentx, tenty, ' ', outx, outy, direction, true)){
					displaychar.at(outx, outy) = lil::dirchar[direction];
					do_fov_foralight(outx, outy, 9, { 255, 255, 128 },direction);
				}
				
			}
			else {
				if (shootrays(tentx, tenty, '#', outx, outy, direction)){
					displaychar.at(outx, outy) = lil::dirchar_rev[direction];
					do_fov_foralight(outx, outy, 9, { 255, 255, 128 },lil::opdir[direction]);
				}
			}
			
		}
	}

	//debug test maziacs
	//if (PathfindAStar(player.posx, player.posy, x, y)){
	//	for (auto& f : lastpath){
	//		displaychar.at(f.x, f.y) = 'L';
	//	}
	//}
	//


	//PLACE STUFF ITEMS PLAYER MOBS EXIT ETC

	//current cheats 20 extra swords, 20 extra imps
	//additem(20, Eitemtype::ITEM_SWORD);
	//addmob(20, Eitemtype::MOB_ESR);
	//addmob(20, Eitemtype::MOB_SKEL);
	//additem(20, Eitemtype::ITEM_BATTERY);
	//additem(20, Eitemtype::ITEM_MEDPACK);

	player.hp = 10;


	//place player
	freespace(player.posx, player.posy);
	player.level++;
	player.turns = 0;

	//place exit
	int x, y;
	freespace(x, y);
	displaychar.at(x, y) = 'E';
	locked.set(x, y, true);

	//place rabbit gate
	freespace(x, y);
	displaychar.at(x, y) = 'G';
	rabbitgatex = x, rabbitgatey = y;

	//place broom cupboard containerizing broom
	freespace(x, y);
	displaychar.at(x, y) = 'C';
	locked.set(x, y, true);


	//place chest containerizing golden tziken on level 10
	if (player.level == 10){
		freespace(x, y);
		displaychar.at(x, y) = 'c';
		locked.set(x, y, true);
	}
	

	//add 20 mobs
	//TODO distance from player check
	for (int f = 0; f < 20; f++){
		addmob(1,(Eitemtype) lil::rand( (int)Eitemtype::MOB_SKEL, (int)Eitemtype::MOB_BAT ));
	}

	//add 20 items
	//5 gems
	additem(5, Eitemtype::ITEM_GEM);
	//10 gold
	additem(10, Eitemtype::ITEM_GOLD);
	//2 keys on level 1-9, 3 on 10
	additem((player.level==10)?3:2, Eitemtype::ITEM_KEY);
	//3 random items from batteries/shield/sword/stopwatch/medpack/junk
	for (size_t i = 0; i < 3; i++)
	{
		
		additem(1, (Eitemtype) lil::rand((int)Eitemtype::ITEM_BATTERY, (int)Eitemtype::ITEM_JUNK));
	}


	//floodtest it
	//bool itpassed = this->displaychar.floodtest(' ', 1);
	//std::cout << (itpassed ? "pass " : "fail ") << std::endl;
	//if (!itpassed){
	//	displaychar.ReplaceXWithY(1, ' ');
	//}
}
		
 	
	




	
	
	






