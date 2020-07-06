#pragma once
#include <map>

#include "baseobject.h"
#include "iff.h"
#include "singleton.h"

class CTikiMagicBoxDoc : public BaseObject, public WSingleton<CTikiMagicBoxDoc> {
public:
	CTikiMagicBoxDoc();

private:
	std::map<std::string, std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>> m_outputTableMap;
	std::map<unsigned int, IFF_STRUCT::sTikiPointTable*> m_pointTableMap;
	std::map<unsigned int, IFF_STRUCT::sTikiSpecialRecipe*> m_recipeMap;
};
