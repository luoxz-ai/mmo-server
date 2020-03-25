#ifndef DYNAMONSTER_H
#define DYNAMONSTER_H

#include "Monster.h"

export_lua class CDynaMonster : public CMonster
{
protected:
    CDynaMonster();
public:
    CreateNewImpl(CDynaMonster);
public:
    virtual ~CDynaMonster();

    
    bool Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint32_t idCamp);
};
#endif /* DYNAMONSTER_H */
