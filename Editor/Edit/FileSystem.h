//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_

#include "FS.h"

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

class CFileSystem {
	_finddata_t	FData;
	int 	hFindHandle;
public:
	char 	m_Root[MAX_PATH];
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
	FSPath 	m_Meshes;
	FSPath 	m_Groups;
	FSPath 	m_OMotion;
	FSPath 	m_SMotion;
	FSPath 	m_OMotions;
	FSPath 	m_SMotions;
	FSPath 	m_Objects;
	FSPath 	m_Textures;
	FSPath 	m_TexturesThumbnail;
	FSPath 	m_Config;
	FSPath 	m_Temp;
public:
			CFileSystem		();
	virtual ~CFileSystem	();
    void 	Init			();

	bool 	GetOpenName		(FSPath *initial, char *buffer);
	bool 	GetSaveName		(FSPath *initial, char *buffer);
	bool 	GetOpenName		(FSPath *initial, AnsiString& buf);
	bool 	GetSaveName		(FSPath *initial, AnsiString& buf);

    bool 	Exist			(const char *_FileName, bool bMessage = false);
    bool 	Exist			(FSPath *initial, const char *_FileName, bool bMessage = false);
    void 	DeleteFileByName(const char* nm);
	void 	CopyFileTo		(LPCSTR src, LPCSTR dest, bool bOverwrite=true);
    int		FileLength		(LPCSTR src);

    int  	GetFileAge		(const AnsiString& name);
    void 	SetFileAge		(const AnsiString& name, int age);
    void 	SetFileAgeFrom	(const AnsiString& src_name, const AnsiString& dest_name);
    int 	CompareFileAge	(const AnsiString& fn1, const AnsiString& fn2); // result - 1-equal, 0-different, -1-error

    bool 	CreateNullFile	(const char* fn);

    void 	MarkFile		(const AnsiString& fn);
    void 	BackupFile		(const AnsiString& fn);
	int 	RestoreBackup	(const AnsiString& fn);

    LPCSTR	FindFirst(LPSTR mask);
    LPCSTR	FindNext();
};
extern CFileSystem FS;
#endif /*_INCDEF_FileSystem_H_*/

