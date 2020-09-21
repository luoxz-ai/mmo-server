#ifndef UIDFACTORY_H
#define UIDFACTORY_H

#include <stdint.h>
#include <time.h>

#include "BaseCode.h"

struct UID_TYPE
{
    UID_TYPE(uint64_t data = 0)
        : _uin64_data(data)
    {
    }
    union {
        struct
        {
            uint64_t seq_data : 10;  // 0~0x3FF        0~1023
            uint64_t time_data : 29; // 0~0x1FFFFFFF   0~536870911  = 17year
            uint64_t func_id : 9;    // 0~0x1FF        0~511
            uint64_t world_id : 16;  // 0~0xFFFF       0~65535
        };
        uint64_t _uin64_data;
    };
};

class CUIDFactory
{
public:
    void Init(uint64_t world_id, uint64_t func_id);
    void Uninit() {}

    uint64_t CreateID();

private:
    uint64_t world_id_;  //大区ID
    uint64_t func_id_;   //功能ID
    time_t   self_time_; //自适应时间
    int32_t  seq_;       //序号
};
#endif /* UIDFACTORY_H */
