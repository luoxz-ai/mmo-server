#include "DynaMonster.h"

CDynaMonster::CDynaMonster() {}

CDynaMonster::~CDynaMonster() {}

bool CDynaMonster::Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint64_t idPhase, uint32_t idCamp)
{
    CHECKF(CMonster::Init(idMonsterType, idOwner, idGen, idPhase, idCamp));
    return true;
}
