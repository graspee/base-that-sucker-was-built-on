#pragma once

inline void CLS(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}

inline void SHOW(){
	SDL_RenderPresent(renderer);
}
int GetKey(){
	static SDL_Event e;
	while (1){
		int a = SDL_WaitEvent(&e);
		if (e.type == SDL_KEYDOWN){
			return e.key.keysym.scancode;
		}
	}
}

inline void drawarrow(char pos, bool draw = true){
	static SDL_Rect r = { 130, 0, 12, 8 };
	
	if (draw){
		print(">>", 130, 50 + 20 * pos, 175, 200, 255);
	} else {
		r.y = 50 + 20 * pos;
		SDL_RenderFillRect(renderer,&r);
	}
	SHOW();
}

inline void drawblankline(char pos){
	static SDL_Rect r = { 250, 0, 640 - 251, 8 };
	r.y = 50 + 20 * pos;
	SDL_RenderFillRect(renderer, &r);
	SHOW();
}

void instructions(){
	CLS();
	middleprint("Sucker: Instructions", 0, 200, 140, 200);

	print("You are Pango Pango, an employee of the Happillama Realtech corporation. Your job is to investigate", 0, 16, 200, 140, 200);
	print("seemingly-abandoned space vessels and bring back valuable treasure. In order to do your job you have");
	print("been given a state-of-the-art vacuum device which sucks up and shrinks down matter, living or dead.");

	print("");
	print("The device normally keeps items in stasis to prevent them going off, exploding or fighting each other");
	print("but your device has always been less than 100% functional and so will only achieve stasis if you use");
	print("a stasis device you may find on your travels.");
	print("");

	print("The device has two modes: suck and blow. The device consumes steam to operate. The steam will build up");
	print("over time, one unit at a time, to a maximum of 5 units. Normal sucking or blowing uses one unit of");
	print("steam, but you can also use super mode which is more powerful but needs and consumes all 5 units of steam.");
	print("");
	print("A normal suck will suck in a monster or item on one of the 8 squares around you. If there's more than one");
	print("target then a selector will show you which are valid directions and you can press a direction to confirm.");
	print("In super mode, sucking will be able to suck an item or monster right across the room and into your weapon.");
	print("Some things are too big to fit in the chamber of your vacuum, but you can still suck them towards you.");

	print("");
	print("Blowing will blow the topmost item out of your vacuum. In normal mode this will deposit the item onto one");
	print("of the 8 squares around you. In super mode the item will shoot out across the room and can even damage");
	print("mobs that are in the way. Do note though, that you can't fire an item at a mob that is right next to you");
	print("as there would be no room.");

	print("");
	print("Sucking, blowing and changing the toggle switch from suck to blow or vice versa all use up one turn.");
	print("To pick up an item just bump into it. If you are already carrying an item it will be swapped.");
	print("Some things you will find are locked, e.g. the exit or the broom cupboard (Please rescue broom!)");
	print("To unlock something, simply be carrying a key and bump into it, then bump into it again to open it or");
	print("leave in the case of the exit. You can't drop the item you're carrying, only swap it with another.");
	print("");
	print("There is still an option for a controller to be used but it doesn't work, sorry. You can redefine the keys");
	print("from the menu. F1 to F4 raise and lower the music and sound effect volume while in the game. All settings");
	print("are saved in a file called optionz.dat. If the game ever acts weirdly, a good first step is to delete this");
	print("file so the screen size, keys and sound volumes are reset to the defaults.");
	print("");
	print("7DRL version notes: although I consider this a success there are several things I didn't get a chance");
	print("to implement. Nothing happens in the vacuum chamber. Mobs were supposed to fight each other, pick up and");
	print("use items there were next to and so on. On the map, cubes were supposed to pick up items randomly, bombers");
	print("were supposed to throw bombs that exploded after 5 turns and mobs were supposed to pick up and use various");
	print("items. I will be adding all this and more, I hope, if my enthusiasm for the game continues after 7drl end.");


	SHOW();
	GetKey();
}

void spritetest(){
	CLS();

std::cout << dicosprite.size();
	int x = 0, y = 0;
	for (auto &f : dicosprite){
		
		drawsprite(x*16+16, y*16+16, f.second);
		print(f.first, x*16 +16+ 20, y*16+8+16, 225, 225, 45);
		y++;
		if (y == 20){ y = 0, x += 10; }
	}
	

	SHOW();
	GetKey();
}


void doredefkeys(){
	//make temp copy of key bindings
	vector<int> OPTION_buttons_copy=OPTION_buttons;
	
	CLS();
	print("REDEFINE KEYS:", 0, 0, 225, 225, 40);
	print("<esc> to cancel changes and go back, Enter/Return to accept them and go back.", 0, 16, 225, 225, 40);
	print("Up and Down arrows to move, space to set key.", 0, 8, 225, 225, 0);
	print("Non-bindable: In game: F1-F4 set fx/music vol; ESC quits to title screen.", 0, 24, 75, 211, 41);
	SHOW();
	int howmany = 0;
	for (auto f : OPTION_buttons_copy){
		print(button_names[howmany], 150, 50 + 20 * howmany, 40, 225, 225);
		print(SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(f))), 300, 50 + 20 * howmany, 40, 75, 250);
		howmany++;
	}
	howmany--;
	SHOW();

	int highlighted = 0;
	drawarrow(0);


	while (1){
		int k = GetKey();
		switch (k){
		case SDL_SCANCODE_DOWN:
			drawarrow(highlighted, false);
			highlighted++;
			if (highlighted > howmany)highlighted = 0;
			drawarrow(highlighted);
			continue;
			break;
		case SDL_SCANCODE_UP:
			drawarrow(highlighted, false);
			highlighted--;
			if (highlighted == -1)highlighted = howmany;
			drawarrow(highlighted);
			continue;
			break;
		case SDL_SCANCODE_SPACE:
			{
				drawblankline(highlighted);
				print("  ???    Press a key or press <esc> to cancel", 250, 50 + 20 * highlighted, 240, 50, 50);
				SHOW();
				int a = GetKey();
				if (a != SDL_SCANCODE_ESCAPE)OPTION_buttons_copy[highlighted] = a;
				drawblankline(highlighted);
				print(SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(OPTION_buttons_copy[highlighted]))), 300, 50 + 20 * highlighted, 40, 75, 250);
				SHOW();
				break; 
			}
			case SDL_SCANCODE_RETURN: case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_KP_ENTER:
				OPTION_buttons = OPTION_buttons_copy ;
				options_dirty = true;
				return;
				break;
			case SDL_SCANCODE_ESCAPE:
				return;
				break;
		}
	}
	

}

void setres(char x){
	//if (OPTION_res == x)return;//already that res
	OPTION_res = x;
	//if fullscreen we have to dip back into windowed to change then go back
	if (OPTION_fullscreen)SDL_SetWindowFullscreen(window, 0);
	switch (x){
	case 0:
		SDL_SetWindowSize(window, 640, 360);
		break;
	case 1:
		SDL_SetWindowSize(window, 1280, 720);
		break;
	case 2:
		SDL_SetWindowSize(window, 1920, 1080);
		break;
	}
	if (OPTION_fullscreen)SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}
int displaymainmenu(vector<string>& s, bool istop = false){
	SDL_RenderCopy(renderer, titlepage, NULL, NULL);

	int n = 1;
	for (auto f : s){
		print(to_string(n), 280, 100 + 25 * n, 225, 225, 225);
		print(f, 300, 100 + 25 * n, 225, 225, 225);
		n++;
	}
	if (!istop){
		print(to_string(n), 280, 100 + 25 * n, 225, 225, 225);
		print("Back", 300, 100 + 25 * n, 225, 225, 225);
	}
	SHOW();


	int chosen = 0;
	while (1){
		int a = GetKey();

		switch (a){
		case SDL_SCANCODE_1: case SDL_SCANCODE_KP_2: chosen = 1; break;
		case SDL_SCANCODE_2: case SDL_SCANCODE_KP_3: chosen = 2; break;
		case SDL_SCANCODE_3: case SDL_SCANCODE_KP_4: chosen = 3; break;
		case SDL_SCANCODE_4: case SDL_SCANCODE_KP_5: chosen = 4; break;
		case SDL_SCANCODE_5: case SDL_SCANCODE_KP_6: chosen = 5; break;
		case SDL_SCANCODE_6: case SDL_SCANCODE_KP_7: chosen = 6; break;
		case SDL_SCANCODE_7: case SDL_SCANCODE_KP_8: chosen = 7; break;
		case SDL_SCANCODE_8: case SDL_SCANCODE_KP_9: chosen = 8; break;
		case SDL_SCANCODE_9: case SDL_SCANCODE_KP_1: chosen = 9; break;
		case SDL_SCANCODE_ESCAPE:
			if (!istop)chosen = s.size() + 1;
			break;
		default: chosen = 0; break;
		}
		if ((istop && chosen > 0 && chosen <= s.size()) ||
			(!istop && chosen > 0 && chosen <= s.size() + 1))return chosen;

	}
	
}


bool dotitlepage(){
	
	vector<string> mainmenu  = { "Play game", "Instructions", "Sprite Test", "Options", "Exit" };
	vector<string> optionsmenu = { "Video", "Controls" };
	vector<string> videomenu = { "Fullscreen", "Windowed", "640x360", "1280x720", "1920x1080" };
	vector<string> controlmenu = { "Use Controller", "Use Keyboard", "Redefine Controller Buttons", "Redefine Keys" };

	labelmain:
	int choice=displaymainmenu(mainmenu,true);
	switch (choice){
		case 1://play
			SDL_RenderCopy(renderer, titlepage, NULL, NULL);
			SHOW();
			return true; break;
		case 2://instructions
			instructions();
			goto labelmain;
			break;
		case 3://sprite test
			spritetest();
			goto labelmain;
			break;
		case 4:goto labeloptions;break;//options
		case 5:return false; break;//exit
	}
	labeloptions:
	choice = displaymainmenu(optionsmenu);
	switch (choice){
		case 1:goto labelvideo; break;
		case 2:goto labelcontrols; break;
		case 3:goto labelmain;//back
	}
labelvideo:
	choice = displaymainmenu(videomenu);
	switch (choice){
		case 1://fullscreen
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			OPTION_fullscreen = true;
			options_dirty = true;
			goto labelvideo;
			break;
		case 2://windowed
			SDL_SetWindowFullscreen(window, 0);
			OPTION_fullscreen = false;
			setres(OPTION_res);
			options_dirty = true;
			goto labelvideo;
			break;
		case 3://small 
			setres(0);
			options_dirty = true;
			goto labelvideo;
			break;
		case 4://medium
			setres(1);
			options_dirty = true;
			goto labelvideo;
			break;
		case 5://large
			setres(2);
			options_dirty = true;
			goto labelvideo;
			break;
		case 6:goto labeloptions; //back
	}
labelcontrols:
	choice = displaymainmenu(controlmenu);
	switch (choice){
	case 1:goto labelcontrols; break;//use controller
	case 2:goto labelcontrols; break;//use kbd
	case 3:goto labelcontrols; break;//redef controller
	case 4://redef kbd
		doredefkeys();
		goto labelcontrols;
		break;
	case 5:goto labeloptions; //back

	}

}