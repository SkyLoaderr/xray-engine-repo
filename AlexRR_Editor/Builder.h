//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef _INCDEF_Builder_H_
#define _INCDEF_Builder_H_

#include "FileSystem.h"
#include "Scene.h"
#include "StaticMesh.h"
//----------------------------------------------------

class SObject2;
class SLandscape;
class Mesh;
class SObject2Mesh;
class ObjectOptionPack;

class IMatrix;
class MPoint;
class MMaterial;
class MPointLighting;


#define LMAP_W 32
#define LMAP_H 32
#define LMAP_QW 8
#define LMAP_QH 8

class BuilderWindow {
protected:
	HANDLE m_DialogThread;
	DWORD m_DialogThreadID;
public:
	HWND m_Window;
	char m_LastStage[256];
	float m_LastProgress;
public:
	void SetStage( char *stage );
	void SetProgress( float progress );
	void Open();
	BuilderWindow();
	~BuilderWindow();
};


class StaticListItem{
public:
	int m_FileIndex;
	char m_OGF_Name[MAX_PATH];
	SObject2 *m_Def;
public:
	StaticListItem(){
		m_FileIndex = -1;
		m_OGF_Name[0] = 0;
		m_Def = 0;
	};
	StaticListItem( const StaticListItem& source ){
		m_FileIndex = source.m_FileIndex;
		strcpy(m_OGF_Name,source.m_OGF_Name);
		m_Def = source.m_Def;
	};
	~StaticListItem(){
	};
};

typedef list<StaticListItem> SI_List;
typedef SI_List::iterator SI_ListIt;


class VisItem{
public:
	vector<StaticListItem*> m_Files;
public:
	VisItem(){
	};
	VisItem( const VisItem& source ){
		m_Files.insert( m_Files.end(),
			source.m_Files.begin(),
			source.m_Files.end() );
	};
	~VisItem(){
		m_Files.clear();
	};
};


class VisMap{
protected:
	VisItem *m_Map;
	int dx,dz;
public:
	VisMap();
	~VisMap();
	void Allocate( int _dx, int _dz );
	void Free( );

	__forceinline int X(){
		return dx; }

	__forceinline int Z(){
		return dz; }

	__forceinline VisItem *Get(int x, int z){
		_ASSERTE( m_Map );
		_ASSERTE( x >= 0 && x<dx );
		_ASSERTE( z >= 0 && z<dz );
		return (m_Map + (z * dx + x));
	};

};

struct POINT_LIGHT_PARAM {
	IVector *position;
	IVector *normal;
	IFColor *ambient;
	IFColor *diffuse;
	IFColor *emission;
	IFColor *rescolor;
};

struct LIGHT_PICK_PARAM {
	IVector *point;
	IVector *light;
	float *pickdist;
};

struct CURRENT_TMAP {
	ETexture *map;
	int wfill;
	int hfill;
	int prev_wfill;
	int prev_hfill;
};

class SceneBuilder {
protected:

	EScene *m_Scene;
	char m_TempFilename[MAX_PATH];
	bool m_SceneSaved;

	bool m_InProgress;

	HANDLE m_Thread;
	DWORD m_ThreadID;

	FSPath m_LevelPath;
	FSPath m_TexPath;
	IVector m_LevelShift;
	IAABox m_LevelBox;

	SI_List m_Statics;
	VisMap m_VMap;
	SSTRLIST m_TexNames;

protected:

	bool PrepareFolders();
	bool ConvertLandscape();
	bool ResolveReferences();
	bool OptimizeObjects();
	
	bool BuildCollisionModel();
	bool AddObjectCollisionForm( int handle, SObject2 *obj );
	
	bool GetShift();
	bool ShiftLevel();

	bool BuildFakeCollision();
	bool PickCFCell( int x, int z, float *top, float *bottom );

	bool OptimizeRenderObjects();
	bool OptimizeRenderObjects2();

	bool VerifyConnectionOptions(
		SObject2 *first, SObject2 *second );
	bool ConnectObjects(
		SObject2 *first, SObject2 *second );
	bool VerifyMeshConnect(
		SObject2Mesh *first, SObject2Mesh *second );

	bool BuildLightModel();
	bool VertexLighting();
	bool TextureLighting();
	bool OptimizeLightmaps();
	void PointLighting( POINT_LIGHT_PARAM *param );
	bool LightPick( LIGHT_PICK_PARAM *param );
	bool CreateOptimizedLightMap( Mesh *mesh );

	void CMAP_Create( CURRENT_TMAP *map, int& nameindex );
	void CMAP_Delete( CURRENT_TMAP *map );
	void CMAP_Validate( CURRENT_TMAP *map );
	bool CMAP_Add( CURRENT_TMAP *map, ETexture *t, int& nameindex );

	bool BuildLTX();
	bool CompileText();

	bool WriteTextures();
	void AddUniqueTexName( char *name );

	bool BuildVisibility();
	bool BuildRenderModel();
	bool BuildObjectOGF( int handle, int file_index, SObject2 *obj );
	bool BuildSingleOGF( int handle, Mesh *mesh, ObjectOptionPack *ops );

public:

	virtual DWORD Thread();

public:
	
	SceneBuilder();
	virtual ~SceneBuilder();

	virtual bool Execute( EScene *scene );
	virtual bool Stop();
	
	__forceinline bool InProgress(){
		return m_InProgress; }
};

extern SceneBuilder Builder;
extern BuilderWindow BWindow;

//----------------------------------------------------
#endif /*_INCDEF_Builder_H_*/

