
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "ProtobuffParse.h"
#include "ProtobuffUtil.h"
#include "StringAlgo.h"
#include "get_opt.h"

int main(int argc, char** argv)
{
    get_opt opt(argc, (const char**)argv);
    if(opt.has("--input") == false || opt.has("--pbdir") == false || opt.has("--pb") == false ||
       opt.has("--help") == true)
    {
        std::cout << "pbbin2txt [--input=xxx.bytes] [--pbdir=xxxxx] [--pb=xxx.proto] [--output=output.txt]"
                  << std::endl;
        return 0;
    }

    std::string in_file_name = opt["--input"];
    std::string pbdirname    = opt["--pbdir"];
    std::string pbname       = opt["--pb"];
    std::string cfgname      = GetFileNameWithoutExt(GetFileNameFromFullPath(pbname));
    if(opt.has("--cfg"))
        cfgname = opt["--cfg"];

    ProtoBufParser parser;
    parser.Init(pbdirname);
    if(parser.ParsePBFile(pbname) == false)
    {
        std::cerr << "importer fail";
        return -1;
    }

    const google::protobuf::Descriptor* desc = parser.FindDescByName(cfgname);
    if(desc == nullptr)
    {
        std::cerr << "find cfgname fail";
        return -1;
    }

    std::cout << desc->DebugString() << std::endl;
    std::cout << "prass any key to start convert" << std::endl;
    if(opt.has("-i"))
    {
        getchar();
    }

    auto pData = parser.NewMessage(cfgname);

    pb_util::LoadFromBinaryFile(in_file_name, *pData);
    if(opt.has("--output"))
    {
        pb_util::SaveToJsonFile(*pData, opt["--output"]);
    }
    else
    {
        std::string json_txt;
        pb_util::SaveToJsonTxt(*pData, json_txt);
        std::cout << json_txt << std::endl;
    }
}