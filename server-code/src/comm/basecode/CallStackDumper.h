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
	CallFrameMap(int skip_calldepth = 0);
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

	CALLFRAME_NODE* MakeCallFrame(int skip_calldepth);

  public:
	bool  m_bClear;
	void* m_pCallFunc;

	CALLFRAME_NODE*						m_pParent;
	typedef std::deque<CALLFRAME_NODE*> CHILD_CALLFRAME_NODE;
	CHILD_CALLFRAME_NODE				m_setChild;
};

bool DumpStack(const CallFrameMap& data);
bool DumpStack(const CALLFRAME_NODE* pFrame);
bool DumpStackFile(const CallFrameMap& data);
#endif /* CALLSTACKDUMPER_H */
