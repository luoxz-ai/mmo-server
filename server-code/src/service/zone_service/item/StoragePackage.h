#ifndef STORAGEPACKAGE_H
#define STORAGEPACKAGE_H

#include "Package.h"

export_lua class CStoragePackage : public CPackage
{
protected:
    CStoragePackage();

public:
    virtual ~CStoragePackage();
    CREATE_NEW_IMPL(CStoragePackage);

public:
    export_lua void CheckIn(uint32_t nGridInPackage);
    export_lua void CheckOut(uint32_t nGridInStorage);
};
#endif /* STORAGEPACKAGE_H */
