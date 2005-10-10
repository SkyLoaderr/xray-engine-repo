//----------------------------------------------------
// file: Log.h
//----------------------------------------------------

#ifndef _INCDEF_LOG_H_
#define _INCDEF_LOG_H_

class CLog{
protected:
	BOOL		bReady;
	char 		m_FileName[MAX_PATH];
public:
				CLog	(){bReady=false;}
				~CLog	(){bReady=false;}
    void		Create  (LPCSTR _FileName,bool bContinue=false);

	void 		Msg   	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...);
};

extern CLog ELog;
extern bool g_ErrorMode;

#define VPUSH(a)	a.x,a.y,a.z

void ENGINE_API __cdecl Msg	(LPCSTR format, ...);
void ENGINE_API	Log	(LPCSTR msg);
void ENGINE_API	Log	(LPCSTR msg);
void ENGINE_API	Log	(LPCSTR msg, LPCSTR 		dop);
void ENGINE_API	Log	(LPCSTR msg, u32			dop);
void ENGINE_API	Log	(LPCSTR msg, int  			dop);
void ENGINE_API	Log	(LPCSTR msg, float			dop);
void ENGINE_API	Log (LPCSTR msg, const Fvector& dop);
void ENGINE_API	Log	(LPCSTR msg, const Fmatrix& dop);

#endif /*_INCDEF_NETDEVICELOG_H_*/

