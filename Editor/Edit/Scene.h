//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------
#ifndef _INCDEF_Scene_H_
#define _INCDEF_Scene_H_

#include "SceneObject.h";
#include "cl_defs.h";
#include "SceneClassList.h"

//refs
struct FSChunkDef;
//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )

class Light;
class SObject2;
class SObject2Mesh;

struct SPickInfo{
    SObject2*   obj;
    SObject2Mesh*mesh;
    Fvector     pt;
    raypick_info rp_inf;
    SPickInfo(){ ZeroMemory(this,sizeof(SPickInfo));rp_inf.range = 999999;}
};

int __cdecl _CompareByDist( const void *a, const void *b)
{
    return ((SPickInfo*)a)->rp_inf.range - ((SPickInfo*)b)->rp_inf.range;
}

class EScene {
public:
	// globals options
	bool WriteGlobals( FSChunkDef *chunk );
	bool ReadGlobals( FSChunkDef *chunk );
	void ResetGlobals();

	char m_FNLevelPath[MAX_PATH];
	char m_LevelName[MAX_PATH];

	void InitDefaultText();
	char m_BOPText[MAX_LTX_ADD];
// Build options
	BOOL		m_bTesselate;
	BOOL		m_bConvertProgressive;
	BOOL		m_bLightMaps;

	float		m_maxedge;			// for tesselation - by default: 1m
	DWORD		m_VB_maxSize;		// by default: 64k
	DWORD		m_VB_maxVertices;	// by default: 4096
	float		m_pixels_per_meter;	// LM - by default: 20 ppm
	float		m_maxsize;			// max object size - by default: 16m
	float		m_relevance;		// dimension of VIS slot - by default: 10m
	float		m_viewdist;			// view distance - by default: 100m
//------------------------------------------------------------------------------
    vector<Light*> frame_light;
    CList<SPickInfo> pickFaces;
protected:
	bool m_Valid;
	int m_Locked;

	int m_LastAvailObject;

	ObjectList  m_Objects;

	IGroundPoint m_LandGrid;

	deque<UndoItem> m_UndoStack;
	deque<UndoItem> m_RedoStack;

protected:
	bool ReadObject( FSChunkDef *chunk );
	void clearobjects();

public:
//    SHMap       m_HMap;
    bool m_Modified;

public:

	bool Load(char *_FileName);
	void Save(char *_FileName);
	bool LoadSelection(char *_FileName);
	void SaveSelection(int classfilter, char *_FileName);
	void Unload();

	__inline bool valid()           { return m_Valid; }
	__inline float lx()             { return m_LandGrid.x; }
	__inline float lz()             { return m_LandGrid.z; }

	__inline bool locked()          { return m_Locked!=0; }
	__inline void lock()            { m_Locked++; }
	__inline void unlock()          { m_Locked--; }
	__inline void waitlock()        { while( locked() ) Sleep(0); }

	__inline ObjectIt FirstObj()    { return m_Objects.begin(); }
	__inline ObjectIt LastObj()     { return m_Objects.end(); }
	__inline int ObjCount()         { return m_Objects.size(); }

	__inline void ReplaceList( ObjectList& objlist ){
                                    lock();
                                    m_Objects.clear();
		                            m_Objects.merge( objlist );
		                            unlock(); }

	void Render                     ( float _Aspect, Fmatrix *_Camera );
	void Update                     ( float dT );

	void AddObject                  ( SceneObject* object );
	void RemoveObject               ( SceneObject* object );
	bool FullRemove                 ( SceneObject* object );
    bool ContainsObject             ( SceneObject* object );
	SceneObject *QuadPick           ( int x, int z, int classfilter );
	SceneObject *RTL_Pick           ( Fvector& start, Fvector& direction, int classfilter, SPickInfo* pinf = NULL );
	int BoxPickSelect               ( ICullPlane *planes, int classfilter, bool flag );
	int SelectObjects               ( bool flag, int classfilter );
	int ShowObjects                 ( bool flag, int classfilter, bool bOnlySelected = false );
	int InvertSelection             ( int classfilter );
	int SelectionCount              ( int classfilter, bool testflag );
	int RemoveSelection             ( int classfilter = OBJCLASS_NONE );
	int CutSelection                ( int classfilter = OBJCLASS_NONE );
	int CopySelection               ( int classfilter = OBJCLASS_NONE );
	int PasteSelection              ();
	int ObjectCount                 ( int classfilter = OBJCLASS_NONE );

	void UndoClear();
	void UndoPrepare();
	bool Undo();
	bool Redo();

	void SetLights();
	void ClearLights();
    void TurnLightsForObject(SObject2* obj);

	void GenObjectName( char *buffer );
	bool SearchName( char *name );
	void RebuildAllObjects();

    void ClearPickList() { pickFaces.Clear(); }
    __forceinline void SortPickList() { pickFaces.Sort(_CompareByDist); }
public:

	void Init();
	void Clear();
    void Modified(){m_Modified = true;}
    bool IfModified();

	EScene();
	virtual ~EScene();
};


//----------------------------------------------------


extern EScene Scene;


//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

