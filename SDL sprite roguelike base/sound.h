#pragma once

#include "SDL_mixer.h"

Mix_Chunk* steps;

#define ASSET(x) "c:\\users\\graspee\\devel\\"##x

void soundinit(void){
	return;//we don't need sound while we debug this other stuff
	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	steps = Mix_LoadWAV(ASSET("downstairs.wav"));
}

void playsound(void){
	return;//we don't need sound while we debug this other stuff
	Mix_PlayChannel(-1, steps, 0);
}

void soundexit(void){
	return;//we don't need sound while we debug this other stuff
	Mix_FreeChunk(steps);
	Mix_CloseAudio();
}