#ifndef LocatorAPI_defsH
#define LocatorAPI_defsH

enum FS_List
{
	FS_ListFiles	=(1<<0),
	FS_ListFolders	=(1<<1),
	FS_ClampExt		=(1<<2),
	FS_RootOnly		=(1<<3),
	FS_forcedword	=u32(-1)
};

class XRCORE_API FS_Path
{
public:
	enum{
    	flRecurse	= (1<<0),
    	flNotif		= (1<<1),
    	flNeedRescan= (1<<2),
    };
public:
	LPSTR		m_Path;
	LPSTR		m_Root;
	LPSTR		m_Add;
	LPSTR		m_DefExt;
	LPSTR		m_FilterCaption;
    Flags32		m_Flags;
public:
				FS_Path		(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt=0, LPCSTR _FilterString=0, u32 flags=0);
				~FS_Path	();
	LPCSTR		_update		(LPSTR dest, LPCSTR src) const;
	void		_update		(xr_string& dest, LPCSTR src) const;
	void		_set		(LPSTR add);

    void __stdcall rescan_path_cb	();
};

#endif //LocatorAPI_defsH
