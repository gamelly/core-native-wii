#include "zeebo.h"

#ifndef FPS_DELTA
#define FPS_DELTA (16u)
#endif

static const struct {
    const char *key;
    uint16_t wpad_btn;
} key_map[] = {
    {"a",     WPAD_BUTTON_A},
    {"b",     WPAD_BUTTON_B},
    {"menu",  WPAD_BUTTON_HOME},
    {"menu",  WPAD_BUTTON_PLUS},
    {"right", WPAD_BUTTON_RIGHT},
    {"left",  WPAD_BUTTON_LEFT},
    {"up",    WPAD_BUTTON_UP},
    {"down",  WPAD_BUTTON_DOWN},
    {"c",     WPAD_BUTTON_1},
    {"d",     WPAD_BUTTON_2},
};

void runtime(lua_State* L)
{
    WPAD_Init();
	WPAD_SetVRes(0, rmode->fbWidth, rmode->efbHeight);
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_ScanPads();

    lua_getglobal(L, "native_callback_loop");
    int native_callback_loop = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_getglobal(L, "native_callback_draw");
    int native_callback_draw = luaL_ref(L, LUA_REGISTRYINDEX);
    
    lua_getglobal(L, "native_callback_keyboard");
    int native_callback_keyboard = luaL_ref(L, LUA_REGISTRYINDEX);

    while (1) {
        WPAD_ScanPads();

        for (int pad = 0; pad < 1; ++pad) {
            if (!WPAD_Probe(pad, NULL)) {
                continue;
            }

            uint32_t pressed = WPAD_ButtonsDown(pad);
            uint32_t released = WPAD_ButtonsUp(pad);

            for (uint8_t index = 0; index < (sizeof(key_map) / sizeof(key_map[0])); ++index) {
                if (pressed & key_map[index].wpad_btn || released & key_map[index].wpad_btn) {
                    lua_rawgeti(L, LUA_REGISTRYINDEX, native_callback_keyboard);
                    lua_pushstring(L, key_map[index].key);
                    lua_pushinteger(L, (released & key_map[index].wpad_btn) ? 1 : 0);
                    lua_pcall(L, 2, 0, 0);
                }
            }
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, native_callback_loop);
        lua_pushnumber(L, FPS_DELTA);
        lua_pcall(L, 1, 0, 0);
        
        lua_rawgeti(L, LUA_REGISTRYINDEX, native_callback_draw);
        lua_pcall(L, 0, 0, 0);
    }
}