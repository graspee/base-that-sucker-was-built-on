#pragma once

#define ASSET(x) "C:\\Users\\graspee\\git\\Sucker\\media\\"##x

//global vars for sprites
std::array<SDL_Texture*, 40> sprites;
std::array<SDL_Texture*, 40> bank2;
std::unordered_map <std::string, SDL_Texture*> dicosprite;

SDL_Texture* hobbitfont, *titlepage;

SDL_Texture* SpriteLoad(const char* name){
	SDL_Surface* h = SDL_LoadBMP(name);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, h);
	SDL_FreeSurface(h);
	return tex;
}
void myspriteload(string filename, array<SDL_Texture*,40>& arr){
	//load my sprites from pango sprite editor
	std::ifstream ifile;
	ifile.open(filename, std::ios::in | std::ios::binary);
	char bigbuffer[4 * 16 * 16];
	char buffer[4];
	for (int s = 0; s < 40; s++){
		if (arr[s] == nullptr)arr[s] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 16, 16);
		Uint8 r, g, b;
		char* bufptr = bigbuffer;
		bool alphap = false;
		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				ifile.read(buffer, 4);
				if (buffer[3] == 1){
					*(bufptr++) = 255; *(bufptr++) = 0; *(bufptr++) = 255;
					*(bufptr++) = 0;
					alphap = true;
				}
				else {
					*(bufptr++) = buffer[0]; *(bufptr++) = buffer[1]; *(bufptr++) = buffer[2];
					*(bufptr++) = 255;
				}
			}
		}
		SDL_UpdateTexture(arr[s], 0, (void*) bigbuffer, 16 * 4);
		//set blendmode of sprites containing transparency 
		if (alphap)SDL_SetTextureBlendMode(arr[s], SDL_BLENDMODE_BLEND);
	}
	//load names
	for (int i = 0; i < 40; i++)
	{
		char buffer[16];
		char c;
		ifile.read(&c, 1);
		ifile.read(buffer, c);
		if (c < 16)buffer[c] = 0;
		dicosprite.insert({ buffer, arr[i] });
	}
	ifile.close();
}
void spriteinit(void){

	myspriteload(ASSET("sprites.psf"), sprites);
	myspriteload(ASSET("bank2.psf"), bank2);

	hobbitfont=SpriteLoad(ASSET("hobbitfont.bmp"));
	titlepage = SpriteLoad(ASSET("title.bmp"));
}



void spriteexit(void){
	//Cleanup my sprites
	for (size_t i = 0; i < 40; i++){
		SDL_DestroyTexture(sprites[i]);
		SDL_DestroyTexture(bank2[i]);
	}
	//cleanup font
	SDL_DestroyTexture(hobbitfont);
	SDL_DestroyTexture(titlepage);


}


void print(const std::string &s, int x=0, int y=-1, Uint8 r=225, Uint8 g=225, Uint8 b=225){
	static int lasty = 0;
	if (y == -1){
		y = lasty+8;
		lasty = y;
	}
	static SDL_Rect srect = { 0, 0, 6, 8 };
	static SDL_Rect drect = { 0, 0, 6, 8 };

	SDL_SetTextureColorMod(hobbitfont, r, g, b);

	for (auto f : s){
		srect.x = (f - 32) * 6;
		drect.x = x;
		drect.y = y;
		//SDL_RenderCopy(renderer, hobbitfont,NULL,NULL);
		SDL_RenderCopy(renderer, hobbitfont, &srect, &drect);
		x += 6;
	}
}
void middleprint(const string&s, int y, Uint8 r = 225, Uint8 g = 225, Uint8 b = 225){
	print(s, (640-(s.size()*6))/2, y, r, g, b);
}
void messagelog(const std::string& s,Uint8 _r=225,Uint8 _g=225,Uint8 _b=225){
	static string log[10] = { "", "", "", "", "", "", "", "", "", ""};
	static char pointer = 0;
	static Uint8 rc[10], gc[10], bc[10];

	if (s.size() > 46)
		log[pointer] = s.substr(0,46);
	else
		log[pointer] = s+string(46-s.size(),' ');
	
	rc[pointer] = _r, gc[pointer] = _g, bc[pointer] = _b;
	//pointer++; if (pointer > 9)pointer = 0;
	

	char g = pointer + 1; if (g > 9)g = 0;
	for (char f = 0; f < 10; f++){
		
		print(log[g], (22 * 16)+6, 360 - (8 * (10 - f)),rc[g],gc[g],bc[g]);
		g++; if (g>9)g = 0;
	}
	pointer++; if (pointer > 9)pointer = 0;
}

void superprint(const std::string &s, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity){
	static SDL_Rect srect = { 0, 0, 6, 8 };
	static SDL_Rect drect = { 0, 0, 60, 80 };

	int x = (640 - (s.size()*60)) / 2;
	
	SDL_SetTextureColorMod(hobbitfont, r, g, b);
	
	for (auto f : s){
		srect.x = (f - 32) * 6;
		drect.x = x;
		drect.y = y;
		SDL_RenderCopy(renderer, hobbitfont, &srect, &drect);
		x += 60;
	}
}

inline void drawsprite(int x, int y, SDL_Texture* t){
	static SDL_Rect r = { 0, 0, 16, 16 };
	r.x = x, r.y = y;
	SDL_RenderCopy(renderer, t, NULL, &r);

}
inline void drawsprite(int x, int y, string name){
	static SDL_Rect r = { 0, 0, 16, 16 };
	r.x = x, r.y = y;
	SDL_RenderCopy(renderer, dicosprite.at(name), NULL, &r);
}