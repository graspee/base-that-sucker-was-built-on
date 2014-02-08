#pragma once
#include <cmath>
#include <iostream>

class BitArray
{
private:
	void init(bool _i,size_t _c, size_t _r){
		width = _c, height = _r;
		float tempval =ceil((float) (width*height) / bitswithin);
		numcells = (int) tempval;
		
		data = new unsigned int[numcells];
		Fill(_i);
	} 

	void init_novalue(size_t _c, size_t _r) {
		//why is this code different? why is there even an option for init with no value?
		//remmed out all ver- (next 2 lines) and made uniform with init with value
		//width = _c, height = _r;
		//data = new unsigned int[(width*height) / (sizeof(unsigned int) * 8)];
		width = _c, height = _r;
		float tempval = ceil((float) (width*height) / bitswithin);
		numcells = (int) tempval;
		data = new unsigned int[numcells];
	}

public:

	size_t width, height, numcells;
	unsigned int* data;

	const static size_t bitswithin;
	const static unsigned int  maxval;





	void Init(bool _initialvalue, size_t _cols, size_t _rows){
		init(_initialvalue, _cols, _rows);
	}

	void Init(size_t _cols, size_t _rows){
		init_novalue(_cols, _rows);
	}

	inline void set(size_t _x, size_t _y, bool value){
		int pos = (_y*width) + _x;
		int whichblock  = pos / bitswithin;
		int withinblock = pos % bitswithin;
		if (value){
			data[whichblock] |= (unsigned int)(1 << withinblock); //or
		}
		else {
			data[whichblock] &= ~((unsigned int)(1 << withinblock)); //and

		}

	}

	inline bool get(size_t _x, size_t _y){
		int pos = (_y*width) + _x;
		int whichblock = pos / bitswithin;
		int withinblock = pos % bitswithin;
		unsigned int yes = data[whichblock] & (unsigned int)(1 << withinblock); //and
		return (yes > 0);
	}

	void Fill(bool value){
		if (value){
			for (size_t f = 0; f < numcells; data[f++] = maxval);
		}
		else {
			for (size_t f = 0; f < numcells; data[f++] = 0);
		}
		
	}

	//Constructors
	BitArray(){
		width = -1;
		height = -1;
	}

	BitArray(bool _initialvalue, size_t _cols, size_t _rows){
		init(_initialvalue, _cols, _rows);
	}

	BitArray(size_t _cols, size_t _rows){
		init_novalue(_cols, _rows);
	}
	//Destructor
	~BitArray(){
		delete [] data;
	}


};

const size_t BitArray::bitswithin=sizeof(unsigned int) * 8;
const unsigned int BitArray::maxval=~0;

