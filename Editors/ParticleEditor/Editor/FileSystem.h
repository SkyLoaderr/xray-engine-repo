//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_

class FSPath{
public:
	char m_Root[MAX_PATH];
	char m_Add[MAX_PATH];
	char m_Path[MAX_PATH];
	char m_DefExt[MAX_PATH];
	char m_FilterString[1024];
public:

	void Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString );
	void Update( char *_FileName ) const;
	void VerifyPath();
#ifdef M_BORLAND
	void Update( AnsiString& _FileName ) const;
#endif
};

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
    CLog*		m_AccessLog;
public:
    string128 	m_UserName;
    string128 	m_CompName;
public:
	string256	m_Local;
	string256	m_Server;
	string256	m_ServerData;
	string256	m_ServerBackup;
	FSPath 		m_ServerBackupRoot;
	FSPath 		m_ServerDataRoot;
	FSPath 		m_ServerRoot;
	FSPath 		m_LocalRoot;
	FSPath 		m_GameRoot;
	FSPath 		m_GameSounds;
	FSPath 		m_GameCForms;
	FSPath 		m_GameMeshes;
	FSPath		m_GameKeys;
	FSPath 		m_GameDO;
	FSPath 		m_GameTextures;
	FSPath 		m_GameLevels;
	FSPath 		m_Maps;
	FSPath 		m_Import;
	FSPath 		m_DetailObjects;
	FSPath		m_Envelope;
	FSPath 		m_Groups;
	FSPath 		m_OMotion;
	FSPath 		m_SMotion;
	FSPath 		m_OMotions;
	FSPath 		m_SMotions;
	FSPath		m_SBones;
	FSPath 		m_Objects;
	FSPath 		m_Textures;
	FSPath 		m_Temp;
public:
				CFileSystem		();
	virtual 	~CFileSystem	();
    void 		OnCreate		();
    void 		OnDestroy		(){;}

    CStream*	Open			(LPCSTR fn);
	void 		Close			(CStream*& F);

	bool 		GetOpenName		(FSPath& initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=1 );
	bool 		GetSaveName		(FSPath& initial, char *buffer, int sz_buf, LPCSTR offset=0, int start_flt_ext=1 );
#ifdef M_BORLAND
	bool 		GetOpenName		(FSPath& initial, AnsiString& buf, bool bMulti=false, LPCSTR offset=0, int start_flt_ext=1 );
	bool 		GetSaveName		(FSPath& initial, AnsiString& buf, LPCSTR offset=0, int start_flt_ext=1 );
#endif

    bool 		Exist			(LPCSTR _FileName, bool bMessage = false);
    bool 		Exist			(FSPath *initial, const char *_FileName, bool bMessage = false);
	bool		Exist			(char* fn, const char* path, const char* name, bool bMessage = false);
	bool		Exist			(char* fn, const char* path, const char* name, const char* ext, bool bMessage = false);
    void 		DeleteFileByName(const char* nm);
	void 		CopyFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
	void 		MoveFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
    int			FileLength		(LPCSTR src);

    int  		GetFileAge		(const AnsiString& name);
    void 		SetFileAge		(const AnsiString& name, int FT);

    bool 		CreateNullFile	(const char* fn);

    void 		MarkFile		(const AnsiString& fn);
    void		MarkFiles		(FSPath* initial, FileMap& files);
    void		MarkFiles		(FSPath* initial, LPSTRVec& files);
	void 		BackupFile		(const AnsiString& fn);
	bool 		RestoreBackup	(const AnsiString& fn);
	void 		BackupFile		(FSPath *initial, const AnsiString& fname);

    int			GetFileList		(LPCSTR path, FileMap& items, bool bClampPath, bool bClampExt, bool bRootOnly, LPCSTR ext_mask="*.*"); // return item count

    void		VerifyPath		(LPCSTR path);
	LPSTR		UpdateTextureNameWithFolder(LPSTR tex_name);
	LPSTR		UpdateTextureNameWithFolder(LPCSTR src_name, LPSTR dest_name);

    BOOL		CheckLocking	(FSPath *initial, LPSTR fn, bool bOnlySelf, bool bMsg);
    BOOL		LockFile		(FSPath *initial, LPSTR fn, bool bLog=true);
    BOOL		UnlockFile		(FSPath *initial, LPSTR fn, bool bLog=true);
    LPCSTR		GetLockOwner	(FSPath *initial, LPSTR fn);

    void		RegisterAccess	(LPSTR fn, LPSTR start_msg);
    void		WriteAccessLog	(LPSTR fn, LPSTR start_msg);

    LPCSTR		GenerateName	(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name);
};
#endif /*_INCDEF_FileSystem_H_*/

