//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

#ifdef M_BORLAND
DEFINE_MAP(AnsiString,HANDLE,HANDLEMap,HANDLEPairIt);
#endif

class XRCORE_API EFS_Utils {
#ifdef M_BORLAND
    HANDLEMap 	m_LockFiles;
    string256	m_LastAccessFN;
    string256	m_AccessLog;
#endif
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
    void 		OnCreate		();
    void 		OnDestroy		(){;}

	LPCSTR		GenerateName	(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name);

	bool 		GetOpenName		(LPCSTR initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, LPSTR buffer, int sz_buf, LPCSTR offset=0, int start_flt_ext=-1 );
#ifdef M_BORLAND
	bool 		GetOpenName		(LPCSTR initial, AnsiString& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, AnsiString& buf, LPCSTR offset=0, int start_flt_ext=-1 );

	void 		MarkFile		(const AnsiString& fn, bool bDeleteSource);
	void 		BackupFile		(LPCSTR initial, const AnsiString& fname);

	BOOL		CheckLocking	(LPCSTR initial, LPSTR fn, bool bOnlySelf, bool bMsg);
	BOOL		LockFile		(LPCSTR initial, LPSTR fn, bool bLog=true);
	BOOL		UnlockFile		(LPCSTR initial, LPSTR fn, bool bLog=true);
	LPCSTR		GetLockOwner	(LPCSTR initial, LPSTR fn);

	void		RegisterAccess	(LPSTR fn, LPSTR start_msg, bool bLog=true);
	void		WriteAccessLog	(LPSTR fn, LPSTR start_msg);

	AnsiString&	UpdateTextureNameWithFolder(AnsiString& tex_name);
#endif
	LPSTR		UpdateTextureNameWithFolder(LPSTR tex_name);
	LPSTR		UpdateTextureNameWithFolder(LPCSTR src_name, LPSTR dest_name);
};
extern XRCORE_API EFS_Utils EFS;

#endif /*_INCDEF_FileSystem_H_*/

