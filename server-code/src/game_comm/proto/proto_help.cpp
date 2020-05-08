
#include "proto_help.h"

#include "ts_cmd.pb.h"
#include "server_msg/server_side.pb.h"

std::string cmd_to_enum_name(uint32_t cmd)
{
    if(cmd < TS_CMD::CMD_MAX)
    {
        if(CS_CMD_IsValid(cmd))
            return CS_CMD_Name(cmd);
        else
            return SC_CMD_Name(cmd);
    }
    else
    {
        if(ServerMSG::OMsgID_IsValid(cmd))
            return ServerMSG::OMsgID_Name(cmd);
        else
            return std::string();
    }
}

