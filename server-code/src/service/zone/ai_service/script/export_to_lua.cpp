#include "Common.pb.h"
#include "ScriptManager.h"
extern void ai2lua(lua_State* L);

void export_to_lua(lua_State* L, void*)
{
    //向lua注册c++类型
    ai2lua(L);

    //注册各种辅助函数
}