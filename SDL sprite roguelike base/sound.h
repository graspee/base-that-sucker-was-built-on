#pragma once

#include "SDL_mixer.h"

Mix_Chunk* steps;

#define ASSET(x) "c:\\users\\graspee\\devel\\"##x

void soundinit(void){
	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	steps = Mix_LoadWAV(ASSET("downstairs.wav"));
}

void playsound(void){
	Mix_PlayChannel(-1, steps, 0);
}

void soundexit(void){
	Mix_FreeChunk(steps);
	Mix_CloseAudio();
}