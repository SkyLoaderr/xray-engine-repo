// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LocatorAPIH
#define LocatorAPIH
#pragma once

// refs 
class TShellChangeThread;

enum FS_List
{
	FS_ListFiles	=(1<<0),
	FS_ListFolders	=(1<<1),
	FS_ClampExt		=(1<<2),
	FS_RootOnly		=(1<<3),
	FS_forcedword	=u32(-1)
};

class XRCORE_API FS_Path{
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
#ifdef __BORLANDC__
	const AnsiString& _update(AnsiString& dest) const;
	const AnsiString& _update(AnsiString& dest, LPCSTR src) const;
    void __fastcall rescan_path_cb();
#endif
public:
	FS_Path		(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt=0, LPCSTR _FilterString=0, u32 flags=0);
	~FS_Path	()
	{
		xr_free	(m_Root);
		xr_free	(m_Path);
		xr_free	(m_Add);
		xr_free	(m_DefExt);
		xr_free	(m_FilterCaption);
	}
	LPCSTR		_update		(LPSTR dest, LPCSTR src) const;
	void		_set		(LPSTR add)
	{
		// m_Add
		R_ASSERT		(add);
		xr_free			(m_Add);
		m_Add			= strlwr(xr_strdup(add));

		// m_Path
		string256		temp;
		strconcat		(temp,m_Root,m_Add);
		if (temp[strlen(temp)-1]!='\\') strcat(temp,"\\");
		xr_free			(m_Path);
		m_Path			= strlwr(xr_strdup(temp));
	}
};

#ifdef __BORLANDC__
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
    			FS_QueryItem	(u32 sz, u32 mf, u32 fl=0)
    {
    	size	= sz;
        modif	= mf;
        flags.set(fl);
    }
};
DEFINE_MAP(AnsiString,FS_QueryItem,FS_QueryMap,FS_QueryPairIt);
#endif

class XRCORE_API CLocatorAPI  
{
public:
	struct	file
	{
		LPCSTR	name;			// low-case name
		u32		vfs;			// 0xffffffff - standart file
		u32		ptr;			// pointer inside vfs
		u32		size;			// for REAL file - its file size, for COMPRESSED inside VFS - compressed size, for PLAIN inside VFS - file size
        u32		modif;			// for editor
		BOOL	bCompressed;
	};
	struct	file_pred		: public std::binary_function<file&, file&, bool> 
	{	
		IC bool operator()	(const file& x, const file& y) const
		{	return strcmp(x.name,y.name)<0;	}
	};
	struct	archive
	{
		IReader*	vfs;
	};
	DEFINE_MAP_PRED(LPCSTR,FS_Path*,PathMap,PathPairIt,pred_str);
	PathMap						pathes;

	DEFINE_SET_PRED(file,files_set,files_it,file_pred);
    DEFINE_VECTOR(archive,archives_vec,archives_it);

    TShellChangeThread*			FThread;
	enum{
    	flNeedRescan	= (1<<0),
        flLockRescan	= (1<<1)
    };    
    Flags32						m_Flags;
    void						rescan_path		(LPCSTR full_path, BOOL bRecurse);
    void						rescan_pathes	();
    void						check_pathes	();
private:
	files_set					files;
    archives_vec				archives;
	BOOL						bNoRecurse;

	void						Register		(LPCSTR name, u32 vfs, u32 ptr, u32 size, BOOL bCompressed, u32 modif);
	void						ProcessArchive	(LPCSTR path);
	void						ProcessOne		(LPCSTR path, LPVOID F);
	bool						Recurse			(LPCSTR path);

    void						SetEventNotification	();
    void						ClearEventNotification	();
public:
								CLocatorAPI		();
								~CLocatorAPI	();
	void						_initialize		();
	void						_destroy		();

	IReader*					r_open			(LPCSTR initial, LPCSTR N);
	IC IReader*					r_open			(LPCSTR N){return r_open(0,N);}
	void						r_close			(IReader* &S);

	IWriter*					w_open			(LPCSTR initial, LPCSTR N);
	IC IWriter*					w_open			(LPCSTR N){return w_open(0,N);}
	void						w_close			(IWriter* &S, bool bDiscard=false);

	const file*					exist			(LPCSTR N);
	const file*					exist			(LPCSTR path, LPCSTR name);
	const file*					exist			(LPSTR fn, LPCSTR path, LPCSTR name);
	const file*					exist			(LPSTR fn, LPCSTR path, LPCSTR name, LPCSTR ext);

	files_it					file_find		(LPCSTR n);
    void 						file_delete		(LPCSTR path,LPCSTR nm);
    void 						file_delete		(LPCSTR full_path){file_delete(0,full_path);}
	void 						file_copy		(LPCSTR src, LPCSTR dest);
	void 						file_rename		(LPCSTR src, LPCSTR dest);
    int							file_length		(LPCSTR src);

    int  						get_file_age	(LPCSTR nm);
    void 						set_file_age	(LPCSTR nm, int age);

	vector<char*>*				file_list_open	(LPCSTR path, u32 flags=FS_ListFiles);
	void						file_list_close	(vector<char*>* &lst);

    bool						path_exist		(LPCSTR path);
    FS_Path*					get_path		(LPCSTR path);
    LPCSTR						update_path		(LPSTR dest, LPCSTR initial, LPCSTR src);

#ifdef __BORLANDC__
	int							file_list		(FS_QueryMap& dest, LPCSTR path, u32 flags=FS_ListFiles, LPCSTR ext_mask=0);
	const AnsiString&			update_path		(LPCSTR initial, AnsiString& dest);
    const AnsiString&			update_path		(AnsiString& dest, LPCSTR initial, LPCSTR src);
	void						lock_rescan		();
	void						unlock_rescan	();
#endif    
};

extern XRCORE_API	CLocatorAPI	FS;

#endif // LocatorAPIH
/*
class FS_query
{
    files_set		files;
public:
    ~files_query	()	
    { 
        clear		();
    }
    IC void clear	()
    {
        for(CLocatorAPI::files_it I=files.begin(); I!=files.end(); I++){
            char* str	= LPSTR(I->name);
            xr_free		(str);
        }
        files.clear	();
    }
    IC bool insert	(LPCSTR name, u32 vfs, u32 ptr, u32 size, BOOL bCompressed, u32 modif, bool bClampExt=false)
    {
        file desc;
        desc.name		= strlwr(xr_strdup(name));
        if (bClampExt&&(strext(desc.name)))	*strext(desc.name) = 0;
        desc.vfs		= vfs;
        desc.ptr		= ptr;
        desc.size		= size;
        desc.modif		= modif;
        desc.bCompressed= bCompressed;
        pair<files_it,bool> I = files.insert(desc);
        if (!I.second)	xr_free(desc.name);
        return I.second;
    }
    IC bool insert	(const file& entry, bool bClampExt=false)
    {
        return insert	(entry.name,entry.vfs,entry.ptr,entry.size,entry.modif,entry.bCompressed,bClampExt);
    }
    IC bool insert	(LPCSTR new_name, const file& entry, bool bClampExt=false)
    {
        return insert	(new_name,entry.vfs,entry.ptr,entry.size,entry.modif,entry.bCompressed,bClampExt);
    }
    IC files_it find(LPCSTR nm)	{ file desc; desc.name=nm; return files.find(desc);	}
    IC int size		()			{ return files.size();		}
    IC bool empty	()			{ return files.empty();		}
    IC files_it 	begin	()	{ return files.begin();		}
    IC files_it		end		()	{ return files.end();		}
};
*/
