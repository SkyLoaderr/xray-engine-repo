#pragma once

struct lua_State;
struct Proto;
struct lua_Debug;
class CScriptFile;

class CDbgLuaHelper  
{
public:
	void		RestoreGlobals				();
	void		CoverGlobals				();
	void		Describe					(char* szRet, int nIndex);
	BOOL		Eval						(const char* szCode, char* szRet);
	BOOL		GetCalltip					(const char *szWord, char *szCalltip);
	void		DrawGlobalVariables			();
	void		DrawLocalVariables			();
	const char* GetSource					();
/*
	static BOOL ErrorStringToFileLine(CString strError, CString &strPathName, int &nLine);
*/
	static BOOL LoadDebugLines(CScriptFile* pPF);

	CDbgLuaHelper							();
	virtual ~CDbgLuaHelper					();

	// debugger functions
	BOOL		PrepareDebugger				(lua_State* l);
	BOOL		StartDebugger				();	
	void		StopDebugger				();

	void		DrawStackTrace				();

	lua_State*  GetState					(){ return L; };
	void		SetState					(lua_State* l);
protected:
	void		Free						();
	static CDbgLuaHelper*					m_pThis;

	static int  OutputTop					(lua_State* L);
	static int  errormessage				(lua_State* L);
	static int  lua_loadlib					(lua_State* L);
	static void line_hook					(lua_State *L, lua_Debug *ar);
	static void func_hook					(lua_State *L, lua_Debug *ar);
	static void hook						(lua_State *L, lua_Debug *ar);

	lua_State*								L;
	lua_Debug*								m_pAr;
/*
	HMODULE m_hLoaded[8192];
	int m_nLoaded;
*/
};
