//----------------------------------------------------
// file: Light.h
//----------------------------------------------------

#ifndef _INCDEF_Light_H_
#define _INCDEF_Light_H_

#include "SceneObject.h"

enum ELightType{
    ltPoint = 0,
    ltSun   = 1,
    ltForceWord = 0xffff
};

class Light : public SceneObject {
public:
    float fSunSize;
    float fPower;
    BOOL bGradient;
    BOOL bFlares;

	// build options
	int m_CastShadows;

	// type of light
	ELightType m_Type; // flag !!!

    float m_Brightness;

	// common light parameters
	Fcolor m_Ambient;
	Fcolor m_Diffuse;
	Fcolor m_Specular;

	// omni light parameters
	Fvector m_Position;
	float m_Range;
	float m_Attenuation0;
	float m_Attenuation1;
	float m_Attenuation2;

	// directional light parameters
	Fvector m_Direction;

    BOOL m_Enabled;

	// d3d's light parameters
	int m_D3DIndex;
	Flight m_D3D;

public:

	void FillD3DParams();
	virtual void Render( Fmatrix& parent );

	virtual bool RTL_Pick(
		float *distance,
		Fvector& start,
		Fvector& direction,
		Fmatrix& parent, SPickInfo* pinf = NULL );

	virtual bool BoxPick(
		ICullPlane *planes,
		Fmatrix& parent );

	void Set( int d3dindex );
	void UnSet();

    void Enable( BOOL flag );

	virtual void Move( Fvector& amount );
	virtual void Rotate( Fvector& center, Fvector& axis, float angle );
	virtual void LocalRotate( Fvector& axis, float angle );

	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	Light();
	Light( char *name );
	void Construct();

	virtual ~Light();
};

#endif /*_INCDEF_Light_H_*/

