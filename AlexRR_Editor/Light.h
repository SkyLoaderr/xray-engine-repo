//----------------------------------------------------
// file: Light.h
//----------------------------------------------------

#ifndef _INCDEF_Light_H_
#define _INCDEF_Light_H_

#include "Scene.h"
#include "Texture.h"


#define LIGHT_CHUNK_VERSION				0xB411
#define LIGHT_CHUNK_BUILD_OPTIONS		0xB412
#define LIGHT_CHUNK_DIRECTIONAL_FLAG	0xB413
#define LIGHT_CHUNK_COLOR_PARAMS		0xB414
#define LIGHT_CHUNK_OMNI_PARAMS			0xB415
#define LIGHT_CHUNK_DIR_PARAMS			0xB416


#define LIGHT_OMNI         0
#define LIGHT_DIRECTIONAL  1


class Light : public SceneObject {
public:

	// build options
	int m_CastShadows;

	// type of light 
	int m_Directional; // flag !!!

	// common light parameters
	IFColor m_Ambient;
	IFColor m_Diffuse;
	IFColor m_Specular;

	// omni light parameters
	IVector m_Position;
	float m_Range;
	float m_Attenuation0;
	float m_Attenuation1;
	float m_Attenuation2;
	
	// directional light parameters
	IVector m_Direction;

protected:

	friend class SceneBuilder;
	
	// d3d's light parameters
	int m_D3DIndex;
	D3DLIGHT7 m_D3D;

public:

	void FillD3DParams();
	virtual void Render( IMatrix& parent );
	
	virtual bool RTL_Pick(
		float *distance,
		IVector& start,
		IVector& direction,
		IMatrix& parent );

	virtual bool BoxPick(
		ICullPlane *planes,
		IMatrix& parent );

	void Set( int d3dindex );
	void UnSet();

	virtual void Move( IVector& amount );
	virtual void Rotate( IVector& center, IVector& axis, float angle );
	virtual void LocalRotate( IVector& axis, float angle );

	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	Light();
	Light( char *name );
	void Construct();

	virtual ~Light();
};

#endif /*_INCDEF_Light_H_*/

