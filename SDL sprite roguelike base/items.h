#pragma once

enum class Eitemtype  {
	MOB_SKEL, MOB_KOBBY, MOB_GOLEM, MOB_CUEB, MOB_IMP, MOB_BAT,
	MOB_FIRESKEL, MOB_CUEBOBJ, MOB_ESR, MOB_ESRSHIELD, MOB_BROOM, MOB_BROOMSWORD, MOB_TZIKEN,
	ITEM_GEM, ITEM_GOLD, ITEM_BATTERY, ITEM_SHIELD, ITEM_STOPWATCH, ITEM_SWORD, ITEM_MEDPACK, 
	ITEM_KEY, ITEM_JUNK};

//There is one instance of mob_archetype per monster
struct item_archetype {
	string name;
	Eitemtype type;
	SDL_Texture* sprite;
	bool ismob;

	
	//mob only stuff
	bool rangedattacker;
	int optimumrangeddistance;
	int maxhp;

	//item only stuff
	//um.. none?

	item_archetype(string _name, Eitemtype _type, SDL_Texture* _sprite, bool _ismob=false,
		bool _rangedattacker=false, int _optimumrangeddistance=0, int _maxhp=0):
		name(_name), type(_type), sprite(_sprite), ismob(_ismob), rangedattacker(_rangedattacker),
		optimumrangeddistance(_optimumrangeddistance), maxhp(_maxhp){
	}
};

 enum class EBehaviour { BEHAVIOUR_STATIC, BEHAVIOUR_WANDERING, BEHAVIOUR_CHASING };

vector<item_archetype> item_archetype_list;

struct item_instance {
	item_archetype* type;
	EBehaviour behaviour=EBehaviour::BEHAVIOUR_WANDERING;
	int posx, posy;
	int hp;

	item_instance(int _type, int _x, int _y, int _hp){
		posx = _x, posy = _y;
		type = &item_archetype_list[_type];
		hp = _hp;
	}

};



void setupitems(){
	item_archetype_list.push_back(//0
	{ "skelington",Eitemtype::MOB_SKEL, dicosprite.at("skel"), true, false, 0, 1});
	
	item_archetype_list.push_back(//1
	{ "kobby bomber", Eitemtype::MOB_KOBBY, dicosprite.at("kobby bomber"), true, true, 5, 1 });

	item_archetype_list.push_back(//2
	{ "caek golem", Eitemtype::MOB_GOLEM, dicosprite.at("cake golem"), true, false, 0, 1 });

	item_archetype_list.push_back(//3
	{ "gelatinous cueb", Eitemtype::MOB_CUEB, dicosprite.at("gelatinous cueb"), true, false, 0, 1 });

	item_archetype_list.push_back(//4
	{ "fier imp", Eitemtype::MOB_IMP, dicosprite.at("fire imp"), true, false, 0, 1 });
	
	item_archetype_list.push_back(//5
	{ "vampier bat", Eitemtype::MOB_BAT, dicosprite.at("bat"), true, false, 0, 1 });

	item_archetype_list.push_back(//6
	{ "fier skelington", Eitemtype::MOB_FIRESKEL, dicosprite.at("fire skel"), true, false, 0, 1 });

	item_archetype_list.push_back(//7
	{ "fat cueb", Eitemtype::MOB_CUEBOBJ, dicosprite.at("cueb object"), true, false, 0, 1 });

	item_archetype_list.push_back(//8
	{ "Extra Space Rabbit", Eitemtype::MOB_ESR, dicosprite.at("esr"), true, true, 3, 1 });

	item_archetype_list.push_back(//9
	{ "Shielded Rabbit", Eitemtype::MOB_ESRSHIELD, dicosprite.at("esr shield"), true, true, 3, 1 });

	item_archetype_list.push_back(//10
	{ "Princess Broom", Eitemtype::MOB_BROOM, dicosprite.at("broom"), true, false, 0, 3 });

	//ITEMS

	item_archetype_list.push_back(//11
	{ "shineh gemz", Eitemtype::ITEM_GEM, dicosprite.at("gems"), false, false, 0, 0 });

	item_archetype_list.push_back(//12
	{ "gold barz", Eitemtype::ITEM_GOLD, dicosprite.at("gold"), false, false, 0, 0 });
	
	item_archetype_list.push_back(//13
	{ "key", Eitemtype::ITEM_KEY, dicosprite.at("key"), false, false, 0, 0 });
	
	item_archetype_list.push_back(//14
	{ "batterehs", Eitemtype::ITEM_BATTERY, dicosprite.at("batteries"), false, false, 0, 0 });

	item_archetype_list.push_back(//15
	{ "Aegis", Eitemtype::ITEM_SHIELD, dicosprite.at("shield"), false, false, 0, 0 });

	item_archetype_list.push_back(//16
	{ "stasis device", Eitemtype::ITEM_STOPWATCH, dicosprite.at("stopwatch"), false, false, 0, 0 });

	item_archetype_list.push_back(//17
	{ "Curtana", Eitemtype::ITEM_SWORD, dicosprite.at("sword"), false, false, 0, 0 });

	item_archetype_list.push_back(//18
	{ "medpack", Eitemtype::ITEM_MEDPACK, dicosprite.at("medpack"), false, false, 0, 0 });

	item_archetype_list.push_back(//19
	{ "junk", Eitemtype::ITEM_JUNK, dicosprite.at("junk"), false, false, 0, 0 });

	//mobs

	item_archetype_list.push_back(//20
	{ "Golden Tziken", Eitemtype::MOB_TZIKEN, dicosprite.at("golden tziken"), true, false, 0, 3 });

	item_archetype_list.push_back(//21
	{ "Battle broom", Eitemtype::MOB_BROOMSWORD, dicosprite.at("broom sword"), true, false, 0, 3 });

}