
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

void ProtobufLogHandler(google::protobuf::LogLevel level, const char* file, int32_t line, const std::string& msg)
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

    if(opt.has("--excel") == false || opt.has("--inputpbdir") == false || opt.has("--inputpb") == false ||
       opt.has("--out") == false || opt.has("--help") == true)
    {
        std::cout << "execl2pb [--excel=xxx.xlsx] [--inputpbdir=input.pb] [--inputpb=input.pb] [--out=output.txt]"
                  << std::endl;
        return 0;
    }

    std::string execl_full_path = opt["--excel"];
    std::string execl_file_name = GetFileNameFromFullPath(execl_full_path);
    std::string execl_name      = GetFileNameWithoutExt(execl_file_name);
    std::string pb_file_name    = opt["--inputpb"];
    std::string pb_dir_name     = opt["--inputpbdir"];
    std::string out_file_name   = opt["--out"];

    using namespace google::protobuf;
    using namespace google::protobuf::compiler;

    DiskSourceTree sourceTree;
    sourceTree.MapPath("", pb_dir_name);
    Importer              importer(&sourceTree, nullptr);
    const FileDescriptor* fdes = importer.Import(pb_file_name);
    if(fdes == nullptr)
    {
        fmt::print("importer fail");
        return -1;
    }

    const Descriptor* desc = importer.pool()->FindMessageTypeByName(execl_name);
    if(desc == nullptr)
    {
        fmt::print("find cfgname fail");
        return -1;
    }

    DynamicMessageFactory factory;
    const Message*        message_const   = factory.GetPrototype(desc);
    Message*              pData           = message_const->New();
    auto                  pPBRowFieldDesc = pData->GetDescriptor()->FindFieldByNumber(1);
    auto                  sub_msg_desc    = pPBRowFieldDesc->message_type();
    if(sub_msg_desc == nullptr)
    {
        fmt::print("{} field 1 is not a message", execl_name);
        return -1;
    }

    std::string    debug_txt;
    xlnt::workbook wb;
    wb.load(execl_full_path);
    for(const auto& ws: wb)
    {
        auto        rows      = ws.rows(false);
        const auto& row_name  = rows.vector(0);
        const auto& row_proto = rows.vector(0);

        if(opt.has("--showexecl"))
        {
            int32_t x = 0, y = 0;
            for(auto row: rows)
            {

                for(auto cell: row)
                {
                    fmt::print("[{},{}]{} \t", y + 1, x + 1, cell.to_string().c_str());
                    x++;
                }
                y++;
                x = 0;
                std::cout << std::endl;
            }
            return 0;
        }

        const Message* row_message_const = factory.GetPrototype(sub_msg_desc);
        size_t         x                 = 0;
        size_t         y                 = 0;
        for(auto row: rows)
        {
            if(y < 2)
            {
                y++;
                continue;
            }
            Message* pPBRow  = row_message_const->New();
            bool     bUpdate = false;
            for(auto cell: row)
            {
                if(x > row_name.length())
                    continue;
                const auto& cell_name = row_name[x];
                std::string name      = cell_name.to_string();
                if(name.empty() || name[0] == '#')
                {
                    x++;
                    continue;
                }

                std::string data = cell.to_string();
                if(data.empty())
                {
                    x++;
                    continue;
                }

                try
                {
                    if(pb_util::SetMessageData(pPBRow, name, data) == false)
                    {
                        fmt::print("process fail: sheet={} y={} x={} cell:{} data:{} \n",
                                   ws.title(),
                                   y + 1,
                                   x + 1,
                                   name,
                                   data);
                    }
                }
                catch(...)
                {
                    fmt::print("process fail: sheet={} y={} x={} cell:{} data:{} \n",
                               ws.title(),
                               y + 1,
                               x + 1,
                               name,
                               data);
                }

                bUpdate = true;
                x++;
            }

            if(bUpdate)
            {
                pData->GetReflection()->AddAllocatedMessage(pData, pPBRowFieldDesc, pPBRow);
                std::string json_txt;
                pb_util::SaveToJsonTxt(*pPBRow, json_txt);

                if(debug_txt.empty() == false)
                    debug_txt += ",\n";
                debug_txt += std::string_view{json_txt.c_str(), json_txt.size() - 2};
                debug_txt +=
                    fmt::format(",\"__debug\":\"file:{} sheet:{} line:{}  \"\n}} ", execl_file_name, ws.title(), y + 1);
            }
            else
            {
                SAFE_DELETE(pPBRow);
            }

            x = 0;
            y++;
        }
    }

    if(opt.has("--debug"))
    {
        std::string   debug_file_name = out_file_name + ".debug.json";
        std::ofstream ofs(debug_file_name.c_str(), std::ios::out | std::ios::trunc);
        if(ofs.is_open())
        {
            ofs << "[";
            ofs << debug_txt;
            ofs << "]";
            ofs.close();
        }
    }

    pb_util::SaveToBinaryFile(*pData, out_file_name.c_str());
    SAFE_DELETE(pData);

    fmt::printf("write to file succ.\n");

    return 0;
}