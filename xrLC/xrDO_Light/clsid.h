#ifndef _CLSID_H
#define _CLSID_H

#pragma once

//***** CLASS ID type
typedef unsigned __int64	CLASS_ID;
#define MK_CLSID(a,b,c,d,e,f,g,h) \
    	CLASS_ID((CLASS_ID(a)<<CLASS_ID(56))|(CLASS_ID(b)<<CLASS_ID(48))|(CLASS_ID(c)<<CLASS_ID(40))|(CLASS_ID(d)<<CLASS_ID(32))|(CLASS_ID(e)<<CLASS_ID(24))|(CLASS_ID(f)<<CLASS_ID(16))|(CLASS_ID(g)<<CLASS_ID(8))|(CLASS_ID(h)))

#define MK_CLSID_INV(a,b,c,d,e,f,g,h) MK_CLSID(h,g,f,e,d,c,b,a)

extern ENGINE_API void		CLSID2TEXT(CLASS_ID id, char *text);
extern ENGINE_API CLASS_ID	TEXT2CLSID(const char *text);

#endif