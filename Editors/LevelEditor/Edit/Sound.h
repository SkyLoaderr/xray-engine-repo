// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
#define AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_
#pragma once

#include "CustomObject.h"               

class CFrustum;

class CSound : public CCustomObject
{
	friend class TfrmPropertiesSound;
	friend class SceneBuilder;
protected:
	float	m_Range;
	char	m_fName[128];
//    virtual Fvector& GetScale		()	{ FScale.set(m_Range,m_Range,m_Range); return FScale; 	}
//	virtual void 	SetScale		(Fvector& sc){float v=m_Range; if (!fsimilar(FScale.x,sc.x)) v=sc.x; if (!fsimilar(FScale.y,sc.y)) v=sc.y; if (!fsimilar(FScale.z,sc.z)) v=sc.z; FScale.set(v,v,v); m_Range=v; UpdateTransform();}
public:
			CSound		(LPVOID data);
	void 	Construct	(LPVOID data);
	virtual ~CSound		();

	void	SetFName	(const char *N)	{
		strcpy(m_fName,N);
		if (strchr(m_fName,'.')) *strchr(m_fName,'.')=0;
	}

	virtual void Render(int priority, bool strictB2F);

	virtual bool RayPick(
		float& distance,
		Fvector& start,
		Fvector& direction,
		SRayPickInfo* pinf = NULL );

    virtual bool 	FrustumPick	( const CFrustum& frustum );

  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

	virtual bool GetBox( Fbox& box );

    // change position/orientation methods
	virtual void 	Scale		(Fvector& amount){;}
};

#endif // !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
