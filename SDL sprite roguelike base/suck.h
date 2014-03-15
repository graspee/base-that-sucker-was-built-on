#pragma once

//happy pangolin's manual for instruction in the use of a class 1 vacuum weapon


//returns false if nothing at all to suck otherwise true
//

//vac




struct suckpath{
	int destx=-1,desty=-1;
	//int lavax=-1,lavay=-1;// (first lava obj you're sucking will hit)
	int pathlength;

	suckpath(){
		destx = -1, desty = -1;
		//lavax = -1, lavay = -1;
		pathlength = 0;
	}
};



//return true for a real operation (so you get no mana and it uses a turn)
bool suck(int operatorx, int operatory, int range){

	char numpathfound=0, lastfound=0;

	suckpath path[8];

	//we check outwards range squares (1 normal, 10 super)

	for(char g=0;g<8;g++){
		int curx = operatorx, cury = operatory;
		for(char f=0;f<range;f++){
			curx+=lil::deltax[g], cury+=lil::deltay[g];
			
			if (curx<0 || cury<0 || curx>=map->width || cury>=map->height){//if off map
				path[g].destx = -1, path[g].desty = -1;//fail
				goto DongSlicingMadeEasyByGarthStubbs;
			}
			
			if(map->itemgrid.at(curx,cury)!=nullptr){//if the square curx, cury has obj on it
				path[g].destx=curx, path[g].desty=cury; //success
				//std::cout << "hit" << (int) g;
				numpathfound++, lastfound=g;
				path[g].pathlength=f;
				goto DongSlicingMadeEasyByGarthStubbs;
			}

			if (!map->passable.get(curx,cury)){  //(map->displaychar.at(curx, cury) == '#'){//if hit a wall or a light...
				path[g].destx = -1, path[g].desty = -1; //fail
				goto DongSlicingMadeEasyByGarthStubbs;
			}



			//if (map->displaychar.at(curx, cury) == 'L'){//if curx,cury is lava
			//	path[g].lavax=curx, path[g].lavay=cury;
			//	goto DongSlicingMadeEasyByGarthStubbs;
			//}
		}
	DongSlicingMadeEasyByGarthStubbs:;
	}
	//for (int f = 0; f < 8; f++){
	//		std::cout << lil::dirnames[f]  << " " << ((path[f].destx != -1) ? " is hit" : "miss") << std::endl;
	//	}

	//std::cout << std::endl;

	if(numpathfound==0)return false;
	
	
	//now we have at least one path
	//if only one path use it right away, else maek user pickerize
	
	if(numpathfound>1){	
		//draw the direction selector
		drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target");
	
		
		//(player.posx - originx) * 16, (player.posy - originy) * 16

		if (path[0].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target n");
		if (path[1].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target s");
		if (path[2].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target e");
		if (path[3].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target w");
		if (path[4].destx == -1) 
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target nw");
		if (path[5].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target ne");
		if (path[6].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target sw");
		if (path[7].destx == -1)
			drawsprite((player.posx - originx) * 16, (player.posy - originy) * 16, "target se");
		messagelog("Select direction to suck in.");
		SHOW();
		int command= Scancode_to_command[GetKey()];
		if(command<0 || command>7 || path[command].destx==-1){
			renderscreen();//a bit overkill but meh
			return false;
		}
		lastfound = command;
	}
	//fuck me we are now actually ready to get sucking
	//sucking in direction = lastfound
	//target square = path[lastfound].destx, path[lastfound].desty
	//if there's lava for it to go in then it's at path lastfound. lavax, lavay otherwise it's -1

	//deal with the fact that the object could be a non fit in vac obje
	//in which case either it sucks towards you and ends up one sq away,
	//or if it's only one sq away anyway, you use up a turn but nothing happens
	//deal with the fact that mob could die in transit

	int sx = path[lastfound].destx, sy = path[lastfound].desty;

	item_instance* i = map->itemgrid.at(sx, sy);

		//std::cout << "sucking to the " << lil::dirnames[lastfound] << std::endl;
	
		//std::cout << "item is: " << i->type->name << std::endl;

		//std::cout << "distance is: " << (int) path[lastfound].pathlength << std::endl;

	//TODO don't let it suck stuff you can't see

	//the terrible cost to suckerizing: steam usage! oh noes mama rabbit, ma steam is going down!
	if (range == 1)
		player.mana -= 1;
	else
		player.mana = 0;

	playsound("suck");


	//if object is too big to fight in vac and it's right next to you anyway then nothing happens
	//but you did try so it should make a noise and use mana
	
	if (i->type->fitsinvacuum == false && path[lastfound].pathlength == 0){
		messagelog("The "+i->type->name+" is too big to fit in.");
		//std::cout << "no fit";
		return true;
	}
	
	

	//animation stage 1- item from its position to next to you
	if (path[lastfound].pathlength > 0){
		messagelog("You suck the " + i->type->name + " towards you.");

		//this is where you need to account for lava in the middle of the path
		//we removed lava check on path gen so check lava each square

		//int sx = path[lastfound].destx, sy = path[lastfound].desty;
		for (int f = 0; f < path[lastfound].pathlength;f++){
			//move it, render screen, wait
			
			

			//map->itemgrid.at(sx,sy) = nullptr;
			sx -= lil::deltax[lastfound];
			sy -= lil::deltay[lastfound];
			//map->itemgrid.at(sx, sy) = i;

			map->itemmoveto(i, sx, sy);
			

			renderscreen();
			pangodelay(50);
			
			}
	}

	//ok at this point we are sucking. the object is right next to us
	//if obj fits in vacuums (i.e. not golem or cueb) and there's room- it's going in, baby!

	//is vacuum chamber full?
	if (player.vacuum_chamber.size() == 10){
		messagelog("Vacuum chamber is full.");
		return true;
	}
		
	if (!i->type->fitsinvacuum){
		return true;
	}
		
	//ok now we have room and it fits. INSERTION TIME
	//ADDSOUND SUCKING INTO CHAMBER
	messagelog("The " + i->type->name + " enters the vacuum chamber.");

	//animate it
	map->itemremovefrom(i);
	
	
	char dest = player.vacuum_chamber.size() + 9;
	for (int f = 21; f > dest; f--){
		renderscreen();
		if (f != 20)drawsprite(21 * 16, f * 16, i->type->sprite);
		else drawsprite(21 * 16, 21 * 16, "blank");
		SHOW();
		pangodelay(50);
	}

	//add to the chamber
	player.vacuum_chamber.push_back(i);
	i->invacuum = true;




	

	
}