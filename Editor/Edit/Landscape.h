//----------------------------------------------------
// file: Landscape.h
//----------------------------------------------------

#ifndef _INCDEF_Landscape_H_
#define _INCDEF_Landscape_H_

#include "Scene.h"
#include "ObjectOptionPack.h"
#include "Texture.h"


#define LAND_CHUNK_DATA     0xE420
#define LAND_CHUNK_OPS      0xE421


class SLandscape : public SceneObject {
public:

	// build options
	ObjectOptionPack m_Ops;

	// material
	IFColor m_Ambient;
	IFColor m_Diffuse;
	IFColor m_Specular;
	IFColor m_Emission;
	float m_Power;

protected:

	// for converting
	friend class SObject2;
	
	IVector m_Center;
	int m_MapX,m_MapZ;

	// sequence defined as
	//  0 - center
	//  1,2,3,4 - edges;
	IVector m_Points[5];
	IVector m_Normals[5];
	IVector m_TexCoord[5];
	ETexture *m_Texture;

	// per-face normals
	IVector m_FaceNormals[4];

	// d3d points
	vector<D3DVERTEX> m_D3D_Points;
	vector<WORD> m_D3D_Indices;

public:

	bool TexCompare( char *texturename );
	void SetTexture( ETexture *t );
	void SetQuad( int x, int z );
	void CalcDefCenter();
	void CalcDefPoints();
	void FillD3DPoints();
	void UpdateNormals();
	void TryConnectTo( SLandscape *to );

	void ResetPointNormals();
	void TryAddPointNormals( SLandscape *from );
	void NormalizePointNormals();

	__inline bool CompareMapPos( int x, int z){
		return (m_MapX==x && m_MapZ==z); }

	virtual void Render( IMatrix& parent );
	
	virtual bool RTL_Pick(
		float *distance,
		IVector& start,
		IVector& direction,
		IMatrix& parent );

	virtual bool BoxPick(
		ICullPlane *planes,
		IMatrix& parent );

	virtual bool QuadPick( int x, int z );

	virtual void Move( IVector& amount );
	virtual void Rotate( IVector& center, IVector& axis, float angle );
	virtual void Scale( IVector& center, IVector& amount );

	void LoadData( FSChunkDef *chunk );
	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	SLandscape();
	SLandscape( char *name );
	void Construct();

	virtual ~SLandscape();
};

#endif /*_INCDEF_Landscape_H_*/

