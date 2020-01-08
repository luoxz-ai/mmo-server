#pragma once

#include "Npc.h"

export_lua class CDynaNpc : public CNpc
{
protected:
	CDynaNpc();
public:
	virtual ~CDynaNpc();

	CREATE_NEW_IMPL(CDynaNpc);
	bool Init();

};