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
	void Update( AnsiString& _FileName ) const;
	void VerifyPath();
};

DEFINE_MAP(AnsiString,int,FindDataMap,FindDataPairIt);

class CFileSystem {
	_finddata_t	FData;
	int 	hFindHandle;
	void 	ProcessOne(_finddata_t& F, const char* path);
	void 	Recurse(const char* path);

	FindDataMap* m_FindItems;
    bool	bFiles;
public:
	char 	m_Root[MAX_PATH];
	char 	m_Server[MAX_PATH];
	FSPath 	m_ExeRoot;
	FSPath 	m_GameRoot;
	FSPath 	m_GameSounds;
	FSPath 	m_GameCForms;
	FSPath 	m_GameMeshes;
	FSPath 	m_GameDO;
	FSPath 	m_GameTextures;
	FSPath 	m_GameLevels;
	FSPath 	m_Maps;
	FSPath 	m_Import;
	FSPath 	m_Groups;
	FSPath 	m_OMotion;
	FSPath 	m_SMotion;
	FSPath 	m_OMotions;
	FSPath 	m_SMotions;
	FSPath 	m_Objects;
	FSPath 	m_Textures;
	FSPath 	m_Temp;
public:
			CFileSystem		();
	virtual ~CFileSystem	();
    void 	OnCreate		();

	bool 	GetOpenName		(FSPath *initial, char *buffer);
	bool 	GetSaveName		(FSPath *initial, char *buffer);
	bool 	GetOpenName		(FSPath *initial, AnsiString& buf);
	bool 	GetSaveName		(FSPath *initial, AnsiString& buf);

    bool 	Exist			(LPCSTR _FileName, bool bMessage = false);
    bool 	Exist			(FSPath *initial, const char *_FileName, bool bMessage = false);
	bool	Exist			(char* fn, const char* path, const char* name, bool bMessage = false);
	bool	Exist			(char* fn, const char* path, const char* name, const char* ext, bool bMessage = false);
    void 	DeleteFileByName(const char* nm);
	void 	CopyFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
    int		FileLength		(LPCSTR src);

    BOOL  	GetFileAge		(const AnsiString& name, int FT);
    void 	SetFileAge		(const AnsiString& name, int FT);
    void 	SetFileAgeFrom	(const AnsiString& src_name, const AnsiString& dest_name);
    int 	CompareFileAge	(const AnsiString& fn1, const AnsiString& fn2); // result - 1-equal, 0-different, -1-error

    bool 	CreateNullFile	(const char* fn);

    void 	MarkFile		(const AnsiString& fn);
    void 	BackupFile		(const AnsiString& fn);
	int 	RestoreBackup	(const AnsiString& fn);

    LPCSTR	FindFirst(LPSTR mask);
    LPCSTR	FindNext();

    int		GetFiles		(LPCSTR path, FindDataMap& items); // return item count
	int		GetDirectories	(LPCSTR path, FindDataMap& items); // return item count

    void	VerifyPath		(LPCSTR path);
};
extern CFileSystem FS;
#endif /*_INCDEF_FileSystem_H_*/

