//----------------------------------------------------
// file: NetDeviceLog.h
//----------------------------------------------------

#ifndef _INCDEF_NETDEVICELOG_H_
#define _INCDEF_NETDEVICELOG_H_

// -------
#define NLOG_CONSOLE_OUT
// -------

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
		
		std::list<_ConsoleMsg> m_Messages;

	float fMaxVal, fStatusProgress;
public:

	bool Init( HINSTANCE _Inst, HWND _Window );
	void Clear();

	void print	(const char *_Mesage, ...);

//	bool in( char *_Buffer );
//	bool cmdtest( char *_Buffer );
	bool valid();

	void ProgressStart(float max_val, const char* text=0);
	void ProgressEnd();
	void ProgressInc();
	void ProgressUpdate(float val);

	NetDeviceConsole();
	~NetDeviceConsole();
};

extern NetDeviceConsole NConsole;

#endif /*_INCDEF_NETDEVICELOG_H_*/

