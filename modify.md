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
��Ӳ�����OP_NEWLIST, OP_SETLISTL

lparser.c
simpleexp���������switch�������'['�Ĵ�������table�Ĵ������̣����constructorlist����
static void simpleexp (LexState *ls, expdesc *v)
    case '[': {
        constructorlist(ls, v);
        return;
    }

constructorlist����ֱ�Ӹ���constructor����
�����{}�滻��[]
ѭ�����field(ls, &cc);��Ϊlistfield(ls, &cc);�������ʾ����table��������ʽ����

lcode.c
luaK_setlistsize�����滻ΪluaK_setlistsize�������ڲ���OP_NEWTABLE��ΪOP_NEWLIST

lvm.c
void luaV_execute (lua_State *L, CallInfo *ci)

ltm.c
luaT_typenames_�ַ����б��"thread"�������"list"�������ʾtype��ȡ�����͡�

lstate.h
union GCUnion +
struct List l;

+
#define gco2l(o)  check_exp((o)->tt == LUA_VLIST, &((cast_u(o))->l))
