#pragma once

#define mapwidth 80
#define mapheight 50
#define patchwidth 20
#define patchheight 20
#define numberofpatches 10

#define EMPTY '.'

#include <vector>
#include "lilhelpers.h"

//tweak so large rooms rarer. most rooms medium


struct Patch {
	Array2D<uint8> cells;		//the actual meat of the patch (map part representing a room)
	int floodcount = 0;			//used in floodfill test to see if shape connected
	float deltax, deltay;		//vector as we move the patch outwards from centre
	int posx, posy;				//position on the map
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
	bool FloodFill(int x, int y){
		if (x<0 || y<0 || x>=cells.width || y>=cells.height)return false;
		if (cells.at(x,y) != 'F')return false;
		cells.at(x, y) = EMPTY;
		floodcount++;
		FloodFill(x - 1, y);
		FloodFill(x + 1, y);
		FloodFill(x, y - 1);
		FloodFill(x, y + 1);
	}
	bool FloodConnectTest(){
		int cellcount = 0;
		bool virgin = true;
		uint xt = 0, yt = 0;
		for (size_t y = 0; y < cells.height; y++)
		{
			for (size_t x = 0; x < cells.width; x++)
			{
				if (cells.at(x, y) == EMPTY){
					cellcount++;	
					cells.at(x, y) = 'F';
					if (virgin){
						virgin = false;
						xt = x;
						yt = y;
					}
				}
			}
		}
		if (cellcount == 0)return false;
		floodcount = 0;
		FloodFill(xt, yt);
		return (floodcount < cellcount) ? false : true;
	}
};
















void randomlevel(){
	
	lil::randseed();
	
	std::vector<Patch*> patches;
	Patch* patch;

	//create a set of patches. a patch is a small area representing one room. it's made of 1-3 shapes: rects or circles
	//the shapes are randomly positioned in the patch to make interesting room shapes. They are guaranteed to have at 
	//least 4 cells of "room" and to be connected.
	for (int f = 0; f < numberofpatches; f++){
		patch = new Patch(patchwidth, patchheight);							//make a new patch and
		patches.push_back(patch);											//add it to the list of patches
		patch->posx = (mapwidth - patchwidth) / 2;							//position patch in the middle of the map
		patch->posy = (mapheight - patchheight) / 2;
		float degrees = (float)f * (360.0 / (float) numberofpatches);		//give patch a trajectory so they move away from the middle
		float radians = degrees*0.0174532925;
		patch->deltax = cos(radians);
		patch->deltay = sin(radians);										
	tryagain:
		patch->patchfill();													//put room shapes into the patch
		if (!patch->CellCountTest())goto tryagain;							//must have >=4 cells 
		if (!patch->FloodConnectTest())goto tryagain;						//all cells must be connected
	}
		
	//now we actually have our set of patches, move them outwards and 
	//stamp them down when they don't overlap
	RLMap patchmap(mapwidth, mapheight);	
	int num_left = numberofpatches;

	while (num_left > 0){
		for (auto& p : patches){

			
			if (!p->stamped_down && !p->out_of_bounds){//if this patch is still moving
				for (int celly = 0; celly < patchheight; celly ++){
					for (int cellx = 0; cellx < patchwidth; cellx ++){//go through each cell
						if(p->cells.at(cellx,celly) == EMPTY){//if the cell is a "room" cell
							int mapx = p->posx + cellx;
							int mapy = p->posy + celly;//get map pos under this cell
							if (mapx < 0 || mapy < 0 || mapx >= mapwidth || mapy >= mapheight){//if map cell under this cell=out of bounds
								p->out_of_bounds = true;//set this patch out of bounds to true
								num_left--;//one fewer to process
								goto NEXT_PATCH;//break to next patch
							}//if mapx or mapy out of bounds
							if (patchmap.displaychar.at(mapx, mapy) != '`'){//if the map underneath this cell is already occupied
								goto MOVE_PHASE;//can't stamp down this turn. break to move phase
							}
						}//if cell is room cell
					}//next cellx
				}//next celly
				//having got through every cell of the patch without breaking
				for (int celly = 0; celly < patchheight; celly++){//stampdown! update map with patch
					for (int cellx = 0; cellx < patchwidth; cellx++){
						if (p->cells.at(cellx, celly) == EMPTY){
							patchmap.displaychar.at(p->posx + cellx, p->posy + celly) 
								= p->cells.at(cellx, celly);
						}
					}
				}
				p->stamped_down = true;//set stamped down to true
				num_left--;//decrease num_left
				MOVE_PHASE://move phase
				p->posx += p->deltax;//move the patch along its trajectory
				p->posy += p->deltay;

			}//if not p stamped down and p out of bounds
		NEXT_PATCH:;
		}//auto p in patches
	}
	
	//draw map on terminal
	for (size_t y = 0; y < patchmap.height; y++)
	{
		for (size_t x = 0; x < patchmap.width; x++)
		{
			std::cout << patchmap.displaychar.at(x, y);
		}
		std::cout << std::endl;
	}

	int xxx = 0;


}
		
 	
	




	
	
	






