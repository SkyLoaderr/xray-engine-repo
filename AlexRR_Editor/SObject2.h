//----------------------------------------------------
// file: SObject2.h
//----------------------------------------------------

#ifndef _INCDEF_SObject2_H_
#define _INCDEF_SObject2_H_

#include "FileSystem.h"
#include "SceneObject.h"
#include "ObjectOptionPack.h"
#include "StaticMesh.h"

//----------------------------------------------------

#define SOBJ_CHUNK_CLASS_NAME    0xA911
#define SOBJ_CHUNK_CLASS_SCRIPT  0xA912
#define SOBJ_CHUNK_POSITION      0xA913
#define SOBJ_CHUNK_REFERENCE     0xA914
#define SOBJ_CHUNK_MESH          0xA915
#define   SOBJ_CHUNK_MESH_NAME   0xA921
#define   SOBJ_CHUNK_MESH_FILE   0xA922
#define   SOBJ_CHUNK_MESH_POS    0xA923
#define   SOBJ_CHUNK_MESH_CORE   0xA924
#define   SOBJ_CHUNK_MESH_OPS    0xA926
#define SOBJ_CHUNK_BOP           0xA917

//----------------------------------------------------

class SObject2Mesh {
public:

	char m_Name[32];
	char m_FileName[32];
	ObjectOptionPack m_Ops;
	IMatrix m_Position;
	Mesh *m_Mesh;

public:
	
	SObject2Mesh( Mesh *mesh = 0 ){
		m_Name[0]=0;
		m_FileName[0]=0;
		m_Position.identity();
		m_Mesh = mesh;
		m_Ops.Reset();
	};

	void FullCopy( const SObject2Mesh& source ){
		strcpy( m_Name, source.m_Name );
		strcpy( m_FileName, source.m_FileName );
		m_Position.set( source.m_Position );
		m_Mesh = source.m_Mesh;
		m_Ops.Copy( source.m_Ops );
	}

	void CopyNames( const SObject2Mesh& source ){
		strcpy( m_Name, source.m_Name );
		strcpy( m_FileName, source.m_FileName );
	}

	SObject2Mesh( const SObject2Mesh& source ){
		FullCopy( source );
	}

	~SObject2Mesh(){
		SAFE_DELETE( m_Mesh );
	}
};

typedef list<SObject2Mesh> SObjMeshList;
typedef list<SObject2Mesh>::iterator SObjMeshIt;

class SLandscape;

class SObject2 : public SceneObject {
protected:

	friend class SceneBuilder;
	friend class SObject2Editor;

	// temporary variables,
	// used in building process
	bool m_OGFCreated;
	char m_OGFName[MAX_PATH];

	// build options
	friend class ELibrary;
	int m_MakeUnique;
	int m_DynamicList;
	
	// object data
	char m_ClassName[MAX_OBJCLS_NAME];
	char m_ClassScript[MAX_CLASS_SCRIPT];
	IMatrix m_Position;
	IAABox m_Box;
	SObjMeshList m_Meshes;
	SObject2 *m_LibRef;

public:

	__forceinline bool IsReference(){
		return (m_LibRef!=0); }
	__forceinline char *GetRefName(){
		_ASSERTE( m_LibRef );
		return m_LibRef->m_Name; }
	__forceinline char *GetClassName(){
		return m_ClassName; }
	__forceinline char *GetClassScript(){
		return m_ClassScript; }
	__forceinline bool ClassCompare( SObject2 *to ){
		if( IsReference() ) return m_LibRef->ClassCompare( to );
		return (0==stricmp(m_ClassName,to->m_ClassName)); }
	__forceinline bool RefCompare( SObject2 *to ){
		return (m_LibRef==to); }

	void UpdateClassScript();
	void UpdateBox();
	void FillD3DPoints();

	SObject2Mesh& AddMesh( Mesh *mesh ){
		m_Meshes.push_back( SObject2Mesh() );
		m_Meshes.back().m_Mesh = mesh;
		return m_Meshes.back();
	}

	virtual void Render( IMatrix& parent );
	virtual void RTL_Update( float dT );

	virtual bool RTL_Pick(
		float *distance,
		IVector& start,
		IVector& direction,
		IMatrix& parent );

	virtual bool BoxPick(
		ICullPlane *planes,
		IMatrix& parent );

	virtual void Move( IVector& amount );
	virtual void Rotate( IVector& center, IVector& axis, float angle );
	virtual void Scale( IVector& center, IVector& amount );
	virtual void LocalRotate( IVector& axis, float angle );
	virtual void LocalScale( IVector& amount );
	
	virtual void LibCopy( SceneObject *source );
	virtual void LibReference( SceneObject *source );
	virtual void ResolveReference();
	virtual void TranslateToWorld();

	void LoadMeshDef( FSChunkDef *chunk );
	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	bool CreateFromLandscape( SLandscape *landscape );

	SObject2();
	SObject2( char *name );
	void Construct();

	virtual ~SObject2();
};



//----------------------------------------------------
#endif /*_INCDEF_SObject2_H_*/


