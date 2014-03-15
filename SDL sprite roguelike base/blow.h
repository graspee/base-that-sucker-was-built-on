#pragma once

//happy pangolin's manual for instruction in the use of a class 1 vacuum weapon



//chapter 2: blowing
//i just thought maybe empty vac should blow existing things on the map
//away from you, like:

//if vacuum is empty:
//	blow blows one thing one square away
//	super blow blows one thing up to 10 sq away

// but this would make the game too easy i think
//as you could keep a single melee mob away from you forever.
//mind you they only have 1 hp at the moment anyway and you can suck them 

// could be a powerup? a few turns of this behaviour? 
// i'm not really convinced
// welcome to commentblogging




//return true for a real operation (so you get no mana and it uses a turn)
bool blow(int operatorx, int operatory, int range){

	char numpathfound = 0, lastfound = 0;

	suckpath path[8];

	//if vacuum empty, make noise, use mana, turn consumed but nothing happens
	if (player.vacuum_chamber.size() == 0){
		//ADDSOUND BLOW NOISE
		messagelog("Vacuum chamber is empty");
		return true;
	}
	
	//we check outwards range squares (1 normal, 10 super)
	//the difference to sucking is that we don't stop on an empty tile

	for (char g = 0; g<8; g++){
		int curx = operatorx, cury = operatory;
		for (char f = 0; f<range; f++){
			curx += lil::deltax[g], cury += lil::deltay[g];

			if ((curx < 0 || cury < 0 || curx >= map->width || cury >= map->height) || //if off map
				(!map->passable.get(curx, cury)) || //or map not passable, e.g. light or wall
				(map->itemgrid.at(curx, cury) != nullptr)){ //or map cell has object/mob on it
				if (path[g].pathlength == 0){
					path[g].destx = -1, path[g].desty = -1;//fail
				}
				else {
					path[g].destx = curx, path[g].desty = cury; //success
					numpathfound++, lastfound = g;
					//path[g].pathlength = f;
				}
				goto MaTzikenIsWheezingMrBarbara;
			}
			path[g].pathlength++;
		}
		path[g].destx = curx, path[g].desty = cury; //success because nothing came forward to stop me!!!! muhahahahhaah
		numpathfound++, lastfound = g;
		//path[g].pathlength = f;
	MaTzikenIsWheezingMrBarbara:;
	}



	//for (int f = 0; f < 8; f++){
	//		std::cout << lil::dirnames[f]  << " " << ((path[f].destx != -1) ? " is hit" : "miss") << 
	//			path[f].pathlength << std::endl;
	//	}

	//std::cout << std::endl;

	if (numpathfound == 0)return false;


	//now we have at least one path
	//if only one path use it right away, else maek user pickerize

	if (numpathfound>1){
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
		messagelog("Select direction.");
		SHOW();
		int command = Scancode_to_command[GetKey()];
		if (command<0 || command>7 || path[command].destx == -1){
			renderscreen();//a bit overkill but meh
			return false;
		}
		lastfound = command;
	}
	//fuck me we are now actually ready to get blowing
	

	

	//i is the item nearest the end of the vacuum
	item_instance*i = player.vacuum_chamber.back();

	//std::cout << "blowing to the " << lil::dirnames[lastfound] << std::endl;
	//std::cout << "item is: " << i->type->name << std::endl;
	//std::cout << "distance is: " << (int) path[lastfound].pathlength << std::endl;

	//the terrible cost to blowerizing: steam usage! oh noes mama rabbit, ma steam is going down!
	if (range == 1)
		player.mana -= 1;
	else
		player.mana = 0;

	//ADDSOUND BLOWING OBJECT
	messagelog("You blow out the " + i->type->name + ".");


	//animation stage 1- out of vacuum chamber

	lil::removevalue(player.vacuum_chamber, i);
	i->invacuum = false;
	//map->itemgrid.at(sx, sy) = nullptr;

	char src = player.vacuum_chamber.size() + 10;
	for (int f = src; f <21; f++){
		renderscreen();
		if (f != 20)drawsprite(21 * 16, f * 16, i->type->sprite);
		else drawsprite(21 * 16, 21 * 16, "blank");
		SHOW();
		pangodelay(50);
	}

	playsound("firegun");
	

	//animation stage 2- item from next to you to its final resting place
	//if (path[lastfound].pathlength > 0){


		//this is where you need to account for lava in the middle of the path
		int sx = player.posx;
		int sy = player.posy;

		
		
		sx += lil::deltax[lastfound];
		sy += lil::deltay[lastfound];
		map->itemput(i, sx, sy);
		
		renderscreen();
		pangodelay(50);

		for (int f = 0; f < path[lastfound].pathlength-1; f++){
			sx += lil::deltax[lastfound];
			sy += lil::deltay[lastfound];
			map->itemmoveto(i, sx, sy);
			
			renderscreen();
			pangodelay(50);

		}

		/*for (int f = 0; f < path[lastfound].pathlength; f++){
			map->itemgrid.at(sx, sy) = nullptr;
			sx += lil::deltax[lastfound];
			sy += lil::deltay[lastfound];
			map->itemgrid.at(sx, sy) = i;
			renderscreen();
			pangodelay(50);*/



	//}

	//ok object is at destination. let's see if it has enough energy left to
	//damage a mob

	int sx2 =sx + lil::deltax[lastfound];
	int sy2= sy + lil::deltay[lastfound];

	if (sx2 < 0 || sy2 < 0 || sx2 >= map->width || sy2 >= map->height){
		//this should never happen but i want to be sure
		//oh wait no, yeah, this can totally happen

		//std::cout << "major error: blow: kill-check off screen" << std::endl;
		return true;
	}


	//std::cout << "path length " << path[lastfound].pathlength << " range " << range << std::endl;
	if (path[lastfound].pathlength < range){
		item_instance* i2 = map->itemgrid.at(sx2, sy2);
		if (i2 != nullptr && i2->type->ismob && i2->type->blownitemdamagesthem){
			playsound("hit");
			damagemob(i2,sx2,sy2, 1);

		}
	}




	return true;

}