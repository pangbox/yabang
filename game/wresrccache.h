#pragma once
#include "wlist.h"

template <typename T>
class WResrcCache {
public:
	struct w_origin {
		T resource;
		int count;
		int checkflag;
		char name[1];
	};

	struct w_cache {
		w_origin* origin;
		T resource;
	};

	WList<w_cache*> resrcList;
	WList<w_origin*> originList;
};
