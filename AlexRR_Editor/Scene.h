//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------

#ifndef _INCDEF_Scene_H_
#define _INCDEF_Scene_H_

#include "FileSystem.h"
#include "SceneObject.h"


//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )


class EScene {
public:

	// globals options

	bool WriteGlobals( FSChunkDef *chunk );
	bool ReadGlobals( FSChunkDef *chunk );
	void ResetGlobals();

	char m_FNLtx[MAX_PATH];
	char m_FNLights[MAX_PATH];
	char m_FNForms[MAX_PATH];
	char m_FNMeshList[MAX_PATH];
	char m_FNMap[MAX_PATH];
	char m_FNVisibility[MAX_PATH];

	char m_FNLevelPath[MAX_PATH];
	char m_FNTexPath[MAX_PATH];

	char m_LevelName[MAX_PATH];

	int m_BOPVisMap;
	int m_BOPLightMaps;
	int m_BOPCheck;
	int m_BOPCollapseMaps;

	void InitDefaultText();
	char m_BOPText[MAX_LTX_ADD];
	char m_BOPAddFiles[MAX_ADD_FILES_TEXT];
	char m_BOPLMapPrefix[MAX_PATH];

	float m_BOPVisQuad;

	int m_BOPOptimize;
	float m_BOPConnectDist;
	float m_BOPAvObjSize;

	IGroundPoint m_BOPVisCellSize;
	IGroundPoint m_BOPIndentLow;
	IGroundPoint m_BOPIndentHigh;

	IFColor m_GlobalAmbient;
	IFColor m_GlobalDiffuse;
	IFColor m_GlobalSpecular;
	IVector m_GlobalLightDirection;

protected:

	bool m_Valid;
	int m_Locked;

	int m_LastAvailObject;

	ObjectList m_Objects;

	IGroundPoint m_LandGrid;

	deque<UndoItem> m_UndoStack;
	deque<UndoItem> m_RedoStack;

protected:
	void WriteDefaultLight( FSChunkDef *chunk );
	void ReadDefaultLight( FSChunkDef *chunk );
	bool ReadObject( FSChunkDef *chunk );
	void clearobjects();

public:

	bool Load(char *_FileName);
	void Save(char *_FileName);
	bool LoadSelection(char *_FileName);
	void SaveSelection(int classfilter, char *_FileName);
	void Unload();
	
	__forceinline bool valid(){ return m_Valid; }
	__forceinline float lx(){ return m_LandGrid.x; }
	__forceinline float lz(){ return m_LandGrid.z; }

	__forceinline bool locked(){ return m_Locked!=0; }
	__forceinline void lock(){ m_Locked++; }
	__forceinline void unlock(){ m_Locked--; }
	__forceinline void waitlock(){ while( locked() ) Sleep(0); }

	__forceinline ObjectIt FirstObj(){
		return m_Objects.begin(); }
	__forceinline ObjectIt LastObj(){
		return m_Objects.end(); }
	
	__forceinline void ReplaceList( ObjectList& objlist ){
		lock();
		m_Objects.clear();
		m_Objects.merge( objlist );
		unlock(); }

	void Render( float _Aspect, IMatrix *_Camera );
	void Update( float dT );
	
	void AddObject( SceneObject* object );
	void RemoveObject( SceneObject* object );
	bool FullRemove( SceneObject *object );
	SceneObject *QuadPick( int x, int z, int classfilter );
	SceneObject *RTL_Pick( IVector& start, IVector& direction, int classfilter );
	int BoxPickSelect( ICullPlane *planes, int classfilter, bool flag );
	int SelectObjects( bool flag, int classfilter );
	int InvertSelection( int classfilter );
	int SelectionCount( int classfilter, bool testflag );
	int RemoveSelection( int classfilter = -1 );
	int CutSelection( int classfilter = -1 );
	int CopySelection( int classfilter = -1 );
	int PasteSelection();
	int ObjectCount( int classfilter = -1 );

	void UndoClear();
	void UndoPrepare();
	bool Undo();
	bool Redo();

	void ResetDefaultLight();
	void FillDefaultLight( D3DLIGHT7 *light );
	void SetLights();
	void ClearLights();
	void SmoothLandscape();
	void GenObjectName( char *buffer );
	bool SearchName( char *name );
	void RebuildAllObjects();

public:

	void Init();
	void Clear();

	EScene();
	virtual ~EScene();
};


//----------------------------------------------------


extern EScene Scene;


//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

