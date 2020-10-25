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

	WList<w_cache*> resrcList{32, 32};
	WList<w_origin*> originList{16, 32};

	T FirstResource() {
		auto *resource = this->resrcList.Start();
		return resource ? resource->resource : nullptr;
	}

	T NextResource() {
		auto* resource = this->resrcList.Next();
		return resource ? resource->resource : nullptr;
	}

	T FindResource(const char* name) {
		auto* resource = this->resrcList.Find(name);
		return resource ? resource->resource : nullptr;
	}

	void AddOrigin(const char* name, T resource) {
		auto origin = reinterpret_cast<w_origin*>(g_mem.Alloc(strlen(name) + sizeof(w_origin)));
		strcpy(origin->name, name);
		origin->resource = resource;
		origin->count = 0;
		origin->checkflag = 0;
		this->originList.AddItem(origin, origin->name, false);
	}

	bool AddResource(const char* name, T resource) {
		auto* origin = this->originList.Find(name);
		if (!origin) {
			this->AddOrigin(name, resource);
			origin = this->originList.Find(name);
		}
		auto cache = reinterpret_cast<w_cache*>(g_mem.Alloc(sizeof(w_cache)));
		cache->origin = origin;
		cache->resource = resource;
		++origin->count;
		origin->checkflag = 0;
		this->resrcList.AddItem(cache, origin->name, false);
		return origin->count <= 1;
	}
};
