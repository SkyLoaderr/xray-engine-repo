//----------------------------------------------------
// file: SndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_SndPlane_H_
#define _INCDEF_SndPlane_H_

#include "Scene.h"


class SndPlane : public SceneObject {
public:

	// main data
	IVector m_Points[4];
	float m_Dencity;

public:

	virtual void Render( IMatrix& parent );
	
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

	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	SndPlane();
	SndPlane( char *name );
	void Construct();

	virtual ~SndPlane();
};

#endif /*_INCDEF_SndPlane_H_*/

