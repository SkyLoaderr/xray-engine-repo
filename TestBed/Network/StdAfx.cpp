// stdafx.cpp : source file that includes just the standard includes
//	NetWork.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#pragma comment(lib,"DxErr8.lib")
#pragma comment(lib,"dplayx.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

#include "dxerr8.h"
#include "conio.h"
void _error(HRESULT hr,LPCSTR file,int line)
{
	char errmsg_buf[1024];

	const char *errStr = DXGetErrorString8A(hr);
	if (errStr==0) {
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,errmsg_buf,1024,0);
		errStr = errmsg_buf;
	}

	printf	("<* ERROR *>: F(%s), L(%d)\n%s\n(A)bort/(D)ebug/(I)gnore?",file,line,errStr);
	switch	(tolower(getch()))
	{
	case 'a':	abort();		break;
	case 'd':	__asm int 3;	break;
	case 'i':
	default:
		break;
	}
}
