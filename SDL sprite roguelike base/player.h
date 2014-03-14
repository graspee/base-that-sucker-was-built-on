#pragma once

struct Player {
	int posx, posy;
	bool lantern;
	char hp, mana;
	bool selectsuck = true;
	item_instance* held = nullptr;
	int score;
	vector<item_instance*> vacuum_chamber;
	bool stealthed = false;
	int level = 0;

	Player(){
		init();
	}

	void init(){
		posx = 0, posy = 0, lantern = true, hp = 10, mana = 0;
	}

	void damage(int amount, bool willshieldhelp, string reason){
		if (held != nullptr 
			&& held->type->type == Eitemtype::ITEM_SHIELD){
			//ADDSOUND HIT ON SHIELD
			//ADDSTATUS ABOUT SHIELD HELPING YOU
			hp -= amount / 2;
		}
		else
			hp -= amount;

		if (hp <= 0){
			//game over
			//addsound game over
			//addstatus you were killed by <reason>
			std::cout << "You were killed by " << reason << ".";
			gameover = true;
			
		}
	}

};