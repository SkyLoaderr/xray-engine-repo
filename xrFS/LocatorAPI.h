// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LocatorAPIH
#define LocatorAPIH
#pragma once

// refs 
class	CFS_PathNotificator;

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
	void		_update		(std::string& dest, LPCSTR src) const;
	void		_set		(LPSTR add);

    void __stdcall rescan_path_cb	();
};

// query
struct FS_QueryItem
{
	enum{
    	flSubDir=(1<<0),
    	flVFS	=(1<<1)
    };
	u32			size;
    u32			modif;
    Flags32		flags;
    			FS_QueryItem	():size(0),modif(0){flags.zero();}
    			FS_QueryItem	(u32 sz, u32 mf, u32 fl=0){set(sz,mf,fl);}
    void 		set				(u32 sz, u32 mf, u32 fl=0)
    {
    	size	= sz;
        modif	= mf;
        flags.assign(fl);
    }
};
DEFINE_MAP(std::string,FS_QueryItem,FS_QueryMap,FS_QueryPairIt);

class XRCORE_API CLocatorAPI  
{
public:
	struct	file
	{
		LPCSTR					name;			// low-case name
		u32						vfs;			// 0xffffffff - standart file
		u32						ptr;			// pointer inside vfs
		u32						size_real;		// 
		u32						size_compressed;// if (size_real==size_compressed) - uncompressed
        u32						modif;			// for editor
	};
	struct	file_pred: public 	std::binary_function<file&, file&, bool> 
	{	
		IC bool operator()	(const file& x, const file& y) const
		{	return xr_strcmp(x.name,y.name)<0;	}
	};
	struct	archive
	{
		ref_str					path;
		IReader*				vfs;
	};
	DEFINE_MAP_PRED				(LPCSTR,FS_Path*,PathMap,PathPairIt,pred_str);
	PathMap						pathes;

	DEFINE_SET_PRED				(file,files_set,files_it,file_pred);
    DEFINE_VECTOR				(archive,archives_vec,archives_it);

    CFS_PathNotificator*		FThread;
	enum{
    	flNeedRescan			= (1<<0),
        flLockRescan			= (1<<1),
		flBuildCopy				= (1<<2),
		flReady					= (1<<3),
		flEBuildCopy			= (1<<4),
        flEventNotificator      = (1<<5),
		flTargetFolderOnly		= (1<<6),
		flCacheFiles			= (1<<7),
		flScanAppRoot			= (1<<8),
    };    
    Flags32						m_Flags;
    void						rescan_path		(LPCSTR full_path, BOOL bRecurse);
    void						rescan_pathes	();
    void						check_pathes	();
private:
	files_set					files;
    archives_vec				archives;
	BOOL						bNoRecurse;

	void						Register		(LPCSTR name, u32 vfs, u32 ptr, u32 size_real, u32 size_compressed, u32 modif);
	void						ProcessArchive	(LPCSTR path);
	void						ProcessOne		(LPCSTR path, void* F);
	bool						Recurse			(LPCSTR path);

    void						SetEventNotification	();
    void						ClearEventNotification	();

	files_it					file_find_it	(LPCSTR n);
public:
								CLocatorAPI		();
								~CLocatorAPI	();
	void						_initialize		(u32 flags, LPCSTR target_folder=0);
	void						_destroy		();

	IReader*					r_open			(LPCSTR initial, LPCSTR N);
	IC IReader*					r_open			(LPCSTR N){return r_open(0,N);}
	void						r_close			(IReader* &S);

	IWriter*					w_open			(LPCSTR initial, LPCSTR N);
	IC IWriter*					w_open			(LPCSTR N){return w_open(0,N);}
	void						w_close			(IWriter* &S);

	const file*					exist			(LPCSTR N);
	const file*					exist			(LPCSTR path, LPCSTR name);
	const file*					exist			(LPSTR fn, LPCSTR path, LPCSTR name);
	const file*					exist			(LPSTR fn, LPCSTR path, LPCSTR name, LPCSTR ext);

    BOOL 						dir_delete		(LPCSTR path,LPCSTR nm,BOOL remove_files);
    BOOL 						dir_delete		(LPCSTR full_path,BOOL remove_files){return dir_delete(0,full_path,remove_files);}
    void 						file_delete		(LPCSTR path,LPCSTR nm);
    void 						file_delete		(LPCSTR full_path){file_delete(0,full_path);}
	void 						file_copy		(LPCSTR src, LPCSTR dest);
	void 						file_rename		(LPCSTR src, LPCSTR dest,bool bOwerwrite=true);
    int							file_length		(LPCSTR src);

    int  						get_file_age	(LPCSTR nm);
    void 						set_file_age	(LPCSTR nm, int age);

	xr_vector<LPSTR>*			file_list_open	(LPCSTR initial, LPCSTR folder,	u32 flags=FS_ListFiles);
	xr_vector<LPSTR>*			file_list_open	(LPCSTR path,					u32 flags=FS_ListFiles);
	void						file_list_close	(xr_vector<LPSTR>* &lst);
                                                     
    bool						path_exist		(LPCSTR path);
    FS_Path*					get_path		(LPCSTR path);
    FS_Path*					append_path		(LPCSTR path_alias, LPCSTR root, LPCSTR add, BOOL recursive);
    LPCSTR						update_path		(LPSTR dest, LPCSTR initial, LPCSTR src);

    // editor functions
	int							file_list		(FS_QueryMap& dest, LPCSTR path, u32 flags=FS_ListFiles, LPCSTR mask=0);
	bool						file_find		(FS_QueryItem& dest, LPCSTR path, LPCSTR name, bool clamp_ext);
    void						update_path		(std::string& dest, LPCSTR initial, LPCSTR src);
	void						lock_rescan		();
	void						unlock_rescan	();
};

extern XRCORE_API	CLocatorAPI*			xr_FS;
#define FS (*xr_FS)

#endif // LocatorAPIH

