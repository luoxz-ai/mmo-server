#ifndef PROTOMSG_TO_CMD_H
#define PROTOMSG_TO_CMD_H

#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"

using proto_msg_t = google::protobuf::Message;

#include "msg/ts_cmd.pb.h"
#include "server_msg/server_side.pb.h"


inline uint32_t to_cmd(const proto_msg_t& msg)
{
    __ENTER_FUNCTION
    auto msg_desc = msg.GetDescriptor();
    auto options   = msg_desc->options();
    
    {
        auto cmd_extension = options.GetExtension(cmd);
        if(cmd_extension.ts_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() == TS_CMD_Name(cmd_extension.ts_cmd()) );
            return cmd_extension.ts_cmd();
        }
        if(cmd_extension.sc_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() ==  SC_CMD_Name(cmd_extension.sc_cmd()) );
            return cmd_extension.sc_cmd();
        }   
        if(cmd_extension.cs_cmd() != 0)
        {
            CHECK_DEBUG( "CMD_" + msg_desc->name() == CS_CMD_Name(cmd_extension.cs_cmd())  );
            return cmd_extension.cs_cmd();
        } 
    }
   
    {
        auto server_cmd_extension = options.GetExtension(ServerMSG::server_cmd);
        if(server_cmd_extension.id() != 0)
        {
            CHECK_DEBUG( "MsgID_" + msg_desc->name() == ServerMSG::OMsgID_Name(server_cmd_extension.id()) );
            return server_cmd_extension.id();
        }
    }

    __LEAVE_FUNCTION
    return 0;
}


#endif /* PROTOMSG_TO_CMD_H */
