//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

DEFINE_MAP(AnsiString,int,FileMap,FilePairIt);
DEFINE_MAP(AnsiString,HANDLE,HANDLEMap,HANDLEPairIt);

class CFileSystem {
	void 		ProcessOne(_finddata_t& F, const char* path, bool bOnlyDir, bool bRootOnly);
	void 		Recurse(const char* path, bool bRootOnly);

	FileMap* 	m_FindItems;
    LPSTR 		ext_mask;
	bool 		bClampExt;
    int			path_size;

    HANDLEMap 	m_LockFiles;
    string256	m_LastAccessFN;
    string256	m_AccessLog;
public:
				CFileSystem		();
	virtual 	~CFileSystem	();
    void 		OnCreate		();
    void 		OnDestroy		(){;}

	bool 		GetOpenName		(LPCSTR initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=1 );
	bool 		GetSaveName		(LPCSTR initial, LPSTR buffer, int sz_buf, LPCSTR offset=0, int start_flt_ext=1 );
#ifdef M_BORLAND
	bool 		GetOpenName		(LPCSTR initial, AnsiString& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=1 );
	bool 		GetSaveName		(LPCSTR initial, AnsiString& buf, LPCSTR offset=0, int start_flt_ext=1 );
#endif
/*
    bool 		Exist			(LPCSTR _FileName, bool bMessage = false);
    bool 		Exist			(LPCSTR initial, const char *_FileName, bool bMessage = false);
	bool		Exist			(char* fn, LPCSTR initial, const char* name, bool bMessage = false);
	bool		Exist			(char* fn, LPCSTR initial, const char* name, const char* ext, bool bMessage = false);
    void 		DeleteFileByName(const char* nm);
    void 		DeleteFileByName(LPCSTR initial, const char* nm);
	void 		CopyFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
	void 		MoveFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
    int			FileLength		(LPCSTR src);

    int  		GetFileAge		(const AnsiString& name);
    void 		SetFileAge		(const AnsiString& name, int FT);
*/
    void 		MarkFile		(const AnsiString& fn, bool bDeleteSource);
    void		MarkFiles		(LPCSTR initial, FileMap& files, bool bDeleteSource);
    void		MarkFiles		(LPCSTR initial, LPSTRVec& files, bool bDeleteSource);
	void 		BackupFile		(LPCSTR initial, const AnsiString& fname);

    int			GetFileList		(LPCSTR path, FileMap& items, bool bClampPath, bool bClampExt, bool bRootOnly, LPCSTR ext_mask="*.*"); // return item count

	AnsiString&	UpdateTextureNameWithFolder(AnsiString& tex_name);
	LPSTR		UpdateTextureNameWithFolder(LPSTR tex_name);
	LPSTR		UpdateTextureNameWithFolder(LPCSTR src_name, LPSTR dest_name);

    BOOL		CheckLocking	(LPCSTR initial, LPSTR fn, bool bOnlySelf, bool bMsg);
    BOOL		LockFile		(LPCSTR initial, LPSTR fn, bool bLog=true);
    BOOL		UnlockFile		(LPCSTR initial, LPSTR fn, bool bLog=true);
    LPCSTR		GetLockOwner	(LPCSTR initial, LPSTR fn);

    void		RegisterAccess	(LPSTR fn, LPSTR start_msg, bool bLog=true);
    void		WriteAccessLog	(LPSTR fn, LPSTR start_msg);

    LPCSTR		GenerateName	(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name);
};
#endif /*_INCDEF_FileSystem_H_*/

