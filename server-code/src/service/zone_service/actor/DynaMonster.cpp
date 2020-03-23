#include "DynaMonster.h"

CDynaMonster::CDynaMonster() {}

CDynaMonster::~CDynaMonster() {}

bool CDynaMonster::Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint32_t idCamp)
{
    CHECKF(CMonster::Init(idMonsterType, idOwner, idGen, idCamp));
    return true;
}
