//----------------------------------------------------
// file: NetDeviceLog.h
//----------------------------------------------------

#ifndef _INCDEF_NETDEVICELOG_H_
#define _INCDEF_NETDEVICELOG_H_

// -------
#define NLOG_CONSOLE_OUT
// -------

using namespace std;


class NetLog{
protected:
	char m_FileName[MAX_PATH];
public:
	NetLog( char *_FileName );
	void __cdecl Msg( char *_Format, ... );
};

class NetDeviceConsole{
protected:

	bool m_Valid;
	HANDLE m_hThread;
	DWORD m_ThreadId;


public:

	HWND m_hParent;
	HWND m_hWindow;
	HINSTANCE m_hInstance;

	CRITICAL_SECTION m_CSection;

	bool m_Enter;
	char m_EnterBuffer[256];

	class _ConsoleMsg{
	public:
		char buf[1024];
		_ConsoleMsg(){ buf[0] = 0; }
		_ConsoleMsg(_ConsoleMsg*_S){strcpy(buf,_S->buf);} };
		
	list<_ConsoleMsg> m_Messages;

public:

	bool Init( HINSTANCE _Inst, HWND _Window );
	void Clear();

	void __cdecl print( char *_Mesage, ... );
	bool in( char *_Buffer );
	bool cmdtest( char *_Buffer );
	bool valid();

	NetDeviceConsole();
	~NetDeviceConsole();
};

extern NetLog NLog;
extern NetDeviceConsole NConsole;


#endif /*_INCDEF_NETDEVICELOG_H_*/

