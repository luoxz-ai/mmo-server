#ifndef CALLSTACKDUMPER_H
#define CALLSTACKDUMPER_H

#include <algorithm>
#include <deque>
#include <string>
#include <utility>
#include <vector>

extern std::string demangle(const char* name);

struct CallFrameMap
{
    CallFrameMap(int32_t skip_calldepth = 0, int32_t max_calldepth = 100);
    std::vector<std::pair<void*, std::string>> m_Addr;
};

class CALLFRAME_NODE
{
public:
    CALLFRAME_NODE(CALLFRAME_NODE* pParent = NULL, void* p = NULL);
    ~CALLFRAME_NODE();

    struct Equal
    {
        explicit Equal(void* p);
        bool  operator()(CALLFRAME_NODE* const rht) const;
        void* m_pFunc;
    };

    CALLFRAME_NODE* append(void* p);

    void remove(CALLFRAME_NODE* pChild);

    CALLFRAME_NODE* MakeCallFrame(int32_t skip_calldepth);

public:
    bool  m_bClear;
    void* m_pCallFunc;

    CALLFRAME_NODE*                     m_pParent;
    typedef std::deque<CALLFRAME_NODE*> CHILD_CALLFRAME_NODE;
    CHILD_CALLFRAME_NODE                m_setChild;
};

std::string GetStackTraceString(const CallFrameMap& data);

bool DumpStack(const CallFrameMap& data);
bool DumpStack(const CALLFRAME_NODE* pFrame);
bool DumpStackFile(const CallFrameMap& data);
#endif /* CALLSTACKDUMPER_H */
