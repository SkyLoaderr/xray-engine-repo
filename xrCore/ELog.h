//----------------------------------------------------
// file: Log.h
//----------------------------------------------------

#ifndef ELogH
#define ELogH

class CLog{
public:
	bool 		in_use;
public:
				CLog	(){in_use=false;}
	void 		Msg   	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, LPCSTR _Format, ...);
	int 		DlgMsg 	(TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...);
};

void ELogCallback(LPCSTR string);

extern CLog ELog;

#endif /*_INCDEF_NETDEVICELOG_H_*/

