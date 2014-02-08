#pragma once

#include <functional>

template <typename T>
class Array2D
{
private:
	void init(T _i, size_t _c, size_t _r){
		width = _c, height = _r;
		data = new T[width*height];
		Fill(_i);
	}

	void init_novalue( size_t _c, size_t _r) {
		width = _c, height = _r;
		data = new T[width*height];
	}

public:

	size_t width, height;
	T* data;
	//function<void(size_t, size_t, T&)>
	void ForEach(std::function<void(size_t, size_t, T&)> f){
		for (int y = 0; y < height; y++){
			for (int x = 0; x<width; x++){
				f(x, y, at(x, y));
			}
		}
	}

	void Init(T _initialvalue, size_t _cols, size_t _rows){
		init(_initialvalue, _cols, _rows);
	}

	void Init(size_t _cols, size_t _rows){
		init_novalue(_cols, _rows);
	}

	inline T& at(size_t _x, size_t _y){
		return *(data + ((_y*width) + _x));
	}

	void Fill(T value){
		for (size_t f = 0; f < (width*height); data[f++] = value);
	}

	//Constructors
	Array2D(){
		width = -1;
		height = -1;
	}

	Array2D(T _initialvalue, size_t _cols, size_t _rows){
		init(_initialvalue, _cols, _rows);
	}

	Array2D(size_t _cols, size_t rows){
		init_novalue( _cols, _rows);
	}
	//Destructor
	~Array2D(){
		delete [] data;
	}

	
};



