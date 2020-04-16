#ifndef DBROW2PROTO_H
#define DBROW2PROTO_H

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "DBField.h"
#include "DBRecord.h"

struct DBRow2Proto
{
    bool Convert(const CDBRecord& row, google::protobuf::Messgage& msg)
    {
        using namespace google::protobuf;
        const Descriptor* msg_desc = msg.GetDescriptor();
        const Reflection* reflect = msg->GetReflection();
        for(int i = 0; i < row->GetFieldCount(); i++)
        {
            const auto& field_ptr = row->Field(i);
            const auto& field_name = field_ptr->GetFieldName();
            const FieldDescriptor* field_desc = msg_desc->FindFieldByName(field_name);
            if(field_desc == nullptr)
                continue;

            switch(field_desc->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    reflect->SetDouble(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    reflect->SetFloat(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_INT64:
                    reflect->SetInt64(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_UINT64:
                    reflect->SetUInt64(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_INT32:
                    reflect->SetInt32(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_UINT32:
                    reflect->SetUInt32(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_STRING:
                    reflect->SetString(&msg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_BYTES:
                    reflect->SetString(&msg, field_desc, field_ptr);
                    break;
                default:
                    break;
            }
        }

        return;
    }
};

#endif /* DBROW2PROTO_H */
