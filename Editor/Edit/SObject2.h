//----------------------------------------------------
// file: SObject2.h
//----------------------------------------------------
#ifndef _INCDEF_SObject2_H_
#define _INCDEF_SObject2_H_

#include "SceneObject.h"
//----------------------------------------------------
struct SPickInfo;
class Mesh;
class CTextureLayer;

class SObject2Mesh {
	void FullCopy( const SObject2Mesh* source );
public:
	char m_Name[32];
	char m_FileName[32];
	Fmatrix m_Position;
	Mesh *m_Mesh;
public:
	SObject2Mesh( Mesh *mesh = 0 ){
		m_Name[0]=0;
		m_FileName[0]=0;
		m_Position.identity();
		m_Mesh = mesh;
	};

	void CopyNames( const SObject2Mesh* source ){
        VERIFY(source);
		strcpy( m_Name, source->m_Name );
		strcpy( m_FileName, source->m_FileName );
	}

	SObject2Mesh( const SObject2Mesh* source ){
        VERIFY(source);
		m_Name[0]=0;
		m_FileName[0]=0;
		m_Position.identity();
		m_Mesh = 0;
		FullCopy( source );
	}

	~SObject2Mesh(){
		SAFE_DELETE( m_Mesh );
	}
	__inline char* GetName(){return m_Name; }
};

typedef list<SObject2Mesh*> SObjMeshList;
typedef SObjMeshList::iterator SObjMeshIt;

class SObject2 : public SceneObject {
protected:
	friend class SceneBuilder;
	friend class TfrmPropertiesObject;
	friend class Scene;
	friend class ELibrary;

    AnsiString m_FolderName;

	// temporary variables,
	// used in building process
	bool m_OGFCreated;
	char m_OGFName[MAX_PATH];

	// build options
	int m_MakeUnique;
	int m_DynamicList;

	// object data
	Fmatrix m_Position;
	IAABox m_Box;
    float m_fRadius;
    Fvector m_vDeltaOffs;
    Fvector m_Center;
	SObject2 *m_LibRef;
    bool bLibItem;

    void CopyMeshes (SObject2* source);
    void ClearMeshes();
public:
	char m_ClassName[MAX_OBJCLS_NAME];
	char m_ClassScript[MAX_CLASS_SCRIPT];
	SObjMeshList m_Meshes;
public:
	__inline bool IsLibItem     (){return bLibItem; }
	__inline bool IsReference   (){return (m_LibRef!=0); }
	__inline bool IsSceneItem   (){return ((m_LibRef==0)&&!bLibItem); }
	__inline char *GetRefName   (){_ASSERTE( m_LibRef );return m_LibRef->m_Name; }
	__inline char *GetClassName (){return m_ClassName; }
	__inline char *GetClassScript(){return m_ClassScript; }
	__inline bool ClassCompare( SObject2 *to ){	if( IsReference() ) return m_LibRef->ClassCompare( to );
                                        		return (0==stricmp(m_ClassName,to->m_ClassName)); }
	__inline bool RefCompare( SObject2 *to ){return (m_LibRef==to); }

	void UpdateClassScript();
	void UpdateBox();

	int GetFaceCount();
	int GetVertexCount();
    int GetLayerCount();

	SObject2Mesh* AddMesh( Mesh *mesh ){
		m_Meshes.push_back( new SObject2Mesh() );
		m_Meshes.back()->m_Mesh = mesh;
		return m_Meshes.back();
	}

	virtual void Render( Fmatrix& parent );
	void RenderEdge( Fmatrix& parent, SObject2Mesh* m, CTextureLayer* l=0 );
	virtual void RTL_Update( float dT );

	virtual bool RTL_Pick(
		float *distance,
		Fvector& start,
		Fvector& direction,
		Fmatrix& parent, SPickInfo* pinf = NULL );

	virtual bool BoxPick(
		ICullPlane *planes,
		Fmatrix& parent );

	virtual void Move( Fvector& amount );
	virtual void Rotate( Fvector& center, Fvector& axis, float angle );
	virtual void Scale( Fvector& center, Fvector& amount );
	virtual void LocalRotate( Fvector& axis, float angle );
	virtual void LocalScale( Fvector& amount );

	virtual void CloneFromLib( SceneObject *source );
	virtual void LibReference( SceneObject *source );
	virtual void ResolveReference();
	virtual void TranslateToWorld();

	void LoadMeshDef( FSChunkDef *chunk );
	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	SObject2( bool bLib = false );
	SObject2( char *name, bool bLib = false );
    SObject2( SObject2* source );
	virtual ~SObject2();
	void Construct();
    void CloneFrom( SObject2* source );
    bool ContainsMesh( SObject2Mesh* m );
};
//----------------------------------------------------
#endif /*_INCDEF_SObject2_H_*/


