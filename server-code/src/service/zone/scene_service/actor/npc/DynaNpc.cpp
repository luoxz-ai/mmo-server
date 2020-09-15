#include "DynaNpc.h"

CDynaNpc::CDynaNpc() {}

CDynaNpc::~CDynaNpc() {}

bool CDynaNpc::Init(uint32_t idType)
{
    CHECKF(CNpc::Init(idType));
    return true;
}
