//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

DEFINE_MAP(std::string,HANDLE,HANDLEMap,HANDLEPairIt);

class XRFS_API EFS_Utils {
    HANDLEMap 	m_LockFiles;
    std::string	m_LastAccessFN;
    std::string	m_AccessLog;
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
    void 		_initialize		();
    void 		_destroy		(){;}

	LPCSTR		GenerateName	(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name);

	bool 		GetOpenName		(LPCSTR initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetOpenName		(LPCSTR initial, std::string& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=-1 );

	bool 		GetSaveName		(LPCSTR initial, LPSTR buffer, int sz_buf, LPCSTR offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(LPCSTR initial, std::string& buf, LPCSTR offset=0, int start_flt_ext=-1 );

	void 		MarkFile		(LPCSTR fn, bool bDeleteSource);
	void 		BackupFile		(LPCSTR initial, LPCSTR fname, bool bMsg=FALSE);

	BOOL		CheckLocking	(LPCSTR initial, LPCSTR fn, bool bOnlySelf, bool bMsg);
	BOOL		LockFile		(LPCSTR initial, LPCSTR fn, bool bLog=true);
	BOOL		UnlockFile		(LPCSTR initial, LPCSTR fn, bool bLog=true);
	LPCSTR		GetLockOwner	(LPCSTR initial, LPCSTR fn);

	void		RegisterAccess	(LPCSTR fn, LPCSTR start_msg, bool bLog=true);
	void		WriteAccessLog	(LPCSTR fn, LPCSTR start_msg);

	std::string AppendFolderToName(std::string& tex_name, int depth, BOOL full_name);

	LPCSTR		AppendFolderToName(LPSTR tex_name, int depth, BOOL full_name);
	LPCSTR		AppendFolderToName(LPCSTR src_name, LPSTR dest_name, int depth, BOOL full_name);

    std::string	ChangeFileExt	(LPCSTR src, LPCSTR ext);
    std::string	ChangeFileExt	(const std::string& src, LPCSTR ext);
};
extern XRFS_API	EFS_Utils*	xr_EFS;
#define EFS (*xr_EFS)

#endif /*_INCDEF_FileSystem_H_*/

