/************************************************************************

  MxAsp

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxAsp.cxx,v 1.8.2.1 2004/07/01 18:39:01 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxAsp.h"


#define MXASP_OWN_MEMORY 0x1

void MxAspVar::own_memory(bool t)
{
    if( t ) flags |= MXASP_OWN_MEMORY;
    else    flags &= ~MXASP_OWN_MEMORY;
}

bool MxAspVar::is_memory_owned()
{
    return flags & MXASP_OWN_MEMORY;
}

void MxAspVar::clear()
{
    type = MXASP_NULL;
    size = 0;
    flags = 0;
    as.any = NULL;
    name = NULL;
}

void MxAspVar::bind(const char *n, MxAspType t, void *v, uint len)
{
    unbind();

    name = strdup(n);
    type = t;
    as.any = v;
    size = len;
}

void MxAspVar::unbind()
{
    if( is_memory_owned() && as.any ) free(as.any);
    if( name ) free(name);

    clear();
}

#define asprint(V)\
out<< *as.V##_var; \
for(i=1; i<size; i++) out << " " << as.V##_var[i]

ostream& MxAspVar::print(ostream& out) const
{
    uint i;

    switch( type )
    {
    case MXASP_BOOL:    asprint(bool); break;
    case MXASP_INT:     asprint(int); break;
    case MXASP_UINT:    asprint(uint); break;
    case MXASP_FLOAT:   asprint(float); break;
    case MXASP_DOUBLE:  asprint(double); break;
    case MXASP_PROC:    out << "#<proc " << name << ">"; break;

    default: out << "#<avar '" << name << "' of unknown type>"; break;
    }

    return out;
}
#undef asprint

#define defset(V, T) \
bool MxAspVar::set_##V(V val, uint i) \
{\
     if( type==T && i<size ) { as.V##_var[i]=val; return true; } \
     else  return false; \
}

defset(bool,   MXASP_BOOL)
defset(int,    MXASP_INT)
defset(uint,   MXASP_UINT)
defset(float,  MXASP_FLOAT)
defset(double, MXASP_DOUBLE)

#undef defset

#define defget(V, T) \
V *MxAspVar::get_##V(V *out, uint i) \
{\
     if( type==T && i<size ) { *out = as.V##_var[i]; return out; } \
     else  return NULL; \
} 

defget(bool,   MXASP_BOOL)
defget(int,    MXASP_INT)
defget(uint,   MXASP_UINT)
defget(float,  MXASP_FLOAT)
defget(double, MXASP_DOUBLE)

#undef defget


bool MxAspVar::read_from(const char *str, uint i)
{
    if( i>=size )  return false;

    switch( type )
    {
    case MXASP_BOOL:   as.bool_var[i] = atoi(str)&1; break;
    case MXASP_INT:    as.int_var[i] = atoi(str); break;
    case MXASP_UINT:   as.uint_var[i] = atoi(str); break;
    case MXASP_FLOAT:  as.float_var[i] = atof(str); break;
    case MXASP_DOUBLE: as.double_var[i] = atof(str); break;

    default:  return false;
    }

    return true;
}

bool MxAspVar::apply(int argc, const char **argv, MxAspStore *store)
{
    if( type==MXASP_PROC && argc>=as.proc_var->args_expected )
    {
	if( as.proc_var->args_expected==0 )
	{
	    apply(NULL, store);
	}
	else
	{
	    void *value = mxasp_alloc_value(as.proc_var->arg_type,
					    as.proc_var->args_expected);

	    MxAspVar arg("{frame}", as.proc_var->arg_type,
			 value, as.proc_var->args_expected);

	    arg.own_memory(true);
	    for(uint i=0; i<argc; i++) arg.read_from(argv[i], i);

	    apply(&arg, store);
	}

	return true;
    }

    return false;
}


bool MxAspVar::apply(MxAspVar *arg, MxAspStore *store)
{
    if( type==MXASP_PROC )
    {
	(*as.proc_var->proc)(arg, store);
	return true;
    }

    return false;
}



MxAspStore::MxAspStore()
    : vars(8)
{
}


MxAspVar *MxAspStore::lookup(const char *name) const
{
    for(uint i=0; i<vars.length(); i++)
	if( streq(vars[i].nameof(), name) )
	    //
	    // NOTE: You might think it odd that we need the cast here.
	    //       However, some compilers (i.e., SGI's) seem to need
	    //       this to avoid getting confused.  Presumably because
	    //       there is both a const and non-const access method.
	    return (MxAspVar *)&vars[i];

    return NULL;
}

MxAspVar *MxAspStore::defvar(const char *name, MxAspType t, void *v, uint len)
{
    MxAspVar& avar = vars.add();

    avar.bind(name, t, v, len);
    return &avar;
}

MxAspVar *MxAspStore::defun(const char *name, MxAspProc proc,
			    MxAspType type, uint expects)
{
    MxAspProcDefn *defn = (MxAspProcDefn *)calloc(1, sizeof(MxAspProcDefn));
    defn->proc = proc;
    defn->arg_type = type;
    defn->args_expected = expects;

    return defvar(name, MXASP_PROC, defn, 1);
}

bool MxAspStore::execute_command(int argc, const char *cmd, const char **argv)
{
    const char *varname  = argc?argv[0]:(const char*)NULL;
    MxAspVar *avar = varname?lookup(varname):(MxAspVar*)NULL;

    if( streq(cmd, "set") )
    {
	if( avar )
	{
	    for(uint i=1; i<argc; i++)
		avar->read_from(argv[i], i-1);
	}
	else
	    mxmsg_signal(MXMSG_WARN,
			 "Tried to SET undefined Asp variable", varname);
    }
    else if( streq(cmd, "print") )
    {
	if( avar )
	{
	    cout << "  : " << varname << " = ";
	    avar->print(cout) << endl;
	}
	else
	    mxmsg_signal(MXMSG_WARN,
			 "Tried to PRINT undefined Asp variable", varname);
    }
    else if( streq(cmd, "defvar") )
    {
	if( avar )
	    mxmsg_signal(MXMSG_WARN, "Can't redefine Asp variable", varname);
	else
	{
	    MxAspType type = mxasp_type_from_name(argv[1]);
	    uint size = (argc>2)?atoi(argv[2]):1;
	    void *value = mxasp_alloc_value(type, size);

	    avar = defvar(varname, type, value, size);
	    avar->own_memory(true);
	}
    }
    else if( (avar=lookup(cmd)) )
    {
	avar->apply(argc, argv, this);
    }
    else
	return false;

    return true;
}

void MxAspStore::write(ostream& out)
{
    out << "# ASP automatic state dump" << endl;
    for(uint i=0; i<vars.length(); i++)
	out << "set " << vars[i] << endl;
}



static
char *typenames[] =
{
    "null",
    "bool",
    "int",
    "uint",
    "float",
    "double",
    "proc",
    NULL
};

static
uint typesizes[] = 
{
    0,
    sizeof(bool),
    sizeof(int),
    sizeof(uint),
    sizeof(float),
    sizeof(double),
    sizeof(MxAspProcDefn)
};


MxAspType mxasp_type_from_name(const char *name)
{
    for(uint i=1; i<MXASP_USER_TYPE; i++)
	if( streq(name, typenames[i]) )
	    return (MxAspType)i;

    return MXASP_NULL;
}

void *mxasp_alloc_value(MxAspType t, uint size)
{
    if( t>MXASP_NULL && t<MXASP_USER_TYPE )
    {
	return (void *)calloc(size, typesizes[t]);
    }
    else
	return NULL;
}
