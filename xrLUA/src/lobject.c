/*
** $Id: lobject.c,v 1.97 2003/04/03 13:35:34 roberto Exp $
** Some generic functions over Lua objects
** See Copyright Notice in lua.h
*/

#include "stdafx.h"
#pragma hdrstop

#define lobject_c

#include "lua.h"

#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lvm.h"

#include "ltable.h"
#include "lfunc.h"

/* function to convert a string to a lua_Number */
#ifndef lua_str2number
#define lua_str2number(s,p)     strtod((s), (p))
#endif


const TObject luaO_nilobject = {LUA_TNIL, {NULL}};


/*
** converts an integer to a "floating point byte", represented as
** (mmmmmxxx), where the real value is (xxx) * 2^(mmmmm)
*/
int luaO_int2fb (unsigned int x) {
  int m = 0;  /* mantissa */
  while (x >= (1<<3)) {
    x = (x+1) >> 1;
    m++;
  }
  return (m << 3) | cast(int, x);
}


int luaO_log2 (unsigned int x) {
  static const lu_byte log_8[255] = {
    0,
    1,1,
    2,2,2,2,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
  };
  if (x >= 0x00010000) {
    if (x >= 0x01000000) return log_8[((x>>24) & 0xff) - 1]+24;
    else return log_8[((x>>16) & 0xff) - 1]+16;
  }
  else {
    if (x >= 0x00000100) return log_8[((x>>8) & 0xff) - 1]+8;
    else if (x) return log_8[(x & 0xff) - 1];
    return -1;  /* special `log' for 0 */
  }
}


int luaO_rawequalObj (const TObject *t1, const TObject *t2) {
  if (ttype(t1) != ttype(t2)) return 0;
  else switch (ttype(t1)) {
    case LUA_TNIL:
      return 1;
    case LUA_TNUMBER:
      return nvalue(t1) == nvalue(t2);
    case LUA_TBOOLEAN:
      return bvalue(t1) == bvalue(t2);  /* boolean true must be 1 !! */
    case LUA_TLIGHTUSERDATA:
      return pvalue(t1) == pvalue(t2);
    default:
      lua_assert(iscollectable(t1));
      return gcvalue(t1) == gcvalue(t2);
  }
}


int luaO_str2d (const char *s, lua_Number *result) {
  char *endptr;
  lua_Number res = lua_str2number(s, &endptr);
  if (endptr == s) return 0;  /* no conversion */
  while (isspace((unsigned char)(*endptr))) endptr++;
  if (*endptr != '\0') return 0;  /* invalid trailing characters? */
  *result = res;
  return 1;
}



static void pushstr (lua_State *L, const char *str) {
  setsvalue2s(L->top, luaS_new(L, str));
  incr_top(L);
}


/* this function handles only `%d', `%c', %f, and `%s' formats */
const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp) {
  int n = 1;
  pushstr(L, "");
  for (;;) {
    const char *e = strchr(fmt, '%');
    if (e == NULL) break;
    setsvalue2s(L->top, luaS_newlstr(L, fmt, e-fmt));
    incr_top(L);
    switch (*(e+1)) {
      case 's':
        pushstr(L, va_arg(argp, char *));
        break;
      case 'c': {
        char buff[2];
        buff[0] = cast(char, va_arg(argp, int));
        buff[1] = '\0';
        pushstr(L, buff);
        break;
      }
      case 'd':
        setnvalue(L->top, cast(lua_Number, va_arg(argp, int)));
        incr_top(L);
        break;
      case 'f':
        setnvalue(L->top, cast(lua_Number, va_arg(argp, l_uacNumber)));
        incr_top(L);
        break;
      case '%':
        pushstr(L, "%");
        break;
      default: lua_assert(0);
    }
    n += 2;
    fmt = e+2;
  }
  pushstr(L, fmt);
  luaV_concat(L, n+1, L->top - L->base - 1);
  cleanarray(L->top - n, L->top);
  L->top -= n;
  return svalue(L->top - 1);
}


const char *luaO_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *msg;
  va_list argp;
  va_start(argp, fmt);
  msg = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  return msg;
}


void luaO_chunkid (char *out, const char *source, int bufflen) {
  if (*source == '=') {
    strncpy(out, source+1, bufflen);  /* remove first char */
    out[bufflen-1] = '\0';  /* ensures null termination */
  }
  else {  /* out = "source", or "...source" */
    if (*source == '@') {
      int l;
      source++;  /* skip the `@' */
      bufflen -= sizeof(" `...' ");
      l = strlen(source);
      strcpy(out, "");
      if (l>bufflen) {
        source += (l-bufflen);  /* get last part of file name */
        strcat(out, "...");
      }
      strcat(out, source);
    }
    else {  /* out = [string "string"] */
      int len = strcspn(source, "\n");  /* stop at first newline */
      bufflen -= sizeof(" [string \"...\"] ");
      if (len > bufflen) len = bufflen;
      strcpy(out, "[string \"");
      if (source[len] != '\0') {  /* must truncate? */
        strncat(out, source, len);
        strcat(out, "...");
      }
      else
        strcat(out, source);
      strcat(out, "\"]");
    }
  }
}


static void traverseclosure (lua_State *L, Closure *cl) {
  if (cl->c.isC) {
    int i;
    for (i=0; i<cl->c.nupvalues; i++)  /* mark its upvalues */
	  lua_release(L, &cl->c.upvalue[i]);
  }
  else {
    int i;
    lua_assert(cl->l.nupvalues == cl->l.p->nups);
	lua_releasetable(L, hvalue(&cl->l.g));
    lua_releaseproto(L, cl->l.p);
    for (i=0; i<cl->l.nupvalues; i++) {  /* mark its upvalues */
      UpVal *u = cl->l.upvals[i];
	  lua_release(L, &u->value);
    }
  }
}


#define KEYWEAKBIT    1
#define VALUEWEAKBIT  2
#define KEYWEAK         (1<<KEYWEAKBIT)
#define VALUEWEAK       (1<<VALUEWEAKBIT)

static void traversetable (lua_State *L, Table *h) {
  int i;
  int weakkey = 0;
  int weakvalue = 0;
#if 0
  const TObject *mode;
  mode = gfasttm(h->L->l_G, h->metatable, TM_MODE);
  if (mode && ttisstring(mode)) {  /* is there a weak mode? */
    weakkey = (strchr(svalue(mode), 'k') != NULL);
    weakvalue = (strchr(svalue(mode), 'v') != NULL);
    if (weakkey || weakvalue) {  /* is really weak? */
      GCObject **weaklist;
      h->marked &= ~(KEYWEAK | VALUEWEAK);  /* clear bits */
      h->marked |= cast(lu_byte, (weakkey << KEYWEAKBIT) |
                                 (weakvalue << VALUEWEAKBIT));
      weaklist = (weakkey && weakvalue) ? &st->wkv :
                              (weakkey) ? &st->wk :
                                          &st->wv;
      h->gclist = *weaklist;  /* must be cleared after GC, ... */
      *weaklist = valtogco(h);  /* ... so put in the appropriate list */
    }
  }
#endif // 0
  if (!weakvalue) {
    i = h->sizearray;
    while (i--)
      lua_release(L, &h->array[i]);
  }
  i = sizenode(h);
  while (i--) {
    Node *n = gnode(h, i);
    if (!ttisnil(gval(n))) {
      lua_assert(!ttisnil(gkey(n)));
	  if (!weakkey)
		  lua_release(L, gkey(n));
	  if (!weakvalue)
		  lua_release(L, gval(n));
    }
  }
  lua_releasetable(L, h->metatable);
}


static void traverseproto (lua_State *L, Proto *f) {
  int i;
  lua_releasestring(L, f->source);
  for (i=0; i<f->sizek; i++) {  /* mark literal strings */
    if (ttisstring(f->k+i))
      lua_release(L, f->k+i);
  }
  for (i=0; i<f->sizeupvalues; i++)  /* mark upvalue names */
    lua_releasestring(L, f->upvalues[i]);
  for (i=0; i<f->sizep; i++)  /* mark nested protos */
    lua_releaseproto(L, f->p[i]);
  for (i=0; i<f->sizelocvars; i++)  /* mark local-variable names */
    lua_releasestring(L, f->locvars[i].varname);
}


static void traversestack (lua_State *L, lua_State *L1) {
  StkId o, lim;
  CallInfo *ci;
  lua_release(L, gt(L1));
  lim = L1->top;
  for (ci = L1->base_ci; ci <= L1->ci; ci++) {
    lua_assert(ci->top <= L1->stack_last);
    lua_assert(ci->state & (CI_C | CI_HASFRAME | CI_SAVEDPC));
    if (lim < ci->top)
      lim = ci->top;
  }
  for (o = L1->stack; o < L1->top; o++)
    lua_release(L1, o);
  for (; o <= lim; o++)
    setnilvalue(o);
//  checkstacksizes(L1, lim);
}


static void Unlink(GCObject* o)
{
	o->gch.prev->gch.next = o->gch.next;
    if (o->gch.next)
		o->gch.next->gch.prev = o->gch.prev;
}


static void UnlinkString(lua_State *L, TString *ts)
{
  if (ts->tsv.prev)
  {
    ts->tsv.prev->gch.next = ts->tsv.next;
  }
  else
  {
	 unsigned int index = lmod(ts->tsv.hash, G(L)->strt.size);
     lua_assert(G(L)->strt.hash[index] == (GCObject*)ts);
     G(L)->strt.hash[index] = ts->tsv.next;
  }

  if (ts->tsv.next)
  {
    ts->tsv.next->gch.prev = ts->tsv.prev;
  }

  G(L)->strt.nuse--;
}


extern void do1gcTM (lua_State *L, Udata *udata);


void lua_releaseobject(lua_State *L, GCObject* o)
{
	switch (o->gch.tt)
	{
		case LUA_TSTRING: {
			const char* str = getstr(&o->ts); (void)str;
			UnlinkString(L, gcotots(o));
			luaM_free(L, o, sizestring(gcotots(o)->tsv.len));
			break;
		}
		case LUA_TUSERDATA: {
			/* we're freeing this object, but fake ref count it, because do1gcTM actually uses it */
			o->gch.ref = 1;
            do1gcTM(L, gcotou(o));

			Unlink((GCObject*)gcotou(o));

            lua_releasetable(L, gcotou(o)->uv.metatable);

			luaM_free(L, o, sizeudata(gcotou(o)->uv.len));
			break;
		}
		case LUA_TFUNCTION: {
			Closure *cl = gcotocl(o);
			Unlink((GCObject*)cl);
			traverseclosure(L, cl);
			luaF_freeclosure(L, cl);
			break;
		}
		case LUA_TTABLE: {
			Table *h = gcotoh(o);
			Unlink((GCObject*)h);
			traversetable(L, h);
			luaH_free(L, gcotoh(o));
			break;
		}

		case LUA_TPROTO: {
			Proto *p = gcotop(o);
			Unlink((GCObject*)p);
			traverseproto(L, p);
			luaF_freeproto(L, p);
			break;
		}

		case LUA_TUPVAL: {
			luaM_freelem(L, gcotouv(o));
			break;
		}

		case LUA_TTHREAD: {
			lua_assert(gcototh(o) != L && gcototh(o) != G(L)->mainthread);
			Unlink((GCObject*)gcototh(o));
			traversestack(L, gcototh(o));
			luaE_freethread(L, gcototh(o));
			break;
		}

		default:
			lua_assert(0);
	}
}
