// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
#define AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_
#pragma once

class CFrustum;

class CSound : public CCustomObject
{
	typedef CCustomObject inherited;
	friend class TfrmPropertiesSound;
	friend class SceneBuilder;
protected:
	float			m_Range;
	AnsiString		m_WAVName;
public:
					CSound		(LPVOID data, LPCSTR name);
	void 			Construct	(LPVOID data);
	virtual 		~CSound		();

	virtual void 	Render		(int priority, bool strictB2F);

	virtual bool 	RayPick		(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf = NULL);

    virtual bool 	FrustumPick	(const CFrustum& frustum);

  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

	virtual bool 	GetBox		(Fbox& box);

    // change position/orientation methods
	virtual void 	Scale		(Fvector& amount){;}

	// editor integration
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
};

#endif // !defined(AFX_SOUND_H__2B9B91B2_9F9F_4A42_96CB_B50CC8B42FA9__INCLUDED_)
