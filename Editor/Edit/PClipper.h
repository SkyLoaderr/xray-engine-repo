//----------------------------------------------------
// file: PClipper.h
//----------------------------------------------------

#ifndef _INCDEF_PClipper_H_
#define _INCDEF_PClipper_H_

#include "SceneObject.h"

class PClipper: public SceneObject {
    void CreatePlane();
public:
    Fvector m_Center;
    float w, h;
    Fvector m_Angle;

	// main data
	Fvector m_Points[4];
	float m_Density;
public:

	virtual void Render( Fmatrix& parent );

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

	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	PClipper();
	PClipper( char *name );
	void Construct();
    void SetPlane(int p_num, Fvector& p, float d=0);

	virtual ~PClipper();
};

#endif /*_INCDEF_PClipper_H_*/

