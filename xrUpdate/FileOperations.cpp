/** \file	FileOperations.cpp
			Project: FopDemo\n
			Project type: MFC App\n
			Author: Vinnichenko Alexey\n
			E-mail: subj@mail.ru\n
			Description: Implementation of CFileOperation class and CFileExeption class.
*/ 

#include "stdafx.h" 
#include "resource.h" 
#include "FileOperations.h" 

//************************************************************************************************************
CFExeption::CFExeption(DWORD dwErrCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			      NULL, dwErrCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	m_sError = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
	m_dwError = dwErrCode;
}


CFExeption::CFExeption(CString sErrText)
{
	m_sError = sErrText;
	m_dwError = 0;
}


//************************************************************************************************************

CFileOperation::CFileOperation()
{
	Initialize();
}


void CFileOperation::Initialize()
{
	m_sError = _T("No error");
	m_dwError = 0;
	m_bAskIfReadOnly = true;
	m_bOverwriteMode = false;
	m_bAborted = false;
	m_iRecursionLimit = -1;
}


void CFileOperation::DoDelete(CString sPathName)
{
	CFileFind ff;
	CString sPath = sPathName;

	if (CheckPath(sPath) == PATH_IS_FILE)
	{
		if (!CanDelete(sPath)) 
		{
			m_bAborted = true;
			return;
		}
		if (!DeleteFile(sPath)) throw new CFExeption(GetLastError());
		return;
	}

	PreparePath(sPath);
	sPath += "*.*";

	BOOL bRes = ff.FindFile(sPath);
	while(bRes)
	{
		bRes = ff.FindNextFile();
		if (ff.IsDots()) continue;
		if (ff.IsDirectory())
		{
			sPath = ff.GetFilePath();
			DoDelete(sPath);
		}
		else DoDelete(ff.GetFilePath());
	}
	ff.Close();
	if (!RemoveDirectory(sPathName) && !m_bAborted) throw new CFExeption(GetLastError());
}


void CFileOperation::DoFolderCopy(CString sSourceFolder, CString sDestFolder, bool bDelteAfterCopy)
{
	CFileFind ff;
	CString sPathSource = sSourceFolder;
	BOOL bRes = ff.FindFile(sPathSource);
	while (bRes)
	{
		bRes = ff.FindNextFile();
		if (ff.IsDots()) continue;
		if (ff.IsDirectory()) // source is a folder
		{
			if (m_iRecursionLimit == 0) continue;
			sPathSource = ff.GetFilePath() + CString("\\") + CString("*.*");
			CString sPathDest = sDestFolder + ff.GetFileName() + CString("\\");
			if (CheckPath(sPathDest) == PATH_NOT_FOUND) 
			{
				if (!CreateDirectory(sPathDest, NULL))
				{
					ff.Close();
					throw new CFExeption(GetLastError());
				}
			}
			if (m_iRecursionLimit > 0) m_iRecursionLimit --;
			DoFolderCopy(sPathSource, sPathDest, bDelteAfterCopy);
		}
		else // source is a file
		{
			CString sNewFileName = sDestFolder + ff.GetFileName();
			DoFileCopy(ff.GetFilePath(), sNewFileName, bDelteAfterCopy);
		}
	}
	ff.Close();
}


bool CFileOperation::Delete(CString sPathName)
{
	try
	{
		DoDelete(sPathName);
	}
	catch(CFExeption* e)
	{
		m_sError = e->GetErrorText();
		m_dwError = e->GetErrorCode();
		delete e;
		if (m_dwError == 0) return true;
		return false;
	}
	return true;
}


bool CFileOperation::Rename(CString sSource, CString sDest)
{
	try
	{
		DoRename(sSource, sDest);
	}
	catch(CFExeption* e)
	{
		m_sError = e->GetErrorText();
		m_dwError = e->GetErrorCode();
		delete e;
		return false;
	}
	return true;
}


void CFileOperation::DoRename(CString sSource, CString sDest)
{
	if (!MoveFile(sSource, sDest)) throw new CFExeption(GetLastError());
}


void CFileOperation::DoCopy(CString sSource, CString sDest, bool bDelteAfterCopy)
{
	CheckSelfRecursion(sSource, sDest);
	// source not found
	if (CheckPath(sSource) == PATH_NOT_FOUND)
	{
		CString sError = sSource + CString(" not found");
		throw new CFExeption(sError);
	}
	// dest not found
	if (CheckPath(sDest) == PATH_NOT_FOUND)
	{
		CString sError = sDest + CString(" not found");
		throw new CFExeption(sError);
	}
	// folder to file
	if (CheckPath(sSource) == PATH_IS_FOLDER && CheckPath(sDest) == PATH_IS_FILE) 
	{
		throw new CFExeption("Wrong operation");
	}
	// folder to folder
	if (CheckPath(sSource) == PATH_IS_FOLDER && CheckPath(sDest) == PATH_IS_FOLDER) 
	{
		CFileFind ff;
		CString sError = sSource + CString(" not found");
		PreparePath(sSource);
		PreparePath(sDest);
		sSource += "*.*";
		if (!ff.FindFile(sSource)) 
		{
			ff.Close();
			throw new CFExeption(sError);
		}
		if (!ff.FindNextFile()) 
		{
			ff.Close();
			throw new CFExeption(sError);
		}
		CString sFolderName = ParseFolderName(sSource);
		if (!sFolderName.IsEmpty()) // the source is not drive
		{
			sDest += sFolderName;
			PreparePath(sDest);
			if (!CreateDirectory(sDest, NULL))
			{
				DWORD dwErr = GetLastError();
				if (dwErr != 183)
				{
					ff.Close();
					throw new CFExeption(dwErr);
				}
			}
		}
		ff.Close();
		DoFolderCopy(sSource, sDest, bDelteAfterCopy);
	}
	// file to file
	if (CheckPath(sSource) == PATH_IS_FILE && CheckPath(sDest) == PATH_IS_FILE) 
	{
		DoFileCopy(sSource, sDest);
	}
	// file to folder
	if (CheckPath(sSource) == PATH_IS_FILE && CheckPath(sDest) == PATH_IS_FOLDER) 
	{
		PreparePath(sDest);
		char drive[MAX_PATH], dir[MAX_PATH], name[MAX_PATH], ext[MAX_PATH];
		_splitpath(sSource, drive, dir, name, ext);
		sDest = sDest + CString(name) + CString(ext);
		DoFileCopy(sSource, sDest);
	}
}


void CFileOperation::DoFileCopy(CString sSourceFile, CString sDestFile, bool bDelteAfterCopy)
{
	BOOL bOvrwriteFails = FALSE;
	if (!m_bOverwriteMode)
	{
		while (IsFileExist(sDestFile)) 
		{
			sDestFile = ChangeFileName(sDestFile);
		}
		bOvrwriteFails = TRUE;
	}
	if (!CopyFile(sSourceFile, sDestFile, bOvrwriteFails)) throw new CFExeption(GetLastError());
	if (bDelteAfterCopy)
	{
		DoDelete(sSourceFile);
	}
}


bool CFileOperation::Copy(CString sSource, CString sDest)
{
	if (CheckSelfCopy(sSource, sDest)) return true;
	bool bRes;
	try
	{
		DoCopy(sSource, sDest);
		bRes = true;
	}
	catch(CFExeption* e)
	{
		m_sError = e->GetErrorText();
		m_dwError = e->GetErrorCode();
		delete e;
		if (m_dwError == 0) bRes = true;
		bRes = false;
	}
	m_iRecursionLimit = -1;
	return bRes;
}


bool CFileOperation::Replace(CString sSource, CString sDest)
{
	if (CheckSelfCopy(sSource, sDest)) return true;
	bool bRes;
	try
	{
		bool b = m_bAskIfReadOnly;
		m_bAskIfReadOnly = false;
		DoCopy(sSource, sDest, true);
		DoDelete(sSource);
		m_bAskIfReadOnly = b;
		bRes = true;
	}
	catch(CFExeption* e)
	{
		m_sError = e->GetErrorText();
		m_dwError = e->GetErrorCode();
		delete e;
		if (m_dwError == 0) bRes = true;
		bRes = false;
	}
	m_iRecursionLimit = -1;
	return bRes;
}


CString CFileOperation::ChangeFileName(CString sFileName)
{
	CString sName, sNewName, sResult;
	char drive[MAX_PATH];
	char dir  [MAX_PATH];
	char name [MAX_PATH];
	char ext  [MAX_PATH];
	_splitpath((LPCTSTR)sFileName, drive, dir, name, ext);
	sName = name;

	int pos = sName.Find("Copy ");
	if (pos == -1)
	{
		sNewName = CString("Copy of ") + sName + CString(ext);
	}
	else
	{
		int pos1 = sName.Find('(');
		if (pos1 == -1)
		{
			sNewName = sName;
			sNewName.Delete(0, 8);
			sNewName = CString("Copy (1) of ") + sNewName + CString(ext);
		}
		else
		{
			CString sCount;
			int pos2 = sName.Find(')');
			if (pos2 == -1)
			{
				sNewName = CString("Copy of ") + sNewName + CString(ext);
			}
			else
			{
				sCount = sName.Mid(pos1 + 1, pos2 - pos1 - 1);
				sName.Delete(0, pos2 + 5);
				int iCount = atoi((LPCTSTR)sCount);
				iCount ++;
				sNewName.Format("%s%d%s%s%s", "Copy (", iCount, ") of ", (LPCTSTR)sName, ext);
			}
		}
	}

	sResult = CString(drive) + CString(dir) + sNewName;

	return sResult;
}


bool CFileOperation::IsFileExist(CString sPathName)
{
	HANDLE hFile;
	hFile = CreateFile(sPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	CloseHandle(hFile);
	return true;
}


int CFileOperation::CheckPath(CString sPath)
{
	DWORD dwAttr = GetFileAttributes(sPath);
	if (dwAttr == 0xffffffff) 
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND) 
			return PATH_NOT_FOUND;
		return PATH_ERROR;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) return PATH_IS_FOLDER;
	return PATH_IS_FILE;
}


void CFileOperation::PreparePath(CString &sPath)
{
	if(sPath.Right(1) != "\\") sPath += "\\";
}


bool CFileOperation::CanDelete(CString sPathName)
{
	DWORD dwAttr = GetFileAttributes(sPathName);
	if (dwAttr == -1) return false;
	if (dwAttr & FILE_ATTRIBUTE_READONLY)
	{
		if (m_bAskIfReadOnly)
		{
			CString sTmp = sPathName;
			int pos = sTmp.ReverseFind('\\');
			if (pos != -1) sTmp.Delete(0, pos + 1);
			CString sText = sTmp + CString(" is read olny. Do you want delete it?");
			int iRes = MessageBox(NULL, sText, _T("Warning"), MB_YESNOCANCEL | MB_ICONQUESTION);
			switch (iRes)
			{
				case IDYES:
				{
					if (!SetFileAttributes(sPathName, FILE_ATTRIBUTE_NORMAL)) return false;
					return true;
				}
				case IDNO:
				{
					return false;
				}
				case IDCANCEL:
				{
					m_bAborted = true;
					throw new CFExeption(0);
					return false;
				}
			}
		}
		else
		{
			if (!SetFileAttributes(sPathName, FILE_ATTRIBUTE_NORMAL)) return false;
			return true;
		}
	}
	return true;
}


CString CFileOperation::ParseFolderName(CString sPathName)
{
	CString sFolderName = sPathName;
	int pos = sFolderName.ReverseFind('\\');
	if (pos != -1) sFolderName.Delete(pos, sFolderName.GetLength() - pos);
	pos = sFolderName.ReverseFind('\\');
	if (pos != -1) sFolderName = sFolderName.Right(sFolderName.GetLength() - pos - 1);
	else sFolderName.Empty();
	return sFolderName;
}


void CFileOperation::CheckSelfRecursion(CString sSource, CString sDest)
{
	if (sDest.Find(sSource) != -1)
	{
		int i = 0, count1 = 0, count2 = 0;
		for(i = 0; i < sSource.GetLength(); i ++)	if (sSource[i] == '\\') count1 ++;
		for(i = 0; i < sDest.GetLength(); i ++)	if (sDest[i] == '\\') count2 ++;
		if (count2 >= count1) m_iRecursionLimit = count2 - count1;
	}
}


bool CFileOperation::CheckSelfCopy(CString sSource, CString sDest)
{
	bool bRes = false;
	if (CheckPath(sSource) == PATH_IS_FOLDER)
	{
		CString sTmp = sSource;
		int pos = sTmp.ReverseFind('\\');
		if (pos != -1)
		{
			sTmp.Delete(pos, sTmp.GetLength() - pos);
			if (sTmp.CompareNoCase(sDest) == 0) bRes = true;
		}
	}
	return bRes;
}