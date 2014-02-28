#pragma once

#include <functional>

template <typename T>
class Array2D
{
private:

	int floodcount = 0;

	

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

	int ReplaceXWithY(T x, T y){
		int count = 0;
		for (int f = 0; f < width*height; f++){
			if (data[f] == x){
				count++;
				data[f] = y;
			}
		}
		return count;
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

	bool FloodFill(int x, int y, T space, T safetemp){
		if (x<0 || y<0 || x >= width || y >= height)return false;
		if (at(x, y) != safetemp)return false;
		at(x, y) = space;
		floodcount++;
		FloodFill(x - 1, y, space, safetemp);
		FloodFill(x + 1, y, space, safetemp);
		FloodFill(x, y - 1, space, safetemp);
		FloodFill(x, y + 1, space, safetemp);
	}

	//ret: if all connected. space=flood into this, safetemp: temp value guaranteed not in grid
	bool floodtest(T space, T safetemp){
		int cellcount = 0;
		bool virgin = true;
		uint xt = 0, yt = 0;
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				if (at(x, y) == space){
					cellcount++;
					at(x, y) = safetemp;
					if (virgin){
						virgin = false;
						xt = x;
						yt = y;
					}
				}
			}
		}
		if (cellcount == 0)return false;
		floodcount = 0;
		FloodFill(xt, yt, space, safetemp);
		return (floodcount < cellcount) ? false : true;
	}
};



