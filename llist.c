#include "lua.h"
#include "lauxlib.h"
#include "lgc.h"
#include "llist.h"


static const TValue absentkey = { ABSTKEYCONSTANT };

List *luaL_new (lua_State *L)
{
	GCObject *o = luaC_newobj(L, LUA_VLIST, sizeof(List));
	List *l = gco2l(o);
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;

	//快速查找节点，使连续查找的时间复杂度降低为O(1)
	l->fastnode = NULL;
	l->fastindex = -1;
	return l;
}

void luaL_free (lua_State *L, List *l)
{
	LNode *head = l->head;
	LNode *ln = head;
	
	while (head)
	{
		head = head->next;
		luaM_free(L, ln);
		ln = head;
	}
	luaM_free(L, l);
}

LNode *indexnode(List *l, lua_Integer idx)
{
	LNode *ln_s = l->head, *ln_e = l->tail, *ln = NULL;
	lua_Integer idx_s = 1, idx_e = l->size, i;

	if (likely(l->fastnode))
	{
		//减小查找范围
		if (idx > l->fastindex)
		{
			ln_s = l->fastnode->next;
			idx_s = l->fastindex + 1;
		}
		else if (idx < l->fastindex)
		{
			ln_e = l->fastnode->prev;
			idx_e = l->fastindex - 1;
		}
		else
			return l->fastnode;
	}

	if (idx > ((idx_s + idx_e) >> 1))
		for (ln = ln_e, i = idx_e; i > idx; ln = ln->prev, i--);
	else
		for (ln = ln_s, i = idx_s; i < idx; ln = ln->next, i++);

	l->fastnode = ln;
	l->fastindex = idx;
	return ln;
}

TValue *luaL_push(lua_State *L, List *l, lua_Integer idx)
{
	idx = realidx(l, idx);
	LNode *node = luaM_new(L, LNode);
	LNode *prev;

	if (idx <= 0 || unlikely(l->size == 0))
	{
		node->next = NULL;
		node->prev = NULL;

		if (!l->head)
			l->tail = node;
		else
		{
			l->head->prev = node;
			node->next = l->head;
		}
		l->head = node;
	}
	else
	{
		prev = idx >= l->size ? l->tail : indexnode(l, idx);
		node->prev = prev;
		node->next = prev->next;

		if (prev->next)
			prev->next->prev = node;
		else
			l->tail = node;
		prev->next = node;
	}
	l->size++;

	return &node->val;
}

TValue *luaL_pop(lua_State *L, List *l, lua_Integer idx)
{
	idx = realidx(l, idx);
	if (idx <= 0 || idx > l->size)
		return &absentkey;

	LNode *ln = NULL;
	TValue *t;

	ln = indexnode(l, idx);
	if (ln->prev)
		ln->prev->next = ln->next;
	else
		l->head = l->head->next;
	if (ln->next)
		ln->next->prev = ln->prev;
	else
		l->tail = l->tail->prev;
	
	l->fastnode = ln->next;

	t = &ln->val;
	luaM_free(L, ln);
	l->size--;

	return t;
}

void luaL_resize (lua_State *L, List *l, lua_Integer size)
{
	lua_Integer i;
	for (i = l->size; i < size; i++)
		luaL_push(L, l, 0);
	l->size = size;
}

const TValue *luaL_get (List *l, lua_Integer idx)
{
	idx = realidx(l, idx);
	if (idx <= 0 || idx > l->size)
		return &absentkey;
	return &indexnode(l, idx)->val;
}

/*
	idx < 1, push in head
	idx > size, push in tail
*/
const TValue *luaL_newnode (lua_State *L, List *l, lua_Integer idx)
{
	idx = realidx(l, idx);

	if (idx <= 0)
		return luaL_push(L, l, 0);
	else if (idx > l->size)
		return luaL_push(L, l, l->size + 1);
	return &absentkey;
}

List *luaL_extend (lua_State *L, List *l1, List *l2)
{
	List *l = luaL_new(L);
	luaL_resize(L, l, l1->size + l2->size);

	LNode *lnt = l->head, *ln;
	for (ln = l1->head; ln; ln = ln->next, lnt = lnt->next)
		setobj(L, &lnt->val, &ln->val);
	for (ln = l2->head; ln; ln = ln->next, lnt = lnt->next)
		setobj(L, &lnt->val, &ln->val);
	return l;
}
