#pragma once

#include <cstdlib>
#include <time.h>
#include <random>
#include <algorithm>
#include <string>

#define coldex(x) pe.palette->u32vals[##x]
#define coldexp(x) pe->palette->u32vals[##x]
#define vpa(a,b) a->insert(a->end(),b->begin(),b->end())

typedef unsigned char uint8;


namespace lil {

	static const char opdir[4] = { 1, 0, 3, 2 };
	static const char deltax[4] = { 0, 0, 1, -1 };
	static const char deltay[4] = { -1, 1, 0, 0 };
	static const char dirchar[4] =		{ '^', 'v', '>', '<' };
	static const char dirchar_rev[4] =	{ 'v', '^', '<', '>' };

	std::default_random_engine e;

	inline void randseed(void){
		//static std::default_random_engine e;
		e.seed(time(0));
		//std::srand(time(0));
	}

	template <typename T>
	inline T& randmember(std::vector<T>& v){
		//return v [std::rand()%v.size()];
		return v [e()%v.size()];
	}



	inline int rand(int low, int high){
		// int i= (std::rand()%((high-low)+1))+low;
		//std::cout << i << std::endl;
		//return i;
		//return (std::rand() % ((high - low) + 1)) + low;
		return (e() % ((high - low) + 1)) + low;
	}

	inline int percentof(int self, int x){

		return (int) (((float) x / 100.0)*(float) self) + 0.5;

	}

	template <typename T>
	inline void shuffle(std::vector<T> &v){
		//static std::default_random_engine e;
		//e.seed(time(0));
		std::shuffle(v.begin(), v.end(), e);
	}
	//need another way to signal error
	template <typename T>
	inline T onefromthetop(std::vector<T> &v){
		if (v.empty()){
			std::cout << "fatal error: attempt to onefromthetop empty vector";
		}
		else {
			T r = v.front();
			v.erase(v.begin());
			return r;
		}
	}
	
	//or just use pushback...
	template <typename T>
	inline void stickonthebottom(std::vector<T> &v, T& data){
		v.push_back(data);
	}

	inline std::string numformat(int n, int numplaces){
		std::string s = std::to_string(n);
		int l = s.length();
		if (l<numplaces){
			for (int i = 0; i<numplaces - l; i++)
				s = "0" + s;
		}
		return s;
	}

	inline std::string RandStrFrom(std::string v[]){
		return v[lil::rand(0, 4)];
	}


}

