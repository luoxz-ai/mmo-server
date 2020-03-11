
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "fmt/format.h"
#include "fmt/printf.h"

//////////////////////////////////////////////////////////////////////
std::string ReplaceStr(std::string& strSource, const std::string& strRepl, const std::string& strNew)
{
	std::string::size_type pos = 0;
	while((pos = strSource.find(strRepl, pos)) != std::string::npos)
	{
		strSource.replace(pos, strRepl.length(), strNew);
		pos += strNew.length();
	}
	return strSource;
}

inline std::vector<std::string> split_string(const std::string& str, const std::string& delimiters)
{
	std::vector<std::string> v;
	std::string::size_type	 start = 0;
	auto					 pos   = str.find_first_of(delimiters, start);
	while(pos != std::string::npos)
	{
		if(pos != start) // ignore empty tokens
			v.emplace_back(str, start, pos - start);
		start = pos + 1;
		pos	  = str.find_first_of(delimiters, start);
	}
	if(start < str.length())							  // ignore trailing delimiter
		v.emplace_back(str, start, str.length() - start); // add what's left of the string
	return v;
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		std::cout << "ddl2cpp [in_file_name] [out_dir] [out_file_name]" << std::endl;
		return 0;
	}

	std::string in_file_name{argv[1]};
	std::string out_dir{argv[2]};
	std::string out_file_name{argv[3]};

	std::ifstream input_file(in_file_name);
	if(input_file.is_open() == false)
	{
		std::cout << "gamedb.sql open fail" << std::endl;
		return -1;
	}
	std::string input_string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

	//将输入按;进行分割
	std::string output_header;
	std::string output_cpp;
	auto		vecString = split_string(input_string, ";");
	for(auto& v: vecString)
	{
		std::cout << "string1:" << v << std::endl;

		std::string regextxt = R"(CREATE TABLE `(.*)` \(([\s\S]*)\) ENGINE=InnoDB (.*))";
		std::smatch base_match;
		if(std::regex_search(v, base_match, std::regex(regextxt)))
		{
			std::string table_name = base_match[1];
			std::string content	   = base_match[2];
			auto		vecStr	   = split_string(content, "\n");

			struct field_type_t
			{
				std::string field_name;
				std::string field_type;
				std::string field_comment;
			};
			std::vector<field_type_t>		vecFieldType;
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
					std::string field_name	  = field_match[1];
					std::string field_type	  = field_match[2];
					std::string field_comment = field_match[3];
					vecFieldType.push_back({field_name, field_type, field_comment});
				}
				else if(std::regex_search(v, field_match, std::regex{R"(.*`(.*)` (.*) COMMENT ('.*').*)"}))
				{
					std::string field_name	  = field_match[1];
					std::string field_type	  = field_match[2];
					std::string field_comment = field_match[3];
					vecFieldType.push_back({field_name, field_type, field_comment});
				}
			}

			std::string fields_name;
			std::string fields_enum_list;
			std::string fields_type;
			std::string fields_type_enum;
			std::string is_pri_key_list;

			for(unsigned int i = 0; i < vecFieldType.size(); i++)
			{
				const auto& v = vecFieldType[i];
				std::smatch field_match;
				if(v.field_name.empty() == false)
				{
					if(fields_name.empty() == false)
						fields_name += ",";
					if(fields_enum_list.empty() == false)
						fields_enum_list += "\t\t";
					if(fields_type.empty() == false)
						fields_type += ",";
					if(fields_type_enum.empty() == false)
						fields_type_enum += ",";
					if(is_pri_key_list.empty() == false)
						is_pri_key_list += ",";

					if(PriKeys.find(v.field_name) != PriKeys.end())
						is_pri_key_list += "true";
					else
						is_pri_key_list += "false";

					fields_name += "\"" + v.field_name + "\"";

					std::string field_name_UP = v.field_name;
					std::transform(field_name_UP.begin(), field_name_UP.end(), field_name_UP.begin(), ::toupper);
					fields_enum_list += field_name_UP + ",//" + v.field_comment + "\n";

					if(std::regex_search(v.field_type, field_match, std::regex{R"((.*)\((.*)\)(.*))"}))
					{
						std::string field_type = field_match[1];
						std::string field_bits = field_match[2];
						bool		bUnsigned  = field_match.size() > 2 && field_match[3] == " unsigned";
						std::string field_type_cpp;
						std::string field_type_enum;
						if(field_type == "bigint")
						{
							if(bUnsigned)
							{
								field_type_cpp	= "uint64_t ";
								field_type_enum = "DB_FIELD_TYPE_LONGLONG_UNSIGNED";
							}
							else
							{
								field_type_cpp	= "int64_t ";
								field_type_enum = "DB_FIELD_TYPE_LONGLONG";
							}
						}
						else if(field_type == "int")
						{
							if(bUnsigned)
							{
								field_type_cpp	= "uint32_t ";
								field_type_enum = "DB_FIELD_TYPE_LONG_UNSIGNED";
							}
							else
							{
								field_type_cpp	= "int32_t ";
								field_type_enum = "DB_FIELD_TYPE_LONG";
							}
						}
						else if(field_type == "smallint")
						{
							if(bUnsigned)
							{
								field_type_cpp	= "uint16_t ";
								field_type_enum = "DB_FIELD_TYPE_SHORT_UNSIGNED";
							}
							else
							{
								field_type_cpp	= "int16_t ";
								field_type_enum = "DB_FIELD_TYPE_SHORT";
							}
						}
						else if(field_type == "tinyint")
						{
							if(bUnsigned)
							{
								field_type_cpp	= "uint8_t ";
								field_type_enum = "DB_FIELD_TYPE_TINY_UNSIGNED";
							}
							else
							{
								field_type_cpp	= "int8_t ";
								field_type_enum = "DB_FIELD_TYPE_TINY";
							}
						}
						else if(field_type == "float")
						{
							field_type_cpp	= "float ";
							field_type_enum = "DB_FIELD_TYPE_FLOAT";
						}
						else if(field_type == "double")
						{
							field_type_cpp	= "double ";
							field_type_enum = "DB_FIELD_TYPE_DOUBLE";
						}
						else if(field_type == "varchar")
						{
							if(field_bits.empty())
							{
								field_type_cpp	= "std::string ";
								field_type_enum = "DB_FIELD_TYPE_VARCHAR";
							}
							else
							{
								field_type_cpp	= "char[" + field_bits + "] ";
								field_type_enum = "DB_FIELD_TYPE_VARCHAR";
							}
						}
						else if(field_type == "blob")
						{
							field_type_cpp	= "std::string ";
							field_type_enum = "DB_FIELD_TYPE_BLOB";
						}

						fields_type += field_type_cpp;
						fields_type_enum += field_type_enum;
					}
				}
			}

			std::string output_format = R"(
struct {0}
{{
	static constexpr const char* table_name = "{1}";
	enum FIELD_ENUMS
	{{
		{2}
	}};
	static constexpr const char* field_name[] = {{ {3} }};
	using field_type_t = type_list<{4}>;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = {{ {5} }};
	static constexpr bool pri_key_idx[] = {{ {6} }};
}};

		)";

			char		szBuf[4096]	  = {};
			std::string table_name_UP = table_name;
			std::transform(table_name_UP.begin(), table_name_UP.end(), table_name_UP.begin(), ::toupper);
			fmt::format_to(szBuf, output_format.c_str(), table_name_UP.c_str(), table_name.c_str(), fields_enum_list.c_str(), fields_name.c_str(), fields_type.c_str(),
						   fields_type_enum.c_str(), is_pri_key_list.c_str());

			output_header += szBuf;
			fmt::printf("%s", szBuf);

			{
				std::string output_format = R"(
constexpr const char* {}::table_name;
constexpr const char* {}::field_name[];
constexpr DB_FIELD_TYPES {}::field_type_enum_list[];
constexpr bool {}::pri_key_idx[];
		)";

				char		szBuf[4096]	  = {};
				std::string table_name_UP = table_name;
				std::transform(table_name_UP.begin(), table_name_UP.end(), table_name_UP.begin(), ::toupper);
				fmt::format_to(szBuf, output_format.c_str(), table_name_UP.c_str(), table_name_UP.c_str(), table_name_UP.c_str(), table_name_UP.c_str());

				output_cpp += szBuf;
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
	}
	{
		std::ofstream output_file(out_dir + out_file_name + ".cpp");
		output_file << "#include \"DBField.h\"\n";
		output_file << "#include \"" + out_file_name + ".h\"\n";
		output_file << output_cpp;
		output_file.close();
	}
}