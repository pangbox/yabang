#pragma once
#include <exception>
#include <stdexcept>

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

	void AddItem(T const &item, const char *keycode, bool alloc) {
		ListInfo* newItem = Alloc();
		newItem->item = item;
		newItem->alloc = alloc;

		if (alloc) {
			newItem->keycode = _strdup(keycode);
		} else {
			newItem->keycode = const_cast<char *>(keycode);
		}

		if (keycode) {
			this->AddHash(this->Hashcode(keycode), newItem);
		}

		this->m_list = Link(this->m_list, newItem);
	}

	void operator+=(T const &item) {
		this->AddItem(item, nullptr, false);
	}

	T Find(const char *name) {
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
		while (strcmp(it->keycode, name)) {
			it = it->hash;
			if ( !it ) {
				return 0;
			}
		}
		return it->item;
	}

private:
	int Hashcode(const char *ptr) {
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
		char *keycode;
		bool alloc;
		ListInfo *prev;
		ListInfo *next;
		ListInfo *hash;
	};

	ListInfo *Alloc() {
		ListInfo *item;

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

	void AddHash(int hashcode, ListInfo *node) {
		node->hash = this->m_hashList[hashcode];
		this->m_hashList[hashcode] = node;
	}

	static ListInfo *Link(ListInfo *pivot, ListInfo *node) {
		if (pivot) {
			node->next = pivot;
			node->prev = pivot->prev;
			pivot->prev = node;
			node->prev->next = node;
			return pivot;
		} else {
			node->prev = node;
			node->next = node;
			return node;
		}
	}

	ListInfo *m_list = nullptr;
	ListInfo *m_surf = nullptr;
	ListInfo *m_preAlloc = nullptr;
	ListInfo *m_idle = nullptr;
	int m_blkLen{};
	int m_hashMask{};
	int m_hashNum{};
	ListInfo** m_hashList = nullptr;
};
