// Project.h: interface for the CProject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT_H__07580CB8_BA8B_47B6_9813_78E51B8C971C__INCLUDED_)
#define AFX_PROJECT_H__07580CB8_BA8B_47B6_9813_78E51B8C971C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProjectFile.h"

class CWorkspaceWnd;

typedef CTypedPtrArray<CPtrArray, CProjectFile*> CProjectFileArray;
#define PROJ_VERSION 0x03

class CProject  
{
public:
	void RemoveFile(CProjectFile* pPF);
	BOOL Close();
	BOOL New();
	BOOL HasBreakPoint(const char* szFile, int nLine);
	CProject();
	virtual ~CProject();

	BOOL PositionBreakPoints();
	BOOL CheckBeforeBuild();

	void SetModifiedFlag(BOOL bModified);
	void SaveModified();
	BOOL SaveAs();
	BOOL Save();
	BOOL Save(CString strPathName);
	BOOL Save(CArchive& ar);
	BOOL Load();
	BOOL Load(CString);

	void RemoveProjectFiles();
	void OnRunApplication();
	void OnDebugOptions();
	void AddFile(CString strPathName, BOOL bFindExisting=TRUE);
	void AddFile(CProjectFile* pPF);
	int NofFiles() { return m_files.GetSize(); };
	void AddFiles();
	CProjectFile* GetProjectFile(CString strPathName);
	void RedrawFilesTree();

	CString GetName(){return m_strName;};
	void	SetName(LPCSTR s){m_strName = s;};
	void FillBreakPoints(CMailSlotMsg* msg);
protected:
	BOOL Load(CArchive& ar);

	BOOL m_bModified;

	CProjectFileArray	m_files;
	CString m_strName;

	CString m_command;
	CString m_arguments;
	CString m_working_dir;

};

#endif // !defined(AFX_PROJECT_H__07580CB8_BA8B_47B6_9813_78E51B8C971C__INCLUDED_)
