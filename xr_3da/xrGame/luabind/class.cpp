// Copyright (c) 2004 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include <luabind/lua_include.hpp>

#include <luabind/config.hpp>
#include <luabind/class.hpp>

namespace luabind { namespace detail {
    
    struct class_registration : registration
    {   
        class_registration(char const* name);

        void register_(lua_State* L) const;

        const char* m_name;

        mutable std::map<const char*, detail::method_rep, detail::ltstr> m_methods;

        // datamembers, some members may be readonly, and
        // only have a getter function
        mutable std::map<const char*, detail::class_rep::callback, detail::ltstr> m_getters;
        mutable std::map<const char*, detail::class_rep::callback, detail::ltstr> m_setters;

        // the operators in lua
        mutable std::vector<detail::class_rep::operator_callback> m_operators[detail::number_of_operators]; 
        mutable std::map<const char*, int, detail::ltstr> m_static_constants;

        mutable std::vector<class_base::base_desc> m_bases;
        mutable detail::construct_rep m_constructor;

        void(*m_destructor)(void*);
        void(*m_const_holder_destructor)(void*);

        void*(*m_extractor)(void*);
        const void*(*m_const_extractor)(void*);

        void(*m_const_converter)(void*,void*);

        void(*m_construct_holder)(void*, void*);
        void(*m_construct_const_holder)(void*, void*);

        void(*m_default_construct_holder)(void*);
        void(*m_default_construct_const_holder)(void*);

        int m_holder_size;
        int m_holder_alignment;

        LUABIND_TYPE_INFO m_type;
        LUABIND_TYPE_INFO m_holder_type;
        LUABIND_TYPE_INFO m_const_holder_type;

        scope m_scope;
    };

    class_registration::class_registration(char const* name)
    {
        m_name = name;
    }

    void class_registration::register_(lua_State* L) const
    {
        lua_pushstring(L, m_name);

        detail::class_rep* crep;

        detail::class_registry* r = detail::class_registry::get_registry(L);
        // create a class_rep structure for this class.
        // allocate it within lua to let lua collect it on
        // lua_close(). This is better than allocating it
        // as a static, since it will then be destructed
        // when the program exits instead.
        // warning: we assume that lua will not
        // move the userdata memory.
        lua_newuserdata(L, sizeof(detail::class_rep));
        crep = reinterpret_cast<detail::class_rep*>(lua_touserdata(L, -1));

        new(crep) detail::class_rep(
            m_type
            , m_name
            , L
            , m_destructor
            , m_const_holder_destructor
            , m_holder_type
            , m_const_holder_type
            , m_extractor
            , m_const_extractor
            , m_const_converter
            , m_construct_holder
            , m_construct_const_holder
            , m_default_construct_holder
            , m_default_construct_const_holder
            , m_holder_size
            , m_holder_alignment);

        // register this new type in the class registry
        r->add_class(m_type, crep);
        if (!(LUABIND_TYPE_INFO_EQUAL(m_holder_type, LUABIND_INVALID_TYPE_INFO)))
        {
            // if we have a held type
            // we have to register it in the class-table
            // but only for the base class, if it already
            // exists, we don't have to register it
            detail::class_rep* c = r->find_class(m_holder_type);
            if (c == 0)
            {
                r->add_class(m_holder_type, crep);
                r->add_class(m_const_holder_type, crep);
            }
        }

        // constructors
        m_constructor.swap(crep->m_constructor);

        crep->m_getters.swap(m_getters);
        crep->m_setters.swap(m_setters);

        for(int i = 0; i < detail::number_of_operators; ++i)
            crep->m_operators[i].swap(m_operators[i]);

        crep->m_static_constants.swap(m_static_constants);

        // here!
        //            crep->m_methods = m_methods;

        for (std::vector<class_base::base_desc>::iterator i = m_bases.begin();
            i != m_bases.end(); 
            ++i)
        {
            detail::class_registry* r = detail::class_registry::get_registry(L);

            // the baseclass' class_rep structure
            detail::class_rep* bcrep = r->find_class(i->type);

            detail::class_rep::base_info base;
            base.pointer_offset = i->ptr_offset;
            base.base = bcrep;

            crep->add_base_class(base);

            typedef std::map<const char*, detail::method_rep, detail::ltstr> methods_t;

            for (methods_t::const_iterator i 
                = bcrep->m_methods.begin()
                ; i != bcrep->m_methods.end()
                ; ++i)
            {
                detail::method_rep& m = m_methods[i->first];

                typedef std::vector<detail::overload_rep> overloads_t;

                for (overloads_t::const_iterator j
                    = i->second.overloads().begin()
                    ; j != i->second.overloads().end()
                    ; ++j)
                {
                    detail::overload_rep o = *j;
                    o.add_offset(base.pointer_offset);
                    m.add_overload(o);
                }
            }

            // copy base class table
            detail::getref(L, crep->table_ref());
            detail::getref(L, bcrep->table_ref());
            lua_pushnil(L);

            while (lua_next(L, -2))
            {
                lua_pushvalue(L, -2); // copy key
                lua_insert(L, -2);
                lua_settable(L, -5);
            }

            lua_pop(L, 2);
        }

        crep->m_methods = m_methods;

        for (std::map<const char*, detail::method_rep, detail::ltstr>::iterator i 
            = crep->m_methods.begin(); i != crep->m_methods.end(); ++i)
        {
            i->second.crep = crep;
        }

        // add methods
        for (std::map<const char*, detail::method_rep, detail::ltstr>::iterator i 
            = m_methods.begin(); i != m_methods.end(); ++i)
        {
            detail::getref(L, crep->table_ref());
            lua_pushstring(L, i->first);
            lua_pushnil(L);
            lua_settable(L, -3);
            lua_pop(L, 1);

            crep->add_method(L, i->first, crep->m_methods[i->first]);
            i->second.crep = crep;
        }

        m_methods.clear();

        detail::getref(L, crep->table_ref());
        m_scope.register_(L);
        lua_pop(L, 1);

        lua_settable(L, -3);
    }
    
    // -- interface ---------------------------------------------------------

    class_base::class_base(char const* name)
        : scope(std::auto_ptr<registration>(
                m_registration = new class_registration(name))
          )
    {
    }

    void class_base::init(
        LUABIND_TYPE_INFO type_
        , LUABIND_TYPE_INFO holder_type
        , LUABIND_TYPE_INFO const_holder_type
        , void*(*extractor)(void*)
        , const void*(*const_extractor)(void*)
        , void(*const_converter_)(void*,void*)
        , void(*holder_constructor_)(void*,void*)
        , void(*const_holder_constructor_)(void*,void*)
        , void(*holder_default_constructor_)(void*)
        , void(*const_holder_default_constructor_)(void*)
        , void(*destructor)(void*)
        , void(*const_holder_destructor)(void*)
        , int holder_size
        , int holder_alignment)
    {
        m_registration->m_type = type_;
        m_registration->m_holder_type = holder_type;
        m_registration->m_const_holder_type = const_holder_type;
        m_registration->m_extractor = extractor;
        m_registration->m_const_extractor = const_extractor;
        m_registration->m_const_converter = const_converter_;
        m_registration->m_construct_holder = holder_constructor_;
        m_registration->m_construct_const_holder = const_holder_constructor_;
        m_registration->m_default_construct_holder = holder_default_constructor_;
        m_registration->m_default_construct_const_holder = const_holder_default_constructor_;
        m_registration->m_destructor = destructor;
        m_registration->m_const_holder_destructor = const_holder_destructor;
        m_registration->m_holder_size = holder_size;
        m_registration->m_holder_alignment = holder_alignment;
    }

    void class_base::add_getter(
        const char* name, const boost::function2<int, lua_State*, int>& g)
    {
        detail::class_rep::callback c;
        c.func = g;
        c.pointer_offset = 0;

        const char* key = name;
        m_registration->m_getters[key] = c;
    }

#ifdef LUABIND_NO_ERROR_CHECKING
    void class_base::add_setter(
        const char* name
        , const boost::function2<int, lua_State*, int>& s)
#else
    void class_base::add_setter(
        const char* name
        , const boost::function2<int, lua_State*, int>& s
        , int (*match)(lua_State*, int)
        , void (*get_sig_ptr)(lua_State*, std::string&))
#endif
    {
        detail::class_rep::callback c;
        c.func = s;
        c.pointer_offset = 0;

#ifndef LUABIND_NO_ERROR_CHECKING
        c.match = match;
        c.sig = get_sig_ptr;
#endif


        const char* key = name;
        m_registration->m_setters[key] = c;
    }

    void class_base::add_base(const base_desc& b)
    {
        m_registration->m_bases.push_back(b);
    }

    void class_base::add_constructor(const detail::construct_rep::overload_t& o)
    {
        m_registration->m_constructor.overloads.push_back(o);
    }

    void class_base::add_method(const char* name, const detail::overload_rep& o)
    {
        detail::method_rep& method = m_registration->m_methods[name];
        method.name = name;
        method.add_overload(o);
        method.crep = 0;
    }

#ifndef LUABIND_NO_ERROR_CHECKING
    void class_base::add_operator(
        int op_id,  int(*func)(lua_State*), int(*matcher)(lua_State*)
        , void(*sig)(lua_State*, std::string&), int arity)
#else
    void class_base::add_operator(
        int op_id,  int(*func)(lua_State*)
        , int(*matcher)(lua_State*), int arity)
#endif
    {
        detail::class_rep::operator_callback o;
        o.set_fun(func);
        o.set_match_fun(matcher);
        o.set_arity(arity);

#ifndef LUABIND_NO_ERROR_CHECKING

        o.set_sig_fun(sig);

#endif
        m_registration->m_operators[op_id].push_back(o);
    }

    const char* class_base::name() const 
    { 
        return m_registration->m_name; 
    }

    void class_base::add_static_constant(const char* name, int val)
    {
        m_registration->m_static_constants[name] = val;
    }

    void class_base::add_inner_scope(scope& s)
    {
        m_registration->m_scope.operator,(s);
    }

    std::string get_class_name(lua_State* L, LUABIND_TYPE_INFO i)
    {
        std::string ret;
        class_registry* r = class_registry::get_registry(L);
        class_rep* crep = r->find_class(i);

        if (crep == 0)
        {
            ret = "custom";
        }
        else
        {
            if (LUABIND_TYPE_INFO_EQUAL(i, crep->holder_type()))
            {
                ret += "smart_ptr<";
                ret += crep->name();
                ret += ">";
            }
            else if (LUABIND_TYPE_INFO_EQUAL(i, crep->const_holder_type()))
            {
                ret += "smart_ptr<const ";
                ret += crep->name();
                ret += ">";
            }
            else
            {
                ret += crep->name();
            }
        }
        return ret;
    };

}} // namespace luabind::detail

