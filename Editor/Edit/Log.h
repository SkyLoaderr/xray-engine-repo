//----------------------------------------------------
// file: NetDeviceLog.h
//----------------------------------------------------

#ifndef _INCDEF_NETDEVICELOG_H_
#define _INCDEF_NETDEVICELOG_H_

class CLog{
protected:
	static BOOL	bReady;
	static char m_FileName[MAX_PATH];
public:
				CLog	(){bReady=false;}
				~CLog	(){bReady=false;}
    void		Create  (LPCSTR _FileName );
	static void Msg   	(TMsgDlgType mt, LPCSTR _Format, ...);
	static int 	DlgMsg 	(TMsgDlgType mt, LPCSTR _Format, ...);
	static int 	DlgMsg 	(TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...);
};

extern CLog ELog;

void ENGINE_API __cdecl Msg	(LPCSTR format, ...);
void ENGINE_API	Log	(LPCSTR msg);
void ENGINE_API	Log	(LPCSTR msg);
void ENGINE_API	Log	(LPCSTR msg, LPCSTR 		dop);
void ENGINE_API	Log	(LPCSTR msg, DWORD			dop);
void ENGINE_API	Log	(LPCSTR msg, int  			dop);
void ENGINE_API	Log	(LPCSTR msg, float			dop);
void ENGINE_API	Log (LPCSTR msg, const Fvector& dop);
void ENGINE_API	Log	(LPCSTR msg, const Fmatrix& dop);

#endif /*_INCDEF_NETDEVICELOG_H_*/

