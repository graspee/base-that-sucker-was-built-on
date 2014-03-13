#pragma once

#include "SDL_mixer.h"

unordered_map <std::string, Mix_Chunk*> dicosound;

//Mix_Chunk* steps=nullptr; //sound effect
Mix_Music* titlemusic=nullptr; //music
Mix_Music* gamemusic = nullptr;

#define WAVASSET(x) "C:\\Users\\graspee\\git\\Sucker\\media\\"##x##".wav"
//mix_volumemusic 0-128

void loadsound(char* filename,char* name){
	Mix_Chunk* m = Mix_LoadWAV(filename);
	dicosound.insert({ name, m });
}
void soundinit(void){
	//return;//we don't need sound while we debug this other stuff
	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);
	//Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	
	loadsound(WAVASSET("lantern"), "lantern");
	loadsound(WAVASSET("switch1"),"blow");
	loadsound(WAVASSET("switch2"),"suck");
	loadsound(WAVASSET("chuff"), "steam");
	loadsound(WAVASSET("ready"), "ready");

	Mix_Init(MIX_INIT_MP3);
	titlemusic = Mix_LoadMUS(ASSET("tumirada.mp3"));
	gamemusic = Mix_LoadMUS(ASSET("somewhatfoolish.mp3"));
}

void playsound(string name){
	//Mix_Chunk* m = nullptr;
	//m=dicosound.at(name);
	//if(m!=nullptr)
	Mix_PlayChannel(-1, dicosound.at(name), 0);
	//else std::cout << "error: sound tried to play unknown name";
}

void playmusic(bool title=true){
	Mix_HaltMusic();
	if (title){
		Mix_PlayMusic(titlemusic, -1);
	}
	else { Mix_PlayMusic(gamemusic, -1);
	}
}

void soundexit(void){
	//free all the wavs
	for (auto& f : dicosound)
		Mix_FreeChunk(f.second);

	if (titlemusic != nullptr)Mix_FreeMusic(titlemusic);
	if (gamemusic != nullptr)Mix_FreeMusic(gamemusic);
	Mix_CloseAudio();
}