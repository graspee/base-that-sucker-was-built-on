#pragma once

#define ASSET(x) "c:\\users\\graspee\\devel\\"##x

//global vars for sprites
std::array<SDL_Texture*, 40> sprites_bg;
std::unordered_map <std::string, SDL_Texture*> dicosprite;

SDL_Texture* hobbitfont;

void spriteinit(void){

	//load my sprites from pango sprite editor

	std::ifstream ifile;//srsnaobg.psf
	ifile.open(ASSET("kobbies.psf"), std::ios::in | std::ios::binary);
	char bigbuffer[4 * 16 * 16];
	char buffer[4];
	for (int s = 0; s < 40; s++){
		if (sprites_bg[s] == nullptr)sprites_bg[s] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 16, 16);
		Uint8 r, g, b;
		char* bufptr = bigbuffer;
		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				ifile.read(buffer, 4);
				if (buffer[3] == 1){
					*(bufptr++) = 255; *(bufptr++) = 0; *(bufptr++) = 255;
					*(bufptr++) = 0;
				}
				else {
					*(bufptr++) = buffer[0]; *(bufptr++) = buffer[1]; *(bufptr++) = buffer[2];
					*(bufptr++) = 255;
				}
			}
		}
		SDL_UpdateTexture(sprites_bg[s], 0, (void*) bigbuffer, 16 * 4);
	}
	//load names
	for (int i = 0; i < 40; i++)
	{
		char buffer[16];
		char c;
		ifile.read(&c, 1);
		ifile.read(buffer, c);
		if (c < 16)buffer[c] = 0;
		dicosprite.insert({ buffer, sprites_bg[i] });
	}
	ifile.close();

	//load "normal" sprites
	SDL_Surface* h = SDL_LoadBMP(ASSET("hobbitfont.bmp"));
	hobbitfont = SDL_CreateTextureFromSurface(renderer, h);
	SDL_FreeSurface(h);

}

void spriteexit(void){
	//Cleanup my sprites
	for (size_t i = 0; i < 40; i++)
		SDL_DestroyTexture(sprites_bg[i]);
	//cleanup font
	SDL_DestroyTexture(hobbitfont);


}

void print(const std::string &s, int x, int y, Uint8 r, Uint8 g, Uint8 b){
	static SDL_Rect srect = { 0, 0, 6, 8 };
	static SDL_Rect drect = { 0, 0, 6, 8 };

	SDL_SetTextureColorMod(hobbitfont, r, g, b);

	for (auto f : s){
		srect.x = (f - 33) * 6;
		drect.x = x;
		drect.y = y;
		//SDL_RenderCopy(renderer, hobbitfont,NULL,NULL);
		SDL_RenderCopy(renderer, hobbitfont, &srect, &drect);
		x += 6;
	}

}