// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LocatorAPIH
#define LocatorAPIH
#pragma once

enum FS_List
{
	FS_ListFiles	=(1<<0),
	FS_ListFolders	=(1<<1),
	FS_forcedword	=u32(-1)
};

#ifdef __BORLANDC__
DEFINE_MAP(AnsiString,int,FileMap,FilePairIt);
#endif

class XRCORE_API FS_Path{
public:
	LPSTR		m_Path;
	LPSTR		m_Root;
	LPSTR		m_Add;
	LPSTR		m_DefExt;
	LPSTR		m_FilterCaption;
#ifdef __BORLANDC__
	const AnsiString& _update(AnsiString& fname) const;
	const AnsiString& _update(AnsiString& dest, LPCSTR src) const;
#endif
public:
	FS_Path		(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt=0, LPCSTR _FilterString=0);
	~FS_Path	()
	{
		xr_free	(m_Root);
		xr_free	(m_Path);
		xr_free	(m_Add);
		xr_free	(m_DefExt);
		xr_free	(m_FilterCaption);
	}
	LPCSTR		_update		(LPSTR fname) const;
	LPCSTR		_update		(LPSTR dest, LPCSTR src) const;
};

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
		CVirtualFileReader*		vfs;
	};

	DEFINE_MAP_PRED(LPCSTR,FS_Path*,PathMap,PathPairIt,pred_str);
	PathMap						pathes;
private:
	typedef set<file,file_pred>		set_files;
	typedef set_files::iterator		set_files_it;
	typedef vector<archive>			vec_archives;
	typedef vec_archives::iterator	vec_archives_it;

	BOOL						bNoRecurse;
	set_files					files;
	vec_archives				archives;

	void						Register		(const char* name, u32 vfs, u32 ptr, u32 size, BOOL bCompressed, u32 modif);
	void						ProcessArchive	(const char* path);
	void						ProcessOne		(const char* path, LPVOID F);
	bool						Recurse			(const char* path);
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

	BOOL						exist			(const char* N);
	BOOL						exist			(char* fn, const char* path, const char* name);
	BOOL						exist			(char* fn, const char* path, const char* name, const char* ext);

	void						List			(vector<char*>& dest, const char* path, u32 flags=FS_ListFiles);
                                      
    bool						path_exist		(LPCSTR path);
    FS_Path*					get_path		(LPCSTR path);
    void						update_path		(LPCSTR initial, LPSTR path);
    void						update_path		(LPSTR dest, LPCSTR initial, LPCSTR src);

#ifdef __BORLANDC__
	void						List			(FileMap& dest, const char* path, u32 flags=FS_ListFiles);

    void						update_path		(LPCSTR initial, AnsiString& path);
    void						update_path		(AnsiString& dest, LPCSTR initial, LPCSTR src);
#endif    
};

extern XRCORE_API	CLocatorAPI	FS;

#endif // LocatorAPIH
