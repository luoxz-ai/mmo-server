#include "sql_code_generator.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/stubs/scoped_ptr.h>
#include <google/protobuf/stubs/stl_util.h>
#include <google/protobuf/stubs/strutil.h>

#include "sql_options/sql_options.pb.h"
#include "fmt/format.h"


std::string StripProto(const std::string& filename) 
{
	using namespace google::protobuf;
  if (HasSuffixString(filename, ".protodevel")) {
    return StripSuffixString(filename, ".protodevel");
  } else {
    return StripSuffixString(filename, ".proto");
  }
}

inline std::vector<std::string> split_string(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> v;
    std::string::size_type   start = 0;
    auto                     pos   = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
            v.emplace_back(str, start, pos - start);
        start = pos + 1;
        pos   = str.find_first_of(delimiters, start);
    }
    if(start < str.length())                              // ignore trailing delimiter
        v.emplace_back(str, start, str.length() - start); // add what's left of the string
    return v;
}

std::string string_concat(std::vector<std::string> vecStr, const std::string& delimiters,  const std::string& pre,  const std::string& post)
{
	std::string result;
	int i = 0;
	for(const auto& v : vecStr)
	{
		if(result.empty() == false)
			result += delimiters;

		result += pre+v+post;
	}
	return result;
}

SQLCodeGenerator::SQLCodeGenerator(const std::string& name)
{
    sqltypes["int32"]   = "int(11)";
    sqltypes["int64"]   = "bigint(20)";
    sqltypes["uint32"]  = "int(11)";
    sqltypes["uint64"]  = "bigint(20)";
    sqltypes["double"]  = "double";
    sqltypes["float"]   = "float";
    sqltypes["bool"]    = "tinyint(1)";
    sqltypes["enum"]    = "enum";
    sqltypes["string"]  = "text";
    sqltypes["message"] = "";
}

SQLCodeGenerator::~SQLCodeGenerator() {}

std::string getSuffix(const google::protobuf::FieldDescriptor* fd)
{
    std::stringstream ss;
    if(fd->is_required())
        ss << " NOT NULL ";
    if(fd->has_default_value())
    {
        ss << " DEFAULT ";
        switch(fd->cpp_type())
        {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                ss << fd->default_value_int32();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                ss << fd->default_value_int64();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                ss << fd->default_value_uint32();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                ss << fd->default_value_uint64();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                ss << fd->default_value_double();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                ss << fd->default_value_float();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                ss << fd->default_value_bool();
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            {
                ss << "'" << fd->default_value_enum()->number() << "'";
            }
            break;
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                ss << "'" << fd->default_value_string() << "'";
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                ss << "0";
                break;
            default:
                ss << "0";
        }
    }
    return ss.str();
}

const char* PrimitiveTypeName(google::protobuf::FieldDescriptor::Type type)
{
    using namespace google::protobuf;
    switch(type)
    {
        case FieldDescriptor::TYPE_DOUBLE:
            return "double";
        case FieldDescriptor::TYPE_FLOAT:
            return "float";
        case FieldDescriptor::TYPE_INT64:
            return "bigint";
        case FieldDescriptor::TYPE_UINT64:
            return "bigint";
        case FieldDescriptor::TYPE_INT32:
            return "int";
        case FieldDescriptor::TYPE_FIXED64:
            return "bigint";
        case FieldDescriptor::TYPE_FIXED32:
            return "int";
        case FieldDescriptor::TYPE_BOOL:
            return "tinyint";
        case FieldDescriptor::TYPE_STRING:
            return "varchar";
        case FieldDescriptor::TYPE_GROUP:
            return NULL;
        case FieldDescriptor::TYPE_MESSAGE:
            return NULL;
        case FieldDescriptor::TYPE_BYTES:
            return "blob";
        case FieldDescriptor::TYPE_UINT32:
            return "int";
        case FieldDescriptor::TYPE_ENUM:
            return NULL;
        case FieldDescriptor::TYPE_SFIXED32:
            return "int";
        case FieldDescriptor::TYPE_SFIXED64:
            return "bigint";
        case FieldDescriptor::TYPE_SINT32:
            return "int";
        case FieldDescriptor::TYPE_SINT64:
            return "bigint";

            // No default because we want the compiler to complain if any new
            // CppTypes are added.
    }

    GOOGLE_LOG(FATAL) << "Can't get here.";
    return NULL;
}


std::string FieldUnsigned(google::protobuf::FieldDescriptor::Type type)
{
    using namespace google::protobuf;
    switch(type)
    {

        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_UINT32:
            return "UNSIGNED ";
			break;
		default:
            return "";
			break;
    }

     return "";
}

std::string FieldName(const google::protobuf::FieldDescriptor* field)
{
    std::string result = field->lowercase_name();
    return result;
}

std::string FieldSize(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    if(extension.size() > 0)
	{
		return fmt::format("({})", extension.size());
	}
	else
	{
		using namespace google::protobuf;
		switch(field->type())
		{
			case FieldDescriptor::TYPE_DOUBLE:
				return "(24,6)";
			case FieldDescriptor::TYPE_FLOAT:
				return "(24,6)";
			case FieldDescriptor::TYPE_FIXED64:
			case FieldDescriptor::TYPE_SFIXED64:
			case FieldDescriptor::TYPE_INT64:
			case FieldDescriptor::TYPE_UINT64:
			case FieldDescriptor::TYPE_SINT64:
				return "(20)";
			case FieldDescriptor::TYPE_FIXED32:
			case FieldDescriptor::TYPE_SFIXED32:
			case FieldDescriptor::TYPE_INT32:
			case FieldDescriptor::TYPE_UINT32:
			case FieldDescriptor::TYPE_SINT32:
			case FieldDescriptor::TYPE_BOOL:
			case FieldDescriptor::TYPE_ENUM:
				return "(11)";
			default:
				return "";
		}
	}
	
	return "";
}

std::string FieldUTF8(google::protobuf::FieldDescriptor::Type type)
{
	if(type == google::protobuf::FieldDescriptor::TYPE_STRING)
	{
		return "CHARACTER SET utf8";
	}
    return "";
}

std::string AutoIncrement(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    if(extension.auto_increment())
	{
		return "AUTO_INCREMENT ";
	}
	return "";
}

std::string PrimaryKey(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    return extension.primary_key();
}

std::string Key(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    return extension.keys();
}

std::string Unique(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    return extension.unique();
}

std::string DefaultVal(const google::protobuf::FieldDescriptor* field)
{
    auto options   = field->options();
    auto extension = options.GetExtension(sql);
    return extension.default_val();
}

std::string Comment(const google::protobuf::FieldDescriptor* field)
{
	google::protobuf::SourceLocation out_location;
    field->GetSourceLocation(&out_location);
	std::string comments = out_location.trailing_comments;
	google::protobuf::ReplaceCharacters(&comments, "\n", ' ');
	google::protobuf::ReplaceCharacters(&comments, "\r", ' ');
	google::protobuf::StripWhitespace(&comments);
	return comments;
}


void PrintMessage(const google::protobuf::Descriptor& message_descriptor, google::protobuf::io::Printer& printer)
{
    printer.Print("DROP TABLE IF EXISTS `$name$`;\n", "name", message_descriptor.name());
    printer.Print("CREATE TABLE `$name$` (\n", "name", message_descriptor.name());
    

	std::vector<std::string> primary_key_list;
	std::unordered_map< std::string, std::vector<std::string> > keys_list;
	std::unordered_map< std::string, std::vector<std::string> > unique_list;

    for(int i = 0; i < message_descriptor.field_count(); ++i)
    {
        auto desc           = message_descriptor.field(i);

        auto type_str       = PrimitiveTypeName(desc->type());
        auto name_str       = FieldName(desc);
        auto unsigned_str   = FieldUnsigned(desc->type());
        auto utf8_str       = FieldUTF8(desc->type());
        auto size_str       = FieldSize(desc);
        auto default_str    = DefaultVal(desc);
        auto primary_key    = PrimaryKey(desc);
        auto key_str        = Key(desc);
        auto auto_increment = AutoIncrement(desc);
        auto unique_str     = Unique(desc);
        auto comment        = Comment(desc);

		if(primary_key.empty() == false)
        {
			primary_key_list.push_back(name_str);
		}

		auto vecKey = split_string(key_str, ",");
		for(auto k : vecKey)
		{
			google::protobuf::StripWhitespace(&k);
			keys_list[k].push_back(name_str);
		}
		auto vecUnique = split_string(unique_str, ",");
		for(auto k : vecUnique)
		{
			google::protobuf::StripWhitespace(&k);
			unique_list[k].push_back(name_str);
		}
		
        std::string str = fmt::format("  `{}` {}{} {}{} NOT NULL {}COMMENT '{}',\n",
                                      name_str,
                                      type_str,
									  size_str,
                                      unsigned_str,
									  utf8_str,
									  auto_increment,
									  comment);
		printer.PrintRaw(str);

    }
	
	std::vector<std::string> key_str_list;
	{
		std::string key_str = fmt::format("  PRIMARY KEY ({})", string_concat(primary_key_list, ",", "`", "`"));
		key_str_list.emplace_back(std::move(key_str));
	}
	for(const auto& [k,vecList] : keys_list)
	{
		std::string key_str = fmt::format("  KEY `{}`({})", k, string_concat(vecList, ",", "`", "`"));
		key_str_list.emplace_back(std::move(key_str));
	}
	for(const auto& [k,vecList] : unique_list)
	{
		std::string key_str = fmt::format("  UNIQUE KEY `{}`({})", k, string_concat(vecList, ",", "`", "`"));
		key_str_list.emplace_back(std::move(key_str));
	}
	std::string keys_str = string_concat(key_str_list, ",\n", "", "");
	printer.PrintRaw(keys_str);
	

    printer.Print("\n) ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n");
}


bool SQLCodeGenerator::Generate(const google::protobuf::FileDescriptor*       file,
                                const std::string&                            parameter,
                                google::protobuf::compiler::GeneratorContext* context,
                                std::string*                                  error) const
{
    using namespace google::protobuf;
	
    std::string basename = StripProto(file->name());
    basename.append(".pb.sql");

	std::string file_descriptor_serialized_;
    FileDescriptorProto file_proto;
    file->CopyTo(&file_proto);
    file_proto.SerializeToString(&file_descriptor_serialized_);

    scoped_ptr<io::ZeroCopyOutputStream> output(context->Open(basename));
    GOOGLE_CHECK(output.get());
    google::protobuf::io::Printer printer(output.get(), '$');

    for(int i = 0; i < file->message_type_count(); ++i)
    {
        PrintMessage(*file->message_type(i),  printer);
        printer.Print("\n");
    }

    return true;
}
