#ifndef NetworkMessage_h__
#define NetworkMessage_h__

#include <memory>

#include "BaseCode.h"
#include "MemoryHeap.h"
#include "NetworkDefine.h"
#include "google/protobuf/message.h"

export_lua enum MultiType {
	MULTITYPE_NONE = 0,
	MULTITYPE_BROADCAST,
	MULTITYPE_VIRTUALSOCKET,
	MULTITYPE_USERID,
	MULTITYPE_GROUPID,
};
export_lua class CNetworkMessage
{
public:
	CNetworkMessage();
	~CNetworkMessage();
	export_lua CNetworkMessage(const CNetworkMessage& rht);
	export_lua CNetworkMessage(byte* buf, size_t len, VirtualSocket from = 0, VirtualSocket to = 0, VirtualSocket forward = 0);
	export_lua CNetworkMessage(uint16_t usCmd, byte* body, size_t body_len, VirtualSocket from = 0, VirtualSocket to = 0, VirtualSocket forward = 0);
	export_lua CNetworkMessage(uint16_t usCmd, const ::google::protobuf::Message& msg, VirtualSocket from = 0, VirtualSocket to = 0, VirtualSocket forward = 0);

	export_lua uint16_t GetSize() const { return GetMsgHead()->usSize; }
	export_lua uint16_t GetCmd() const { return GetMsgHead()->usCmd; }
	export_lua uint16_t GetBodySize() const { return GetSize() - sizeof(MSG_HEAD); }
	export_lua byte* GetBuf() const;
	export_lua MSG_HEAD* GetMsgHead() { return (MSG_HEAD*)GetBuf(); }
	export_lua const MSG_HEAD* GetMsgHead() const { return (const MSG_HEAD*)GetBuf(); }
	export_lua byte* GetMsgBody() { return GetBuf() + sizeof(MSG_HEAD); }
	export_lua const byte* GetMsgBody() const { return GetBuf() + sizeof(MSG_HEAD); }

	export_lua VirtualSocket GetFrom() const { return m_nFrom; }
	export_lua void			 SetFrom(VirtualSocket val) { m_nFrom = val; }
	export_lua VirtualSocket GetTo() const { return m_nTo; }
	export_lua void			 SetTo(VirtualSocket val) { m_nTo = val; }
	export_lua VirtualSocket GetForward() const { return m_nForward; }
	export_lua void			 SetForward(VirtualSocket val) { m_nForward = val; }
	export_lua void			 SetMultiType(uint32_t nMultiType) { m_nMultiType = nMultiType; }
	export_lua uint32_t		 GetMultiType() const { return m_nMultiType; }

    export_lua const std::vector<VirtualSocket>& GetMultiTo() const { return m_MultiTo; }

    export_lua bool IsMultiTo() const { return m_MultiTo.empty() == false; }
    export_lua void SetMultiTo(const uint64_t* pVS, size_t len);
    export_lua void SetMultiTo(const std::vector<VirtualSocket>& _MultiTo);

    export_lua const std::vector<OBJID>& GetMultiIDTo() const { return m_MultiIDTo; }

    export_lua bool IsMultiIDTo() const { return m_MultiIDTo.empty() == false; }
    export_lua void SetMultiIDTo(const OBJID* pIDS, size_t len);
    export_lua void SetMultiIDTo(const std::vector<OBJID>& _MultiTo);

    export_lua bool IsBroadcast() const { return m_nMultiType == MULTITYPE_BROADCAST; }
	export_lua void SetBroadcast() { m_nMultiType = MULTITYPE_BROADCAST; }

	export_lua void CopyBuffer();

private:
	void AllocBuffer(size_t len);

public:
	MEMORYHEAP_DECLARATION(s_Heap);

private:
	VirtualSocket			   m_nFrom;
	VirtualSocket			   m_nTo;
	VirtualSocket			   m_nForward;
	std::vector<VirtualSocket> m_MultiTo;
	std::vector<OBJID>		   m_MultiIDTo;
	uint32_t				   m_nMultiType = 0;
	byte*					   m_pBuf;
	std::shared_ptr<byte>	   m_pBuffer;
	size_t					   m_nBufSize;
};

#endif // NetworkMessage_h__
