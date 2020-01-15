#ifndef PROTOBUFFPARSE_H
#define PROTOBUFFPARSE_H

#include <unordered_map>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>

class ProtoBufParser
{
public:
	ProtoBufParser() {}
	~ProtoBufParser() {}

	void Init(const std::string& pbdirname)
	{
		m_sourceTree.MapPath("", pbdirname);
		m_pImporter.reset(new google::protobuf::compiler::Importer(&m_sourceTree, nullptr));
	}

	bool ParsePBFile(const std::string& pbname)
	{
		const google::protobuf::FileDescriptor* fdes = m_pImporter->Import(pbname);
		if(fdes == nullptr)
		{
			return false;
		}
		return true;
	}

	void RegisterCMD(const std::string& cmd_enum_typename)
	{
		const ::google::protobuf::EnumDescriptor* pEnumDesc = m_pImporter->pool()->FindEnumTypeByName(cmd_enum_typename);
		if(pEnumDesc == nullptr)
			return;

		for(int i = 0; i < pEnumDesc->value_count(); i++)
		{
			auto		pEnumValDesc = pEnumDesc->value(i);
			std::string msg_name	 = pEnumValDesc->name();
			if(msg_name.substr(0, 4) == "CMD_")
			{
				m_CMD2Name[pEnumValDesc->number()] = msg_name.substr(4, msg_name.size() - 4);
			}
		}
	}

	const google::protobuf::Descriptor* FindDescByName(const std::string& msg_name) { return m_pImporter->pool()->FindMessageTypeByName(msg_name); }

	google::protobuf::Message* NewMessage(const std::string& msg_name)
	{
		const google::protobuf::Descriptor* desc = m_pImporter->pool()->FindMessageTypeByName(msg_name);
		if(desc == nullptr)
		{
			return nullptr;
		}

		const google::protobuf::Message* message = m_factory.GetPrototype(desc);
		if(message == nullptr)
			return nullptr;
		return message->New();
	}

	google::protobuf::Message* NewMessageByCMD(uint32_t nCmd)
	{
		auto itName = m_CMD2Name.find(nCmd);
		if(itName == m_CMD2Name.end())
			return nullptr;
		return NewMessage(itName->second);
	}

private:
	google::protobuf::compiler::DiskSourceTree			  m_sourceTree;
	std::unique_ptr<google::protobuf::compiler::Importer> m_pImporter;
	google::protobuf::DynamicMessageFactory				  m_factory;
	std::unordered_map<uint32_t, std::string>			  m_CMD2Name;
};
#endif /* PROTOBUFFPARSE_H */
