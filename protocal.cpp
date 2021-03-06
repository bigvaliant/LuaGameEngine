#include "protocal.h"
#include "log.h"

/* the Lua interpreter */
extern lua_State* L;

CProtocal::CProtocal()
{
}

CProtocal::~CProtocal()
{
}

int CProtocal::message_count = 0;
MessageMap_t CProtocal::message_table = MessageMap_t();

int 
CProtocal::init()
{
    luaL_openlibs(L);
    // Load file.
    if(luaL_dofile(L, "script/protocal.lua")) {
        log_error("Cannot run file:%s\n",lua_tostring(L,-1));
        return -1;
    }

    lua_getglobal(L, "protocal_define_table");
    lua_pushnil(L);
    Message message;
    while(lua_next(L, -2) != 0) {
        lua_pushnil(L);
        lua_next(L, -2);    //cmd
        message.cmd = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_next(L, -2);    //format
        strcpy(message.format, lua_tostring(L, -1));
        lua_pop(L, 1);
        
        lua_next(L, -2);    //desc
        strcpy(message.desc, lua_tostring(L, -1));
        lua_pop(L, 1);
        
        lua_next(L, -2);    //callback
        strcpy(message.call_back, lua_tostring(L, -1));
        lua_pop(L, 1);

        lua_next(L, -2);
        lua_pop(L, 1);
        
        message_table[message.cmd] = message;

    }

	trace_message();

	return 0;
}

void 
CProtocal::trace_message()
{
    MessageMapItr_t iter;
	
    for(iter = message_table.begin(); iter != message_table.end(); iter++) {
        log_debug("cmd: [%#x] Format: [%s] desc: [%s] callback: [%s]", 
			iter->second.cmd, iter->second.format, iter->second.desc, iter->second.call_back);
    }

}

Message 
CProtocal::get_message(unsigned short cmd)
{
    MessageMapItr_t iter = message_table.find(cmd);
    if(iter != message_table.end()) {
        return iter->second;
    }
	
	return Message();     //return default message
}

