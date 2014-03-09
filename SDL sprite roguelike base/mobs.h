#pragma once


//There is one instance of mob_archetype per monster
struct mob_archetype {
	string name;
	SDL_Texture* sprite;
	bool rangedattacker;
	int optimumrangeddistance;
	int maxhp;
	//more stuff that is game specific

	mob_archetype(string _name, SDL_Texture* _sprite,bool _rangedattacker=false,
		int _opt=0):
		name(_name), sprite(_sprite), rangedattacker(_rangedattacker),
		optimumrangeddistance(_opt)
	{
		
		
	}
};

 enum class EBehaviour { BEHAVIOUR_STATIC, BEHAVIOUR_WANDERING, BEHAVIOUR_CHASING };

vector<mob_archetype> mob_archetype_list;

struct mob_instance {
	mob_archetype* type;
	EBehaviour behaviour=EBehaviour::BEHAVIOUR_WANDERING;
	int posx, posy;
	int hp;

	mob_instance(int _type, int _x, int _y){
		posx = _x, posy = _y;
		type = &mob_archetype_list[_type];
	}

};



void setupmobs(){
	mob_archetype_list.push_back({ "kobold", dicosprite.at("kobby") });
	mob_archetype_list.push_back({ "kobold archer", dicosprite.at("kobby bow") });
}