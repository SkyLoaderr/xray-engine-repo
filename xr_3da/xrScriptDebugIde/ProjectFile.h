// ProjectFile.h: interface for the CProjectFile class.
//
#pragma once

class CLuaEditor;
class CMailSlotMsg;

class CProjectFile
{
public:
	BOOL		Save(CArchive& ar);
	BOOL		Load(CArchive& ar);
	void		RemoveBreakPoint(int nLine);
	BOOL		HasFile(CString strPathName);
	void		SetBreakPointsIn(CLuaEditor* pEditor);
	BOOL		HasBreakPoint(int nLine);
				CProjectFile();
	virtual		~CProjectFile();

	void		AddDebugLine(int nLine);
	void		RemoveAllDebugLines();
	void		AddBreakPoint(int nLine);
	void		RemoveAllBreakPoints();

	BOOL		PositionBreakPoints();
	int			GetNearestDebugLine(int nLine);
	int			GetPreviousDebugLine(int nLine);
	int			GetNextDebugLine(int nLine);

	void		SetPathName(CString strPathName) { m_strPathName=strPathName;/* UpdateRelPathName();*/ };
	CString		GetPathName() { return m_strPathName; };
	CString		GetName();
	static CString	GetName(CString&);
	CString		GetNameExt();
	void		FillBreakPoints(CMailSlotMsg*);
	BOOL		HasBreakPoint(){return m_breakPoints.GetSize();};
	BOOL		m_bBreakPointsSaved;
	CLuaView*	m_lua_view;
protected:
	CString							m_strPathName;
	CMap<int, int, BOOL, BOOL>		m_breakPoints;
	int								m_nMinBreakPoint;
	int								m_nMaxBreakPoint;
	CMap<int, int, BOOL, BOOL>		m_debugLines;
	int								m_nMinDebugLine;
	int								m_nMaxDebugLine;
};
