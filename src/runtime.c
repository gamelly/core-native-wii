#include "zeebo.h"

#ifndef FPS_DELTA
#define FPS_DELTA (16u)
#endif

void runtime(lua_State* L)
{
    lua_getglobal(L, "native_callback_loop");
    int native_callback_loop = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_getglobal(L, "native_callback_draw");
    int native_callback_draw = luaL_ref(L, LUA_REGISTRYINDEX);
    
    lua_getglobal(L, "native_callback_keyboard");
    int native_callback_keyboard = luaL_ref(L, LUA_REGISTRYINDEX);

    
    while (1) {
        WPAD_ScanPads();

        lua_rawgeti(L, LUA_REGISTRYINDEX, native_callback_loop);
        lua_pushnumber(L, FPS_DELTA);
        lua_pcall(L, 1, 0, 0);
        
        lua_rawgeti(L, LUA_REGISTRYINDEX, native_callback_draw);
        lua_pcall(L, 0, 0, 0);
    }
}