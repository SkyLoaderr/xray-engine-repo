// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
#define AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_
#pragma once

#include "Scene.h"

class CSound : public SceneObject  
{
	friend class CSoundEditor;
	friend class SceneBuilder;
protected:
	IVector m_Position;
	float	m_Range;
	char	m_fName[128];
public:
	void	SetFName(const char *N)	{
		strcpy(m_fName,N); 
		if (strchr(m_fName,'.')) *strchr(m_fName,'.')=0;
	}

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
	virtual void LocalRotate( IVector& axis, float angle );

	virtual void Load( FSChunkDef *chunk );
	virtual void Save( int handle );

	virtual bool GetBox( IAABox& box );

	CSound();
	CSound( char *name );
	void Construct();

	virtual ~CSound();
};

#endif // !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
