//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------

#ifndef _INCDEF_StaticMesh_H_
#define _INCDEF_StaticMesh_H_

#include "Texture.h"
#include "FileSystem.h"
#include "StaticMeshChunks.h"
//----------------------------------------------------

#pragma pack(push,1)

class MMaterial {
public:

	long m_FaceStart;
	long m_FaceCount;

	ETexture *m_Texture;
	ETexture *m_LightMap;

	union{
		struct {
			IFColor m_Diffuse;
			IFColor m_Ambient;
			IFColor m_Specular;
			IFColor m_Emission;
			float m_Power;
		};
		D3DMATERIAL7 m_D3D;
	};

public:
	MMaterial();
	MMaterial( const MMaterial& source );
	~MMaterial();
};

class MPoint {
public:
	IVector m_Point;
	IVector m_Normal;
	IVector m_TexCoord;
};

class MPointLighting {
public:
	bool m_Valid;
	IColor m_Diffuse;
	IColor m_Specular;
};

class MLMPoint {
public:
	bool m_Valid;
	float u,v;
};

class MFace {
public:
	union{
		WORD p[3];
		struct { WORD p0,p1,p2; };
	};
};

#pragma pack(pop)



class Mesh {
protected:

	friend class SceneBuilder;

	// internal structure
	vector<MMaterial> m_Materials;
	vector<MPoint> m_Points;
	vector<MFace> m_Faces;

	// d3d points
	vector<D3DVERTEX> m_D3D_Points;

	// may be absent
	vector<MPointLighting> m_Lighting;
	vector<MLMPoint> m_LMap;

public:

	void FillD3DPoints();

	bool Verify();
	void Render( IMatrix& parent );
	void Append( Mesh *source );
	void Copy( Mesh *source );
	void GetMaterial( Mesh *dest, MMaterial *material );
	void Transform( IMatrix& parent );
	void GetBox( IAABox& box );
	bool AddPoint( MPoint& point, MPointLighting *lt = 0, MLMPoint *lpmt = 0 );
	bool AddFace( MFace& face );
	bool AddMaterial( MMaterial& material );
	bool CollapseMaterials( int id1, int to_collapse );
	bool CompareMaterials( int id1, int to_collapse );

	bool Pick(
		float *distance,
		IVector& start,
		IVector& direction,
		IMatrix& parent );

	bool BoxPick(
		ICullPlane *planes,
		IMatrix& parent );

	bool Load( char *filename );
	bool LoadMesh( FSChunkDef *chunk );
	bool LoadMaterial( FSChunkDef *chunk );
	void SaveMesh( int handle );

	Mesh();
	~Mesh();
};


typedef list<Mesh*> MeshList;
typedef list<Mesh*>::iterator MeshIt;

//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

