//----------------------------------------------------
// file: NetDeviceLog.h
//----------------------------------------------------

#ifndef _INCDEF_NETDEVICELOG_H_
#define _INCDEF_NETDEVICELOG_H_

class CLog{
protected:
	char m_FileName[MAX_PATH];
public:
	CLog( char *_FileName );
	void Msg   ( TMsgDlgType mt, char *_Format, ... );
	int DlgMsg ( TMsgDlgType mt, char *_Format, ... );
	int DlgMsg ( TMsgDlgType mt, TMsgDlgButtons btn, char *_Format, ... );
};

extern CLog* Log;

#endif /*_INCDEF_NETDEVICELOG_H_*/

