#pragma once

void playsound(string name);
void playsoundloop(string name);

void stopsound();

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

	bool devcheat = false;


	Player(){
		init();
	}

	void init(){
		posx = 0, posy = 0;
		lantern = true, stealthed = false;
		
		hp = 10, mana = 0;

		selectsuck = true, score = 0, level = 0;

		if (held != nullptr){
			delete held;
			held = nullptr;
		}

		if (vacuum_chamber.size() > 0){
			for (auto f : vacuum_chamber)
				delete f;
			vacuum_chamber.clear();
		}


		if(devcheat){
			//doesn't work. don't have time to investigate
			hp = 255;
			held = new item_instance(Eitemtype::ITEM_SWORD, 0, 0);
			vacuum_chamber.push_back(new item_instance(Eitemtype::ITEM_KEY, 0, 0));
			vacuum_chamber.push_back(new item_instance(Eitemtype::ITEM_KEY, 0, 0));
			vacuum_chamber.push_back(new item_instance(Eitemtype::ITEM_KEY, 0, 0));
		}


	}

	void damage(int amount, bool willshieldhelp, string reason){
		if (held != nullptr 
			&& held->type->type == Eitemtype::ITEM_SHIELD){
			playsound("shield");
			messagelog("You block for half damage.");
			hp -= amount;
		}
		else
			hp -= amount*2;

		if (hp <= 0){
			//game over
			//stop playing heart
			stopsound();
			playsound("sadtrombone");

			messagelog("You were killed by " + reason+".",225,0,0);
			gameover = true;
			
		}
		else if (hp <= 2){
			playsoundloop("heart");
		}
	}

};