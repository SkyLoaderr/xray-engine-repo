// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
#define AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_
#pragma once

#include "SceneObject.h"

class CFrustum;

class CSound : public SceneObject
{
	friend class TfrmPropertiesSound;
	friend class SceneBuilder;
protected:
	Fvector m_Position;
	float	m_Range;
	char	m_fName[128];
public:
	void	SetFName(const char *N)	{
		strcpy(m_fName,N);
		if (strchr(m_fName,'.')) *strchr(m_fName,'.')=0;
	}

	virtual void Render( Fmatrix& parent, ERenderPriority flag );

	virtual bool RayPick(
		float& distance,
		Fvector& start,
		Fvector& direction,
		Fmatrix& parent, SRayPickInfo* pinf = NULL );

    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void 	Move( Fvector& amount );
	virtual void 	Rotate( Fvector& center, Fvector& axis, float angle );
	virtual void 	LocalRotate( Fvector& axis, float angle );

  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

	virtual bool GetBox( Fbox& box );

	CSound();
	CSound( char *name );
	void Construct();

	virtual ~CSound();
};

#endif // !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
