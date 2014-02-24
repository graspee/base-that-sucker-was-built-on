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


	inline void randseed(void){
		std::srand(time(0));
	}


	inline int rand(int low, int high){
		// int i= (std::rand()%((high-low)+1))+low;
		//std::cout << i << std::endl;
		//return i;
		return (std::rand() % ((high - low) + 1)) + low;
	}

	inline int percentof(int self, int x){

		return (int) (((float) x / 100.0)*(float) self) + 0.5;

	}

	inline void shuffle(std::vector<std::pair<int, int>> &v){
		static std::default_random_engine e;
		e.seed(time(0));
		std::shuffle(v.begin(), v.end(), e);
	}

	inline std::pair<int, int> onefromthetop(std::vector<std::pair<int, int>> &v){
		if (v.empty()){
			return std::make_pair(-1, -1);
		}
		else {
			std::pair<int, int> r = v.front();
			v.erase(v.begin());
			return r;
		}
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

