#ifndef llist_h
#define llist_h

#include "luaconf.h"
#include "lobject.h"


#define clip(n, a, b) ((n) < (a) ? (a) : (n) > (b) ? (b) : (n))
#define realidx(l, idx) ((idx) < 0 ? (idx) + (l)->size + 1 : (idx))

LNode *indexnode(List *l, lua_Integer idx);

LUAI_FUNC List *luaL_new (lua_State *L);
LUAI_FUNC void luaL_free (lua_State *L, List *l);
LUAI_FUNC TValue *luaL_push(lua_State *L, List *l, lua_Integer idx);
LUAI_FUNC TValue *luaL_pop(lua_State *L, List *l, lua_Integer idx);
LUAI_FUNC void luaL_resize (lua_State *L, List *l, lua_Integer size);
LUAI_FUNC const TValue *luaL_get (List *l, lua_Integer idx);
LUAI_FUNC const TValue *luaL_newnode (lua_State *L, List *l, lua_Integer idx);
LUAI_FUNC List *luaL_extend (lua_State *L, List *l1, List *l2);

#endif
