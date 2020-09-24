#ifndef NetworkMessage_h__
#define NetworkMessage_h__

#include <memory>

#include "BaseCode.h"
#include "NetworkDefine.h"
#include "ObjectHeap.h"
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
    CNetworkMessage(CNetworkMessage&& rht);
    export_lua CNetworkMessage(const CNetworkMessage& rht);
    export_lua CNetworkMessage(byte* buf, size_t len, VirtualSocket from = 0, VirtualSocket to = 0);
    export_lua CNetworkMessage(uint16_t usCmd, byte* body, size_t body_len, VirtualSocket from = 0, VirtualSocket to = 0);
    export_lua CNetworkMessage(uint16_t usCmd, const proto_msg_t& msg, VirtualSocket from = 0, VirtualSocket to = 0);

    export_lua void CopyRawMessage(const CNetworkMessage& rht);

    export_lua uint16_t GetSize() const { return GetMsgHead()->usSize; }
    export_lua uint16_t GetCmd() const { return GetMsgHead()->usCmd; }
    export_lua uint16_t GetBodySize() const { return GetSize() - sizeof(MSG_HEAD); }
    export_lua byte* GetBuf() const;
    export_lua MSG_HEAD* GetMsgHead() { return (MSG_HEAD*)GetBuf(); }
    export_lua const MSG_HEAD* GetMsgHead() const { return (const MSG_HEAD*)GetBuf(); }
    export_lua byte* GetMsgBody() { return GetBuf() + sizeof(MSG_HEAD); }
    export_lua const byte* GetMsgBody() const { return GetBuf() + sizeof(MSG_HEAD); }

    export_lua VirtualSocket GetFrom() const { return m_nFrom; }
    export_lua void          SetFrom(VirtualSocket val) { m_nFrom = val; }
    export_lua VirtualSocket GetTo() const { return m_nTo; }
    export_lua void          SetTo(VirtualSocket val) { m_nTo = val; }

    export_lua const std::deque<VirtualSocket>& GetForward() const { return m_setForward; }
    export_lua void                             SetForward(const uint64_t* pVS, size_t len);
    export_lua void                             SetForward(const std::deque<VirtualSocket>& setForward) { m_setForward = setForward; }
    export_lua void                             AddForward(const VirtualSocket& forward) { m_setForward.push_back(forward); }
    export_lua void                             ClearForward() { m_setForward.clear(); }
    export_lua void                             PopForward() { m_setForward.pop_front(); }

    export_lua const std::vector<VirtualSocket>& GetMultiTo() const { return m_MultiTo; }
    export_lua bool                              IsMultiTo() const { return m_MultiTo.empty() == false; }
    export_lua void                              SetMultiTo(const uint64_t* pVS, size_t len);
    export_lua void                              SetMultiTo(const std::vector<VirtualSocket>& _MultiTo);
    export_lua void                              AddMultiTo(const VirtualSocket& to);
    export_lua void                              ClearMultiTo() { m_MultiTo.clear(); }

    export_lua const std::vector<OBJID>& GetMultiIDTo() const { return m_MultiIDTo; }
    export_lua bool                      IsMultiIDTo() const { return m_MultiIDTo.empty() == false; }
    export_lua void                      SetMultiIDTo(const OBJID* pIDS, size_t len);
    export_lua void                      SetMultiIDTo(const std::vector<OBJID>& _MultiTo);
    export_lua void                      AddMultiIDTo(OBJID to);
    export_lua void                      ClearMultiIDTo() { m_MultiIDTo.clear(); }

    export_lua bool IsBroadcast() const { return m_bBoradCastAll; }
    export_lua void SetBroadcast() { m_bBoradCastAll = true; }
    export_lua void ClearBroadcast() { m_bBoradCastAll = false; }

    export_lua void CopyBuffer();

private:
    void AllocBuffer(size_t len);

public:
    OBJECTHEAP_DECLARATION(s_Heap);

private:
    VirtualSocket              m_nFrom;
    VirtualSocket              m_nTo;
    std::deque<VirtualSocket>  m_setForward;
    std::vector<VirtualSocket> m_MultiTo;
    std::vector<OBJID>         m_MultiIDTo;
    bool                       m_bBoradCastAll = false;
    byte*                      m_pBuf;
    std::shared_ptr<byte>      m_pBuffer;
    size_t                     m_nBufSize;
};

#endif // NetworkMessage_h__
