#ifndef GFXSCRIPT_INCLUDED // -*- C++ -*-
#define GFXSCRIPT_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Simple low-level file format scripting support.

  This is the THIRD major revision of this code.  It represents a
  significant departure from the previous scripting structure.
    
  $Id: script.h,v 1.14 2003/07/25 10:51:25 garland Exp $

 ************************************************************************/

#include "gfx.h"
#include <vector>
#include <string>
#include <map>
//#include <hash_map>

enum {
    SCRIPT_OK = 0,
    SCRIPT_ERR_UNDEF,
    SCRIPT_ERR_SYNTAX,
    SCRIPT_ERR_UNSUPPORTED,
    SCRIPT_ERR_NOFILE,
};

class CmdLine
{
public:
    typedef std::string::size_type index_type;
    typedef std::pair<index_type, index_type> range_type;

    const std::string &line;
    range_type op;
    std::vector<range_type> tokens;

    std::string substr(const range_type& r) const
	{ return line.substr(r.first, r.second-r.first);}

    std::string token_to_string(int i) const;
    double token_to_double(int i) const;
    float token_to_float(int i) const;
    int token_to_int(int i) const;
    

    CmdLine(const std::string &l) : line(l) { }

    std::string opname() const { return substr(op); }
    int argcount() const { return tokens.size(); }
    std::string argline() const;

    int collect_as_strings(std::vector<std::string> &v, int offset=0) const;
    int collect_as_numbers(std::vector<double> &v, int offset=0) const;
    int collect_as_numbers(std::vector<int> &v, int offset=0) const;

    int collect_as_numbers(double *v, int size, int offset=0) const;
    int collect_as_numbers(float *v, int size, int offset=0) const;
    int collect_as_numbers(int *v, int size, int offset=0) const;
};

typedef int (*CmdHandler)(const CmdLine&);

struct CmdObject { virtual int operator()(const CmdLine& cmd) = 0; };

struct CmdFunction : public CmdObject
{
    CmdHandler fn;
    CmdFunction(CmdHandler f) { fn=f; }
    virtual int operator()(const CmdLine& cmd) { return (*fn)(cmd); }
};

template<class T> struct CmdMethod : public CmdObject
{
    typedef int (T::*member_handler)(const CmdLine&);
    T *self;
    member_handler fn;

    CmdMethod(T &obj, member_handler p) { self=&obj; fn=p; }
    virtual int operator()(const CmdLine& cmd) { return (self->*fn)(cmd); }
};

//typedef std::hash_map< std::string, CmdHandler > CmdTable;
typedef std::map< std::string, CmdObject* > CmdTable;

class CmdEnv
{
private:
    CmdTable script_commands;

    int script_include(const CmdLine&);
    int script_ignore(const CmdLine&);

public:
    CmdEnv();
    virtual ~CmdEnv();

    virtual int script_fallback(const CmdLine&, void *);
    void register_command(const std::string& name, CmdObject *fn);
    CmdObject *lookup_command(const std::string& name);

    void register_command(const std::string& name, CmdHandler proc);

    template<class T> inline void register_method(const std::string& name,
						  T *obj,
						  int (T::*fn)(const CmdLine&))
    { register_command(name, new CmdMethod<T>(*obj, fn)); }

    void ignore_command(const std::string& name);

    int do_line(const std::string& line);
    int do_stream(std::istream& in);
    int do_file(const std::string& filename);
    int do_string(const std::string& line);
};

inline int script_do_line(const std::string &line, CmdEnv &env)
	    { return env.do_line(line); }
inline int script_do_stream(std::istream &in, CmdEnv &env)
	    { return env.do_stream(in); }
inline int script_do_file(const std::string& name, CmdEnv &env)
	    { return env.do_file(name); }
inline int script_do_string(const std::string& str, CmdEnv &env)
	    { return env.do_string(str); }

// GFXSCRIPT_INCLUDED
#endif
