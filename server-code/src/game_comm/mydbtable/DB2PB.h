#ifndef DBORM_H
#define DBORM_H
#include <memory>
#include <vector>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "DBField.h"
#include "DBRecord.h"

class DB2PB
{
public:
    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    static std::unique_ptr<PROTO_T> QueryOne(DB_T* pDB, KEY_T key)
    {
        auto result_ptr = pDB->template QueryTLimit<TABLE_T, nKeyID>(key, 1);
        if(result_ptr)
        {
            auto pDBRecord = result_ptr->fetch_row(false);
            if(pDBRecord)
            {
                std::unique_ptr<PROTO_T> proto_obj{PROTO_T::default_instance().New()};
                DBField2PB(pDBRecord.get(), proto_obj.get());
                return proto_obj;
            }
        }

        return nullptr;
    }

    template<class TABLE_T, class PROTO_T, uint32_t nKeyID, class DB_T, class KEY_T>
    static std::vector<std::unique_ptr<PROTO_T> > QueryVector(DB_T* pDB, KEY_T key)
    {
        std::vector<std::unique_ptr<PROTO_T> > result;

        auto result_ptr = pDB->template QueryT<TABLE_T, nKeyID>(key);
        if(result_ptr)
        {
            for(int32_t i = 0; i < result_ptr->get_num_row(); i++)
            {
                auto pDBRecord = result_ptr->fetch_row(false);
                if(pDBRecord)
                {
                    std::unique_ptr<PROTO_T> proto_obj{PROTO_T::default_instance().New()};
                    DBField2PB(pDBRecord.get(), proto_obj.get());
                    result.emplace(std::move(proto_obj));
                }
            }
        }

        return result;
    }

public:
    template<class PROTO_T, class DBRecord_T>
    static void DBField2PB(DBRecord_T* pDBRecord, PROTO_T* pMsg)
    {
        using namespace google::protobuf;
        const Descriptor* msg_desc = PROTO_T::GetDescriptor();
        const Reflection* reflect  = PROTO_T::GetReflection();
        for(int32_t i = 0; i < msg_desc->field_count(); i++)
        {
            const auto& field_ptr  = pDBRecord->Field(i);
            auto        field_desc = msg_desc->field(i);
            if(field_desc == nullptr)
                continue;

            switch(field_desc->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    reflect->SetDouble(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    reflect->SetFloat(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_INT64:
                    reflect->SetInt64(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_UINT64:
                    reflect->SetUInt64(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_INT32:
                    reflect->SetInt32(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_UINT32:
                    reflect->SetUInt32(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_STRING:
                    reflect->SetString(pMsg, field_desc, field_ptr);
                    break;
                case FieldDescriptor::TYPE_BYTES:
                    reflect->SetString(pMsg, field_desc, field_ptr);
                    break;
                default:
                    break;
            }
        }
    }

    template<class PROTO_T, class DBRecord_T>
    static void PB2DBField(PROTO_T* pMsg, DBRecord_T* pDBRecord)
    {
        using namespace google::protobuf;
        const Descriptor* msg_desc = PROTO_T::GetDescriptor();
        const Reflection* reflect  = PROTO_T::GetReflection();
        for(int32_t i = 0; i < msg_desc->field_count(); i++)
        {
            auto& field_ptr  = pDBRecord->Field(i);
            auto  field_desc = msg_desc->field(i);
            if(field_desc == nullptr)
                continue;

            switch(field_desc->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    field_ptr = reflect->GetDouble(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    field_ptr = reflect->GetFloat(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_INT64:
                    field_ptr = reflect->GetInt64(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_UINT64:
                    field_ptr = reflect->GetUInt64(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_INT32:
                    field_ptr = reflect->GetInt32(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_UINT32:
                    field_ptr = reflect->GetUInt32(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_STRING:
                    field_ptr = reflect->GetString(pMsg, field_desc);
                    break;
                case FieldDescriptor::TYPE_BYTES:
                    field_ptr = reflect->GetString(pMsg, field_desc);
                    break;
                default:
                    break;
            }
        }
    }
};



template<class PROTO_T>
class TDBObj
{
public:
    TDBObj(CDBRecordPtr&& pDBRecordPtr)
    :m_pDBRecordPtr(std::move(pDBRecordPtr))
    ,m_pPBMsg(PROTO_T::default_instance().New())
    {
        DB2PB::DBField2PB(m_pDBRecordPtr.get(), m_pPBMsg.get());
    }
    
    TDBObj(CDBRecord* pDBRecordPtr, PROTO_T* pPBMsg)
    :m_pDBRecordPtr(pDBRecordPtr)
    ,m_pPBMsg(pPBMsg)
    {}

    CDBRecordPtr m_pDBRecordPtr;
    std::unique_ptr<PROTO_T> m_pPBMsg;
};

template<class PROTO_T>
using TDBObjPtr = std::unique_ptr<TDBObj<PROTO_T>>;



#endif /* DBORM_H */
