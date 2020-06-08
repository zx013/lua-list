#define llistlib_c
#define LUA_LIB

#include "lprefix.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lobject.h"
#include "lapi.h"
#include "lgc.h"

#include "llist.h"


/* 将TValue值压入栈 */
void lua_push(lua_State *L, TValue *t) {
    lua_lock(L);
    setobj2s(L, L->top, t);
    api_incr_top(L);
    lua_unlock(L);
}


/* 和ltablib.c中的addfield相同 */
static void addfield(lua_State *L, luaL_Buffer *b, lua_Integer i)
{
    lua_geti(L, 1, i);
    if (!lua_isstring(L, -1))
        luaL_error(L, "invalid value (%s) at index %d in table for 'concat'",
            luaL_typename(L, -1), i);
    luaL_addvalue(b);
}


static int lconcat(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TLIST);

    luaL_Buffer b;
    char *sep = "";
    size_t lsep = 0;
    lua_Integer i = 1;
    lua_Integer last = luaL_len(L, 1);

    switch (lua_gettop(L))
    {
    case 4:
        last = luaL_checkinteger(L, 4);
    case 3:
        i = luaL_checkinteger(L, 3);
    case 2:
        sep = (char *)luaL_checklstring(L, 2, &lsep);
    case 1:
        luaL_buffinit(L, &b);
        for (; i < last; i++)
        {
            addfield(L, &b, i);
            luaL_addlstring(&b, sep, lsep);
        }
        if (i == last)  /* add last value (if interval was not empty) */
            addfield(L, &b, i);
        break;
    default:
        return luaL_error(L, "wrong number of arguments to 'concat'");
    }

    luaL_pushresult(&b);
    return 1;
}


static int linsert(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TLIST);

    lua_Integer pos = luaL_len(L, 1) + 1;
    switch (lua_gettop(L))
    {
    case 3:
        pos = luaL_checkinteger(L, 2);
    case 2:
    {  /* called with only 2 arguments */
        List *l = lvalue(index2value(L, 1));
        TValue *val = index2value(L, -1);

        TValue *slot = luaL_push(L, l, pos);

        setobj2l(L, cast(TValue *, slot), val);
        luaC_barrierback(L, obj2gco(l), val);
        break;
    }
    default:
        return luaL_error(L, "wrong number of arguments to 'insert'");
    }
    return 0;
}


/* 不存在值时返回nil */
static int lremove(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TLIST);
    List *l = lvalue(index2value(L, 1));
    lua_Integer pos;

    switch (lua_gettop(L))
    {
    case 1:
    {  /* called with only 1 arguments */
        pos = luaL_len(L, 1);  /* first empty element */
        break;
    }
    case 2:
    {
        pos = luaL_checkinteger(L, 2);  /* 1nd argument is the position */
        break;
    }
    default:
        return luaL_error(L, "wrong number of arguments to 'remove'");
    }
    lua_push(L, luaL_pop(L, l, pos));
    return 1;
}


static void movenode(lua_State *L, List *l, List *sl, lua_Integer i)
{
    LNode *ln;
    TValue *slot, *val;

    slot = luaL_push(L, sl, sl->size + 1);
    ln = indexnode(l, i);
    val = &ln->val;

    setobj2l(L, cast(TValue *, slot), val);
    luaC_barrierback(L, obj2gco(l), val);
}

static int lslice(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TLIST);
    List *l, *sl;
    
    lua_Integer start = 1;
    lua_Integer end = luaL_len(L, 1);
    lua_Integer step = 1;
    lua_Integer i;

    switch (lua_gettop(L))
    {
    case 4:
        step = luaL_checkinteger(L, 4);
    case 3:
        end = luaL_checkinteger(L, 3);
    case 2:
        start = luaL_checkinteger(L, 2);
    case 1:
        l = lvalue(index2value(L, 1));
        sl = luaL_new(L);
        start = realidx(l, start);
        end = realidx(l, end);

        if (step > 0)
        {
            start = clip(start, 1, l->size + 1); //起始位置超过右边界时为空
            end = clip(end, 0, l->size); //结束位置超过左边界时为空
            if (start <= end)
            {
                for (i = start; i <= end; i += step)
                    movenode(L, l, sl, i);
            }
        }
        else if (step < 0)
        {
            start = clip(start, 0, l->size);
            end = clip(end, 1, l->size + 1);

            if (start >= end)
            {
                for (i = start; i >= end; i += step)
                    movenode(L, l, sl, i);
            }
        }
        
        break;
    default:
        return luaL_error(L, "wrong number of arguments to 'slice'");
    }
    setlvalue2s(L, L->top, sl);
    api_incr_top(L);
    return 1;
}


static int lpack(lua_State *L)
{
    int i;
    int n = lua_gettop(L);  /* number of elements to pack */
    List *l = luaL_new(L);
    
    for (i = n; i >= 1; i--)  /* assign elements */
    {
        TValue *val = index2value(L, i);
        TValue *slot = luaL_push(L, l, 0);

        setobj2l(L, cast(TValue *, slot), val);
        luaC_barrierback(L, obj2gco(l), val);
    }
    setlvalue2s(L, L->top, l);
    api_incr_top(L);
    return 1;
}

/* 和ltablib.c中的tunpack相同 */
static int lunpack(lua_State *L)
{
    lua_Unsigned n;
    lua_Integer i = luaL_optinteger(L, 2, 1);
    lua_Integer e = luaL_opt(L, luaL_checkinteger, 3, luaL_len(L, 1));
    if (i > e) return 0;  /* empty range */
    n = (lua_Unsigned)e - i;  /* number of elements minus 1 (avoid overflows) */
    if (n >= (unsigned int)INT_MAX || !lua_checkstack(L, (int)(++n)))
        return luaL_error(L, "too many results to unpack");
    for (; i < e; i++) {  /* push arg[i..e - 1] (to avoid overflows) */
        lua_geti(L, 1, i);
    }
    lua_geti(L, 1, e);  /* push last element */
    return (int)n;
}


static int lsort(lua_State *L)
{
    printf("Function <sort> is not implement");
    return 0;
}


static const luaL_Reg list_funcs[] = {
  {"concat", lconcat},
  {"insert", linsert},
  {"remove", lremove},
  {"slice", lslice},
  {"pack", lpack},
  {"unpack", lunpack},
  {"sort", lsort},
  {NULL, NULL}
};


LUAMOD_API int luaopen_list(lua_State *L)
{
	luaL_newlib(L, list_funcs);
	return 1;
}
