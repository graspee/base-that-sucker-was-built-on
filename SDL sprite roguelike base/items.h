#pragma once
//#include "lilhelpers.h"



enum class Eitemtype  {
	NOTHING,
	MOB_SKEL, MOB_KOBBY, MOB_GOLEM, MOB_CUEB,
	MOB_IMP, MOB_BAT, MOB_FIRESKEL, MOB_CUEBOBJ,
	MOB_ESR, MOB_ESRSHIELD,
	
	MOB_BROOM, MOB_BROOMSWORD, MOB_TZIKEN,
	
	ITEM_GEM, ITEM_GOLD, 
	ITEM_BATTERY, ITEM_SHIELD, ITEM_STOPWATCH, ITEM_SWORD, ITEM_MEDPACK, ITEM_JUNK, 
	ITEM_LAVA,ITEM_KEY, 
	DEAD_SKEL, DEAD_KOBBY, DEAD_GOLEM, DEAD_CUEB, DEAD_IMP, DEAD_BAT,
	DEAD_RABBIT, DEAD_BROOM, DEAD_TZIKEN,
	ITEM_BOMB5, ITEM_BOMB4, ITEM_BOMB3, ITEM_BOMB2, ITEM_BOMB1};

//There is one instance of mob_archetype per monster
struct item_archetype {
	string name;
	Eitemtype type;
	Eitemtype deadoneofthem;
	SDL_Texture* sprite;
	
	bool ismob;
	bool rangedattacker;
	int optimumrangeddistance;
	int maxhp;
	

	int damagetheydeal = 1;
	bool lavadamagesthem = true;
	bool fitsinvacuum = true;
	bool blownitemdamagesthem = true;
	
	bool aggressive = true;

	item_archetype(){
		std::cout << "wtf";
	}

	item_archetype(
		string _name,
		Eitemtype _type,
		Eitemtype _corpse,
		SDL_Texture* _sprite,
		
		bool _ismob,
		bool _rangedattacker,
		int _optimumrangeddistance,
		int _maxhp,

		int _damdeal,
		bool _damlava,
		bool _fitvac, 
		bool _blown, 
		
		bool _aggressive
		
		)
		:

		name(_name),
		type(_type),
		deadoneofthem(_corpse), 
		sprite(_sprite),
		
		ismob(_ismob), 
		rangedattacker(_rangedattacker),
		optimumrangeddistance(_optimumrangeddistance),
		maxhp(_maxhp),
		
		damagetheydeal(_damdeal),
		lavadamagesthem(_damlava),
		fitsinvacuum(_fitvac),
		blownitemdamagesthem(_blown),
		
		aggressive(_aggressive)
		{
	}

	
};

 enum class EBehaviour { BEHAVIOUR_STATIC, BEHAVIOUR_WANDERING, BEHAVIOUR_CHASING };

//vector<item_archetype> item_archetype_list;

 unordered_map<Eitemtype, item_archetype> dicoarchetype;

struct item_instance {
	item_archetype* type;
	EBehaviour behaviour=EBehaviour::BEHAVIOUR_WANDERING;
	int posx = -1, posy = -1;
	int hp;

	bool invacuum = false;
	bool noticedplayer = false;
	//chase target. (requires posx and posy being actually used)

	char movdircurrent = 0;

	int uses=5;

	bool stunned = false;

	item_instance(Eitemtype _type, int _x, int _y
		){
		posx = _x, posy = _y;
		type = &dicoarchetype[_type];
		hp = type->maxhp;
		uses = 5; //uses should go in archetype really but cba changing all the data for items and only 
		//have sword using it atm.
		if (type->ismob)movdircurrent = lil::rand(0, 7);		
	}

	//removes the object from the map
	//inline void mapremovefrom(){
	//	map->itemgrid.at(posx, posy) = nullptr;
	//	map->passable.set(posx, posy, true);
	//	posx = -1, posy = -1;
	//}

	//
	////puts this item (that wasn't on the map, onto the map)
	//inline void mapput(int x,int y){
	//	posx = x, posy = y;
	//	map->itemgrid.at(x, y) = this;
	//	map->passable.set(x, y, false);
	//	return;
	//}
	////moves an item on the map from its current location to a new one
	//inline void mapmoveto(int x, int y){
	//	mapremovefrom();
	//	mapput(x, y);
	//}

	

};




//name, type, deadtype, sprite, ismob?, israngedattacker?, optdist, maxhp, damdealt, dambylava?, fitvac?, takes ranged dam
void setupitems(){

	//item_archetype f = { "skelington", Eitemtype::MOB_SKEL, Eitemtype::DEAD_SKEL, dicosprite.at("skel"), true, false, 0, 1, 1, false, true, true };
	//item_archetype* ff = &f;
	//dicoarchetype.insert(Eitemtype::DEAD_BAT,)

	dicoarchetype.insert(pair<Eitemtype,item_archetype>( Eitemtype::MOB_SKEL,
	{ "skelington", Eitemtype::MOB_SKEL, Eitemtype::DEAD_SKEL, dicosprite.at("skel"), true, false, 0, 1, 1, false, true, true,true }));
	
	
	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_KOBBY,
	{ "kobby bomber", Eitemtype::MOB_KOBBY, Eitemtype::DEAD_KOBBY, dicosprite.at("kobby bomber"), true, true, 5, 1, 1, true, true, true, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_GOLEM,
	{ "caek golem", Eitemtype::MOB_GOLEM, Eitemtype::DEAD_GOLEM, dicosprite.at("cake golem"), true, false, 0, 1, 1, true, false, true, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_CUEB,
	{ "gelatinous cueb", Eitemtype::MOB_CUEB, Eitemtype::DEAD_CUEB, dicosprite.at("gelatinous cueb"), true, false, 0, 1, 1, true, false, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_IMP,
	{ "fier imp", Eitemtype::MOB_IMP, Eitemtype::DEAD_IMP, dicosprite.at("fire imp"), true, false, 0, 1, 1, false, true, true, true }));
	
	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_BAT,
	{ "vampier bat", Eitemtype::MOB_BAT, Eitemtype::DEAD_BAT, dicosprite.at("bat"), true, false, 0, 1, 1, false, true, true, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_FIRESKEL,
	{ "fier skelington", Eitemtype::MOB_FIRESKEL, Eitemtype::DEAD_SKEL, dicosprite.at("fire skel"), true, false, 0, 1, 2, false, true, true, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_CUEBOBJ,
	{ "fat cueb", Eitemtype::MOB_CUEBOBJ, Eitemtype::DEAD_CUEB, dicosprite.at("cueb object"), true, false, 0, 1, 1, true, false, true , false}));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_ESR,
	{ "Extra Space Rabbit", Eitemtype::MOB_ESR, Eitemtype::DEAD_RABBIT, dicosprite.at("esr"), true, true, 3, 1, 1, true, true, true, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_ESRSHIELD,
	{ "Shielded Rabbit", Eitemtype::MOB_ESRSHIELD, Eitemtype::DEAD_RABBIT, dicosprite.at("esr shield"), true, true, 3, 1, 1, true, true, false, true }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_BROOM,
	{ "Princess Broom", Eitemtype::MOB_BROOM, Eitemtype::DEAD_BROOM, dicosprite.at("broom"), true, false, 0, 3, 0, true, true, true ,false}));
	

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_BROOM,
	{ "Battle broom", Eitemtype::MOB_BROOMSWORD, Eitemtype::DEAD_BROOM, dicosprite.at("broom sword"), true, false, 0, 3, 1, true, true, true,false }));
	
	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::MOB_TZIKEN,
	{ "Golden Tziken", Eitemtype::MOB_TZIKEN, Eitemtype::DEAD_TZIKEN, dicosprite.at("golden tziken"), true, false, 0, 3, 0, true, true, true,false }));

	//ITEMS need to change item and mob numbers maybe just item BATTERY TO JUNK

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_GEM,
	{ "shineh gemz", Eitemtype::ITEM_GEM, Eitemtype::NOTHING, dicosprite.at("gems"), false, false, 0, 0, 0, false, true, true ,false}));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_GOLD,
	{ "gold barz", Eitemtype::ITEM_GOLD, Eitemtype::NOTHING, dicosprite.at("gold"), false, false, 0, 0, 0, false, true, true, false }));
	
	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_KEY,
	{ "key", Eitemtype::ITEM_KEY, Eitemtype::NOTHING, dicosprite.at("key"), false, false, 0, 0, 0, false, true, true, false }));
	
	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BATTERY,
	{ "batterehs", Eitemtype::ITEM_BATTERY, Eitemtype::NOTHING, dicosprite.at("batteries"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_SHIELD,
	{ "Aegis", Eitemtype::ITEM_SHIELD, Eitemtype::NOTHING, dicosprite.at("shield"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_STOPWATCH,
	{ "stasis device", Eitemtype::ITEM_STOPWATCH, Eitemtype::NOTHING, dicosprite.at("stopwatch"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_SWORD,
	{ "Curtana", Eitemtype::ITEM_SWORD, Eitemtype::NOTHING, dicosprite.at("sword"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_MEDPACK,
	{ "medpack", Eitemtype::ITEM_MEDPACK, Eitemtype::NOTHING, dicosprite.at("medpack"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_JUNK,
	{ "junk", Eitemtype::ITEM_JUNK, Eitemtype::NOTHING, dicosprite.at("junk"), false, false, 0, 0, 0, false, true, true, false }));


	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_LAVA,
	{ "lava", Eitemtype::ITEM_LAVA, Eitemtype::NOTHING, dicosprite.at("lava"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BOMB5,
	{ "bomb", Eitemtype::ITEM_BOMB5, Eitemtype::ITEM_BOMB4, dicosprite.at("bomb5"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BOMB4,
	{ "bomb", Eitemtype::ITEM_BOMB4, Eitemtype::ITEM_BOMB3, dicosprite.at("bomb4"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BOMB3,
	{ "bomb", Eitemtype::ITEM_BOMB3, Eitemtype::ITEM_BOMB2, dicosprite.at("bomb3"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BOMB2,
	{ "bomb", Eitemtype::ITEM_BOMB2, Eitemtype::ITEM_BOMB1, dicosprite.at("bomb2"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::ITEM_BOMB1,
	{ "bomb", Eitemtype::ITEM_BOMB1, Eitemtype::NOTHING, dicosprite.at("bomb1"), false, false, 0, 0, 0, false, true, true, false }));



	//mobs

	
	//corpses - items

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_SKEL,
	{ "pile of boenz", Eitemtype::DEAD_SKEL, Eitemtype::NOTHING, dicosprite.at("dead skel"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_KOBBY,
	{ "ex-kobby", Eitemtype::DEAD_KOBBY, Eitemtype::NOTHING, dicosprite.at("kobby dead"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_GOLEM,
	{ "sad crumbs", Eitemtype::DEAD_GOLEM, Eitemtype::NOTHING, dicosprite.at("dead cake"), false, false, 0, 0, 0, false, false, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_CUEB,
	{ "sticky mess", Eitemtype::DEAD_CUEB, Eitemtype::NOTHING, dicosprite.at("dead cueb"), false, false, 0, 0, 0, false, false, true, false }));


	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_IMP,
	{ "smokin' hot corpse", Eitemtype::DEAD_IMP, Eitemtype::NOTHING, dicosprite.at("dead imp"), false, false, 0, 0, 0, false, true, true, false }));


	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_BAT,
	{ "effed-up lil' bat body", Eitemtype::DEAD_BAT, Eitemtype::NOTHING, dicosprite.at("dead bat"), false, false, 0, 0, 0, false, true, true, false }));


	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_RABBIT,
	{ "extra dead rabbit", Eitemtype::DEAD_RABBIT, Eitemtype::NOTHING, dicosprite.at("esr dead"), false, false, 0, 0, 0, false, true, true, false }));


	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_BROOM,
	{ "tomb of broom", Eitemtype::DEAD_BROOM, Eitemtype::NOTHING, dicosprite.at("grave"), false, false, 0, 0, 0, false, true, true, false }));

	dicoarchetype.insert(pair<Eitemtype, item_archetype>(Eitemtype::DEAD_TZIKEN,
	{ "roast tziken", Eitemtype::DEAD_TZIKEN, Eitemtype::NOTHING, dicosprite.at("roast tziken"), false, false, 0, 0, 0, false, true, true, false }));

	

}

