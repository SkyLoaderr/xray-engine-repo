#ifndef _DEFINES_H_
#define _DEFINES_H_

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _SHOW_REF(msg, x)   { VERIFY(x); x->AddRef(); DWORDToFile(msg,x->Release()); }
#define THROW				throw 123456789

#endif
