#pragma once

struct SPath{
	string_path path;
};

class CScriptCallStack
{
public:
	void GotoStackTraceLevel(int nLevel);
	void Add(const char* szDesc, const char* szFile, int nLine);
	void Clear();
	CScriptCallStack();
	~CScriptCallStack();

//	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT);
//	virtual int OnSci(CScintillaView* pView, SCNotification* pNotify);
	int		GetLevel() { return m_nCurrentLevel; };
	void	SetStackTraceLevel(int);
protected:
	int m_nCurrentLevel;
	xr_vector<u32>	m_levels;
	xr_vector<u32>	m_lines;
//	CUIntArray	m_levels, m_lines;
	xr_vector<SPath>	m_files;
//	CStringArray m_files;
};
