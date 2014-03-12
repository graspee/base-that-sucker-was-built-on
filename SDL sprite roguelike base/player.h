#pragma once

struct Player {
	int posx, posy;
	bool lantern;
	char hp, mana;
	bool selectsuck = true;
	item_instance* held = nullptr;
	int score;
	vector<item_instance*> vacuum_chamber;


	Player(){
		init();
	}

	void init(){
		posx = 0, posy = 0, lantern = false, hp = 10, mana = 0;
	}

};