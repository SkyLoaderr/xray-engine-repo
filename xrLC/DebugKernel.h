// DebugKernel.h: interface for the CDebugKernel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGKERNEL_H__F5F12724_363E_42F8_85B5_BAC2BB9B186D__INCLUDED_)
#define AFX_DEBUGKERNEL_H__F5F12724_363E_42F8_85B5_BAC2BB9B186D__INCLUDED_
#pragma once

class CDebugKernel
{
private:
	vector<string>	Stack;

	BOOL            GetFunctionName(HINSTANCE instance, void *pointer, char* fn_name);
	int             UpdateStack(EXCEPTION_POINTERS *pex, int iSkip=3 );
public:
	BOOL			Start	();
	BOOL			Stop	();

	int				LogStack(EXCEPTION_POINTERS *pex);
	void            Update	();
	DWORD           GetCount()			{return Stack.size();}
    const char*		GetName	(DWORD num)	{return Stack[num].c_str();  }
};

extern CDebugKernel Debug;

// ---==( Extended Debugging Support (R) )==---
ENGINE_API void __fastcall _verify(const char *expr, char *file, int line);
#define R_ASSERT(expr)	if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define R_CHK(expr)		{ HRESULT hr = expr; if (FAILED(hr)) Device.Error(hr, __FILE__, __LINE__); }

#ifdef DEBUG
#define	NODEFAULT	VERIFY2(0,"nodefault: reached")
#define VERIFY(expr) if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define VERIFY2(expr, info) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s",#expr,info); _verify(buf, __FILE__, __LINE__); }
#define VERIFY3(expr, info, info2) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s, %s",#expr,info,info2); _verify(buf, __FILE__, __LINE__); }
template <class T> T* _P(T* p) {
	VERIFY(p);
	VERIFY(_CrtIsValidHeapPointer(p));
	return p;
}
#define CHK_DX(a)			{ HRESULT hr = a; if (FAILED(hr)) Device.Error(hr, __FILE__, __LINE__); }
#else
#define NODEFAULT   __assume(0)
#define VERIFY(expr)
#define VERIFY2(expr, info)
#define VERIFY3(expr, info, info2)
#define _P(s) s
#define CHK_DX(a)			a
#endif

//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
//---------------------------------------------------------------------------------------------
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n"           \
        " ------------------------------------------------\n" \
        "|  TODO :   " #x "\n" \
        " -------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
        " ------------------------------------------------\n" \
        "|  FIXME :  " #x "\n" \
        " -------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 


#endif // !defined(AFX_DEBUGKERNEL_H__F5F12724_363E_42F8_85B5_BAC2BB9B186D__INCLUDED_)
