#ifndef DYNAMONSTER_H
#define DYNAMONSTER_H

#include "Monster.h"

export_lua class CDynaMonster : public CMonster
{
protected:
	CDynaMonster();

public:
	virtual ~CDynaMonster();

	CREATE_NEW_IMPL(CDynaMonster);
	bool Init();
};
#endif /* DYNAMONSTER_H */
