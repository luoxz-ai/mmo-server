
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "StringAlgo.h"
#include "fmt/format.h"
#include "fmt/printf.h"
#include "get_opt.h"

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "ddl2cpp [in_file_name] [out_dir] [out_file_name]" << std::endl;
        return 0;
    }
    get_opt opt(argc, (const char**)argv);

    std::string in_file_name  = opt["--input"];
    std::string out_dir       = opt["--outdir"];
    std::string out_file_name = opt["--output"];
    bool        bDebug        = opt.has("--debug");

    std::ifstream input_file(in_file_name);
    if(input_file.is_open() == false)
    {
        std::cout << in_file_name << " open fail" << std::endl;
        return -1;
    }
    std::string input_string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    //将输入按;进行分割
    std::string output_header;
    std::string output_cpp;
    auto        vecString = split_string(input_string, ";");
    for(auto& v: vecString)
    {
        if(bDebug)
        {
            std::cout << "string1:" << v << std::endl;
        }

        std::string regextxt = R"(CREATE TABLE `(.*)` \(([\s\S]*)\) ENGINE=InnoDB (.*))";
        std::smatch base_match;
        if(std::regex_search(v, base_match, std::regex(regextxt)))
        {
            std::string table_name = base_match[1];
            std::string content    = base_match[2];
            auto        vecStr     = split_string(content, "\n");

            struct field_type_t
            {
                std::string field_name;
                std::string field_type;
                std::string field_comment;
            };
            std::vector<field_type_t>       vecFieldType;
            std::unordered_set<std::string> PriKeys;

            for(auto& v: vecStr)
            {
                std::smatch field_match;
                if(std::regex_search(v, field_match, std::regex{R"(.*PRIMARY KEY \((.*)\).*)"}))
                {
                    std::string prikey_str = field_match[1];
                    ReplaceStr(prikey_str, "`", "");
                    auto prikeys = split_string(prikey_str, ",");
                    for(auto v: prikeys)
                    {
                        PriKeys.insert(v);
                    }
                }
                else if(std::regex_search(v, field_match, std::regex{R"(.*`(.*)` (.*) NOT NULL.*COMMENT ('.*').*)"}))
                {
                    std::string field_name    = field_match[1];
                    std::string field_type    = field_match[2];
                    std::string field_comment = field_match[3];
                    vecFieldType.push_back({field_name, field_type, field_comment});
                }
                else if(std::regex_search(v, field_match, std::regex{R"(.*`(.*)` (.*) COMMENT ('.*').*)"}))
                {
                    std::string field_name    = field_match[1];
                    std::string field_type    = field_match[2];
                    std::string field_comment = field_match[3];
                    vecFieldType.push_back({field_name, field_type, field_comment});
                }
            }

            
            std::string fields_enum_list;
            std::vector<std::string> fields_tuple;
            std::vector<std::string> field_type_cpp_list;
            fields_tuple.resize(vecFieldType.size());

            for(uint32_t i = 0; i < vecFieldType.size(); i++)
            {
                const auto& v = vecFieldType[i];
                std::smatch field_match;
                if(v.field_name.empty() == false)
                {
                    std::string& field_tuple = fields_tuple[i];
                   
                    if(fields_enum_list.empty() == false)
                        fields_enum_list += "\t\t";
                    
                    field_tuple = "\"" + v.field_name + "\"";

                    std::string field_name_UP = v.field_name;
                    std::transform(field_name_UP.begin(), field_name_UP.end(), field_name_UP.begin(), ::toupper);
                    fields_enum_list += field_name_UP + ",//" + v.field_comment + "\n";

                    if(std::regex_search(v.field_type, field_match, std::regex{R"((.*)\((.*)\)(.*))"}))
                    {
                        std::string field_type = field_match[1];
                        std::string field_bits = field_match[2];
                        bool        bUnsigned =
                            field_match.size() > 2 && lower_cast_copy(trim_copy(field_match[3])) == "unsigned";
                        std::string field_type_cpp;
                        std::string field_type_enum;
                        if(field_type == "bigint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint64_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONGLONG_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int64_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONGLONG";
                            }
                        }
                        else if(field_type == "int")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint32_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONG_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int32_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONG";
                            }
                        }
                        else if(field_type == "smallint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint16_t ";
                                field_type_enum = "DB_FIELD_TYPE_SHORT_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int16_t ";
                                field_type_enum = "DB_FIELD_TYPE_SHORT";
                            }
                        }
                        else if(field_type == "tinyint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint8_t ";
                                field_type_enum = "DB_FIELD_TYPE_TINY_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int8_t ";
                                field_type_enum = "DB_FIELD_TYPE_TINY";
                            }
                        }
                        else if(field_type == "float")
                        {
                            field_type_cpp  = "float ";
                            field_type_enum = "DB_FIELD_TYPE_FLOAT";
                        }
                        else if(field_type == "double")
                        {
                            field_type_cpp  = "double ";
                            field_type_enum = "DB_FIELD_TYPE_DOUBLE";
                        }
                        else if(field_type == "varchar")
                        {
                            if(field_bits.empty())
                            {
                                field_type_cpp  = "std::string ";
                                field_type_enum = "DB_FIELD_TYPE_VARCHAR";
                            }
                            else
                            {
                                field_type_cpp  = "char[" + field_bits + "] ";
                                field_type_enum = "DB_FIELD_TYPE_VARCHAR";
                            }
                        }
                        else if(field_type == "blob")
                        {
                            field_type_cpp  = "std::string ";
                            field_type_enum = "DB_FIELD_TYPE_BLOB";
                        }

                        field_type_cpp_list.push_back(field_type_cpp);
                        field_tuple += "," + field_type_enum;

                        if(PriKeys.find(v.field_name) != PriKeys.end())
                            field_tuple += ",true";
                        else
                            field_tuple += ",false";
                    }
                }
            }

            std::string output_format = R"(
struct {0}
{{
	static constexpr const char* table_name() {{ return "{1}";}} 
	enum FIELD_ENUMS
	{{
		{2}
	}};

	static constexpr auto field_info()
    {{ 
        return std::make_tuple({3});
    }}

	using field_type_t = type_list<{4}>;

    static constexpr size_t field_count() {{return {5};}}
}};
    

		)";

            
            std::string table_name_UP = table_name;
            std::transform(table_name_UP.begin(), table_name_UP.end(), table_name_UP.begin(), ::toupper);
            std::string field_tuple_str = string_concat(fields_tuple, ",", "std::make_tuple(", ")");
            std::string field_types_str = string_concat(field_type_cpp_list, ",", "", "");
            std::string szBuf = fmt::format(
                           output_format,
                           table_name_UP,
                           table_name,
                           fields_enum_list,
                           field_tuple_str,
                           field_types_str,
                           field_type_cpp_list.size()
                           );
            
            output_header += szBuf;
            if(bDebug)
            {
                fmt::printf("%s", szBuf);
            }

            
        }
    }

    {
        std::ofstream output_file(out_dir + out_file_name + ".h");
        output_file << "#pragma once\n";
        output_file << "#include <string>\n";
        output_file << "#include \"BaseCode.h\"\n";
        output_file << "#include \"DBField.h\"\n";
        output_file << output_header;
        output_file.close();
        if(opt.has("--format"))
        {
            system(fmt::format("{} -i {}", opt["--format"], out_dir + out_file_name + ".h").c_str());
        }
    }
    
   
}