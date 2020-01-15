
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>

#include "BaseCode.h"
#include "ProtobuffUtil.h"
#include "StringAlgo.h"
#include "get_opt.h"
#include "xlnt/xlnt.hpp"

void ProtobufLogHandler(google::protobuf::LogLevel level, const char* file, int line, const std::string& msg)
{
	std::cerr << msg << std::endl;
}
struct PB_Initer
{
public:
	PB_Initer()
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;
		google::protobuf::SetLogHandler(ProtobufLogHandler);
	}
	~PB_Initer() {}
} const s_PB_Initer;

int main(int argc, char** argv)
{
	get_opt opt(argc, (const char**)argv);

	if(opt.has("--excel") == false || opt.has("--inputpbdir") == false || opt.has("--inputpb") == false || opt.has("--out") == false || opt.has("--help") == true)
	{
		std::cout << "execl2pb [--excel=xxx.xlsx] [--inputpbdir=input.pb] [--inputpb=input.pb] [--out=output.txt]" << std::endl;
		return 0;
	}

	std::string execl_full_path = opt["--excel"];
	std::string execl_file_name = GetFileNameFromFullPath(execl_full_path);
	std::string execl_name		= GetFileNameWithoutExt(execl_file_name);
	std::string pb_file_name	= opt["--inputpb"];
	std::string pb_dir_name		= opt["--inputpbdir"];
	std::string out_file_name	= opt["--out"];

	xlnt::workbook wb;
	wb.load(execl_full_path);
	auto		ws		  = wb.active_sheet();
	auto		rows	  = ws.rows(false);
	const auto& row_name  = rows.vector(0);
	const auto& row_proto = rows.vector(0);

	if(opt.has("--showexecl"))
	{
		int x = 0, y = 0;
		for(auto row: rows)
		{

			for(auto cell: row)
			{
				fmt::print("[{},{}]{} \t", x, y, cell.to_string().c_str());
				x++;
			}
			y++;
			x = 0;
			std::cout << std::endl;
		}
		return 0;
	}

	using namespace google::protobuf;
	using namespace google::protobuf::compiler;

	DiskSourceTree sourceTree;
	sourceTree.MapPath("", pb_dir_name);
	Importer			  importer(&sourceTree, nullptr);
	const FileDescriptor* fdes = importer.Import(pb_file_name);
	if(fdes == nullptr)
	{
		std::cerr << "importer fail";
		return -1;
	}

	const Descriptor* desc = importer.pool()->FindMessageTypeByName(execl_name);
	if(desc == nullptr)
	{
		std::cerr << "find cfgname fail";
		return -1;
	}

	DynamicMessageFactory factory;
	const Message*		  message_const	  = factory.GetPrototype(desc);
	Message*			  pData			  = message_const->New();
	auto				  pPBRowFieldDesc = pData->GetDescriptor()->FindFieldByNumber(1);
	auto				  sub_msg_desc	  = pPBRowFieldDesc->message_type();
	if(sub_msg_desc == nullptr)
	{
		std::cerr << execl_name << " field 1 is not a message";
		return -1;
	}
	const Message* row_message_const = factory.GetPrototype(sub_msg_desc);
	size_t		   x				 = 0;
	size_t		   y				 = 0;
	for(auto row: rows)
	{
		if(y < 3)
		{
			y++;
			continue;
		}
		Message* pPBRow	 = row_message_const->New();
		bool	 bUpdate = false;
		for(auto cell: row)
		{
			if(x > row_name.length())
				continue;
			const auto& cell_name = row_name[x];
			std::string name	  = cell_name.to_string();
			if(name.empty() || name[0] == '#')
				continue;

			std::string data = cell.to_string();
			if(data.empty())
				continue;
			pb_util::SetMessageData(pPBRow, name, data);
			bUpdate = true;
			x++;
		}
		if(bUpdate)
			pData->GetReflection()->AddAllocatedMessage(pData, pPBRowFieldDesc, pPBRow);
		else
			SAFE_DELETE(pPBRow);
		x = 0;
		y++;
	}

	if(opt.has("--display"))
		std::cout << pData->Utf8DebugString() << std::endl;

	std::ofstream ofs(out_file_name.data());
	if(ofs.is_open())
	{
		pData->SerializeToOstream(&ofs);
	}
	ofs.close();
	printf("write to file succ.\n");

	return 0;
}