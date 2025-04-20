#include "engine/bytecode.h"
#include "game/bytecode.h"

#include "zeebo.h"

int main()
{
    GRRLIB_Init();
    WPAD_Init();
    
    lua_State *L;
    L = luaL_newstate();
    luaL_openlibs(L);
    glyL_openlibs(L);

    luaL_loadbuffer(L, engine_bytecode_lua, engine_bytecode_lua_len, "E");
	lua_pcall(L, 0, 0, 0);

    lua_getglobal(L, "native_callback_init");
    lua_pushnumber(L, rmode->fbWidth);
    lua_pushnumber(L, rmode->efbHeight);

    luaL_loadbuffer(L, game_bytecode_lua, game_bytecode_lua_len, "G");
	lua_pcall(L, 0, 1, 0);
    lua_pcall(L, 3, 0, 0);

    runtime(L);
    GRRLIB_Exit();
    return 0;
}