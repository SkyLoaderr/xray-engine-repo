//----------------------------------------------------
// file: NetDeviceLog.h
//----------------------------------------------------

#ifndef _INCDEF_NETDEVICELOG_H_
#define _INCDEF_NETDEVICELOG_H_

using namespace std;

class NetLog{
protected:
	char m_FileName[MAX_PATH];
public:
	NetLog( char *_FileName );
	void __cdecl Msg( char *_Format, ... );
};

extern NetLog Log;

#endif /*_INCDEF_NETDEVICELOG_H_*/

