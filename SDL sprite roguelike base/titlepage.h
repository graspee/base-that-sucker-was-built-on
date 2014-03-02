#pragma once


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

	SDL_Event sdle;
	int chosen = 0;
	while (1){
		int a = SDL_WaitEvent(&sdle);
		if (sdle.type == SDL_KEYDOWN){
			switch (sdle.key.keysym.sym){
				case SDLK_1: case SDLK_KP_1: chosen = 1; break;
				case SDLK_2: case SDLK_KP_2: chosen = 2; break;
				case SDLK_3: case SDLK_KP_3: chosen = 3; break;
				case SDLK_4: case SDLK_KP_4: chosen = 4; break;
				case SDLK_5: case SDLK_KP_5: chosen = 5; break;
				case SDLK_6: case SDLK_KP_6: chosen = 6; break;
				case SDLK_7: case SDLK_KP_7: chosen = 7; break;
				case SDLK_8: case SDLK_KP_8: chosen = 8; break;
				case SDLK_9: case SDLK_KP_9: chosen = 9; break;
				case SDLK_ESCAPE:
					if (!istop)chosen = s.size() + 1;
					break;
				default: chosen = 0; break;
			}
			if ((istop && chosen > 0 && chosen <= s.size()) ||
				(!istop && chosen > 0 && chosen <= s.size() + 1))return chosen;
		}

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
			goto labelvideo;
			break;
		case 2://windowed
			SDL_SetWindowFullscreen(window, 0);
			goto labelvideo;
			break;
		case 3://small 
			SDL_SetWindowSize(window, 640, 360);
			goto labelvideo;
			break;
		case 4://medium
			SDL_SetWindowSize(window, 1280, 720);
			goto labelvideo;
			break;
		case 5://large
			SDL_SetWindowSize(window,1920, 1080);
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
	case 4:break;//redef kbd
	case 5:goto labeloptions; //back

	}

}