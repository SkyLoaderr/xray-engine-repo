//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_

class FSPath{
public:
	char m_Root[MAX_PATH];
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
	void 		ProcessOne(_finddata_t& F, const char* path, bool bOnlyDir);
	void 		Recurse(const char* path);

    HWND 		m_Handle;

	FileMap* 	m_FindItems;
    LPSTR 		ext_mask;
	bool 		bClampExt;
    int			path_size;

    HANDLEMap 	m_LockFiles;
    string256	m_LastAccessFN;
    CLog*		m_AccessLog;
    void		RegisterAccess(LPSTR fn);
public:
	string256	m_Local;
	string256	m_Server;
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
	FSPath 		m_Groups;
	FSPath 		m_OMotion;
	FSPath 		m_SMotion;
	FSPath 		m_OMotions;
	FSPath 		m_SMotions;
	FSPath 		m_Objects;
	FSPath 		m_Textures;
	FSPath 		m_Temp;
public:
				CFileSystem		();
	virtual 	~CFileSystem	();
    void 		OnCreate		(HWND handle);

	bool 		GetOpenName		(FSPath& initial, LPSTR buffer, int sz_buf, bool bMulti=false, LPCSTR offset=0 );
	bool 		GetSaveName		(FSPath& initial, char *buffer, int sz_buf, LPCSTR offset=0 );
#ifdef M_BORLAND
	bool 		GetOpenName		(FSPath& initial, AnsiString& buf, bool bMulti=false, LPCSTR offset=0 );
	bool 		GetSaveName		(FSPath& initial, AnsiString& buf, LPCSTR offset=0 );
#endif

    bool 		Exist			(LPCSTR _FileName, bool bMessage = false);
    bool 		Exist			(FSPath *initial, const char *_FileName, bool bMessage = false);
	bool		Exist			(char* fn, const char* path, const char* name, bool bMessage = false);
	bool		Exist			(char* fn, const char* path, const char* name, const char* ext, bool bMessage = false);
    void 		DeleteFileByName(const char* nm);
	void 		CopyFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
    int			FileLength		(LPCSTR src);

    int  		GetFileAge		(const AnsiString& name);
    void 		SetFileAge		(const AnsiString& name, int FT);

    bool 		CreateNullFile	(const char* fn);

    void 		MarkFile		(const AnsiString& fn);
	void 		BackupFile		(const AnsiString& fn);
	bool 		RestoreBackup	(const AnsiString& fn);

    int			GetFiles		(LPCSTR path, FileMap& items, bool bClampPath, bool bClampExt, LPCSTR ext_mask="*.*"); // return item count

    void		VerifyPath		(LPCSTR path);
	LPSTR		UpdateTextureNameWithFolder(LPSTR tex_name);

    BOOL		CheckLocking	(FSPath *initial, LPSTR fn, bool bOnlySelf, bool bMsg);
    BOOL		LockFile		(FSPath *initial, LPSTR fn, bool bLog=true);
    BOOL		UnlockFile		(FSPath *initial, LPSTR fn, bool bLog=true);
    LPCSTR		GetLockOwner	(FSPath *initial, LPSTR fn);

    void		GetCompAndUser	(string64& computer, string64& user);
};
extern CFileSystem FS;
#endif /*_INCDEF_FileSystem_H_*/

