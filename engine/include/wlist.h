#pragma once
#include <stdexcept>

class WMemFillBlock {
public:
	WMemFillBlock(int num, int level) {
		int j = 0;
		this->m_num = (num + 31) & ~31;
		this->m_level = level;
		this->m_size = 4 * this->m_num * ((1 << level) - 1);
		this->m_ptr = static_cast<uint8_t*>(malloc(this->m_size + level * (this->m_num / 8)));
		for (int i = 0; i < this->m_level; j += this->m_num << (i++ + 2)) {
			this->m_mem[i] = &this->m_ptr[j];
		}
		for (int i = 0; i < this->m_level; i++) {
			this->m_mask[i] = reinterpret_cast<unsigned int*>(&this->m_ptr[j]);
			memset(&this->m_ptr[j], 0, this->m_num / 8);
			j += this->m_num / 8;
		}
		for (int i = 0; i < this->m_level; i++) {
			this->m_lastidx[i] = 0;
		}
	}

	~WMemFillBlock() {
		free(this->m_ptr);
	}

	int FindSlot(unsigned int mask) {
		int j = 0;
		while (((1 << j) & mask) != 0) {
			if (++j >= 32) {
				return -1;
			}
		}
		return j;
	}

	uint8_t* Alloc(int size) {
		if (size > 1 << (this->m_level + 1) || this->m_level <= 0) {
			return static_cast<uint8_t*>(malloc(size));
		}
		int i = 0;
		while (size > 1 << (i + 2)) {
			if (++i >= this->m_level) {
				return static_cast<uint8_t*>(malloc(size));
			}
		}
		int n = 0, m = this->m_lastidx[i];
		if (this->m_num <= 0) {
			return static_cast<uint8_t*>(malloc(size));
		}
		while (true) {
			if (n + m >= this->m_num) {
				m -= this->m_num;
			}
			if (static_cast<int>(this->m_mask[i][(n + m) >> 5]) != -1) {
				break;
			}
			n += 32;
			if (n >= this->m_num) {
				return static_cast<uint8_t*>(malloc(size));
			}
		}
		int j = FindSlot(this->m_mask[i][(n + m) >> 5]);
		this->m_mask[i][(n + m) >> 5] |= 1 << j;
		this->m_lastidx[i] = n + m;
		return &this->m_mem[i][(m + n + j) << (i + 2)];
	}

	void Free(void* mem) {
		int idx = static_cast<uint8_t*>(mem) - this->m_ptr;
		if (idx < 0 || idx >= this->m_size) {
			free(mem);
			return;
		}
		if (this->m_level <= 0) {
			return;
		}
		int n = 0, m = 0;
		for (; m < this->m_level; m++) {
			int q = this->m_num << (m + 2);
			if (idx - n < q) {
				break;
			}
			n += q;
		}
		int p = (idx - n) >> (m + 2);
		this->m_mask[m][p >> 5] &= ~(1 << (p & 31));
	}

private:
	uint8_t* m_mem[6]{};
	uint32_t* m_mask[6]{};
	uint8_t* m_ptr{};
	int m_lastidx[6]{};
	int m_num{};
	int m_size{};
	int m_level{};
};

extern WMemFillBlock g_mem;

template <typename T>
class WList {
public:
	WList(int len, int hashNum) {
		this->m_blkLen = len;
		this->m_hashList = 0;
		this->m_hashMask = 0;
		this->m_list = 0;
		this->m_idle = 0;
		this->m_preAlloc = 0;
		this->m_surf = 0;
		this->m_hashNum = hashNum;

		if (hashNum <= 0) {
			this->m_hashList = 0;
		} else {
			this->m_hashList = new ListInfo*[hashNum];
			this->m_hashMask = this->m_hashNum - 1;
			for (int i = 0; i < this->m_hashNum; i++) {
				this->m_hashList[i++] = 0;
			}
		}
	}

	T Start() {
		this->m_surf = this->m_list;
		if (this->m_list == nullptr) {
			return NULL;
		}
		this->m_surf = this->m_list->next != this->m_list ? this->m_list->next : NULL;
		return m_list->item;
	}

	T Next() {
		if (!this->m_surf) {
			return NULL;
		}
		T result = this->m_surf->item;
		this->m_surf = this->m_list != this->m_surf->next ? this->m_surf->next : NULL;
		return result;
	}

	void AddItem(T const& item, const char* keycode, bool alloc) {
		ListInfo* newItem = Alloc();
		newItem->item = item;
		newItem->alloc = alloc;

		if (alloc) {
			newItem->keycode = _strdup(keycode);
		} else {
			newItem->keycode = const_cast<char*>(keycode);
		}

		if (keycode) {
			this->AddHash(this->Hashcode(keycode), newItem);
		}

		this->m_list = Link(this->m_list, newItem);
	}

	void operator+=(T const& item) {
		this->AddItem(item, nullptr, false);
	}

	T Find(const char* name) {
		if (!name) {
			return 0;
		}
		if (!this->m_hashList) {
			return 0;
		}
		ListInfo* it = this->m_hashList[this->Hashcode(name)];
		if (!it) {
			return 0;
		}
		while (strcmp(it->keycode, name) != 0) {
			it = it->hash;
			if (!it) {
				return 0;
			}
		}
		return it->item;
	}

	void DelItem(T const& item) {
		ListInfo* i = this->m_list;
		if (this->m_list) {
			while (i->item != item) {
				i = i->next;
				if (i == this->m_list) {
					return;
				}
			}
			if (this->m_surf == i) {
				this->m_surf = this->m_list != this->m_surf->next ? this->m_surf->next : 0;
			}
			if (i->keycode) {
				this->DelHash(i);
				if (i->alloc) {
					g_mem.Free(i->keycode);
				}
			}
			i->next->prev = i->prev;
			i->prev->next = i->next;
			this->m_list = i == this->m_list ? (this->m_list != i->next ? i->next : nullptr) : this->m_list;
			if (this->m_idle) {
				i->next = this->m_idle;
				i->prev = this->m_idle->prev;
				this->m_idle->prev = i;
				i->prev->next = i;
			} else {
				i->prev = i;
				i->next = i;
				this->m_idle = i;
			}
			if (this->m_list == nullptr) {
				this->m_surf = 0;
			}
		}
	}

	void Reset() {
		if (this->m_list) {
			if (this->m_idle) {
				this->m_list->prev->next = this->m_idle->next;
				this->m_idle->next->prev = this->m_list->prev;
				this->m_idle->next = this->m_list;
				this->m_list->prev = this->m_idle;
				this->m_list = 0;
			} else {
				ListInfo* node;
				do {
					this->m_list->next->prev = this->m_list->prev;
					this->m_list->prev->next = this->m_list->next;
					node = this->m_list != this->m_list->next ? this->m_list->next : nullptr;
					if (this->m_idle) {
						this->m_list->next = this->m_idle;
						this->m_list->prev = this->m_idle->prev;
						this->m_idle->prev = this->m_list;
						this->m_list->prev->next = this->m_list;
					} else {
						this->m_list->prev = this->m_list;
						this->m_list->next = this->m_list;
						this->m_idle = this->m_list;
					}
					this->m_list = node;
				} while (node);
			}
		}
		for (int i = 0; i < this->m_hashNum; ++i) {
			this->m_hashList[i] = 0;
		}
	}

private:
	int Hashcode(const char* ptr) {
		int len = strlen(ptr);
		if (len <= 0) {
			len = 0;
		} else {
			len = len - 1;
		}
		return this->m_hashMask & (len << 6 | (ptr[0] & 0x2A) | (ptr[len] & 0x15));
	}

	struct ListInfo {
		T item;
		char* keycode;
		bool alloc;
		ListInfo* prev;
		ListInfo* next;
		ListInfo* hash;
	};

	ListInfo* Alloc() {
		ListInfo* item;

		if (!this->m_idle) {
			item = new ListInfo[this->m_blkLen];
			if (m_preAlloc) {
				item->next = this->m_preAlloc;
				item->prev = this->m_preAlloc->prev;
				this->m_preAlloc->prev = item;
				item->prev->next = item;
			} else {
				item->prev = item;
				item->next = item;
				this->m_preAlloc = item;
			}
			++item;
			for (int i = 1; i < this->m_blkLen; ++i) {
				if (this->m_idle) {
					item->next = this->m_idle;
					item->prev = this->m_idle->prev;
					this->m_idle->prev = item;
					item->prev->next = item;
				} else {
					item->prev = item;
					item->next = item;
					this->m_idle = item;
				}
				++item;
			}
			if (!this->m_idle) {
				throw std::logic_error("m_idle should not be null");
			}
		}

		item = this->m_idle;
		item->next->prev = item->prev;
		item->prev->next = item->next;
		this->m_idle = item != item->next ? item->next : 0;
		return item;
	}

	void AddHash(int hashcode, ListInfo* node) {
		node->hash = this->m_hashList[hashcode];
		this->m_hashList[hashcode] = node;
	}

	void DelHash(ListInfo* node) {
		int hashcode = Hashcode(node->keycode);
		ListInfo* info = this->m_hashList[hashcode];
		if (this->m_hashList[hashcode] == node) {
			this->m_hashList[hashcode] = node->hash;
			return;
		}
		for (; info != nullptr; info = info->hash) {
			if (info->hash == node) {
				info->hash = node->hash;
				return;
			}
		}
	}

	static ListInfo* Link(ListInfo* pivot, ListInfo* node) {
		if (pivot) {
			node->next = pivot;
			node->prev = pivot->prev;
			pivot->prev = node;
			node->prev->next = node;
			return pivot;
		}
		node->prev = node;
		node->next = node;
		return node;
	}

	ListInfo* m_list = nullptr;
	ListInfo* m_surf = nullptr;
	ListInfo* m_preAlloc = nullptr;
	ListInfo* m_idle = nullptr;
	int m_blkLen{};
	int m_hashMask{};
	int m_hashNum{};
	ListInfo** m_hashList = nullptr;
};
