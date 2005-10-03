/*
** $Id: lualib.h,v 1.35 2005/08/10 18:06:58 roberto Exp $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

#include "lua.h"


/* Key to file-handle type */
#define LUA_FILEHANDLE		"FILE*"


#define LUA_COLIBNAME	"coroutine"
LUALIB_API int (LUA_CC luaopen_base) (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUALIB_API int (LUA_CC luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUALIB_API int (LUA_CC luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUALIB_API int (LUA_CC luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUALIB_API int (LUA_CC luaopen_string) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUALIB_API int (LUA_CC luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUALIB_API int (LUA_CC luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUALIB_API int (LUA_CC luaopen_package) (lua_State *L);

#define LUA_JITLIBNAME "jit"
LUALIB_API int (LUA_CC luaopen_jit) (lua_State *L);


/* open all previous libraries */
LUALIB_API void (LUA_CC luaL_openlibs) (lua_State *L); 


#endif
