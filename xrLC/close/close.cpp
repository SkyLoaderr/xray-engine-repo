// close.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "close.h"

#pragma comment(lib,"x:\\xrCore.lib")

bool WriteSlot	(char *name, char* dest, char *msg) 
{
	HANDLE	hFile; 
	DWORD	cbWritten;
	BOOL	fResult;
	char    cName[256];

	sprintf(cName,"\\\\%s\\mailslot\\%s",name,dest);
	hFile = CreateFile(
		cName, 
		GENERIC_WRITE, 
		FILE_SHARE_READ,  // required to write to a mailslot 
		(LPSECURITY_ATTRIBUTES) NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL);  
	if (hFile == INVALID_HANDLE_VALUE) 	return FALSE;
	fResult = WriteFile(
		hFile, 
		msg, 
		(u32) lstrlen(msg) + 1,
		&cbWritten,     
		(LPOVERLAPPED) NULL);  
	if (!fResult) return FALSE;
	fResult = CloseHandle(hFile);
	if (!fResult) return FALSE;
	return TRUE; 
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// Users
	BOOL		bHighPriority	= FALSE;
	char		u_name[128];
	DWORD		u_size	= sizeof(u_name)-1;
	GetUserName	(u_name,&u_size);
	_strlwr		(u_name);
	if ((0==strcmp(u_name,"oles"))||(0==strcmp(u_name,"alexmx")||(0==strcmp(u_name,"dima"))||(0==strcmp(u_name,"version"))))	bHighPriority	= TRUE;

	// Send
	if (bHighPriority) 
	{
		char*		dest	= lpCmdLine;
		char*		msg 	= strchr(lpCmdLine,' ');
		if (0==msg)			return 1;
		*msg++				= 0;
		WriteSlot	("*",dest,msg);
	}

	//
	return 0;
}
