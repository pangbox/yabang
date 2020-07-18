#pragma once
#include <map>
#include <string>

template <typename T>
class FrTypeID {
public:
	T operator[](const std::string& typeId) const {
		return this->m_map.at(typeId);
	}

protected:
	std::map<std::string, T> m_map;
};
