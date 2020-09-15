#ifndef DYNANPC_H
#define DYNANPC_H

#include "Npc.h"

export_lua class CDynaNpc : public CNpc
{
protected:
    CDynaNpc();
    bool Init(uint32_t idType);

public:
    CreateNewImpl(CDynaNpc);

public:
    virtual ~CDynaNpc();
};
#endif /* DYNANPC_H */
