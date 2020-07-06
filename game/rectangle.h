#pragma once
#include "coordinates.h"

template<typename T>
struct Rectangle {
	union {
		Coordinates<T> tl;
		struct { T left, top; };
	};
	union {
		Coordinates<T> br;
		struct { T right, bottom; };
	};
};
