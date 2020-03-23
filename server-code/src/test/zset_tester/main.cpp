#include "BaseCode.h"
#include "ZSet.h"

int main()
{
    __ENTER_FUNCTION
    {
        CZset test_set;
        for(int32_t i = 1; i < 1000; i++)
        {
            test_set.zAdd(i, random_uint32_range(1, 1000));
            // test_set.zAdd(i, i);
        }
        uint64_t nTestMeberID    = 28;
        uint32_t nTestMemberRank = 0;
        test_set.for_each([nTestMeberID, &nTestMemberRank](uint32_t nRank, uint64_t member, uint32_t score) {
            if(member == nTestMeberID)
                nTestMemberRank = nRank;
        });
        uint32_t nRank28 = test_set.zRank(28);
        if(nRank28 != nTestMemberRank)
            return 0;
        test_set.zRange(1, 99, [](uint32_t nRank, uint64_t member, uint32_t score) {
            printf("rank:{} member:%ld score:{}\n", nRank, member, score);
        });
        test_set.zRevRange(1, 99, [](uint32_t nRank, uint64_t member, uint32_t score) {
            printf("rank:{} member:%ld score:{}\n", nRank, member, score);
        });
    }
    {
        CZset test_set;
        test_set.zAdd(1, 1);
        test_set.zAdd(2, 1);
        test_set.zAdd(3, 1);
        uint32_t nRank1 = test_set.zRank(1);
        uint32_t nRank3 = test_set.zRank(3);
        uint32_t nRank2 = test_set.zRank(2);
        test_set.zRevRange(1, 99, [](uint32_t nRank, uint64_t member, uint32_t score) {
            printf("rank:{} member:%ld score:{}\n", nRank, member, score);
        });
    }

    __LEAVE_FUNCTION
}