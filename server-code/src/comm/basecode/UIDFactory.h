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
            uint64_t seq_data  : 13;  // 0~0x1FFF   0~17777
            uint64_t time_data : 27;  // 0~0x7FFFFFF 
            uint64_t func_id   : 8;   // 0~0xFF
            uint64_t world_id  : 16;  // 0~0xFFFF
        };
        uint64_t _uin64_data;
    };
};

class CUIDFactory
{
public:
    void Init(uint16_t world_id, uint8_t func_id);
    void Uninit() {}

    uint64_t CreateID();

private:
    uint16_t world_id_;  //大区ID
    uint8_t func_id_;   //功能ID
    time_t   self_time_; //自适应时间
    int32_t  seq_;       //序号
};
#endif /* UIDFACTORY_H */
