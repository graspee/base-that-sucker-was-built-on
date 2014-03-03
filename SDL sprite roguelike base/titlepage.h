#pragma once

int GetKey(){
	static SDL_Event e;
	while (1){
		int a = SDL_WaitEvent(&e);
		if (e.type == SDL_KEYDOWN){
			return e.key.keysym.scancode;
		}
	}
}

void doredefkeys(){
	SDL_RenderClear(renderer);
	print("REDEFINE KEYS:", 0, 0, 225, 225, 40);
	//print("(Physical keys, not logical are shown)", 0, 8, 225, 225, 40);
	print("Up and Down arrows to move, enter/return to set key", 0, 16, 225, 225, 0);
	SDL_RenderPresent(renderer);
	int x = 0;
	for (auto f : OPTION_buttons){
		print(button_names[x], 150, 50 + 20 * x, 40, 225, 225);
		print(SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(f))), 250, 50 + 20 * x, 40, 75, 250);
		x++;
	}
	SDL_RenderPresent(renderer);

	int highlighted = 0;
	
	while (1){
		int k = GetKey();
		switch (k){
		case SDL_SCANCODE_DOWN:
			break;
		case SDL_SCANCODE_UP:
			break;
		case SDL_SCANCODE_RETURN: case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_KP_ENTER:
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
	SDL_RenderPresent(renderer);


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
	
	vector<string> mainmenu  = { "Play game", "Instructions", "Options", "Exit" };
	vector<string> optionsmenu = { "Video", "Controls" };
	vector<string> videomenu = { "Fullscreen", "Windowed", "640x360", "1280x720", "1920x1080" };
	vector<string> controlmenu = { "Use Controller", "Use Keyboard", "Redefine Controller Buttons", "Redefine Keys" };

	labelmain:
	int choice=displaymainmenu(mainmenu,true);
	switch (choice){
		case 1:return true; break;//play
		case 2:break;//instructions
		case 3:goto labeloptions;break;//options
		case 4:return false; break;//exit
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
			goto labelvideo;
			break;
		case 2://windowed
			SDL_SetWindowFullscreen(window, 0);
			OPTION_fullscreen = false;
			setres(OPTION_res);
			goto labelvideo;
			break;
		case 3://small 
			setres(0);
			goto labelvideo;
			break;
		case 4://medium
			setres(1);
			goto labelvideo;
			break;
		case 5://large
			setres(2);
			goto labelvideo;
			break;
		case 6:goto labeloptions; //back
	}
labelcontrols:
	choice = displaymainmenu(controlmenu);
	switch (choice){
	case 1:break;//use controller
	case 2:break;//use kbd
	case 3:break;//redef controller
	case 4://redef kbd
		doredefkeys();
		goto labelcontrols;
		break;
	case 5:goto labeloptions; //back

	}

}