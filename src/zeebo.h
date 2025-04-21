#include <grrlib.h>
#include <wiiuse/wpad.h>

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"


typedef union {;
    uint32_t hex;
    struct {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } color;
    } c32;
} color_t;

void glyL_openlibs(lua_State* L);
void runtime(lua_State* L);
