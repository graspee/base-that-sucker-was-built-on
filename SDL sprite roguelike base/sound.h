#pragma once

#include "SDL_mixer.h"

Mix_Chunk* steps=nullptr; //sound effect
Mix_Music* music=nullptr; //music

#define ASSET(x) "c:\\users\\graspee\\devel\\"##x
//mix_volumemusic 0-128
void soundinit(void){
	return;//we don't need sound while we debug this other stuff
	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	steps = Mix_LoadWAV(ASSET("downstairs.wav"));
}

void playsound(void){
	return;//we don't need sound while we debug this other stuff
	Mix_PlayChannel(-1, steps, 0);
}

void playmusic(const string name){
	music = Mix_LoadMUS("a");
	Mix_PlayMusic(music,-1);
}

void soundexit(void){
	return;//we don't need sound while we debug this other stuff
	Mix_FreeChunk(steps);
	if (music != nullptr)Mix_FreeMusic(music);
	Mix_CloseAudio();
}