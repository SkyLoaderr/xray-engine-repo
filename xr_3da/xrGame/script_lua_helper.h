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
	int			PrepareLua					(lua_State* );
	void		PrepareLuaBind				();


	void		DrawStackTrace				();
	static int  OutputTop					(lua_State* );

	static void hookLua						(lua_State *, lua_Debug *);
	static int  hookLuaBind					(lua_State *);

protected:
	static		CDbgLuaHelper*				m_pThis;

	static int	errormessageLua				(lua_State* );
	static void errormessageLuaBind			(lua_State* );
	static void line_hook					(lua_State *, lua_Debug *);
	static void func_hook					(lua_State *, lua_Debug *);

	static lua_State*						L;
	lua_Debug*								m_pAr;
};
