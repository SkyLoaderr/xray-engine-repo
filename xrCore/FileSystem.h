//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

DEFINE_MAP(AnsiString,HANDLE,HANDLEMap,HANDLEPairIt);

class EFS_Utils {
    HANDLEMap 	m_LockFiles;
    string256	m_LastAccessFN;
    string256	m_AccessLog;
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
    void 		OnCreate		();
    void 		OnDestroy		(){;}

	bool 		GetOpenName		(LPCSTR initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, LPSTR buffer, int sz_buf, LPCSTR offset=0, int start_flt_ext=-1 );
#ifdef M_BORLAND
	bool 		GetOpenName		(LPCSTR initial, AnsiString& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, AnsiString& buf, LPCSTR offset=0, int start_flt_ext=-1 );
#endif
    void 		MarkFile		(const AnsiString& fn, bool bDeleteSource);
//    void		MarkFiles		(LPCSTR initial, FileMap& files, bool bDeleteSource);
//    void		MarkFiles		(LPCSTR initial, LPSTRVec& files, bool bDeleteSource);
	void 		BackupFile		(LPCSTR initial, const AnsiString& fname);

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
extern EFS_Utils EFS;
// int GetFileList(LPCSTR path, FileMap& items, bool bClampPath, bool bClampExt, bool bRootOnly, LPCSTR ext_mask="*.*");
#endif /*_INCDEF_FileSystem_H_*/

