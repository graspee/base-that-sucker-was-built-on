#pragma once

struct Player {
	int posx, posy;
	bool lantern;
	char hp, mana;
	bool selectsuck = true;
	//item* held;
	//int score

	Player(){
		init();
	}

	void init(){
		posx = 0, posy = 0, lantern = false, hp = 10, mana = 0;
	}

};