#ifndef _DEFINES_H_
#define _DEFINES_H_

typedef	char		FILE_NAME	[ _MAX_PATH	];

#define _CROP(x,l,h)			if((x)<(l)) (x)=(l);else if((x)>(h)) (x)=(h);
#define randf()					( float(rand())/float(RAND_MAX) )
#define X_TO_REAL(_X_)			((_X_)+1.f)*float(Device.dwWidth/2)
#define Y_TO_REAL(_Y_)			((_Y_)+1.f)*float(Device.dwHeight/2)

// 'cause delete(NULL) is valid - there is no sence to check for NULL :)
#define _DELETE(x)			{ delete x;	(x)=NULL; }
#define _DELETEARRAY(x)		{ delete[] x;	(x)=NULL; }
#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _FREE(x)			{ if(x) { free(x);              (x)=NULL; } }
#define _SHOW_REF(msg, x)   { VERIFY(x); x->AddRef(); DWORDToFile(msg,x->Release()); }
#define THROW				throw 123456789

extern void InterpretError(HRESULT hr, const char *file, int line);


#endif
