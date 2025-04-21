#include "font/Noto_Sans/notosans_regular.h"

#include "zeebo.h"

static uint32_t current_color = 0xFFFFFFFF;
static uint8_t font_size = 5;
static uint8_t font_size_previous = 8;
static GRRLIB_ttfFont *font_ttf;

static int native_draw_start(lua_State *L)
{
    return 0;
}

static int native_draw_flush(lua_State *L)
{
    GRRLIB_Render();
    return 0;
}

static int native_draw_clear(lua_State *L)
{
    color_t t = { .hex = luaL_checkinteger(L, 1) };
    GRRLIB_SetBackgroundColour(t.c32.color.r, t.c32.color.g, t.c32.color.b, t.c32.color.a);
    lua_settop(L, 0);
    return 0;
}

static int native_draw_color(lua_State *L)
{
    current_color = luaL_checkinteger(L, 1);
    lua_settop(L, 0);
    return 0;
}

static int native_draw_rect(lua_State *L)
{
    bool mode = luaL_checkinteger(L, 1) ^ true;
    float posx = luaL_checknumber(L, 2);
    float posy = luaL_checknumber(L, 3);
    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    GRRLIB_Rectangle(posx, posy, width, height, current_color, mode);
    lua_settop(L, 0);
    return 0;
}

static int native_draw_line(lua_State *L)
{
    int32_t x1 = luaL_checknumber(L, 1);
    int32_t y1 = luaL_checknumber(L, 2);
    int32_t x2 = luaL_checknumber(L, 3);
    int32_t y2 = luaL_checknumber(L, 4);

    GRRLIB_Line(x1, y1, x2, y2, current_color);
    lua_settop(L, 0);
    return 0;
}

static int native_image_load(lua_State *L) {
    lua_settop(L, 0);
}

static int native_image_draw(lua_State *L) {
    return 0;
}

static int native_text_mock(lua_State *L)
{
    lua_settop(L, 0);
    return 0;
}

static int native_text_print(lua_State *L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    GRRLIB_PrintfTTF(x, y, font_ttf, text, font_size, current_color);
    lua_settop(L, 0);
    return 2;
}

static int native_text_mensure(lua_State *L)
{
    const char* text = luaL_checkstring(L, 1);
    lua_settop(L, 0);
    lua_pushnumber(L, GRRLIB_WidthTTF(font_ttf, text, font_size));
    lua_pushnumber(L, font_size);
    return 2;
}

static int native_text_font_size(lua_State *L)
{
    font_size_previous = font_size;
    font_size = luaL_checknumber(L, 1);
    lua_settop(L, 0);
    return 0;
}

static int native_text_font_previous(lua_State *L)
{
    uint8_t font_size_old = font_size;
    font_size = font_size_previous;
    font_size_previous = font_size_old;
    lua_settop(L, 0);
    return 0;
}

void glyL_openlibs(lua_State* L)
{
    font_ttf = GRRLIB_LoadTTF(notosans_regular_ttf, notosans_regular_ttf_len);

    int i = 0;
    static const luaL_Reg lib[] = {
        {"native_draw_start", native_draw_start},
        {"native_draw_flush", native_draw_flush},
        {"native_draw_clear", native_draw_clear},
        {"native_draw_color", native_draw_color},
        {"native_draw_rect", native_draw_rect},
        {"native_draw_line", native_draw_line},
        {"native_image_load", native_image_load},
        {"native_image_draw", native_image_draw},
        {"native_text_print", native_text_print},
        {"native_text_mensure", native_text_mensure},
        {"native_text_font_size", native_text_font_size},
        {"native_text_font_name", native_text_mock},
        {"native_text_font_default", native_text_mock},
        {"native_text_font_previous", native_text_font_previous}
    };

    while(i < sizeof(lib)/sizeof(luaL_Reg)) {
        lua_pushcfunction(L, lib[i].func);
        lua_setglobal(L, lib[i].name);
        i = i + 1;
    }
}