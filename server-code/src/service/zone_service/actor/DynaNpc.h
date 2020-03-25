#ifndef DYNANPC_H
#define DYNANPC_H

#include "Npc.h"

export_lua class CDynaNpc : public CNpc
{
protected:
    CDynaNpc();
public:
    CreateNewImpl(CDynaNpc);
public:
    virtual ~CDynaNpc();

    bool Init(uint32_t idType);
};
#endif /* DYNANPC_H */
