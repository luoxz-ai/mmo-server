#ifndef PROTOMSG_TO_CMD_H
#define PROTOMSG_TO_CMD_H

#include "BaseType.h"
namespace google
{
    namespace protobuf
    {
        class Message;
    }
}
using proto_msg_t = google::protobuf::Message;
uint32_t msg_to_cmd(const proto_msg_t& msg);

#endif /* PROTOMSG_TO_CMD_H */
