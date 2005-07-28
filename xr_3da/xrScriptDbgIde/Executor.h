// Executor.h: interface for the CExecutor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXECUTOR_H__B467D883_9302_4A7E_810B_058B60881E4A__INCLUDED_)
#define AFX_EXECUTOR_H__B467D883_9302_4A7E_810B_058B60881E4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExecutor  
{
public:
	BOOL SaveOutput(CString strPathName);
	CString GetOutputString();
	CExecutor();
	virtual ~CExecutor();
	BOOL Execute(CString strCmdLine, CMemFile* pInput=NULL);

protected:
	void Close();
	void PrepAndLaunchRedirectedChild(HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr,
											 CString strCmdLine);
	BOOL Write(LPBYTE lpData, int nSize);
	static DWORD WINAPI ReadAndHandleOutput(LPVOID lpvThreadParam);

	HANDLE m_hOutputRead, m_hInputWrite;
	HANDLE m_hThread;
	CMemFile m_output;
};

#endif // !defined(AFX_EXECUTOR_H__B467D883_9302_4A7E_810B_058B60881E4A__INCLUDED_)
