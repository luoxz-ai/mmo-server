#ifndef SQL_CODE_GENERATOR_H
#define SQL_CODE_GENERATOR_H 1

#include <map>
#include <string>

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>

typedef std::vector<const google::protobuf::Descriptor*> messagetypes;

class SQLCodeGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
    SQLCodeGenerator(const std::string& name);
    virtual ~SQLCodeGenerator();
    virtual bool Generate(const google::protobuf::FileDescriptor*       file,
                          const std::string&                            parameter,
                          google::protobuf::compiler::GeneratorContext* context,
                          std::string*                                  error) const;
};

#endif