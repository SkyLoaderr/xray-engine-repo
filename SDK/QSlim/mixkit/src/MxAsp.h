#ifndef MXASP_INCLUDED // -*- C++ -*-
#define MXASP_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxAsp

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxAsp.h,v 1.8 2000/11/28 15:50:14 garland Exp $

 ************************************************************************/

#include "MxDynBlock.h"

enum MxAspType
{
    MXASP_NULL=0,
    MXASP_BOOL=1,
    MXASP_INT=2,
    MXASP_UINT=3,
    MXASP_FLOAT=4,
    MXASP_DOUBLE=5,
    MXASP_PROC=6,
    MXASP_USER_TYPE=7
};

class MxAspVar;
class MxAspStore;

typedef void (*MxAspProc)(MxAspVar *, MxAspStore *);

struct MxAspProcDefn
{
    MxAspProc proc;
    MxAspType arg_type;
    uint args_expected;
};


class MxAspVar
{
private:
    MxAspType type;
    union {
	void    *any;
	bool    *bool_var;
	int     *int_var;
	uint    *uint_var;
	float   *float_var;
	double  *double_var;
	MxAspProcDefn *proc_var;
    } as;

    uint size;
    uint flags;

    char *name;

protected:
    void clear();

public:
    MxAspVar() { clear(); }
    MxAspVar(const char *n,MxAspType t,void *v,uint len=1)
	{ clear(); bind(n,t,v,len); }
    ~MxAspVar() { unbind(); }

    void bind(const char *, MxAspType, void *, uint len=1);
    void unbind();

    const char *nameof() const { return name; }
    uint length() const { return size; }
    MxAspType get_type() const { return type; }
    ostream& print(ostream&) const;

    void own_memory(bool);
    bool is_memory_owned();

#define mxasp_defset(V)  bool set_##V(V, uint i=0)
    mxasp_defset(bool);
    mxasp_defset(int);
    mxasp_defset(uint);
    mxasp_defset(float);
    mxasp_defset(double);
#undef mxasp_defset

#define mxasp_defget(V)  V *get_##V(V *, uint i=0)
    mxasp_defget(bool);
    mxasp_defget(int);
    mxasp_defget(uint);
    mxasp_defget(float);
    mxasp_defget(double);
#undef mxasp_defget

    bool read_from(const char *, uint i=0);
    bool apply(int argc, const char **argv, MxAspStore *store);
    bool apply(MxAspVar *arg, MxAspStore *store);
};


class MxAspStore
{
private:
    MxDynBlock<MxAspVar> vars;

public:
    MxAspStore();

    MxAspVar *lookup(const char *name) const;
    MxAspVar *defvar(const char *name, MxAspType, void *, uint len=1);
    MxAspVar *defun(const char *name, MxAspProc proc,
		    MxAspType type=MXASP_NULL, uint expects=0);

    bool execute_command(int argc, const char *cmd, const char **argv);

    void write(ostream&);
};

inline ostream& operator<<(ostream& out, const MxAspVar& v)
{ return out<<v.nameof()<<" ",v.print(out); }

extern MxAspType mxasp_type_from_name(const char *);
extern void *mxasp_alloc_value(MxAspType, uint size=1);
extern void mxasp_apply(MxAspVar *proc, MxAspVar *arg, MxAspStore *store);
extern void mxasp_apply(MxAspProc proc, MxAspVar *arg, MxAspStore *store);

// MXASP_INCLUDED
#endif
