#ifndef __LUA_HELPER_FUNCTIONS_H__
#define __LUA_HELPER_FUNCTIONS_H__

#define lua_create_table_string_key_value(key, value)           \
    {                                                           \
        lua_pushstring(lua_state, key);                         \
        lua_pushstring(lua_state, value ? value : ezlopi__str); \
        lua_settable(lua_state, -3);                            \
    }
#define lua_create_table_bool_key_value(key, value) \
    {                                               \
        lua_pushstring(lua_state, key);             \
        lua_pushboolean(lua_state, value);          \
        lua_settable(lua_state, -3);                \
    }

#define lua_create_table_number_key_value(key, value) \
    {                                                 \
        lua_pushstring(lua_state, key);               \
        lua_pushnumber(lua_state, value);             \
        lua_settable(lua_state, -3);                  \
    }

#define lua_create_table_string_value(idx, value)               \
    {                                                           \
        lua_pushnumber(lua_state, idx);                         \
        lua_pushstring(lua_state, value ? value : ezlopi__str); \
        lua_settable(lua_state, -3);                            \
    }

#endif // __LUA_HELPER_FUNCTIONS_H__
