//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------

#ifndef _INCDEF_FileSystem_H_
#define _INCDEF_FileSystem_H_

#include "FS.h"


// refs
struct st_ObjVer;

class FSPath{
public:

	char m_Root[MAX_PATH];
	char m_Path[MAX_PATH];
	char m_DefExt[MAX_PATH];
	char m_FilterString[1024];

public:

	void Init( char *_Root, char *_Add, char *_DefExt, char *_FilterString );
	void Update( char *_FileName );
#ifndef _MSC_VER
	void Update( AnsiString& _FileName );
#endif
	void VerifyPath();
};

class CFileSystem {
public:
	char 	m_Root[MAX_PATH];
	FSPath 	m_ExeRoot;
	FSPath 	m_GameRoot;
	FSPath 	m_GameSounds;
	FSPath 	m_GameCForms;
	FSPath 	m_GameMeshes;
	FSPath 	m_GameTextures;
	FSPath 	m_GameLevels;
	FSPath	m_GameKeys;
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
    void 	Init			();

    bool 	Exist			(LPCSTR _FileName, bool bMessage = false);
	bool 	GetOpenName		(FSPath *initial, char *buffer);
	bool 	GetSaveName		(FSPath *initial, char *buffer);

    bool 	VerifyFileExists(const char *_FileName, bool bMessage = false);
    bool 	VerifyFileExists(FSPath *initial, const char *_FileName, bool bMessage = false);
    void 	DeleteFileByName(const char* nm);

#ifndef _MSC_VER
    int  	GetFileAge		(AnsiString& name);
    void 	SetFileAge		(AnsiString& name, int age);
#endif

    void 	GetFileVers		(const char* fn, st_ObjVer& v);
	void	VerifyPath		(LPCSTR path);
};

extern CFileSystem FS;


#endif /*_INCDEF_FileSystem_H_*/

