#include "ProtobuffUtil.h"

#include <fstream>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>

#include "loging_manager.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"

namespace pb_util
{
bool LoadFromTextualFile(const std::string& filename, google::protobuf::Message& pbm)
{
	bool		  rv = false;
	std::ifstream ifs(filename.data());
	if(ifs.is_open())
	{
		google::protobuf::io::IstreamInputStream zcis(&ifs);
		rv = google::protobuf::TextFormat::Parse(&zcis, &pbm);
		ifs.close();
	}
	return rv;
}

bool SaveToTextualFile(const google::protobuf::Message& pbm, const std::string& filename)
{
	bool		  rv = false;
	std::ofstream ofs(filename.data(), std::ios::out | std::ios::trunc);
	if(ofs.is_open())
	{
		std::string text;
		rv = google::protobuf::TextFormat::PrintToString(pbm, &text);
		ofs << text;

		ofs.close();
	}
	return rv;
}

bool SaveToBinaryFile(const google::protobuf::Message& pbm, const std::string& filename)
{
	bool		  rv = false;
	std::ofstream ofs(filename.data(), std::ios::out | std::ios::binary);
	if(ofs.is_open())
	{
		pbm.SerializeToOstream(&ofs);
		ofs.close();
	}
	return rv;
}

bool LoadFromBinaryFile(const std::string& filename, google::protobuf::Message& pbm)
{
	bool rv = false;

	std::ifstream ifs(filename.data());
	if(ifs.is_open())
	{
		rv = pbm.ParseFromIstream(&ifs);
		ifs.close();

		if(rv != true)
			LOGERROR("ParseFromStream failed, filename is [{}]", filename);
	}
	else
	{
		LOGERROR("Open file failed, filename is [{}]", filename);
	}
	return rv;
}

bool FindFieldInMessage(const std::string& field_name, google::protobuf::Message*& pThisRow, const google::protobuf::FieldDescriptor*& pFieldDesc)
{
	__ENTER_FUNCTION
	auto vecName = split_string(field_name, ".");
	while(vecName.empty() == false)
	{
		pFieldDesc = pThisRow->GetDescriptor()->FindFieldByName(vecName.front());
		if(pFieldDesc == nullptr)
		{
			// printf("process {} fail\n", name.c_str());
			return false;
		}
		vecName.erase(vecName.begin());
		if(vecName.empty() == false && pFieldDesc->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			pThisRow = pThisRow->GetReflection()->MutableMessage(pThisRow, pFieldDesc);
		}
	}
	if(pFieldDesc == nullptr)
	{
		return false;
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

void AddFieldData(google::protobuf::Message* pThisRow, const google::protobuf::FieldDescriptor* pFieldDesc, const std::string& data)
{
	using namespace google::protobuf;
	switch(pFieldDesc->type())
	{
		case FieldDescriptor::TYPE_DOUBLE:
		{
			pThisRow->GetReflection()->AddDouble(pThisRow, pFieldDesc, stod(data));
		}
		break;
		case FieldDescriptor::TYPE_FLOAT:
		{
			pThisRow->GetReflection()->AddFloat(pThisRow, pFieldDesc, stof(data));
		}
		break;
		case FieldDescriptor::TYPE_INT32:
		case FieldDescriptor::TYPE_SINT32:
		case FieldDescriptor::TYPE_SFIXED32:
		{
			pThisRow->GetReflection()->AddInt32(pThisRow, pFieldDesc, stol(data));
		}
		break;
		case FieldDescriptor::TYPE_UINT32:
		case FieldDescriptor::TYPE_FIXED32:
		{
			if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
				pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data, 0, 16));
			else
				pThisRow->GetReflection()->AddUInt32(pThisRow, pFieldDesc, stoul(data));
		}
		break;
		case FieldDescriptor::TYPE_INT64:
		case FieldDescriptor::TYPE_SINT64:
		case FieldDescriptor::TYPE_SFIXED64:
		{
			pThisRow->GetReflection()->AddInt64(pThisRow, pFieldDesc, stoll(data));
		}
		break;
		case FieldDescriptor::TYPE_UINT64:
		case FieldDescriptor::TYPE_FIXED64:
		{
			if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
				pThisRow->GetReflection()->AddUInt32(pThisRow, pFieldDesc, stoull(data, 0, 16));
			else
				pThisRow->GetReflection()->AddUInt64(pThisRow, pFieldDesc, stoull(data));
		}
		break;
		case FieldDescriptor::TYPE_BOOL:
		{
			pThisRow->GetReflection()->AddBool(pThisRow, pFieldDesc, (data == "true" || data == "1"));
		}
		break;
		case FieldDescriptor::TYPE_STRING:
		case FieldDescriptor::TYPE_BYTES:
		{
			pThisRow->GetReflection()->AddString(pThisRow, pFieldDesc, data);
		}
		break;
		default:
			break;
	}
}

bool SetMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data)
{
	using namespace google::protobuf;
	using namespace rapidjson;
	Message*			   pThisRow	  = pPBMessage;
	const FieldDescriptor* pFieldDesc = nullptr;
	if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
		return false;

	if(pFieldDesc->is_repeated())
	{
		Document document;
		if(document.Parse(data.c_str()).HasParseError())
		{
			auto vecData = split_string(data, ",");
			for(const std::string& str: vecData)
			{
				AddFieldData(pThisRow, pFieldDesc, str);
			}
		}
		else
		{
			if(document.IsArray() == false)
				return false;

			if(pFieldDesc->type() == FieldDescriptor::TYPE_MESSAGE)
			{
				for(SizeType i = 0; i < document.Size(); i++)
				{
					const auto& v			= document[i];
					auto		pSubMessage = pThisRow->GetReflection()->AddMessage(pThisRow, pFieldDesc, nullptr);
					for(auto it = v.MemberBegin(); it != v.MemberEnd(); it++)
					{
						SetMessageData(pSubMessage, it->name.GetString(), it->value.GetString());
					}
				}
			}
			else
			{
				for(SizeType i = 0; i < document.Size(); i++)
				{
					const auto& v = document[i];
					AddFieldData(pThisRow, pFieldDesc, v.GetString());
				}
			}
		}

		return true;
	}

	switch(pFieldDesc->type())
	{
		case FieldDescriptor::TYPE_ENUM:
		{
			try
			{
				int val = stol(data);
				pThisRow->GetReflection()->SetEnumValue(pThisRow, pFieldDesc, val);
			}
			catch(...)
			{
				auto pEnumVal = pFieldDesc->enum_type()->FindValueByName(data);
				if(pEnumVal == nullptr)
				{
					fmt::printf("can't convert {} to {}\n", data, field_name);
					return false;
				}
				pThisRow->GetReflection()->SetEnum(pThisRow, pFieldDesc, pEnumVal);
			}
		}
		break;
		case FieldDescriptor::TYPE_DOUBLE:
		{
			pThisRow->GetReflection()->SetDouble(pThisRow, pFieldDesc, stod(data));
		}
		break;
		case FieldDescriptor::TYPE_FLOAT:
		{
			pThisRow->GetReflection()->SetFloat(pThisRow, pFieldDesc, stof(data));
		}
		break;
		case FieldDescriptor::TYPE_INT32:
		case FieldDescriptor::TYPE_SINT32:
		case FieldDescriptor::TYPE_SFIXED32:
		{
			pThisRow->GetReflection()->SetInt32(pThisRow, pFieldDesc, stol(data));
		}
		break;
		case FieldDescriptor::TYPE_UINT32:
		case FieldDescriptor::TYPE_FIXED32:
		{
			if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
				pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data, 0, 16));
			else
				pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoul(data));
		}
		break;
		case FieldDescriptor::TYPE_INT64:
		case FieldDescriptor::TYPE_SINT64:
		case FieldDescriptor::TYPE_SFIXED64:
		{
			pThisRow->GetReflection()->SetInt64(pThisRow, pFieldDesc, stoll(data));
		}
		break;
		case FieldDescriptor::TYPE_UINT64:
		case FieldDescriptor::TYPE_FIXED64:
		{
			if(data.substr(0, 2) == "0x" || data.substr(0, 2) == "0X")
				pThisRow->GetReflection()->SetUInt32(pThisRow, pFieldDesc, stoull(data, 0, 16));
			else
				pThisRow->GetReflection()->SetUInt64(pThisRow, pFieldDesc, stoull(data));
		}
		break;
		case FieldDescriptor::TYPE_BOOL:
		{
			bool bVal = (data == "true" || data == "TRUE" || data == "1");
			pThisRow->GetReflection()->SetBool(pThisRow, pFieldDesc, bVal);
		}
		break;
		case FieldDescriptor::TYPE_STRING:
		case FieldDescriptor::TYPE_BYTES:
		{
			pThisRow->GetReflection()->SetString(pThisRow, pFieldDesc, data);
		}
		break;
		default:
			break;
	}

	return true;
}

bool AddMessageData(google::protobuf::Message* pPBMessage, const std::string& field_name, const std::string& data)
{
	using namespace google::protobuf;
	Message*			   pThisRow	  = pPBMessage;
	const FieldDescriptor* pFieldDesc = nullptr;
	if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
		return false;

	AddFieldData(pThisRow, pFieldDesc, data);

	return true;
}

google::protobuf::Message* AddMessageSubMessage(google::protobuf::Message* pPBMessage, const std::string& field_name)
{
	using namespace google::protobuf;
	Message*			   pThisRow	  = pPBMessage;
	const FieldDescriptor* pFieldDesc = nullptr;
	if(FindFieldInMessage(field_name, pThisRow, pFieldDesc) == false)
		return nullptr;

	return pThisRow->GetReflection()->AddMessage(pThisRow, pFieldDesc);
}

google::protobuf::Message* NewProtoMessage(const std::string& MsgType)
{
	using namespace google::protobuf;
	Message*		  pMessage = nullptr;
	const Descriptor* pDesc	   = DescriptorPool::generated_pool()->FindMessageTypeByName(MsgType);
	if(pDesc)
	{
		const Message* pPrototype = MessageFactory::generated_factory()->GetPrototype(pDesc);
		if(pPrototype)
		{
			pMessage = pPrototype->New();
		}
	}
	return pMessage;
}

void DelProtoMessage(google::protobuf::Message* pMessage)
{
	if(pMessage)
	{
		pMessage->Clear();
		delete pMessage;
	}
}

} // namespace pb_util