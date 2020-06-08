lua.h
+
#define LUA_TLIST       9
m
#define LUA_NUMTYPES		10
+
#define lua_islist(L,n)	(lua_type(L, (n)) == LUA_TLIST)

lobject.h
#define setobj2l	setobj

lualib.h
+
#define LUA_LISTLIBNAME	"list"
LUAMOD_API int (luaopen_list)(lua_State *L);

linit.c
+
{LUA_LISTLIBNAME, luaopen_list},


lopcodes.h
添加操作码OP_NEWLIST, OP_SETLISTL

lparser.c
simpleexp这个函数的switch里面添加'['的处理，仿照table的处理流程，添加constructorlist函数
static void simpleexp (LexState *ls, expdesc *v)
    case '[': {
        constructorlist(ls, v);
        return;
    }

constructorlist函数直接复制constructor函数
里面的{}替换成[]
循环里的field(ls, &cc);改为listfield(ls, &cc);，这个表示按照table的数组形式解析

lcode.c
luaK_setlistsize函数替换为luaK_setlistsize函数，内部的OP_NEWTABLE改为OP_NEWLIST

lvm.c
void luaV_execute (lua_State *L, CallInfo *ci)

ltm.c
luaT_typenames_字符串列表里，"thread"后面添加"list"。这个表示type获取的类型。

lstate.h
union GCUnion +
struct List l;

+
#define gco2l(o)  check_exp((o)->tt == LUA_VLIST, &((cast_u(o))->l))
