#include "NetworkMessage.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/descriptor.h"
//#include "proto/options.pb.h"

MEMORYHEAP_IMPLEMENTATION(CNetworkMessage, s_Heap);

CNetworkMessage::CNetworkMessage()
:m_nFrom(0)
,m_nTo(0)
,m_nForward(0)
,m_pBuf(nullptr)
,m_nBufSize(0)
{

}


CNetworkMessage::~CNetworkMessage()
{

}


CNetworkMessage::CNetworkMessage(const CNetworkMessage& rht)
:m_nFrom(rht.m_nFrom)
,m_nTo(rht.m_nTo)
,m_nForward(rht.m_nForward)
,m_pBuf(rht.m_pBuf)
,m_pBuffer(rht.m_pBuffer)
,m_nBufSize(rht.m_nBufSize)
,m_MultiTo(rht.m_MultiTo)
,m_nMultiType(rht.m_nMultiType)
{

}

CNetworkMessage::CNetworkMessage(byte* buf, size_t len, VirtualSocket from /*= 0*/, VirtualSocket to /*= 0*/, VirtualSocket forward /*= 0*/)
:m_nFrom(from)
,m_nTo(to)
,m_nForward(forward)
,m_pBuf(buf)
,m_nBufSize(len)
{

}

CNetworkMessage::CNetworkMessage(uint16_t usCmd, const ::google::protobuf::Message& msg, VirtualSocket from /*= 0*/, VirtualSocket to /*= 0*/, VirtualSocket forward /*= 0*/)
:m_nFrom(from)
,m_nTo(to)
,m_nForward(forward)
,m_pBuf(nullptr)
,m_nBufSize(0)
{
	int nDataSize = msg.ByteSizeLong();
	AllocBuffer(nDataSize + sizeof(MSG_HEAD));
	msg.SerializeToArray(GetMsgBody(), nDataSize);
	
	MSG_HEAD* pHead = GetMsgHead();
	pHead->usSize = nDataSize + sizeof(MSG_HEAD);
	pHead->usCmd = usCmd;
	//pHead->usCmd =  = msg.GetDescriptor()->options().GetExtension(NetMSG::msgid);

	//static auto desp = google::protobuf::DescriptorPool::generated_pool()->FindEnumTypeByName(std::string("MSGID"));
	//auto evd = desp->FindValueByName(msg.GetDescriptor()->full_name());
	//pHead->usCmd = evd->number();
}

CNetworkMessage::CNetworkMessage(uint16_t usCmd, byte* body, size_t body_len, VirtualSocket from /*= 0*/, VirtualSocket to /*= 0*/, VirtualSocket forward /*= 0*/)
:m_nFrom(from)
,m_nTo(to)
,m_nForward(forward)
,m_pBuf(nullptr)
,m_nBufSize(0)
{
	AllocBuffer(body_len + sizeof(MSG_HEAD));
	memcpy(GetMsgBody(), body, body_len);
	MSG_HEAD* pHead = GetMsgHead();
	pHead->usSize = body_len + sizeof(MSG_HEAD);
	pHead->usCmd = usCmd;
}


byte* CNetworkMessage::GetBuf() const
{
	if (m_pBuf)
		return m_pBuf;
	return m_pBuffer.get();
}


void CNetworkMessage::SetMultiTo(const VirtualSocket* pVS, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		m_MultiTo.push_back(pVS[i]);
	}
}

void CNetworkMessage::SetMultiTo(const std::vector<VirtualSocket>& _MultiTo)
{
	m_MultiTo.insert(m_MultiTo.end(),_MultiTo.begin(), _MultiTo.end());
}


void CNetworkMessage::SetMultiIDTo(const OBJID* pIDS, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		m_MultiIDTo.push_back(pIDS[i]);
	}
}

void CNetworkMessage::SetMultiIDTo(const std::vector<OBJID>& _MultiTo)
{
	m_MultiIDTo.insert(m_MultiIDTo.end(), _MultiTo.begin(), _MultiTo.end());
}


void CNetworkMessage::CopyBuffer()
{
	if (m_pBuf == nullptr)
		return;
	AllocBuffer(m_nBufSize);
	memcpy(m_pBuffer.get(), m_pBuf, m_nBufSize);
	m_pBuf = nullptr;
}

void CNetworkMessage::AllocBuffer(size_t len)
{
	m_pBuffer = std::shared_ptr<byte>(new byte[len], [](byte* p) {delete[] p; });
	m_nBufSize = len;
}

