

//---------------------------------------------------------------------------
extern	HRESULT		HRESULT_Proc(HRESULT ret);
extern	ULONG		ULONG_Proc(ULONG ret);
extern	UINT		UINT_Proc(UINT ret);
extern	DWORD		DWORD_Proc(DWORD ret);
extern	BOOL		BOOL_Proc(BOOL ret);
extern	float		FLOAT_Proc(float ret);
extern	void		VOID_proc();
//---------------------------------------------------------------------------
extern	void		LogOut( const char *format, ... );

#ifdef _DEBUG
#define APIDEBUG(str) LogOut("---------------------"#str"-------------------------\n")
#else
#define APIDEBUG(str)
#endif