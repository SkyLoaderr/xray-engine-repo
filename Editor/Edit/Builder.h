//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef _INCDEF_Builder_H_
#define _INCDEF_Builder_H_

#include "FileSystem.h"
//----------------------------------------------------

// refs
class SObject2;
class Mesh;
class SObject2Mesh;
class EScene;
//----------------------------------------------------

class SceneBuilder {
protected:
    bool        bStaticRemoteInProgress;

	EScene *m_Scene;
	char m_TempFilename[MAX_PATH];
	bool m_SceneSaved;

	bool m_InProgress;

	FSPath m_LevelPath;
	FSPath m_TexPath;
	Fvector m_LevelShift;
	IAABox m_LevelBox;

	SSTRLIST m_TexNames;
protected:
	bool PrepareFolders             ();
	bool ResolveReferences          ();
	bool OptimizeObjects            ();

	bool GetShift                   ();
	bool ShiftLevel                 ();

	bool OptimizeRenderObjects      ();
	bool OptimizeRenderObjects2     ();

	bool VerifyConnectionOptions    ( SObject2 *first, SObject2 *second );
	bool ConnectObjects             ( SObject2 *first, SObject2 *second );
	bool VerifyMeshConnect          ( SObject2Mesh *first, SObject2Mesh *second );

	bool BuildLTX                   ();

	bool WriteTextures              ();
	void AddUniqueTexName           ( char *name );

	bool BuildObjectOGF             ( int handle, int file_index, SObject2 *obj );
	bool BuildSingleOGF             ( int handle, Mesh *mesh );

    bool RemoteStaticBuild          ();
    bool RemoteStaticFinalize       ();
public:
	virtual DWORD Thread            ();
public:
	SceneBuilder                    ();
	virtual ~SceneBuilder           ();

	virtual bool Execute            ( EScene *scene );
	virtual bool Stop               ();

	__inline bool InProgress        (){	return m_InProgress; }
};

extern SceneBuilder Builder;

//----------------------------------------------------
#endif /*_INCDEF_Builder_H_*/

