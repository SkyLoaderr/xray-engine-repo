// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ESoundH
#define ESoundH
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

	virtual bool 	RayPick		(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL);

    virtual bool 	FrustumPick	(const CFrustum& frustum);

  	virtual bool 	Load		(IReader&);
	virtual void 	Save		(IWriter&);

	virtual bool 	GetBox		(Fbox& box);

    // change position/orientation methods
	virtual void 	Scale		(Fvector& amount){;}

	// editor integration
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);
};

#endif // SoundH
